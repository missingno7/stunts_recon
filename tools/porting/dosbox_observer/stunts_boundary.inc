// Bounded original call observations and semantic input. No game code patching.
struct BoundaryRegionSpec {std::string name;int argument;unsigned size,read,load_offset;};
struct BoundarySpec {std::string name;uint32_t cookie;unsigned argc,bits,mask;std::vector<BoundaryRegionSpec> regions;};
struct BoundaryRegion {BoundaryRegionSpec spec;uint32_t address;std::vector<uint8_t> before;};
struct BoundaryPending {const BoundarySpec* spec;uint16_t ss,sp;uint32_t ret,frame,scope,active;uint64_t order;std::vector<uint16_t> args;std::vector<BoundaryRegion> regions;};
struct SemanticInput {unsigned phase,at;int code;float u,v;unsigned buttons;bool consumed=false;};
static std::vector<BoundarySpec> boundary_specs;
static std::vector<BoundaryPending> boundary_pending;
static std::vector<std::pair<uint32_t,unsigned>> boundary_phases;
static std::vector<uint32_t> boundary_polls;
static std::vector<SemanticInput> boundary_inputs;
static std::vector<uint8_t> boundary_keyboard;
static uint32_t boundary_input_site=0;
static unsigned boundary_phase=0,boundary_key=0,boundary_scope=0,boundary_frame=0,boundary_limit=0;
static unsigned BoundaryScope(){return boundary_scope;}
static uint64_t boundary_order=0;
static std::ofstream boundary_output;
static std::array<uint8_t,0x20000> boundary_filter{};
struct BoundaryService {uint16_t ss,sp,selector;uint32_t ret,frame,scope;unsigned interrupt;uint64_t order;uint16_t cx,dx;};
static std::vector<BoundaryService> boundary_services;
static uint32_t boundary_origin=0;
static uint16_t boundary_ss=0,boundary_sp=0;
static uint32_t BoundaryPC(uint32_t cookie) {
    return physical(uint16_t((cookie>>16)-0x1010+stunts_base/16),uint16_t(cookie));
}
static std::vector<uint8_t> BoundaryRead(uint32_t address,unsigned size) {
    std::vector<uint8_t> bytes(size);
    for(unsigned i=0;i<size;++i)bytes[i]=mem_readb(address+i);
    return bytes;
}
static void BoundaryHex(const std::vector<uint8_t>& bytes) {
    boundary_output<<'"';for(auto b:bytes)boundary_output<<std::hex<<std::setw(2)<<std::setfill('0')<<unsigned(b);
    boundary_output<<std::dec<<'"';
}
static bool BoundaryConfigure(const char* line) {
    char name[100],label[100],path[512];unsigned pc,n,bits,mask,size,read,offset,phase,at,buttons;int argument,code;float u,v;
    if(sscanf(line,"BOUNDARY_OUTPUT %511[^\n]",path)==1){
        boundary_output.open(path);if(!boundary_output)abort();
        boundary_output<<"{\"kind\":\"header\",\"schema\":\"stunts-port-boundary-v1\"}\n";return true;}
    if(sscanf(line,"BC %99s %x %u %u %u",name,&pc,&n,&bits,&mask)==5){
        boundary_specs.push_back({name,pc,n,bits,mask,{}});return true;}
    if(sscanf(line,"BR %99s %99s %d %u %u %x",name,label,&argument,&size,&read,&offset)==6){
        if(size>514)abort();
        for(auto& spec:boundary_specs)if(spec.name==name){spec.regions.push_back({label,argument,size,read,offset});return true;}
        abort();}
    if(sscanf(line,"PHASE %x %u",&pc,&phase)==2){boundary_phases.push_back({pc,phase});return true;}
    if(sscanf(line,"POLL %x",&pc)==1){boundary_polls.push_back(pc);return true;}
    if(sscanf(line,"INPUTSITE %x",&boundary_input_site)==1)return true;
    if(sscanf(line,"FRAME_LIMIT %u",&boundary_limit)==1)return true;
    if(sscanf(line,"SKEY %u %u %d",&phase,&at,&code)==3){boundary_inputs.push_back({phase,at,code,0,0,0});return true;}
    if(sscanf(line,"SMOUSE %u %u %f %f %u",&phase,&at,&u,&v,&buttons)==5){boundary_inputs.push_back({phase,at,-1,u,v,buttons});return true;}
    return false;
}
static void BoundaryAttach() {
    auto mark=[](uint32_t pc){boundary_filter[pc>>3]|=1u<<(pc&7);};
    for(const auto& spec:boundary_specs)mark(BoundaryPC(spec.cookie));
    for(const auto& phase:boundary_phases)mark(BoundaryPC(phase.first));
    for(auto pc:boundary_polls)mark(BoundaryPC(pc));
    if(boundary_input_site)mark(BoundaryPC(boundary_input_site));
}
static void BoundaryFinishRun() {
    stunts->begin("end");stunts->out<<",\"terminal\":\"frame-limit\",\"instructions\":"<<stunts_instructions<<"}\n";
    stunts->out.flush();stunts->frames.flush();boundary_output.flush();DOSBOX_AcceptanceExit();
}
static void BoundarySimStart() {
    boundary_frame=stunts->r16(stunts->state+320);boundary_scope=1;
    if(boundary_limit && boundary_frame>=boundary_limit && mem_readb(stunts->state+1013))BoundaryFinishRun();
}
static void BoundarySimEnd() {boundary_scope=2;boundary_frame=stunts->r16(stunts->state+320);}
static void BoundaryKeyConsumed() {++boundary_key;}
static void BoundaryScan(uint8_t value) {
    if(boundary_keyboard.empty())return;
    if(value!=boundary_keyboard.front()){LOG_MSG("STUNTS unexpected keyboard byte in semantic transaction");abort();}
    boundary_keyboard.erase(boundary_keyboard.begin());
}
static void BoundaryDeliver(unsigned phase,unsigned at) {
    bool delivered=false;
    for(auto& e:boundary_inputs) {
        if(e.consumed || e.phase!=phase || e.at!=at)continue;
        e.consumed=true;delivered=true;
        if(e.code>=0){KEYBOARD_AddBuffer(uint16_t(e.code));boundary_keyboard.push_back(uint8_t(e.code));}
        else {
            auto shift=mem_readw(stunts_base+197402);
            Mouse_StuntsAcceptancePosition(uint16_t(e.u*320)<<shift,uint16_t(e.v*200),uint8_t(e.buttons));
        }
    }
    if(delivered){
        stunts->begin("semantic_input");stunts->out<<",\"phase\":"<<phase<<",\"at\":"<<at<<"}\n";
        boundary_origin=SegPhys(cs)+reg_ip;boundary_ss=SegValue(ss);boundary_sp=reg_sp;
    }
}
bool StuntsBoundaryYield() {
    // Let the real PIC and original INT9 deliver all raw bytes, and return to
    // the initiating call frame before allowing that input read to execute.
    return !boundary_keyboard.empty() && SegPhys(cs)+reg_ip==boundary_origin &&
        SegValue(ss)==boundary_ss && reg_sp==boundary_sp;
}
static void BoundaryComplete() {
    if(boundary_pending.empty())return;
    unsigned op=mem_readb(SegPhys(cs)+reg_ip);if(op!=0xcb && op!=0xca)return;
    const auto& p=boundary_pending.back();
    if(p.ss!=SegValue(ss) || p.sp!=reg_sp ||
       key(mem_readw(SegPhys(ss)+reg_sp+2),mem_readw(SegPhys(ss)+reg_sp))!=p.ret)return;
    boundary_output<<"{\"kind\":\"boundary\",\"name\":\""<<p.spec->name<<"\",\"scope\":"<<p.scope
        <<",\"frame\":"<<p.frame<<",\"active\":"<<p.active<<",\"order\":"<<p.order<<",\"args\":[";
    for(unsigned i=0;i<p.args.size();++i){if(i)boundary_output<<',';
        if(p.spec->mask&(1u<<i))boundary_output<<"null";else boundary_output<<p.args[i];}
    boundary_output<<"],\"result\":";
    if(!p.spec->bits)boundary_output<<"null";else boundary_output<<reg_ax;
    boundary_output<<",\"regions\":[";
    for(unsigned i=0;i<p.regions.size();++i){const auto& r=p.regions[i];int alias=-1,offset=0;
        for(unsigned j=0;j<i;++j){auto b=p.regions[j].address;
            if(r.address<b+p.regions[j].spec.size && b<r.address+r.spec.size){alias=int(j);offset=int(r.address)-int(b);break;}}
        if(i)boundary_output<<',';
        boundary_output<<"{\"name\":\""<<r.spec.name<<"\",\"read\":"<<r.spec.read<<",\"alias\":"<<alias
            <<",\"offset\":"<<offset<<",\"before\":";
        BoundaryHex(r.before);boundary_output<<",\"after\":";BoundaryHex(BoundaryRead(r.address,r.spec.size));boundary_output<<'}';
    }
    boundary_output<<"]}\n";boundary_pending.pop_back();
}
static void BoundaryBefore(uint32_t pc) {
    if(boundary_output && !boundary_services.empty()) {
        const auto& p=boundary_services.back();auto op=mem_readb(pc);
        if((op==0xcf || op==0xca) && p.ss==SegValue(ss) && uint16_t(reg_sp+6)==p.sp &&
           key(mem_readw(SegPhys(ss)+reg_sp+2),mem_readw(SegPhys(ss)+reg_sp))==p.ret) {
            uint16_t response[3]={0,0,0};
            if(p.interrupt==0x33 && p.selector==3){response[0]=reg_bx;response[1]=reg_cx;response[2]=reg_dx;}
            else if(p.selector==0 || p.interrupt==0x15){response[0]=reg_ax;response[1]=reg_bx;}
            boundary_output<<"{\"kind\":\"boundary\",\"name\":\"int"<<(p.interrupt==0x33?"33":"15")
                <<"\",\"scope\":"<<p.scope<<",\"frame\":"<<p.frame<<",\"order\":"<<p.order
                <<",\"args\":["<<p.selector<<','<<p.cx<<','<<p.dx<<"],\"result\":null,\"regions\":["
                "{\"name\":\"register_result\",\"read\":0,\"alias\":-1,\"offset\":0,\"before\":\"000000000000\",\"after\":";
            std::vector<uint8_t> bytes;for(auto w:response){bytes.push_back(w&255);bytes.push_back(w>>8);}BoundaryHex(bytes);
            boundary_output<<"}]}\n";boundary_services.pop_back();
        }
    }
    if(boundary_output && boundary_pending.empty() && stunts->callback_slot<0 && mem_readb(pc)==0xcd) {
        auto interrupt=mem_readb(pc+1);
        if(interrupt==0x33 || (interrupt==0x15 && reg_ax==0xc201)) {
            bool duplicate=false;for(const auto& p:boundary_services)if(p.ret==key(SegValue(cs),uint16_t(reg_ip+2)) && p.sp==reg_sp && p.ss==SegValue(ss))duplicate=true;
            if(!duplicate){uint16_t cx=0,dx=0;
                if(interrupt==0x33 && (reg_ax==4 || reg_ax==7 || reg_ax==8 || reg_ax==15)){cx=reg_cx;dx=reg_dx;}
                boundary_services.push_back({SegValue(ss),reg_sp,reg_ax,key(SegValue(cs),uint16_t(reg_ip+2)),
                    boundary_scope?boundary_frame:unsigned(stunts->r16(stunts->state+320)),boundary_scope,
                    unsigned(interrupt),boundary_order++,cx,dx});
            }
        }
    }
    BoundaryComplete();
    if(!(boundary_filter[pc>>3]&(1u<<(pc&7))))return;
    for(const auto& p:boundary_phases)if(pc==BoundaryPC(p.first) && boundary_phase!=p.second){boundary_phase=p.second;boundary_key=0;}
    for(auto p:boundary_polls)if(pc==BoundaryPC(p))BoundaryDeliver(boundary_phase,boundary_key);
    if(boundary_input_site && pc==BoundaryPC(boundary_input_site))BoundaryDeliver(6,stunts->r16(stunts->target_frame));
    if(!boundary_output || !boundary_pending.empty() || stunts->callback_slot>=0)return;
    for(const auto& spec:boundary_specs)if(pc==BoundaryPC(spec.cookie)) {
        BoundaryPending p{&spec,SegValue(ss),reg_sp,key(mem_readw(SegPhys(ss)+reg_sp+2),mem_readw(SegPhys(ss)+reg_sp)),
            boundary_scope?boundary_frame:unsigned(stunts->r16(stunts->state+320)),boundary_scope,
            unsigned(mem_readb(stunts->state+1013)),boundary_order++,{}, {}};
        for(unsigned i=0;i<spec.argc;++i)p.args.push_back(mem_readw(SegPhys(ss)+reg_sp+4+2*i));
        for(const auto& r:spec.regions){
            uint32_t address=r.argument<0?stunts_base+r.load_offset:physical(SegValue(ds),p.args.at(r.argument));
            if(r.argument>=0 && unsigned(p.args.at(r.argument))+r.size>65536)abort();
            p.regions.push_back({r,address,BoundaryRead(address,r.size)});
        }
        boundary_pending.push_back(std::move(p));break;
    }
}
