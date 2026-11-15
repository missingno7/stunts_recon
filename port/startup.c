#include "port_runtime.h"
#include <SDL3/SDL.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#endif
static uint64_t s_seed, s_state;
static const char *s_source;
int port_startup_seed_init(const char *seed)
{
    if (seed) {
        char *end;
        errno=0; s_seed=strtoull(seed,&end,seed[0]=='0' && (seed[1]=='x' || seed[1]=='X') ? 16 : 10);
        if (errno || end==seed || *end || *seed=='-') return 0;
        s_source="explicit";
    } else {
#ifdef _WIN32
        typedef LONG (WINAPI *Generate)(void *,unsigned char *,unsigned long,unsigned long);
        HMODULE module=LoadLibraryW(L"bcrypt.dll");
        Generate generate=module ? (Generate)(uintptr_t)GetProcAddress(module,"BCryptGenRandom") : NULL;
        int ok=generate && generate(NULL,(unsigned char *)&s_seed,sizeof s_seed,2)>=0;
        if(module) FreeLibrary(module);
        if(!ok) return 0;
#else
        FILE *f=fopen("/dev/urandom","rb");
        if(!f) return 0;
        int ok=fread(&s_seed,1,sizeof s_seed,f)==sizeof s_seed;
        fclose(f); if(!ok) return 0;
#endif
        s_source="os";
    }
    s_state=s_seed;
    port_random_test_set_state((uint32_t)(s_seed ^ (s_seed>>32)));
    return 1;
}
static uint64_t next_entropy(void)
{
    uint64_t z=(s_state+=UINT64_C(0x9e3779b97f4a7c15));
    z=(z^(z>>30))*UINT64_C(0xbf58476d1ce4e5b9);
    z=(z^(z>>27))*UINT64_C(0x94d049bb133111eb);
    return z^(z>>31);
}
int16_t port_startup_random_wait_count(void)
{
    int fixture=port_test_random_wait_iterations();
    if(fixture>=0) return (int16_t)fixture;
    return (int16_t)(next_entropy()%12001u);
}
void port_startup_trace(void)
{
    char detail[256];
    snprintf(detail,sizeof detail,"seed=%016llx source=%s rate=%d detail=%d drawing=%s",
        (unsigned long long)s_seed,s_source,port_config_frame_rate(),
        port_config_detail_level(),port_config_dirty_rects()?"dirty_rects":"full");
    port_diagnostics_note("startup",detail);
    port_trace_startup(s_seed,s_source,port_config_frame_rate(),
                       port_config_detail_level(),port_config_dirty_rects());
}
