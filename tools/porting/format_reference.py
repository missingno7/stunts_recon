"""Read-only Stunts 1.1 file-format reference parser; no game files are written."""
from __future__ import annotations
import argparse, json, struct
from collections import Counter
from pathlib import Path

class FormatError(ValueError): pass

MAX_DECODED_BYTES = 32 * 1024 * 1024
TERR_E_TO_W = (0,0,0,0,0,0,1,2,1,3,0,2,3,0,0,1,1,3,2,0)
TERR_W_TO_E = (0,0,0,0,0,0,1,2,0,3,1,0,0,3,2,2,3,1,1,0)
TERR_N_TO_S = (0,0,0,0,0,0,1,1,5,0,4,5,0,0,4,1,5,4,1,0)
TERR_S_TO_N = (0,0,0,0,0,0,1,0,5,1,4,0,5,4,0,5,1,1,4)

def u16(b, p):
    if p < 0 or p + 2 > len(b): raise FormatError(f"u16 outside input at {p:#x}")
    return struct.unpack_from('<H', b, p)[0]
def u24(b, p):
    if p < 0 or p + 3 > len(b): raise FormatError(f"u24 outside input at {p:#x}")
    return b[p] | b[p+1] << 8 | b[p+2] << 16
def u32(b, p):
    if p < 0 or p + 4 > len(b): raise FormatError(f"u32 outside input at {p:#x}")
    return struct.unpack_from('<I', b, p)[0]

def decompress_rle(p):
    if len(p) < 9: raise FormatError('short RLE pass')
    outlen = u24(p, 1); srclen = u24(p, 4); reserved = p[7]
    if outlen > MAX_DECODED_BYTES: raise FormatError(f'RLE output exceeds {MAX_DECODED_BYTES} byte cap')
    escraw = p[8]
    skip_seq = bool(escraw & 0x80); nesc = escraw & 0x7f
    table_end = 9 + nesc
    if table_end > len(p): raise FormatError('RLE escape table outside pass')
    esc = p[9:table_end]
    encoded = p[table_end:]
    if not skip_seq and nesc <= 1: raise FormatError('RLE sequence pass requires escape entry 1')
    if not skip_seq and srclen > len(encoded): raise FormatError('RLE sequence source exceeds pass')
    if not skip_seq:
        src = encoded[:srclen]; seq = bytearray(); i = 0; marker = esc[1]
        while i < len(src):
            c = src[i]; i += 1
            if c != marker:
                seq.append(c); continue
            start = i
            while i < len(src) and src[i] != marker: i += 1
            if i >= len(src): raise FormatError('unterminated RLE sequence marker')
            lit = src[start:i]; i += 1
            if i >= len(src): raise FormatError('RLE sequence lacks repetition count')
            count = src[i]; i += 1
            repeats = (count - 1) & 0xff
            if len(seq) + len(lit) * (repeats + 1) > MAX_DECODED_BYTES:
                raise FormatError('RLE sequence expansion exceeds defensive limit')
            seq.extend(lit * (repeats + 1))
        source = bytes(seq)
    else:
        source = encoded
    lookup = {v:i+1 for i,v in enumerate(esc)}
    out = bytearray(); i = 0
    while len(out) < outlen:
        if i >= len(source): raise FormatError(f'RLE stream ended at output {len(out)}/{outlen}')
        c = source[i]; i += 1; kind = lookup.get(c, 0)
        if not kind:
            out.append(c); continue
        if kind == 1:
            if i + 2 > len(source): raise FormatError('short RLE byte-count run')
            count, val = source[i], source[i+1]; i += 2
        elif kind == 3:
            if i + 3 > len(source): raise FormatError('short RLE word-count run')
            count, val = source[i] | source[i+1] << 8, source[i+2]; i += 3
        else:
            count = kind - 1
            if i >= len(source): raise FormatError('short RLE literal escape')
            val = source[i]; i += 1
        if len(out) + count > outlen: raise FormatError('RLE run exceeds declared output length')
        if count: out.extend(bytes((val,)) * count)
    return bytes(out), {'outlen':outlen,'encoded_seq_len':srclen,'escapes':list(esc),'skip_sequence':skip_seq,'reserved':reserved,'source_consumed':i,'source_len':len(source)}

