#include "edge_chain_tessellator.h"

#include <float.h>
#include <math.h>
#include <stdint.h>

typedef struct Edge { unsigned a, b, source; double low, high; } Edge;
typedef struct Boundary { const Edge *edge; int constant, side; double x; } Boundary;

static double cross(EdgeChainPoint a, EdgeChainPoint b, EdgeChainPoint c)
{
    return (b.x-a.x)*(c.y-a.y)-(b.y-a.y)*(c.x-a.x);
}

static double edge_x(const EdgeChainPoint *p, const Edge *e, double y)
{
    double dy=p[e->b].y-p[e->a].y;
    return p[e->a].x+(y-p[e->a].y)*(p[e->b].x-p[e->a].x)/dy;
}

/* A recovered scanline edge contributes every x sample in a one-reference-
   pixel-high row. Model that boundary as the continuous footprint of the
   segment over [y-.5,y+.5], rather than as a zero-width mathematical line. */
static void edge_interval(const EdgeChainPoint *p,const Edge *e,double y,
                          double *left,double *right)
{
    double a=fmax(e->low,y-0.5),b=fmin(e->high,y+0.5);
    double xa=edge_x(p,e,a),xb=edge_x(p,e,b);
    *left=fmin(xa,xb);*right=fmax(xa,xb);
}

static double edge_bound(const EdgeChainPoint *p,const Edge *e,double y,int side)
{
    double left,right;
    edge_interval(p,e,y,&left,&right);
    return side<0?left:right;
}

static int same_y(double a, double b, double eps)
{ return fabs(a-b)<=eps; }

static int add_event(double *ys,size_t *n,double y,double eps)
{
    for(size_t i=0;i<*n;++i) if(same_y(ys[i],y,eps)) return 1;
    if(*n>=EDGE_CHAIN_MAX_SPANS) return 0;
    ys[(*n)++]=y; return 1;
}

static void sort_events(double *ys,size_t n)
{
    for(size_t i=1;i<n;++i) {
        double value=ys[i]; size_t j=i;
        while(j>0&&ys[j-1]>value) {ys[j]=ys[j-1];--j;}
        ys[j]=value;
    }
}

static int band_for_edge(const Edge *e,double y,double eps)
{ return y>e->low-0.5+eps&&y<e->high+0.5-eps; }

static int active_last(const Edge *edges,size_t n,
                       double y,double eps,const Edge **out)
{
    int found=0;
    for(size_t i=0;i<n;++i) if(band_for_edge(&edges[i],y,eps)) {
        *out=&edges[i]; found=1;
    }
    return found;
}

static int push_span(EdgeChainSpan *out,size_t cap,size_t *used,
                     const EdgeChainPoint *p,Boundary left,Boundary right,
                     double y0,double y1,double eps)
{
    double mid=(y0+y1)*0.5;
    double llo,lhi,rlo,rhi;
    if(left.constant) llo=lhi=left.x;
    else { edge_interval(p,left.edge,mid,&llo,&lhi); }
    if(right.constant) rlo=rhi=right.x;
    else { edge_interval(p,right.edge,mid,&rlo,&rhi); }
    double lm=left.constant?left.x:(left.side<0?llo:lhi);
    double rm=right.constant?right.x:(right.side<0?rlo:rhi);
    if(rm-lm<=eps) return 1;
    if(*used>=cap) return 0;
    if(left.constant) llo=lhi=left.x;
    else edge_interval(p,left.edge,y0,&llo,&lhi);
    if(right.constant) rlo=rhi=right.x;
    else edge_interval(p,right.edge,y0,&rlo,&rhi);
    out[*used].top_left=(EdgeChainPoint){left.constant?left.x:(left.side<0?llo:lhi),y0};
    out[*used].top_right=(EdgeChainPoint){right.constant?right.x:(right.side<0?rlo:rhi),y0};
    if(left.constant) llo=lhi=left.x;
    else edge_interval(p,left.edge,y1,&llo,&lhi);
    if(right.constant) rlo=rhi=right.x;
    else edge_interval(p,right.edge,y1,&rlo,&rhi);
    out[*used].bottom_right=(EdgeChainPoint){right.constant?right.x:(right.side<0?rlo:rhi),y1};
    out[*used].bottom_left=(EdgeChainPoint){left.constant?left.x:(left.side<0?llo:lhi),y1};
    ++*used;
    return 1;
}

