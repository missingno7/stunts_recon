/* integ27 fixture: small MSC 6.00A /Zi object with readable inline assembly,
   an optimize pragma region, far calls and DGROUP data (tests only). */
extern int g;
struct T { int a; long b; };
extern struct T t[4];
int counter = 3;
void far h(int x);

int far f(int x)
{
    int i;
    for (i = 0; i < x; i++) {
        t[i].a = g;
        h(i);
    }
    return x + counter;
}

#pragma optimize("tl", on)
int far k(int y)
{
    int j;
    j = y * 3;
    _asm {
        mov ax, j
        add ax, 5
        mov j, ax
    }
    h(j);
    return f(j) + t[1].a;
}
#pragma optimize("tl", off)