def decompress_vle(p):
    if len(p) < 7: raise FormatError('short VLE pass')
    outlen = u24(p, 1); ef = p[4]; additive = bool(ef & 0x80); n = ef & 0x7f
    if outlen > MAX_DECODED_BYTES: raise FormatError(f'VLE output exceeds {MAX_DECODED_BYTES} byte cap')
    if n == 0 or n > 16: raise FormatError(f'invalid VLE width-count length {n}')
    if 5+n > len(p): raise FormatError('short VLE width-count table')
    counts = list(p[5:5+n]); alphlen = sum(counts)
    if alphlen == 0 or alphlen > 256: raise FormatError(f'invalid VLE alphabet length {alphlen}')
    a0=5+n; a1=a0+alphlen
    if a1 > len(p): raise FormatError('short VLE alphabet')
    alph=list(p[a0:a1]); codes=p[a1:]
    if len(codes) < 2 and outlen: raise FormatError('short VLE bitstream')
    # Complete canonical codeword ranges exactly as fileio.c's esc1/esc2 recurrence.
    starts=[]; ends=[]; alpha_starts=[]; code=0; alpha_pos=0
    for count in counts:
        starts.append(code); code += count; ends.append(code); alpha_starts.append(alpha_pos); alpha_pos += count; code <<= 1
    # The original decoder expands the <=8-bit prefix table to all 256 lookahead values.
    widths=[0x40]*256; symbols=[0]*256; alpha_pos=0; code=0
    for width,count in enumerate(counts[:8],1):
        for k in range(count):
            c=code+k; a=alph[alpha_pos]
            base=c << (8-width); span=1 << (8-width)
            for q in range(base,base+span): widths[q]=width; symbols[q]=a
            alpha_pos += 1
        code=(code+count)<<1
    # Read MSB-first, decoding short canonical codes or the >8-bit escape groups.
    bitpos=0
    def bit():
        nonlocal bitpos
        if bitpos >= len(codes)*8: raise FormatError(f'VLE bitstream ended at bit {bitpos}')
        v=(codes[bitpos//8] >> (7-(bitpos%8))) & 1; bitpos += 1; return v
    out=bytearray(); current=0
    while len(out)<outlen:
        acc=0; symindex=None
        for width in range(1,n+1):
            acc=(acc<<1)|bit()
            if width <= 8:
                # A direct symbol's canonical code is discoverable by its exact width prefix.
                start=starts[width-1]; end=ends[width-1]
                if start <= acc < end:
                    symindex=alpha_starts[width-1]+acc-start; break
            else:
                # Long symbols use the same threshold + additive offset as the source decoder.
                if acc < ends[width-1]:
                    symindex=acc + (alpha_starts[width-1]-starts[width-1]); break
        if symindex is None or symindex < 0 or symindex >= alphlen:
            raise FormatError(f'VLE has no code for prefix at output {len(out)} (width {width})')
        if additive: current=(current+alph[symindex])&0xff
        else: current=alph[symindex]
        out.append(current)
    return bytes(out), {'outlen':outlen,'width_counts':counts,'alphabet_len':alphlen,'additive':additive,'bits_used':bitpos,'bitstream_bytes':len(codes)}

def decompress(data):
    if not data: raise FormatError('empty compressed input')
    first=data[0]
    if first & 0x80:
        passes=first&0x7f; final=u24(data,1); payload=data[4:]
        if passes == 0: raise FormatError('zero-pass wrapper')
    else:
        passes=1; final=None; payload=data
    current=payload; details=[]
    for i in range(passes):
        if len(current)<4: raise FormatError(f'pass {i+1}: short pass header')
        typ=current[0]
        if typ==1: current,meta=decompress_rle(current)
        elif typ==2: current,meta=decompress_vle(current)
        else: raise FormatError(f'pass {i+1}: unknown compression type {typ}')
        details.append({'type':'RLE' if typ==1 else 'VLE',**meta})
    if final is not None and len(current)!=final: raise FormatError(f'wrapper final size {final} != decoded {len(current)}')
    return current, {'passes':details,'wrapper_final_size':final,'decoded_size':len(current)}

def parse_archive(b, *, check_declared=True):
    if len(b)<6: raise FormatError('archive shorter than 6-byte header')
    declared=u32(b,0); n=u16(b,4); base=6+8*n
    if n>4096: raise FormatError(f'archive directory count {n} exceeds parser cap')
    if base>len(b): raise FormatError(f'archive directory ends at {base:#x}, file is {len(b):#x}')
    names=[b[6+4*i:10+4*i] for i in range(n)]
    if len(set(names))!=len(names): raise FormatError('duplicate four-byte directory names')
    offpos=6+4*n; offsets=[u32(b,offpos+4*i) for i in range(n)]
    payload_len=len(b)-base
    if any(o>payload_len for o in offsets): raise FormatError('resource offset outside archive payload')
    unique=sorted(set(offsets)); end_by_off={o:(unique[i+1] if i+1<len(unique) else payload_len) for i,o in enumerate(unique)}; entries=[]
    for name,off in zip(names,offsets):
        nxt=end_by_off[off]
        entries.append({'name_raw':name,'name':name.decode('latin1'),'offset':off,'start':base+off,'end':base+nxt,'length':nxt-off})
    if check_declared and declared!=len(b):
        # The common game directory uses total byte length; voice files show a distinct declared-size convention.
        size_note=f'declared={declared}, actual={len(b)}'
    else: size_note=None
    return {'declared_size':declared,'actual_size':len(b),'count':n,'payload_base':base,'entries':entries,'declared_size_note':size_note}

def parse_shape_archive(b, kind):
    a=parse_archive(b); shapes=[]
    for e in a['entries']:
        c=b[e['start']:e['end']]
        if len(c)<16: raise FormatError(f"shape {e['name']} shorter than 16-byte SHAPE2D header")
        w,h,unk1,unk2,x,y=struct.unpack_from('<6H',c,0); attrs=list(c[12:16]); pix=w*h
        if pix>len(c)-16 and kind not in ('VSH',): raise FormatError(f"shape {e['name']} needs {pix} bitmap bytes; chunk has {len(c)-16}")
        shapes.append({'name':e['name'],'offset':e['offset'],'chunk_len':e['length'],'width':w,'height':h,'unknown_words':[unk1,unk2],'origin':[x,y],'attrs':attrs,'bitmap_len':pix})
    return a,shapes

def unflip_pvs(b):
    a,shapes=parse_shape_archive(b,'PVS'); out=bytearray(b); base=a['payload_base']
    for e,s in zip(a['entries'],shapes):
        p=e['start']+16; w,h=s['width'],s['height']; flag=(s['attrs'][2]>>4)&0xff
        if s['attrs'][3]&0xf0 or not flag or flag>=4: continue
        src=bytes(out[p:p+w*h]); dst=bytearray(w*h)
        if flag==1:
            for y in range(h):
                for x in range(w): dst[y*w+x]=src[y+x*h]
        elif flag==2:
            # The original scratch routine writes the second row for every even
            # row, so an odd final height writes one discarded row past h.
            # Its later copy-back loops only over y<h; retain those bytes only.
            for y in range(0,h,2):
                for x in range(w):
                    dst[y*w+x]=src[y//2+x*h]
                    if y+1<h:
                        dst[(y+1)*w+x]=src[(h+y+1)//2+x*h]
        else: raise FormatError(f'unsupported PVS flip flag {flag}')
        out[p:p+w*h]=dst
    return bytes(out)

def unflip_pes(b):
    a,shapes=parse_shape_archive(b,'PES'); out=bytearray(b)
    for e,s in zip(a['entries'],shapes):
        w,h=s['width'],s['height']; pix=w*h; p=e['start']+16; mask=(s['attrs'][2]>>4)&0xf
        if s['attrs'][3]&0xf0 or not mask: continue
        plane_extent=max((i+1 for i in range(4) if mask&(1<<i)),default=0)
        if pix*plane_extent>e['length']-16: raise FormatError(f"PES plane {plane_extent-1} payload short ({s['name']})")
        for plane in range(4):
            if mask&(1<<plane):
                q=p+plane*pix; src=bytes(out[q:q+pix]); dst=bytearray(pix)
                for y in range(h):
                    for x in range(w): dst[y*w+x]=src[x*h+y]
                out[q:q+pix]=dst
    return bytes(out)

def expand_esh(b):
    a,shapes=parse_shape_archive(b,'ESH'); n=a['count']; new=bytearray(b[:6+4*n]); new[:4]=b'\0'*4
    # The expanded directory keeps the count/name table and reconstructs its offset table.
    new.extend(b'\0'*(4*n)); offsets=[]; expanded=[]
    for e,s in zip(a['entries'],shapes):
        c=bytearray(b[e['start']:e['end']]); w,h=s['width'],s['height']; pix=w*h; attrs=s['attrs']; src=c[16:]
        if pix>len(src): raise FormatError(f"ESH packed bitmap short ({s['name']})")
        base=attrs[1]>>4; dst=bytearray([base]*(pix*8)); planes=0
        for j in range(4):
            pat=attrs[j]&0xf
            if pat==0: break
            planes+=1
            if (j+1)*pix>len(src): raise FormatError(f"ESH plane {j} short ({s['name']})")
            for k,v in enumerate(src[j*pix:(j+1)*pix]):
                for bit in range(8):
                    if v & (0x80>>bit): dst[k*8+bit] |= pat
        struct.pack_into('<H',c,0,(w*8)&0xffff)
        c=c[:16]+dst
        offsets.append(len(new)-(6+8*n)); expanded.append(c); new.extend(c)
    for i,off in enumerate(offsets): struct.pack_into('<I',new,6+4*n+4*i,off)
    struct.pack_into('<I',new,0,len(new))
    return bytes(new), {'shapes':n,'expanded_lengths':[len(x)-16 for x in expanded]}

def parse_track(b):
    if len(b)!=0x70a: raise FormatError(f'TRK size {len(b)} != 0x70A')
    elem=b[:0x385]; terr=b[0x385:]
    t=terr[:900]
    if any(v>=len(TERR_E_TO_W) for v in t): raise FormatError('terrain cell code exceeds 20-entry connection tables')
    hbad=[]; vbad=[]
    for y in range(30):
        for x in range(29):
            left=t[y*30+x]; right=t[y*30+x+1]
            if TERR_W_TO_E[left]!=TERR_E_TO_W[right]: hbad.append((x,y,left,right))
    for y in range(29):
        for x in range(30):
            north=t[y*30+x]; south=t[(y+1)*30+x]
            if TERR_S_TO_N[north]!=TERR_N_TO_S[south]: vbad.append((x,y,north,south))
    if hbad or vbad: raise FormatError(f'terrain edge-connection mismatches: horizontal={len(hbad)}, vertical={len(vbad)}; first={(hbad or vbad)[0]}')
    return {'cell_count':900,'width':30,'height':30,'element_plane':elem,'terrain_plane':terr,
            'element_trailer':elem[900],'terrain_trailer':terr[900],
            'element_values':Counter(elem[:900]),'terrain_values':Counter(terr[:900]),
            'terrain_connections_checked':30*29+29*30,'terrain_connection_errors':0}

def parse_replay(b):
    if len(b)<0x724: raise FormatError('RPL shorter than 0x724-byte prefix')
    h=b[:0x1a]; n=u16(h,24); want=0x724+n
    if len(b)!=want: raise FormatError(f'RPL length {len(b)} != 0x724+frame_count({n})={want}')
    return {'header_len':26,'player_car':h[0:4],'player_material':h[4],'player_transmission':h[5],
            'opponent_type':h[6],'opponent_car':h[7:11],'opponent_material':h[11],
            'opponent_transmission':h[12],'track_name':h[13:22],'fps':u16(h,22),'frame_count':n,
            'map_bytes':b[26:0x724],'input_bytes':b[0x724:], 'length':len(b)}

def parse_highscores(b):
    if len(b)!=0x16c: raise FormatError(f'HIG size {len(b)} != 7*0x34')
    return {'record_count':7,'record_size':0x34,'records':[b[i*0x34:(i+1)*0x34] for i in range(7)]}

def serialize_track(elem,terr):
    if len(elem)!=0x385 or len(terr)!=0x385: raise FormatError('TRK serializer needs two 901-byte planes')
    return bytes(elem)+bytes(terr)
def serialize_replay(header,mapbytes,inputbytes):
    if len(header)!=0x1a or len(mapbytes)!=0x70a: raise FormatError('RPL serializer needs 26-byte header and 1802-byte map')
    if u16(header,24)!=len(inputbytes): raise FormatError('RPL serializer frame count does not match input length')
    return bytes(header)+bytes(mapbytes)+bytes(inputbytes)
def serialize_highscores(records):
    if len(records)!=7 or any(len(r)!=0x34 for r in records): raise FormatError('HIG serializer needs seven 52-byte records')
    return b''.join(records)

def parse_font(b):
    if len(b)<0x216: raise FormatError('FNT shorter than 0x216-byte header+256-word offset table')
    height=u16(b,0x0e); fixed_width=u16(b,0x10); variable=bool(b[0x14]); ptrs=[u16(b,0x16+2*i) for i in range(256)]
    if not height: raise FormatError('FNT zero height')
    glyphs=[]; missing=0
    for code,off in enumerate(ptrs):
        if off==0: missing+=1; continue
        if off<0x216 or off>=len(b): raise FormatError(f'FNT glyph {code:#x} offset {off:#x} outside glyph area')
        width=b[off] if variable else fixed_width; prefix=1 if variable else 0
        if width==0: raise FormatError(f'FNT glyph {code:#x} zero width')
        size=prefix+((width+7)//8)*height
        if off+size>len(b): raise FormatError(f'FNT glyph {code:#x} bitmap ends outside file')
        glyphs.append({'code':code,'offset':off,'width':width,'height':height,'bitmap_bytes':size-prefix})
    return {'size':len(b),'height':height,'fixed_width':fixed_width,'variable_width':variable,'offset_table':[0x16,0x216],'glyph_count':len(glyphs),'missing_glyphs':missing,'glyphs':glyphs}

PRIM_INDEX_COUNTS=(0,1,2,3,4,5,6,7,8,9,10,2,6,3,0,0)
PRIM_TYPE_MAP=(0,5,1,0,0,0,0,0,0,0,0,2,3,4,0,0)
def parse_shape3d_chunk(b):
    if len(b)<4: raise FormatError('3D shape shorter than 4-byte header')
    nv,np,paint,reserved=b[:4]; need=4+nv*6+np*16
    primofs=4+nv*6+np*8
    if primofs>len(b): raise FormatError(f'3D cull arrays end at {primofs}, chunk has {len(b)}')
    pos=primofs; records=[]
    for i in range(np):
        if pos+2+paint>len(b): raise FormatError(f'3D primitive {i} header/material table outside chunk')
        typ,flags=b[pos],b[pos+1]
        if typ>=len(PRIM_INDEX_COUNTS): raise FormatError(f'3D primitive {i} type {typ} outside 16-entry count table')
        nidx=PRIM_INDEX_COUNTS[typ]; start=pos; pos+=2+paint
        if pos+nidx>len(b): raise FormatError(f'3D primitive {i} indices outside chunk')
        indices=list(b[pos:pos+nidx])
        if any(v>=nv for v in indices): raise FormatError(f'3D primitive {i} references vertex outside {nv}-vertex table')
        pos+=nidx
        records.append({'type':typ,'flags':flags,'materials':list(b[start+2:start+2+paint]),'vertex_indices':indices,'offset':start,'length':pos-start})
    need=pos
    if need>len(b): raise FormatError(f'3D primitive records end at {need}, chunk has {len(b)}')
    return {'vertices':nv,'primitives':np,'paints':paint,'reserved':reserved,'vertex_offset':4,'cull1_offset':4+nv*6,'cull2_offset':4+nv*6+np*4,'primitive_offset':primofs,'minimum_size':need,'chunk_size':len(b),'primitive_records':records,'trailing_bytes':len(b)-need}

def analyze(path):
    p=Path(path); ext=p.suffix.upper(); b=p.read_bytes(); row={'file':p.name,'extension':ext,'size':len(b)}
    if ext in ('.PRE','.P3S','.PVS','.PES','.XVS'):
        dec,meta=decompress(b); row['compression']=meta; row['decoded_size']=len(dec)
        a=parse_archive(dec); row['archive']={k:v for k,v in a.items() if k!='entries'}; row['archive']['entries']=[{k:v for k,v in e.items() if k not in ('name_raw',)} for e in a['entries']]
        if ext=='.P3S':
            row['3d_shapes']=[{'name':e['name'],**parse_shape3d_chunk(dec[e['start']:e['end']])} for e in a['entries']]
        if ext in ('.PVS','.PES','.XVS'):
            _,shapes=parse_shape_archive(dec,ext[1:]); row['2d_shapes']=shapes
            if ext=='.PVS':
                row['pvs_unflip_verified']=len(unflip_pvs(dec))==len(dec)
            else:
                if ext=='.PES':
                    uf=unflip_pes(dec); expanded,em=expand_esh(uf); parse_shape_archive(expanded,'ESH'); row['pes_unflip_verified']=len(uf)==len(dec); row['expanded_shapes']=em
                pal=next((e for e in a['entries'] if e['name_raw'].rstrip(b' ')==b'!MGA'),None)
                if pal:
                    chunk=dec[pal['start']:pal['end']]
                    if len(chunk)<32: raise FormatError('!MGA chunk shorter than SHAPE2D header + 16-byte remap')
                    row['palette_map_bytes']=list(chunk[16:32])
        if ext=='.PVS':
            pal=next((e for e in a['entries'] if e['name_raw'].rstrip(b' ')==b'!pal'),None)
            if pal:
                c=dec[pal['start']:pal['end']]
                if len(c)<16+0x300: raise FormatError('!pal shorter than 16-byte prefix plus 256xRGB bytes')
                row['display_palette']={'resource_name':'!pal','prefix_hex':c[:16].hex(),'rgb_bytes':len(c)-16,'colors':(len(c)-16)//3}
        return row
    if ext in ('.RES','.3SH','.ESH'):
        a=parse_archive(b); row['archive']={k:v for k,v in a.items() if k!='entries'}; row['archive']['entries']=[{k:v for k,v in e.items() if k not in ('name_raw',)} for e in a['entries']]
        simd=next((e for e in a['entries'] if e['name_raw'].rstrip(b' ')==b'simd'),None)
        if simd:
            if simd['length']!=776: raise FormatError(f"SIMD chunk is {simd['length']} bytes, expected observed/source struct size 776")
            row['simd_chunk']={'offset':simd['offset'],'length':simd['length'],'validated_size':776}
        if ext in ('.3SH','.RES'):
            row['3d_shapes']=[]
            for e in a['entries']:
                if ext=='.3SH' or e['name'].lower() in ('car0','car1','car2','exp0','exp1','exp2','exp3'):
                    row['3d_shapes'].append({'name':e['name'],**parse_shape3d_chunk(b[e['start']:e['end']])})
        if ext=='.ESH':
            _,row['2d_shapes']=parse_shape_archive(b,'ESH')
            expanded,meta=expand_esh(b); parse_shape_archive(expanded,'ESH'); row['expanded_shapes']=meta
            pal=next((e for e in a['entries'] if e['name_raw'].rstrip(b' ')==b'!MGA'),None)
            if pal:
                c=b[pal['start']:pal['end']]
                if len(c)<32: raise FormatError('!MGA chunk shorter than SHAPE2D header + 16-byte remap')
                row['palette_map_bytes']=list(c[16:32])
        return row
    if ext=='.VSH': row['opaque_shape_blob']=True; return row
    if ext in ('.TRK',):
        t=parse_track(b); row.update({k:(dict(v) if isinstance(v,Counter) else v) for k,v in t.items() if k not in ('element_plane','terrain_plane')})
        row['roundtrip_identical']=serialize_track(t['element_plane'],t['terrain_plane'])==b
        return row
    if ext=='.HIG':
        h=parse_highscores(b); row.update({k:v for k,v in h.items() if k!='records'})
        row['roundtrip_identical']=serialize_highscores(h['records'])==b
        row['records']=[x.hex() for x in h['records']]
        return row
    if ext=='.RPL':
        r=parse_replay(b); row.update({k:(v.hex() if isinstance(v,bytes) else v) for k,v in r.items()})
        row['roundtrip_identical']=serialize_replay(b[:0x1a],b[0x1a:0x724],b[0x724:])==b
        return row
    if ext=='.FNT': row['font']=parse_font(b); return row
    if ext in ('.KMS','.SFX','.VCE'):
        a=parse_archive(b,check_declared=False); row['archive']={k:v for k,v in a.items() if k!='entries'}; row['archive']['entries']=[{k:v for k,v in e.items() if k!='name_raw'} for e in a['entries']]
        row['declared_size_matches_actual']=a['declared_size']==len(b)
        if ext=='.VCE':
            row['vce_item_headers']=[{'name':e['name'],'u16_0':u16(b,e['start']),'u16_2':u16(b,e['start']+2),'chunk_start':e['start'],'indexed_span':e['length']} for e in a['entries']]
        return row
    if ext in ('.DRV','.PLB','.SFX','.KMS','.VCE','.DAT','.TD','.CKM'): row['opaque_payload']=True; return row
    return row

def main():
    ap=argparse.ArgumentParser(); ap.add_argument('root',nargs='?',default='assets'); ap.add_argument('--report',default='build/porting/asset-validation.json'); args=ap.parse_args()
    files=sorted(Path(args.root).glob('*'))
    parsed=[]; errors=[]
    for p in files:
        if not p.is_file(): continue
        try: parsed.append(analyze(p))
        except Exception as e: errors.append({'file':p.name,'extension':p.suffix.upper(),'error':str(e)})
    for row in parsed:
        if 'roundtrip_identical' in row and not row['roundtrip_identical']:
            errors.append({'file':row['file'],'extension':row['extension'],'error':'parse/serialize round-trip differs bytewise'})
    typed_exts={'.PRE','.P3S','.PVS','.PES','.XVS','.RES','.3SH','.ESH','.VSH','.TRK','.HIG','.RPL','.FNT','.KMS','.SFX','.VCE','.DRV','.PLB','.DAT','.TD','.CKM'}
    roundtrip_files=[x for x in parsed if 'roundtrip_identical' in x]
    trks=[x for x in parsed if x['extension']=='.TRK']
    terrain_counts=Counter(); element_counts=Counter()
    for p in Path(args.root).glob('*.TRK'):
        t=parse_track(p.read_bytes()); terrain_counts.update(t['terrain_values']); element_counts.update(t['element_values'])
    summary={'input_dir':str(Path(args.root)),'asset_count':len(files),'extension_counts':dict(sorted(Counter(p.suffix.upper() or '(none)' for p in files).items())),
             'catalogued_count':len(parsed),'typed_parser_count':sum(x['extension'] in typed_exts for x in parsed),
             'inventory_only_count':sum(x['extension'] not in typed_exts for x in parsed),
             'inventory_only_extensions':sorted({x['extension'] for x in parsed if x['extension'] not in typed_exts}),
             'error_count':len(errors),'errors':errors,'roundtrip_checks':{'files':len(roundtrip_files),'all_identical':all(x['roundtrip_identical'] for x in roundtrip_files),'extensions':dict(sorted(Counter(x['extension'] for x in roundtrip_files).items()))},
             'track_aggregate':{'files':len(trks),'terrain_connection_edges_checked':sum(x['terrain_connections_checked'] for x in trks),'terrain_cell_codes':dict(sorted(terrain_counts.items())),'element_cell_codes':dict(sorted(element_counts.items()))},
             'files':parsed}
    out=Path(args.report); out.parent.mkdir(parents=True,exist_ok=True); out.write_text(json.dumps(summary,indent=2),encoding='utf-8')
    print(f"assets={len(files)} catalogued={len(parsed)} typed={summary['typed_parser_count']} inventory_only={summary['inventory_only_count']} errors={len(errors)} roundtrip={len(roundtrip_files)} report={out}")
    for e in errors: print(f"ERROR {e['file']} {e['error']}")
    for ext in ('.PRE','.P3S','.PVS','.PES','.RES','.TRK','.HIG','.RPL','.FNT','.KMS','.SFX','.VCE'):
        xs=[x for x in parsed if x['extension']==ext]
        if xs: print(f"{ext}: {len(xs)} parsed")
    for x in parsed:
        if x.get('archive',{}).get('declared_size_note'): print(f"SIZE NOTE {x['file']}: {x['archive']['declared_size_note']}")
    return 1 if errors else 0
if __name__=='__main__': raise SystemExit(main())