static int line_strip(const EdgeChainPoint *p,size_t n,EdgeChainSpan *out,size_t cap)
{
    size_t ai=0,bi=0;
    double far2=-1.0;
    for(size_t i=0;i<n;++i) for(size_t j=i+1;j<n;++j) {
        double dx=p[j].x-p[i].x,dy=p[j].y-p[i].y,d=dx*dx+dy*dy;
        if(d>far2) {far2=d;ai=i;bi=j;}
    }
    double dx=p[bi].x-p[ai].x,dy=p[bi].y-p[ai].y,len=hypot(dx,dy);
    if(len<=DBL_EPSILON) {
        if(cap<1) return EDGE_CHAIN_ERROR_CAPACITY;
        out[0].top_left=(EdgeChainPoint){p[ai].x-0.5,p[ai].y-0.5};
        out[0].top_right=(EdgeChainPoint){p[ai].x+0.5,p[ai].y-0.5};
        out[0].bottom_right=(EdgeChainPoint){p[ai].x+0.5,p[ai].y+0.5};
        out[0].bottom_left=(EdgeChainPoint){p[ai].x-0.5,p[ai].y+0.5};
        return 1;
    }
    double nx=-dy*0.5/len,ny=dx*0.5/len;
    if(cap<1) return EDGE_CHAIN_ERROR_CAPACITY;
    out[0].top_left=(EdgeChainPoint){p[ai].x+nx,p[ai].y+ny};
    out[0].top_right=(EdgeChainPoint){p[bi].x+nx,p[bi].y+ny};
    out[0].bottom_right=(EdgeChainPoint){p[bi].x-nx,p[bi].y-ny};
    out[0].bottom_left=(EdgeChainPoint){p[ai].x-nx,p[ai].y-ny};
    return 1;
}

