/* Canonical WheelRect view; incompatible operational views stay local. */
#ifndef STUNTS_WHEEL_H
#define STUNTS_WHEEL_H
#pragma pack()
struct WheelRect {
    int left;
    int top;
    int section_p1_x;
    int section_p1_y;
    int section_p2_x;
    int section_p2_y;
    int x;
    int y;
};
#endif
