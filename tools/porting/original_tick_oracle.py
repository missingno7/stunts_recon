"""Capture the locked original MCGA image in patched DOSBox-X; compare the manual port."""
from __future__ import annotations
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
ROOT=Path(__file__).resolve().parents[2]
PERIOD=10_000_154
def digest(path):return hashlib.sha256(Path(path).read_bytes()).hexdigest()

def input_ticks(script):
    """Same ceiling and stable equal-tick order as port/input_script.c."""
    if script.get("format") not in ("stunts-exact-input-script-v1","portforge-exact-input-script-v1"):
        raise ValueError("expected stunts-exact-input-script-v1")
    anchor = script.get("anchor_tick", 0)
    if anchor != 0:
        raise ValueError("only session-relative anchor_tick=0 is supported")
    events = []
    for i, event in enumerate(script["events"]):
        stamp = event["visible_tick"]
        if type(stamp) is not int or stamp < 0:
            raise ValueError("visible_tick must be a nonnegative integer nanosecond stamp")
        if event["channel"] not in ("dos.keyboard.scancodes", "dos.mouse.normalized"):
            raise ValueError("unsupported input channel")
        events.append(dict(event, raw_tick=max(1, (stamp + PERIOD - 1) // PERIOD), sequence=i))
    return {"events": sorted(events, key=lambda e: (e["raw_tick"], e["sequence"]))}


def bindings():
    sys.path[:0] = [str(ROOT / "tools"), str(ROOT / "tools/porting"), str(ROOT / "build/python")]
    from diffharness.oracle import OracleImage, SymbolMap
    image, symbols = OracleImage.load(ROOT / "assets"), SymbolMap()
    data = json.loads((ROOT / "layout/data-symbols.json").read_text(encoding="utf-8"))["symbols"]
    hooks = []
    aliases={"gameint":"frame_callback", "movcf":"sprite_blit_to_video",
        "movpackf":"shape2d_op_unk", "movpackfxya":"shape2d_op_unknown5",
        "movpackcfxya":"shape2d_op_unk3", "xorrectc":"draw_filled_rect",
        "drawpolypt":"preRender_polygon", "drawpolyp":"preRender_polygon",
        "preRender_patterned":"preRender_polygon", "sprite_two_color_poly_list":"preRender_polygon",
        "drawrect":"sprite_1_unk", "orfxya":"putpixel_iconFillings", "andfxya":"putpixel_iconMask",
        "orpackf":"shape2d_op_unk4", "andpackf":"shape2d_render_bmp_as_mask", "scalecfxy":"shapeexpl"}
    names=("movcf", "video_set_palette", "sprite_1_unk3", "shape2d_op_unk3",
           "preRender_line", "preRender_sphere", "update_gamestate", "gameint", "loadfilez", "purgememory",
           *[n for n in aliases if n not in ("gameint","movcf")], "doshowmouse", "dohidemouse",
           "clear_rect", "tmaskxcfxya", "tmaskxf", "mousecursor")
    import struct
    for name in names:
        witnesses=[]
        try:
            routine=symbols.resolve(name)
            start=routine.start
            cs,ip=routine.far_at(0x1010)  # PSP=1000, original MZ image=1010
        except KeyError:
            # Two C cursor helpers have registry entries but no decomp symbol
            # row. Ground their frame in relocated ORIGINAL direct FAR calls.
            matches=[int(a) for a,r in symbols.names.items() if r.get("name")==name]
            if len(matches)!=1:raise ValueError(f"ambiguous registry binding: {name}")
            start=matches[0];frames=set()
            for reloc in image.relocations:
                at=reloc["load_offset"]
                if at>=3 and image.load_image[at-3]==0x9a:
                    off,seg=struct.unpack_from("<HH",image.load_image,at-2)
                    if seg*16+off==start:
                        frames.add(seg);witnesses.append(at-3)
            # MSC also synthesizes FAR calls within one code frame with
            # PUSH CS / CALL rel16. Bind CS from the independently mapped
            # caller's frame, never from the helper's guessed neighborhood.
            for at in range(len(image.load_image)-4):
                if image.load_image[at:at+2]!=b"\x0e\xe8":continue
                if at+4+struct.unpack_from("<h",image.load_image,at+2)[0]!=start:continue
                for row in symbols.code.values():
                    target=row.get("mapped_target",{})
                    if target.get("start",1<<30)<=at<target.get("end",0):
                        frames.add(row["frame_load_address"]//16);witnesses.append(at)
            if len(frames)!=1:raise ValueError(f"no unique original FAR-call frame for {name}")
            frame=frames.pop();cs=0x1010+frame;ip=start-frame*16
        hooks.append(dict(name=aliases.get(name,name), source_name=name,address=(cs<<16)|ip,
                          original_load_offset=start,far_call_witnesses=witnesses,
                          entry_bytes=image.load_image[start:start+16].hex()))
    # Accepted timerint.ASM: IVT[8] is installed as CS:1909; first byte CLI.
    # Original audio_load_driver stores the far pointer then LCALL DS:[4e9a].
    anchors = ((0x20329, "fa"), (0x2052e, "509a"), (0x27988, "ff1e9a4e"))
    for offset, hex_bytes in anchors:
        expected = bytes.fromhex(hex_bytes)
        if image.load_image[offset:offset+len(expected)] != expected:
            raise ValueError(f"locked instruction anchor mismatch at {offset:#x}")
    hooks.extend((dict(name="key_read_begin", address=0x2eb21b0c),
                  dict(name="key_read_begin", address=0x2eb2200c),
                  dict(name="callback_enter", address=0x2eb21969),
                  dict(name="callback_leave", address=0x2eb2196d),
                  dict(name="driver_loaded", address=0x371d08b8)))
    controls = {name: 0x10100+data["_"+name]["load_address"] for name in
                ("frm_rate2", "renderlevel", "slow_video_mode_state", "randomseeds",
                 "timer_callback_counter", "last_timer_callback_counter", "timer_callback_busy",
                 "gm_playmode", "g_rplmodui", "tmr2")}
    return dict(timer=0x2eb21909, driver_pointer=0x10100 + data["_audiodriverbinary"]["load_address"],
                state=0x10100 + data["_state"]["load_address"], hooks=hooks, controls=controls,
                # CRT rand's accepted absolute DS:[3930]/[3932] state.
                crt_random=0x3b870+0x3930, sprite1=0x10100+data["_sprite1"]["load_address"],
                voice_near=data["_snd_voices_tbl"]["load_address"]-0x2b770,
                chunk_near=data["_audiochunktable"]["load_address"]-0x2b770)


def run_port(executable, out, script, ticks, audio, *, compare_original=True):
    """Same inputs/settings, manual clock, isolated config/assets/save paths."""
    folder=out / "port"
    folder.mkdir()
    shutil.copytree(ROOT / "build/sdl3/runtime/assets",folder / "assets")
    (folder / "config.json").write_text(json.dumps(dict(manual_word_check=True,
        renderer="faithful", frame_rate=20, detail_level=0, drawing_path="dirty_rects")),encoding="utf-8")
    env=os.environ.copy()
    env.update(SDL_VIDEODRIVER="dummy",SDL_AUDIODRIVER="dummy")
    env["PATH"]=os.pathsep.join(("C:/msys64/mingw32/bin","C:/tools/sdl3-3.4.16-i686/bin",env.get("PATH","")))
    command=[str(executable.resolve()),f"--assets={folder / 'assets'}", f"--config={folder / 'config.json'}",
        f"--diagnostics-dir={folder / 'diagnostics'}", f"--trace={folder / 'trace.jsonl'}",
        f"--sound-requests={folder / 'sound.jsonl'}", f"--input-script={script.resolve()}",
        f"--test-startup-seed={ROOT / 'tests/fixtures/sdl3/startup-seed.json'}", "--seed=1",
        "--clock=manual", f"--run-ticks={ticks}", f"--audio={audio}", "--audio-output=none"]
    result=subprocess.run(command,cwd=folder,env=env,capture_output=True,text=True,timeout=90)
    (folder / "run.log").write_text(result.stdout+result.stderr,encoding="utf-8")
    if result.returncode:
        raise RuntimeError(f"port exited {result.returncode}; see {folder / 'run.log'}")
    sys.path.insert(0,str(ROOT))
    from tools.porting.compare_tick_streams import original_streams,port_streams,compare
    actual=port_streams(folder / "trace.jsonl",folder / "sound.jsonl",ticks+1)
    if not compare_original:
        return actual
    original=original_streams(out / "original")
    report=compare(original,actual)
    (out / "comparison.json").write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")
    print(json.dumps({k:dict(agrees=v['agrees'],original=v['original_count'],port=v['port_count']) for k,v in report.items()}))
    return report


def capture(args):
    """Run ORIGINAL bytes, never a port-derived executable or expectation."""
    import struct
    out=args.output.resolve()
    if out.exists():raise ValueError(f'output already exists: {out}')
    hooks=bindings()
    import oracle
    _,unpacked,_,_=oracle.verify(write=False,asset_dir=ROOT/'assets')
    script=input_ticks(json.loads(args.script.read_text(encoding='utf-8')))
    out.mkdir(parents=True)
    shutil.copytree(ROOT/'assets',out/'assets')
    (out/'assets/ORIGINAL.EXE').write_bytes(unpacked)
    (out/'original.exe').write_bytes(unpacked)
    (out/'bindings.json').write_text(json.dumps(hooks,indent=2)+'\n',encoding='utf-8')
    # Entry signature is read from the locked MZ image, before DOS relocation.
    header=struct.unpack_from('<14H',unpacked)
    entry_ip,entry_cs=header[10],header[11]
    at=header[4]*16+entry_cs*16+entry_ip
    lines=[f'OUTPUT {out / "original"}',f'ASSETS {out / "assets"}',f'AUDIO {args.audio}',
           f'LIMIT {args.ticks}',f'ENTRY {entry_cs:x} {entry_ip:x}']
    lines += [f'BYTE {i} {b:x}' for i,b in enumerate(unpacked[at:at+16])]
    lines += [f'HOOK {h["address"]:x} {h["name"]}' for h in hooks['hooks']]
    for event in script['events']:
        tick=event['raw_tick'];payload=event['payload']
        if event['channel']=='dos.keyboard.scancodes':
            lines += [f'KEY {tick} {s}' for s in payload]
        else:lines.append(f'MOUSE {tick} {payload["u"]:.12g} {payload["v"]:.12g} {payload["buttons"]}')
    (out/'observer.txt').write_text('\n'.join(lines)+'\n')
    (out/'safety.txt').write_text('60000 exit\n')
    config=f'''[sdl]
fullscreen=false
output=surface
[dosbox]
machine=svga_s3
memsize=16
acceptance script={out / 'safety.txt'}
acceptance log={out / 'acceptance.log'}
[cpu]
core=normal
cycles=fixed {args.cycles}
turbo=true
[mixer]
nosound=true
[sblaster]
sbtype=sb16
[dos]
keyboardlayout=us
[autoexec]
mount c "{out / 'assets'}"
c:
ORIGINAL.EXE /s{'ad' if args.audio=='ad15' else 'pc'}
exit
'''
    (out/'dosbox.conf').write_text(config)
    env=os.environ.copy();env.update(SDL_VIDEODRIVER='dummy',SDL_AUDIODRIVER='dummy',STUNTS_OBSERVER=str(out/'observer.txt'))
    command=[str(args.dosbox.resolve()),'-conf',str(out/'dosbox.conf'),'-fastlaunch','-exit','-nomenu']
    import time
    started=time.perf_counter()
    with (out/'run.log').open('w') as log:
        result=subprocess.run(command,cwd=out,env=env,stdout=log,stderr=subprocess.STDOUT,
            timeout=args.timeout,creationflags=getattr(subprocess,'CREATE_NO_WINDOW',0))
    receipt=dict(schema='stunts-dosbox-tick-receipt-v1',returncode=result.returncode,
        seconds=time.perf_counter()-started,audio=args.audio,requested_ticks=args.ticks,cycles=args.cycles,
        executable_sha256=hashlib.sha256(unpacked).hexdigest(),dosbox_sha256=digest(args.dosbox),
        oracle_lock_sha256=digest(ROOT/'layout/oracle.lock.json'),script_sha256=digest(args.script),
        driver_sha256=digest(ROOT/f'assets/{args.audio.upper()}.DRV'),
        observer_sha256=digest(ROOT/'tools/porting/dosbox_observer/stunts_observer.inc'),
        bindings_sha256=digest(out/'bindings.json'),capture_sha256=digest(out/'original.jsonl'),
        frames_sha256=digest(out/'original.frames'))
    (out/'receipt.json').write_text(json.dumps(receipt,indent=2)+'\n')
    from tools.porting.compare_tick_streams import read_rows
    rows=read_rows(out/'original.jsonl')
    print(json.dumps(dict(output=str(out),seconds=receipt['seconds'],end=rows[-1])))
    if result.returncode or rows[-1].get('kind')!='end':raise RuntimeError(f'incomplete DOSBox capture: {out}')
    return out


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--dosbox',type=Path,required=True)
    p.add_argument('--script',type=Path,required=True)
    p.add_argument('--output',type=Path,required=True)
    p.add_argument('--ticks',type=int,default=1000)
    p.add_argument('--audio',choices=('ad15','pc15'),default='ad15')
    p.add_argument('--cycles',type=int,default=30000)
    p.add_argument('--timeout',type=int,default=150)
    p.add_argument('--port-exe',type=Path)
    args=p.parse_args()
    if not 1<=args.ticks<=10000:p.error('--ticks must be in 1..10000')
    sys.path.insert(0,str(ROOT))
    out=capture(args)
    if args.port_exe:run_port(args.port_exe,out,args.script,args.ticks,args.audio)


if __name__=='__main__':main()


