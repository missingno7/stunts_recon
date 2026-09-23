struct POINT2D { int x,y; }; struct RECT { int left,right,top,bottom; }; extern struct RECT select_rect_rc;
unsigned rect_compare_point(struct POINT2D *arg) { register struct POINT2D *pt=arg; char flag; if(pt->y<select_rect_rc.top) flag=1; else if(pt->y>select_rect_rc.bottom) flag=2; else flag=0; if(pt->x<select_rect_rc.left) flag|=4; else if(pt->x>select_rect_rc.right) flag|=8; return flag; }
