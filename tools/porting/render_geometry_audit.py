from __future__ import annotations
import json, struct, sys
from collections import Counter
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'tools'/'porting'))
import format_reference as fmt

def sub(a,b):return tuple(x-y for x,y in zip(a,b))
def cross3(a,b):return (a[1]*b[2]-a[2]*b[1],a[2]*b[0]-a[0]*b[2],a[0]*b[1]-a[1]*b[0])
def dot(a,b):return sum(x*y for x,y in zip(a,b))
def cross2(a,b,c):return (b[0]-a[0])*(c[1]-a[1])-(b[1]-a[1])*(c[0]-a[0])
def onseg(a,b,p):return cross2(a,b,p)==0 and min(a[0],b[0])<=p[0]<=max(a[0],b[0]) and min(a[1],b[1])<=p[1]<=max(a[1],b[1])
def intersection_kind(a,b,c,d):
    o1,o2,o3,o4=cross2(a,b,c),cross2(a,b,d),cross2(c,d,a),cross2(c,d,b)
    if ((o1>0 and o2<0)or(o1<0 and o2>0)) and ((o3>0 and o4<0)or(o3<0 and o4>0)):return 'proper'
    if ((o1==0 and onseg(a,b,c))or(o2==0 and onseg(a,b,d))or(o3==0 and onseg(c,d,a))or(o4==0 and onseg(c,d,b))):return 'touch'
    return None
def polygon_checks(indices,verts,label):
    pts=[verts[i] for i in indices]; n=len(pts)
    rid=len(set(indices))!=n
    rpos=len(set(pts))!=n
    norm=None
    for i in range(n):
        for j in range(i+1,n):
            candidate=cross3(sub(pts[i],pts[0]),sub(pts[j],pts[0]))
            if candidate!=(0,0,0):norm=candidate;break
        if norm is not None:break
    collinear=norm is None
    planar=False
    axis=2
    proj=[]
    if not collinear:
        planar=all(dot(norm,sub(p,pts[0]))==0 for p in pts)
        axis=max(range(3),key=lambda k:abs(norm[k]))
        proj=[tuple(p[k] for k in range(3) if k!=axis) for p in pts]
    else:
        # Any fixed plane projection is diagnostic only for a collinear contour.
        proj=[(p[0],p[1]) for p in pts]
    area=sum(p[0]*proj[(i+1)%n][1]-proj[(i+1)%n][0]*p[1] for i,p in enumerate(proj))
    turns=[cross2(proj[i-1],proj[i],proj[(i+1)%n]) for i in range(n)]
    signs={1 if t>0 else -1 for t in turns if t}
    hits=[]
    for i in range(n):
        for j in range(i+1,n):
            if j==(i+1)%n or (j+1)%n==i:continue
            kind=intersection_kind(proj[i],proj[(i+1)%n],proj[j],proj[(j+1)%n])
            if kind:hits.append({'edges':[i,(i+1)%n,j,(j+1)%n],'kind':kind})
    simple=not hits and not rpos
    nonzero_area=area!=0
    convex=planar and simple and nonzero_area and len(signs)<=1
    concave=planar and simple and nonzero_area and len(signs)>1
    return {'label':label,'indices':indices,'point_count':n,'planar':planar,'collinear_3d':collinear,'dominant_axis_dropped':axis,'area2':area,'turns':turns,'turn_sign_count':len(signs),'repeated_index':rid,'repeated_position':rpos,'simple_projected':simple,'intersections':hits,'weakly_convex':convex,'concave':concave}

def audit():
    assets=ROOT/'assets'; files=sorted(list(assets.glob('ST*.P3S'))+[assets/'GAME1.P3S',assets/'GAME2.P3S'])
    shapes=[]; type_count=Counter(); classes=Counter(); face_rows=[]; all_records=[]; class_resources=Counter()
    for path in files:
        decoded,_=fmt.decompress(path.read_bytes()); archive=fmt.parse_archive(decoded)
        for entry in archive['entries']:
            chunk=decoded[entry['start']:entry['end']]; parsed=fmt.parse_shape3d_chunk(chunk)
            verts=[struct.unpack_from('<hhh',chunk,4+6*i) for i in range(parsed['vertices'])]
            key=f"{path.name}_{entry['name']}"; seen=set()
            for i,r in enumerate(parsed['primitive_records']):
                typ=r['type']; cls=fmt.PRIM_TYPE_MAP[typ]; type_count[typ]+=1; classes[cls]+=1; seen.add(cls)
                if cls==0:face_rows.append(polygon_checks(r['vertex_indices'],verts,f'{key}#{i}:type{typ}'))
                all_records.append({'shape':key,'primitive':i,'type':typ,'class':cls,'n':len(r['vertex_indices'])})
            for c in seen:class_resources[c]+=1
            shapes.append({'key':key,'vertices':parsed['vertices'],'primitive_count':parsed['primitives']})
    planar=[f for f in face_rows if f['planar']]
    summary={
        'archives':len(files),'shapes':len(shapes),'primitives':len(all_records),'filled_faces':len(face_rows),
        'primitive_class_counts':dict(classes),'resources_by_class':dict(class_resources),
        'filled_vertex_count_distribution':dict(sorted(Counter(f['point_count'] for f in face_rows).items())),
        'filled_face_geometry':{
            'planar':len(planar),'nonplanar':len(face_rows)-len(planar),'collinear_3d':sum(f['collinear_3d'] for f in face_rows),
            'weakly_convex_planar_simple':sum(f['weakly_convex'] for f in face_rows),
            'concave_planar_simple':sum(f['concave'] for f in face_rows),
            'projected_any_nonadjacent_intersection':sum(bool(f['intersections']) for f in face_rows),
            'projected_proper_crossing_faces':sum(any(h['kind']=='proper' for h in f['intersections']) for f in face_rows),
            'projected_touch_or_overlap_faces':sum(any(h['kind']=='touch' for h in f['intersections']) for f in face_rows),
            'repeated_index_faces':sum(f['repeated_index'] for f in face_rows),
            'repeated_coordinate_faces':sum(f['repeated_position'] for f in face_rows),
            'nonzero_projected_area_faces':sum(f['area2']!=0 for f in face_rows),
        },
        'encoded_types':dict(sorted(type_count.items())),
        'simple_concave_examples':[f for f in face_rows if f['concave']],
        'proper_crossing_examples':[f for f in face_rows if any(h['kind']=='proper' for h in f['intersections'])],
        'repeated_examples':[f for f in face_rows if f['repeated_index'] or f['repeated_position']],
        'degenerate_examples':[f for f in face_rows if f['collinear_3d'] or f['area2']==0],
    }
    return summary

def main():
    summary=audit()
    out=ROOT / "build" / "render-audit" / "shape-geometry.json"
    out.parent.mkdir(parents=True,exist_ok=True)
    out.write_text(json.dumps(summary,indent=2),encoding="utf-8")
    print(json.dumps({k:v for k,v in summary.items() if not k.endswith("examples")},indent=2))
if __name__=='__main__':main()
