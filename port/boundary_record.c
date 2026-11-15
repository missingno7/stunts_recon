#include "port_runtime.h"
#include "boundary_record.h"
#include <stdio.h>
#include <string.h>
static FILE *s_output;
static unsigned s_depth,s_scope;
static uint32_t s_frame;
static uint16_t s_limit;
static uint64_t s_order;
extern void port_timer_observation(uint64_t *,int *);
static void hex(const void *data,size_t bytes)
{
    const uint8_t *p=data;size_t i;
    fputc('"',s_output);
    for(i=0;i<bytes;++i)fprintf(s_output,"%02x",p[i]);
    fputc('"',s_output);
}
int port_boundary_open(const char *path)
{
    s_depth=s_scope=s_frame=0;s_order=0;
    if(!path || !*path)return 1;
    s_output=fopen(path,"wb");
    if(!s_output)return 0;
    fputs("{\"kind\":\"header\",\"schema\":\"stunts-port-boundary-v1\"}\n",s_output);
    return 1;
}
void port_boundary_close(void)
{
    if(s_output && fclose(s_output))port_guest_set_exit_status(2);
    s_output=NULL;
}
unsigned port_boundary_scope(void){return s_scope;}
uint32_t port_boundary_frame(void){return s_scope?s_frame:port_game_frame_snapshot();}
void port_boundary_frame_limit(uint16_t frame){s_limit=frame;}
void port_boundary_sim_begin(uint16_t frame){
    if(s_limit && frame>=s_limit && port_game_inputmode_snapshot()) {
        port_guest_request_stop("requested game frame reached");port_guest_check_stop();
    }
    s_scope=1;s_frame=frame;
}
void port_boundary_sim_end(void){s_scope=2;s_frame=port_game_frame_snapshot();}
int port_boundary_begin(PortBoundaryCall *call,const char *name,const uint32_t *args,
    unsigned argc,unsigned pointer_mask,const PortBoundaryRegion *regions,unsigned count)
{
    unsigned i;uint64_t tick;int slot;
    if(!s_output)return 0;
    ++s_depth;
    port_timer_observation(&tick,&slot);
    if(s_depth>1 || slot>=0)return -1; /* cross-layer calls, not implementation decomposition */
    if(argc>12 || count>PORT_BOUNDARY_REGIONS)port_guest_unwind("boundary contract exceeds recorder bounds");
    call->name=name;call->order=s_order++;call->scope=s_scope;call->frame=port_boundary_frame();
    call->active=port_game_inputmode_snapshot();
    call->argc=argc;call->pointer_mask=pointer_mask;call->region_count=count;
    if(argc)memcpy(call->args,args,argc*sizeof(*args));
    for(i=0;i<count;++i){
        call->regions[i]=regions[i];
        if(!regions[i].data || regions[i].size>PORT_BOUNDARY_BYTES)
            port_guest_unwind("invalid boundary memory span");
        memcpy(call->before[i],regions[i].data,regions[i].size);
    }
    return 1;
}
void port_boundary_end(PortBoundaryCall *call,uint32_t result,int active)
{
    unsigned i,j;
    if(!active)return;
    if(active<0){--s_depth;return;}
    fprintf(s_output,"{\"kind\":\"boundary\",\"name\":\"%s\",\"scope\":%u,\"frame\":%u,\"active\":%u,\"order\":%llu,\"args\":[",
        call->name,call->scope,call->frame,call->active,(unsigned long long)call->order);
    for(i=0;i<call->argc;++i){if(i)fputc(',',s_output);
        if(call->pointer_mask&(1u<<i))fputs("null",s_output);else fprintf(s_output,"%u",call->args[i]);}
    fputs("],\"result\":",s_output);
    if(result==UINT32_MAX)fputs("null",s_output);else fprintf(s_output,"%u",result);
    fputs(",\"regions\":[",s_output);
    for(i=0;i<call->region_count;++i){
        const PortBoundaryRegion *r=&call->regions[i];int alias=-1,offset=0;
        for(j=0;j<i;++j){const PortBoundaryRegion *other=&call->regions[j];
            uintptr_t a=(uintptr_t)r->data,b=(uintptr_t)other->data;
            if(a<b+other->size && b<a+r->size){alias=(int)j;offset=(int)(a-b);break;}}
        if(i)fputc(',',s_output);
        fprintf(s_output,"{\"name\":\"%s\",\"read\":%u,\"alias\":%d,\"offset\":%d,\"before\":",r->name,r->read,alias,offset);
        hex(call->before[i],r->size);fputs(",\"after\":",s_output);hex(r->data,r->size);fputc('}',s_output);
    }
    fputs("]}\n",s_output);--s_depth;
}
