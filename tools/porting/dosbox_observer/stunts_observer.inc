// Stunts DOSBox-X observer; included by src/dosbox.cpp. GPL-2.0-or-later.
// Normal CPU/PIT/PIC execute unchanged. Only explicit startup controls write
// guest memory; all capture points come from locked-image bindings.
#include "vga.h"
#include <array>
#include <filesystem>
#include <iomanip>
#include <unordered_map>
#include <cctype>
#include <chrono>
extern void Mouse_CursorMoved(float,float,float,float,bool);
extern void Mouse_StuntsAcceptancePosition(uint16_t,uint16_t,uint8_t);
extern void KEYBOARD_AddBuffer(uint16_t);
static bool stunts_no_rasters=false;
static uint32_t key(uint16_t cs,uint16_t ip) {return (uint32_t(cs)<<16)|ip;}
static uint32_t physical(uint16_t cs,uint16_t ip) {return (uint32_t(cs)<<4)+ip;}
static void BoundarySimStart();
static void BoundarySimEnd();
static void BoundaryKeyConsumed();
static void BoundaryScan(uint8_t);
static unsigned BoundaryScope();
struct Hook { std::string name; uint32_t address; };
struct Pending {
    std::string name; uint32_t ret; uint16_t ss, sp;
    unsigned entry=0; uint64_t entered=0; std::vector<uint16_t> args; bool callback=false,screen=false;
    std::string filename;
    int callback_slot=-1;
    bool drew=false;
};
struct Capture {
    std::ofstream out, frames; std::vector<Hook> hooks;
    std::vector<Pending> pending;
    std::vector<uint8_t> filter=std::vector<uint8_t>(0x20000,0);
    uint64_t tick=0, sequence=0, frame=0; uint32_t timer, driver_pointer, state;
    std::chrono::steady_clock::time_point wall_start=std::chrono::steady_clock::now();
    uint16_t driver_segment=0, driver_offset=0;
    uint32_t busy,sprite1;
    uint16_t voice_near,chunk_near;
    std::filesystem::path asset_root;
    struct Resource { uint32_t base;std::vector<uint8_t> bytes;std::string name;uint64_t hash; };
    std::vector<Resource> resources;
    std::array<uint32_t,16> voice_resource{};
    std::array<int,16> voice_chunk;
    int callback_slot=-1;
    uint32_t playmode,replay_control,target_frame;
    uint32_t stunts_mouse_x=0,stunts_mouse_y=0,stunts_mouse_buttons=0;
    std::array<uint16_t,3> last_mouse{0xffff,0xffff,0xffff};
    std::vector<std::vector<uint8_t>> frame_payloads;
    std::unordered_map<uint64_t,std::vector<size_t>> frame_buckets;
    uint32_t key_return=0;
    uint16_t key_ss=0,key_sp=0;
    const char* driver_name;
    Capture(const char* path,const char* audio,const char* assets):
        out(std::string(path)+".jsonl"),frames(std::string(path)+".frames",std::ios::binary),
        asset_root(assets),driver_name(audio) { voice_chunk.fill(-1); }
    void site(uint32_t k) { auto p=physical(k>>16,k);filter[p>>3]|=1u<<(p&7); }
    uint8_t r8(uint32_t a) const { return mem_readb(a); }
    uint16_t r16(uint32_t a) const { return r8(a)|(uint16_t(r8(a+1))<<8); }
    void begin(const char* kind) {
        out<<"{\"kind\":\""<<kind<<"\",\"raw_tick\":"<<tick<<",\"sequence\":"<<sequence++;
        out<<",\"wall_us\":"<<std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-wall_start).count();
    }
    void update_driver() {
        driver_offset=r16(driver_pointer);driver_segment=r16(driver_pointer+2);
        if(driver_segment) for(unsigned e=0;e<=0x42;e+=3) site(key(driver_segment,driver_offset+e));
    }
    void snapshot(const std::string& reason) {
        if(stunts_no_rasters)return;
        std::vector<uint8_t> data(64768);
        // Peek host VRAM: guest aperture reads charge VGA wait states and can
        // change latches. DOSBox-X's compatible chain-4 map is grounded in
        // VGA_ChainedVGA_Handler::chain4remap (pinned vga_memory.cpp).
        if(vga.mode!=M_VGA || !vga.config.chained || !vga.config.compatible_chain4 || vga.svga.bank_read_full) {
            LOG_MSG("STUNTS unsupported video mapping mode=%u chain=%u compatible=%u bank=%u",
                unsigned(vga.mode),unsigned(vga.config.chained),unsigned(vga.config.compatible_chain4),unsigned(vga.svga.bank_read_full));
            abort();
        }
        for(unsigned i=0;i<64000;++i)data[i]=vga.mem.linear[(((i&~3u)<<2)+(i&3u))&vga.mem.memmask];
        for(unsigned i=0;i<768;++i)data[64000+i]=reinterpret_cast<const uint8_t*>(vga.dac.rgb)[i];
        uint64_t hash=14695981039346656037ull;for(auto b:data)hash=(hash^b)*1099511628211ull;
        auto& bucket=frame_buckets[hash];size_t payload=frame_payloads.size();
        for(auto candidate:bucket)if(frame_payloads[candidate]==data){payload=candidate;break;}
        if(payload==frame_payloads.size()) {
            if(payload>=4096)throw std::runtime_error("unique frame capture limit reached");
            frames.write(reinterpret_cast<const char*>(data.data()),data.size());
            frame_payloads.push_back(std::move(data));bucket.push_back(payload);
        }
        begin("frame");out<<",\"reason\":\""<<reason<<"\",\"ordinal\":"<<frame++<<",\"payload\":"<<payload
            <<",\"game_frame\":"<<r16(state+320)<<"}\n";
    }
    static int index(uint16_t off,uint16_t base,unsigned stride,unsigned capacity) {
        unsigned delta=uint16_t(off-base);
        return off && delta%stride==0 && delta/stride<capacity?int(delta/stride):-1;
    }
    void identity(uint32_t address) {
        if(!address){out<<"null";return;}
        for(auto it=resources.rbegin();it!=resources.rend();++it)if(address>=it->base && address-it->base<it->bytes.size()) {
            out<<"{\"name\":\""<<it->name<<"\",\"offset\":"<<address-it->base<<",\"fnv1a64\":\""
                <<std::hex<<std::setw(16)<<std::setfill('0')<<it->hash<<std::dec<<"\"}";return;
        }
        out<<"{\"resolved\":false}";
    }
    static bool requires_write(const std::string& name) {
        return name=="preRender_polygon" || name=="draw_filled_rect" || name=="preRender_sphere" ||
            name=="sprite_1_unk" || name=="putpixel_iconMask" || name=="putpixel_iconFillings";
    }
    void returned(const Pending& p) {
        if(p.name=="driver") {
            begin("driver");out<<",\"driver\":\""<<driver_name<<"\",\"entry\":"<<p.entry
              <<",\"game_frame\":"<<r16(state+320)<<",\"scope\":"<<BoundaryScope()
              <<",\"game_inputmode\":"<<unsigned(r8(state+1013))
              <<",\"entered_tick\":"<<p.entered<<",\"stack_u16\":[";
            for(size_t j=0;j<p.args.size();++j)out<<(j?",":"")<<p.args[j];
            int voice=-1,chunk=-1;uint32_t sample=0;
            if(p.entry==0)sample=physical(driver_segment,driver_offset);
            if((p.entry>=9 && p.entry<=0x15) || (p.entry>=0x21 && p.entry<=0x27))
                voice=index(p.args[1],voice_near,46,16);
            if(p.entry==9 || p.entry==0x21 || p.entry==0x27) {
                chunk=index(p.args[2],chunk_near,76,24);
                unsigned at=p.entry==9?5:3;sample=physical(p.args[at+1],p.args[at]);
            } else if(p.entry==0x1b)chunk=index(p.args[0],chunk_near,76,24);
            else if(p.entry==0x42)sample=physical(p.args[1],p.args[0]);
            if(voice>=0) {
                if(chunk>=0)voice_chunk[voice]=chunk;else chunk=voice_chunk[voice];
                if(sample)voice_resource[voice]=sample;else sample=voice_resource[voice];
            }
            out<<"],\"return_ax\":"<<reg_ax<<",\"source\":\""<<(p.callback?"callback":"foreground")
               <<"\",\"voice\":"<<voice<<",\"chunk\":"<<chunk<<",\"resource\":";
            identity(sample);out<<",\"callback_slot\":"<<p.callback_slot<<",\"payload_hex\":\"";
            if(p.entry==0x39 || p.entry==0x3f) {
                if(p.args[0]>4096)throw std::runtime_error("unbounded driver command payload");
                auto base=physical(p.args[2],p.args[1]);
                for(unsigned i=0;i<p.args[0];++i)out<<std::hex<<std::setw(2)<<std::setfill('0')<<unsigned(r8(base+i))<<std::dec;
            }
            out<<"\"}\n";
        } else if(p.name=="engine_init") {
            begin("engine_init");out<<",\"game_frame\":"<<r16(state+320)
                <<",\"game_inputmode\":"<<unsigned(r8(state+1013))<<",\"scope\":"<<BoundaryScope()
                <<",\"slot\":"<<reg_ax<<",\"values_u16\":[]}\n";
        } else if(p.name=="dashboard_complete") {
            if(p.args.at(0)==2)snapshot("race_frame_complete");
        } else if(p.name=="loadfilez") {
            auto path=asset_root / p.filename;
            if(std::filesystem::is_regular_file(path)) {
                std::ifstream f(path,std::ios::binary);std::vector<uint8_t> data((std::istreambuf_iterator<char>(f)),{});
                uint64_t hash=14695981039346656037ull;for(auto b:data)hash=(hash^b)*1099511628211ull;
                resources.push_back({physical(reg_dx,reg_ax),std::move(data),p.filename,hash});
            }
        } else if(p.name=="mousecursor") {
            std::array<uint16_t,3> now{r16(stunts_mouse_buttons),r16(stunts_mouse_x),r16(stunts_mouse_y)};
            if(now!=last_mouse) {
                last_mouse=now;begin("mouse_read");out<<",\"buttons\":"<<now[0]
                    <<",\"x_game\":"<<now[1]<<",\"y_game\":"<<now[2]<<"}\n";
            }
        } else if(p.name=="frame_callback") {
            begin("callback");out<<",\"ss\":"<<SegValue(ss)<<",\"ds\":"<<SegValue(ds)
                <<",\"playmode\":"<<r16(playmode)<<",\"replay_control\":"<<unsigned(r8(replay_control))
                <<",\"target_frame\":"<<r16(target_frame)<<",\"game_frame\":"<<r16(state+320)<<"}\n";
        } else if(p.name=="update_gamestate") {
            begin("state");out<<",\"bytes_hex\":\"";
            static const char hex[]="0123456789abcdef";
            for(unsigned i=0;i<1120;++i){auto v=r8(state+i);out<<hex[v>>4]<<hex[v&15];}
            out<<"\"}\n";
            BoundarySimEnd();
        } else if(p.name=="video_set_palette" || p.name=="doshowmouse" || p.name=="dohidemouse" || p.screen) {
            snapshot(p.name);
        }
        update_driver();
    }
    void completed_return() {
        // Observe completed bodies immediately before RETF/RETF n; no IRQ can
        // be admitted between this observation and the return instruction.
        auto opcode=r8(SegPhys(cs)+reg_ip);
        if(opcode!=0xcb && opcode!=0xca)return;
        for(size_t i=pending.size();i>0;--i) {
            const auto& p=pending[i-1];
            if(p.ss==SegValue(ss) && p.sp==reg_sp &&
               key(mem_readw(SegPhys(ss)+reg_sp+2),mem_readw(SegPhys(ss)+reg_sp))==p.ret) {
                auto done=p;pending.erase(pending.begin()+i-1);returned(done);break;
            }
        }
        if(key_return && SegValue(ss)==key_ss && uint16_t(reg_sp+6)==key_sp &&
           key(mem_readw(SegPhys(ss)+reg_sp+2),mem_readw(SegPhys(ss)+reg_sp))==key_return) {
            key_return=0;BoundaryKeyConsumed();begin("key");out<<",\"bios_word\":"<<reg_ax<<"}\n";
        }
    }
    void observe() {
        auto k=key(SegValue(cs),reg_ip);
        
        std::string name;
        for(const auto& h:hooks)if(h.address==k){name=h.name;break;}
        if(name=="callback_enter") {callback_slot=reg_di/4;return;}
        if(name=="callback_leave") {callback_slot=-1;return;}
        if(name=="purgememory") {
            auto a=physical(mem_readw(SegPhys(ss)+(reg_sp+6)),mem_readw(SegPhys(ss)+(reg_sp+4)));
            resources.erase(std::remove_if(resources.begin(),resources.end(),[&](const Resource& r){return r.base==a;}),resources.end());
            return;
        }
        if(name=="key_read_begin") {
            key_return=k+2;key_ss=SegValue(ss);key_sp=reg_sp;return;
        }
        unsigned entry=uint16_t(reg_ip-driver_offset);
        if(driver_segment && SegValue(cs)==driver_segment && entry<=0x42 && entry%3==0) name="driver";
        if(name=="driver_loaded") {update_driver();return;}
        if(name.empty())return;
        if(name=="engine_tick" || name=="engine_start" || name=="engine_target") {
            begin(name.c_str());out<<",\"game_frame\":"<<r16(state+320)
                <<",\"game_inputmode\":"<<unsigned(r8(state+1013))<<",\"scope\":"<<BoundaryScope()<<",\"values_u16\":[";
            unsigned count=name=="engine_start"?1:name=="engine_target"?9:0;
            for(unsigned i=0;i<count;++i)out<<(i?",":"")<<mem_readw(SegPhys(ss)+reg_sp+4+2*i);
            out<<"]}\n";return;
        }
        if(name=="update_gamestate")BoundarySimStart();
        Pending p{name,key(mem_readw(SegPhys(ss)+(reg_sp+2)),mem_readw(SegPhys(ss)+(reg_sp))),
                  SegValue(ss),reg_sp,entry,tick,{},r8(busy)!=0,r16(sprite1+2)==0xa000,{}};
        p.callback_slot=callback_slot;
        // Shared tail bodies can pass another named entry with the same FAR
        // frame. It remains one invocation/publication, not a second call.
        for(const auto& old:pending)if(old.ret==p.ret && old.sp==p.sp && old.ss==p.ss && old.name==p.name)return;
        if(name=="loadfilez") {
            auto a=physical(SegValue(ds),mem_readw(SegPhys(ss)+(reg_sp+4)));
            for(unsigned i=0;i<128 && r8(a+i);++i)p.filename+=char(r8(a+i));
            auto slash=p.filename.find_last_of("\\/:");if(slash!=std::string::npos)p.filename.erase(0,slash+1);
            for(auto& c:p.filename)c=char(std::toupper(static_cast<unsigned char>(c)));
        }
        if(name=="driver") {
            // C far-call stack spans grounded in the typed original call sites.
            // Medium model: voice/chunk/table are NEAR data pointers; sample
            // and command payload are explicitly FAR in the original calls.
            static const unsigned counts[]={0,0,0,7,2,2,3,4,0,3,1,5,3,5,0,0,1,0,1,3,0,3,2};
            for(unsigned i=0;i<counts[entry/3];++i)p.args.push_back(mem_readw(SegPhys(ss)+(reg_sp+4+2*i)));
        }
        if(name=="dashboard_complete")p.args.push_back(mem_readw(SegPhys(ss)+reg_sp+4));
        pending.push_back(p);
    }
};

