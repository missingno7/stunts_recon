#include "edge_chain_tessellator.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>

#define NEAR(a,b) (fabs((a)-(b))<1e-8)

static size_t tessellate(const EdgeChainPoint *points,size_t count,
                         EdgeChainSpan *spans)
{
    int n=edge_chain_tessellate(points,count,spans,EDGE_CHAIN_MAX_SPANS);
    assert(n>=0);
    return (size_t)n;
}

static double x_at(double y,double y0,double x0,double y1,double x1)
{ return x0+(y-y0)*(x1-x0)/(y1-y0); }

static void test_forward_chain_overwrite(void)
{
    /* At y=3 both rising edges (0->1 and 2->3) are active in the forward
       path. The recovered helper writes each in input traversal order, so
       edge 2->3 is the selected boundary (x=5.5), not edge 0->1 (x=7.5). */
    const EdgeChainPoint p[]={{0,0},{10,4},{4,2},{8,6},{3,8}};
    EdgeChainSpan spans[EDGE_CHAIN_MAX_SPANS];
    size_t n=tessellate(p,5,spans),matches=0;
    for(size_t i=0;i<n;++i) {
        double top=spans[i].top_left.y,bottom=spans[i].bottom_left.y;
        if(top<3.0&&3.0<bottom) {
            double right=x_at(3.0,spans[i].top_right.y,spans[i].top_right.x,
                              spans[i].bottom_right.y,spans[i].bottom_right.x);
            double left=x_at(3.0,top,spans[i].top_left.x,bottom,spans[i].bottom_left.x);
            assert(NEAR(right,5.5));
            assert(NEAR(left,0.9375));
            ++matches;
        }
    }
    assert(matches==1);
}

static void test_reverse_side_and_crossing(void)
{
    const EdgeChainPoint p[]={{0.25,0.25},{7.5,5.5},{2.0,5.0},{6.5,0.5}};
    EdgeChainSpan spans[EDGE_CHAIN_MAX_SPANS];
    size_t n=tessellate(p,4,spans);
    assert(n>0);
    for(size_t i=0;i<n;++i) {
        const EdgeChainSpan *s=&spans[i];
        assert(s->top_left.y<=s->bottom_left.y);
        assert(NEAR(s->top_left.y,s->top_right.y));
        assert(NEAR(s->bottom_left.y,s->bottom_right.y));
        assert(s->top_left.x<=s->top_right.x+1e-8);
        assert(s->bottom_left.x<=s->bottom_right.x+1e-8);
        assert(isfinite(s->top_left.x)&&isfinite(s->top_right.x));
        assert(isfinite(s->bottom_left.x)&&isfinite(s->bottom_right.x));
    }
}

static void test_adjacent_and_closing_repeats(void)
{
    const EdgeChainPoint repeated[]={{1,0},{1,0},{8,3},{8,3},{4,8},{1,0}};
    const EdgeChainPoint compact[]={{1,0},{8,3},{4,8}};
    EdgeChainSpan a[EDGE_CHAIN_MAX_SPANS],b[EDGE_CHAIN_MAX_SPANS];
    size_t na=tessellate(repeated,6,a),nb=tessellate(compact,3,b);
    assert(na==nb&&na>0);
    for(size_t i=0;i<na;++i) {
        assert(NEAR(a[i].top_left.x,b[i].top_left.x));
        assert(NEAR(a[i].top_left.y,b[i].top_left.y));
        assert(NEAR(a[i].top_right.x,b[i].top_right.x));
        assert(NEAR(a[i].top_right.y,b[i].top_right.y));
        assert(NEAR(a[i].bottom_right.x,b[i].bottom_right.x));
        assert(NEAR(a[i].bottom_right.y,b[i].bottom_right.y));
        assert(NEAR(a[i].bottom_left.x,b[i].bottom_left.x));
        assert(NEAR(a[i].bottom_left.y,b[i].bottom_left.y));
    }
}

static void test_collapsed_policy(void)
{
    const EdgeChainPoint p[]={{1.25,0.5},{3.75,1.5},{6.25,2.5}};
    EdgeChainSpan spans[EDGE_CHAIN_MAX_SPANS];
    size_t n=tessellate(p,3,spans);
    assert(n==1);
    assert(NEAR(hypot(spans[0].top_left.x-spans[0].bottom_left.x,
                      spans[0].top_left.y-spans[0].bottom_left.y),1.0));
    assert(NEAR(hypot(spans[0].top_right.x-spans[0].bottom_right.x,
                      spans[0].top_right.y-spans[0].bottom_right.y),1.0));
    {
        const EdgeChainPoint points[]={ {2.5,4.5},{2.5,4.5},{2.5,4.5} };
        n=tessellate(points,3,spans);
        assert(n==1);
        assert(NEAR(fabs(spans[0].top_right.x-spans[0].top_left.x),1.0));
        assert(NEAR(fabs(spans[0].bottom_left.x-spans[0].bottom_right.x),1.0));
    }
}

static void test_limits_and_errors(void)
{
    const EdgeChainPoint triangle[]={{0.25,0.25},{8.25,6.25},{1.25,6.25}};
    EdgeChainSpan spans[EDGE_CHAIN_MAX_SPANS];
    double nan_value=NAN;
    const EdgeChainPoint bad[]={{0,0},{1,0},{nan_value,1}};
    assert(edge_chain_tessellate(triangle,3,spans,1)==EDGE_CHAIN_ERROR_CAPACITY);
    assert(edge_chain_tessellate(triangle,2,spans,EDGE_CHAIN_MAX_SPANS)==EDGE_CHAIN_ERROR_COUNT);
    assert(edge_chain_tessellate(bad,3,spans,EDGE_CHAIN_MAX_SPANS)==EDGE_CHAIN_ERROR_NONFINITE);
    {
        EdgeChainPoint wheel_face[EDGE_CHAIN_MAX_POINTS];
        for(size_t i=0;i<EDGE_CHAIN_MAX_POINTS;++i) {
            double angle=(double)i*6.2831853071795864769/EDGE_CHAIN_MAX_POINTS;
            wheel_face[i]=(EdgeChainPoint){50.0+12.0*cos(angle),40.0+5.0*sin(angle)};
        }
        assert(tessellate(wheel_face,EDGE_CHAIN_MAX_POINTS,spans)>0);
    }
}

int main(void)
{
    test_forward_chain_overwrite();
    test_reverse_side_and_crossing();
    test_adjacent_and_closing_repeats();
    test_collapsed_policy();
    test_limits_and_errors();
    puts("edge-chain tessellator tests passed");
    return 0;
}
