struct POINT2D { int x, y; };

extern struct RECT { int left, right, top, bottom; } clip;
unsigned rect_compare_point(register struct POINT2D *p) {
    char flag;
    if (p->y < clip.top) flag=1;
    else if (p->y > clip.bottom) flag=2;
    else flag=0;
    if (p->x < clip.left) flag|=4;
    else if (p->x > clip.right) flag|=8;
    return flag;
}