static bool stunts_attached=false;
static Capture* stunts=nullptr;
static uint32_t stunts_base=0,stunts_limit=0;
static uint64_t stunts_instructions=0;
static std::vector<Hook> stunts_hooks;
struct StuntsInput {unsigned tick;int key;float u,v;unsigned buttons;};
static std::vector<StuntsInput> stunts_inputs;
static size_t stunts_next=0;
static unsigned stunts_buttons=0,stunts_seed_calls=0;
static bool stunts_seed_patched=false;
static std::array<uint8_t,16> stunts_entry;
static uint16_t stunts_entry_cs,stunts_entry_ip;
#include "stunts_boundary.inc"
static void StuntsInit() {
    const char* path=getenv("STUNTS_OBSERVER");if(!path)return;
    FILE* f=fopen(path,"r");if(!f)abort();
    char line[1024],name[128],prefix[512],audio[16],assets[512];
    while(fgets(line,sizeof(line),f)) {
        unsigned a,b,c;float u,v;
        if(BoundaryConfigure(line))continue;
        if(strncmp(line,"NO_RASTERS",10)==0){stunts_no_rasters=true;continue;}
        if(sscanf(line,"OUTPUT %511[^\n]",prefix)==1)continue;
        if(sscanf(line,"ASSETS %511[^\n]",assets)==1)continue;
        if(sscanf(line,"AUDIO %15s",audio)==1)continue;
        if(sscanf(line,"LIMIT %u",&stunts_limit)==1)continue;
        if(sscanf(line,"ENTRY %x %x",&a,&b)==2) {stunts_entry_cs=a;stunts_entry_ip=b;continue;}
        if(sscanf(line,"BYTE %u %x",&a,&b)==2) {stunts_entry[a]=b;continue;}
        if(sscanf(line,"HOOK %x %127s",&a,name)==2) {stunts_hooks.push_back({name,a});continue;}
        if(sscanf(line,"KEY %u %u",&a,&b)==2) {stunts_inputs.push_back({a,int(b),0,0,0});continue;}
        if(sscanf(line,"MOUSE %u %f %f %u",&a,&u,&v,&c)==4) {stunts_inputs.push_back({a,-1,u,v,c});continue;}
    }
    fclose(f);stunts=new Capture(prefix,audio,assets);stunts_enabled=true;
    // Driver name must outlive this initialization stack.
    stunts->driver_name=std::string(audio)=="ad15"?"ad15":"pc15";
    DOSBOX_AcceptanceExecArmed=true;
    DOSBOX_AcceptanceIOWatchArmed=true;
    stunts->out<<"{\"kind\":\"header\",\"schema\":\"stunts-dosbox-ticks-v1\","
        "\"tick_source\":\"original-INT8-entry\",\"mouse_present\":true}\n";
}
static KBD_KEYS StuntsKey(unsigned scan) {
    switch(scan&127) {
    case 1:return KBD_esc;case 28:return KBD_enter;case 57:return KBD_space;
    case 72:return KBD_kp8;case 75:return KBD_kp4;case 77:return KBD_kp6;case 80:return KBD_kp2;
    case 25:return KBD_p;case 19:return KBD_r;case 18:return KBD_e;case 47:return KBD_v;
    case 23:return KBD_i;case 17:return KBD_w;case 49:return KBD_n;case 34:return KBD_g;
    default:LOG_MSG("STUNTS unsupported scan %u",scan);abort();
    }
}
static void StuntsDeliver() {
    while(stunts_next<stunts_inputs.size() && stunts_inputs[stunts_next].tick<=stunts->tick) {
        const auto& e=stunts_inputs[stunts_next++];
        // The fixture channel contains raw Set-1 bytes, including its own
        // repeats. AddKey would synthesize typematic bytes absent from it.
        if(e.key>=0)KEYBOARD_AddBuffer(uint16_t(e.key));
        else {
            // Same truncation and inclusive logical 320x200 input extent as
            // port/input_script.c. The DOS mouse driver enforces live bounds.
            auto shift=mem_readw(stunts_base+197402);
            Mouse_StuntsAcceptancePosition(uint16_t(e.u*320)<<shift,uint16_t(e.v*200),uint8_t(e.buttons));
            stunts_buttons=e.buttons;
        }
        stunts->begin("input");stunts->out<<",\"input_sequence\":"<<stunts_next<<"}\n";
    }
}
static void StuntsInstruction() {
    if(!stunts_enabled)return;
    ++stunts_instructions;
    auto pc=SegPhys(cs)+reg_ip;
    if(!stunts_attached) {
        if(reg_ip!=stunts_entry_ip || SegValue(cs)<stunts_entry_cs)return;
        // First ten entry bytes contain no MZ fixup; byte eleven begins a
        // relocated DGROUP immediate and cannot be compared before relocation.
        for(unsigned i=0;i<10;++i)if(mem_readb(pc+i)!=stunts_entry[i])return;
        stunts_base=(SegValue(cs)-stunts_entry_cs)*16;stunts_attached=true;
        for(auto h:stunts_hooks) {
            auto cs=uint16_t((h.address>>16)-0x1010+stunts_base/16);
            h.address=key(cs,uint16_t(h.address));stunts->hooks.push_back(h);stunts->site(h.address);
        }
        stunts->timer=key(stunts_base/16+0x1ea2,0x1909);
        stunts->driver_pointer=stunts_base+0x3060a;
        stunts->state=stunts_base+0x34494;stunts->busy=stunts_base+0x2f88c;
        stunts->sprite1=stunts_base+149824;
        stunts->voice_near=219686-0x2b770;stunts->chunk_near=211308-0x2b770;
        stunts->playmode=stunts_base+220594;stunts->replay_control=stunts_base+215526;
        stunts->target_frame=stunts_base+208130;
        stunts->stunts_mouse_x=stunts_base+221548;
        stunts->stunts_mouse_y=stunts_base+221646;
        stunts->stunts_mouse_buttons=stunts_base+213736;
        stunts->begin("attached");stunts->out<<",\"load_segment\":"<<stunts_base/16<<"}\n";
        BoundaryAttach();
    }
    uint32_t relative=pc-stunts_base;
    if(relative==0x20329) {
        ++stunts->tick;stunts->sequence=0;
        if(stunts->tick>stunts_limit) {
            stunts->begin("end");stunts->out<<",\"terminal\":\"tick-limit\",\"instructions\":"<<stunts_instructions<<"}\n";
            stunts->out.flush();stunts->frames.flush();DOSBOX_AcceptanceExit();return;
        }
        stunts->begin("timer");stunts->out<<",\"pic_ms\":"<<std::setprecision(15)
            <<double(PIC_FullIndex())<<"}\n"; // diagnostic; identity is IRQ count
        StuntsDeliver();
    }
    if(relative==0x2a22b) { // initmodes benchmark decision join, IP 041b
        mem_writew(stunts_base+216398,20);mem_writeb(stunts_base+178426,0);
        mem_writew(stunts_base+216298,1);
        stunts->begin("control");stunts->out<<",\"boundary\":\"benchmark-settings\"}\n";
    }
    if(relative==0x2a25c) {++stunts_seed_calls;stunts_seed_patched=false;} // seedrandom entry
    if(relative==0x2a29a && !stunts_seed_patched) {
        reg_di=stunts_seed_calls==1?12000:8014;stunts_seed_patched=true;
    } // stir join, IP 048a; retain the original decrementing PRNG loops
    if(relative==0x381 && stunts_seed_calls==2) {
        mem_writed(stunts_base+0x2b770+0x3930,0x0b84d1e4);
        const uint8_t seeds[]={0x9d,0x08,0xea,0x4b,0xfc,0x4d};
        for(unsigned i=0;i<6;++i)mem_writeb(stunts_base+219466+i,seeds[i]);
        mem_writed(stunts_base+194680,592);mem_writed(stunts_base+198138,592);
        stunts->begin("control");stunts->out<<",\"boundary\":\"seed-fixture\",\"timer_counter\":"
            <<mem_readd(stunts_base+194680)<<",\"crt_rand_state\":\""<<std::hex<<mem_readd(stunts_base+0x2b770+0x3930)
            <<"\",\"kevin_random_state\":\"";
        for(unsigned i=0;i<6;++i)stunts->out<<std::setw(2)<<std::setfill('0')<<unsigned(mem_readb(stunts_base+219466+i));
        stunts->out<<std::dec<<"\"}\n";
    }
    stunts->completed_return();
    BoundaryBefore(pc);
    auto k=key(SegValue(cs),reg_ip);
    if(stunts->filter[pc>>3]&(1u<<(pc&7)))stunts->observe();
}
static void StuntsIO(uint32_t port,uint32_t value,bool output) {
    if(stunts_attached && !output && port==0x60 && DOSBOX_AcceptanceCpuInstructionActive) {
        BoundaryScan(uint8_t(value));
        stunts->begin("scan");stunts->out<<",\"scancode\":"<<value<<"}\n";
    }
}