int edge_chain_tessellate(const EdgeChainPoint *p,size_t n,
                          EdgeChainSpan *out,size_t cap)
{
    Edge forward[EDGE_CHAIN_MAX_POINTS],reverse[EDGE_CHAIN_MAX_POINTS];
    size_t nf=0,nr=0,nevents=0,used=0;
    double ys[EDGE_CHAIN_MAX_SPANS],miny=DBL_MAX,maxy=-DBL_MAX,minx=DBL_MAX,maxx=-DBL_MAX;
    size_t minv=0,maxv=0; double scale=1.0,eps;
    int reverse_side[EDGE_CHAIN_MAX_POINTS]={0};
    if(!p||!out) return EDGE_CHAIN_ERROR_ARGUMENT;
    if(n<3||n>EDGE_CHAIN_MAX_POINTS) return EDGE_CHAIN_ERROR_COUNT;
    for(size_t i=0;i<n;++i) {
        if(!isfinite(p[i].x)||!isfinite(p[i].y)) return EDGE_CHAIN_ERROR_NONFINITE;
        if(p[i].x<minx) minx=p[i].x;
        if(p[i].x>maxx) maxx=p[i].x;
        if(p[i].y<=miny) { miny=p[i].y; minv=i; }
        if(p[i].y>maxy) { maxy=p[i].y; maxv=i; }
    }
    scale=fmax(fmax(maxx-minx,maxy-miny),1.0);eps=scale*64.0*DBL_EPSILON;
    /* Geometric rank, not shoelace area: a bow-tie has zero area but is not a line. */
    {
        size_t base=1;
        int noncollinear=0;
        while(base<n&&fabs(p[base].x-p[0].x)<=eps&&fabs(p[base].y-p[0].y)<=eps)++base;
        if(base<n) for(size_t i=1;i<n;++i)
            if(fabs(cross(p[0],p[base],p[i]))>eps*scale) {noncollinear=1;break;}
        if(!noncollinear) return line_strip(p,n,out,cap);
    }
    if(maxv==minv||same_y(miny,maxy,eps)||same_y(minx,maxx,eps)) return line_strip(p,n,out,cap);
    for(int chain=0;chain<2;++chain) {
        size_t vertex=minv;
        while(vertex!=maxv) {
            size_t next=chain==0?vertex+1:(vertex==0?n-1:vertex-1);
            if(next==n)next=0;
            if(p[next].y>p[vertex].y+eps) {
                Edge e={(unsigned)vertex,(unsigned)next,(unsigned)vertex,p[vertex].y,p[next].y};
                if(chain==0)forward[nf++]=e;else reverse[nr++]=e;
            }
            vertex=next;
        }
    }
    if(!add_event(ys,&nevents,miny-0.5,eps)||!add_event(ys,&nevents,maxy+0.5,eps)) return EDGE_CHAIN_ERROR_CAPACITY;
    for(size_t i=0;i<n;++i) {
        if(!add_event(ys,&nevents,p[i].y-0.5,eps)||
           !add_event(ys,&nevents,p[i].y+0.5,eps)) return EDGE_CHAIN_ERROR_CAPACITY;
    }
    /* Split at source vertices and at edge-footprint endpoints first. */
    Edge all[2*EDGE_CHAIN_MAX_POINTS];size_t na=0;
    for(size_t i=0;i<nf;++i) all[na++]=forward[i];
    for(size_t i=0;i<nr;++i) all[na++]=reverse[i];
    sort_events(ys,nevents);
    /* Within each event slab the left/right footprints are linear. Split at
       every possible boundary crossing so a band never changes handedness. */
    {
        size_t base_events=nevents;
        for(size_t slab=0;slab+1<base_events;++slab) {
            double lo=ys[slab],hi=ys[slab+1],mid=(lo+hi)*0.5;
            if(hi-lo<=eps)continue;
            for(size_t i=0;i<na;++i)if(band_for_edge(&all[i],mid,eps))
                for(size_t j=i+1;j<na;++j)if(band_for_edge(&all[j],mid,eps))
                    for(int si=-1;si<=1;si+=2)for(int sj=-1;sj<=1;sj+=2) {
                        double d0=edge_bound(p,&all[i],lo,si)-edge_bound(p,&all[j],lo,sj);
                        double d1=edge_bound(p,&all[i],hi,si)-edge_bound(p,&all[j],hi,sj);
                        if(d0*d1<0.0) {
                            double t=-d0/(d1-d0),y=lo+t*(hi-lo);
                            if(!add_event(ys,&nevents,y,eps))return EDGE_CHAIN_ERROR_CAPACITY;
                        }
                    }
        }
    }
    sort_events(ys,nevents);
    for(size_t band=0;band+1<nevents;++band) {
        double y0=ys[band],y1=ys[band+1],ym=(y0+y1)*0.5;
        const Edge *base=NULL; Boundary left={NULL,1,0,0},right={NULL,1,0,0};
        int seeded=active_last(forward,nf,ym,eps,&base);
        if(seeded) {
            left=(Boundary){base,0,-1,0};right=(Boundary){base,0,1,0};
        }
        for(size_t j=0;j<nr;++j) {
            const Edge *e=&reverse[j];
            if(!band_for_edge(e,ym,eps))continue;
            double xleft,xright; edge_interval(p,e,ym,&xleft,&xright);
            int side=reverse_side[e->source];
            if(!seeded) {
                left=(Boundary){e,0,-1,0};right=(Boundary){e,0,1,0};seeded=1;
                continue;
            }
            if(side==0) {
                double llo,lhi,rlo,rhi;
                if(left.constant)llo=lhi=left.x;else edge_interval(p,left.edge,ym,&llo,&lhi);
                if(right.constant)rlo=rhi=right.x;else edge_interval(p,right.edge,ym,&rlo,&rhi);
                double lx=left.constant?left.x:(left.side<0?llo:lhi);
                double rx=right.constant?right.x:(right.side<0?rlo:rhi);
                double dx=p[e->b].x-p[e->a].x,dy=p[e->b].y-p[e->a].y;
                int right_first=dx < -dy;
                if(right_first&&xright>rx+eps)side=1;
                else if(xleft<lx-eps)side=-1;
                else if(xright>rx+eps)side=1;
                reverse_side[e->source]=side;
            }
            if(side<0)left=(Boundary){e,0,-1,0};
            else if(side>0)right=(Boundary){e,0,1,0};
        }
        if(seeded&&!push_span(out,cap,&used,p,left,right,y0,y1,eps))return EDGE_CHAIN_ERROR_CAPACITY;
    }
    /* A contour may collapse after exact edge-side selection even though its
       vertices are not collinear (the stock car fixtures contain 1px loops).
       Preserve it as a one-reference-pixel stroke instead of silently losing it. */
    if(used==0) return line_strip(p,n,out,cap);
    return (int)used;
}
