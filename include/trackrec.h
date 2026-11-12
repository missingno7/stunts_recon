/* Shared trackrec declarations; DOS medium-model types. */
#ifndef STUNTS_TRACKREC_H
#define STUNTS_TRACKREC_H
struct SHAPE3D;
struct TRACKRESULT;
struct TRKOBJINFO_LINK_BYTES;
struct TrackNode;
struct VECTOR;
struct WALLREC;
struct scene_shape;
struct track_object;
struct track_object_info;
#include "vec3.h"
#pragma pack()
struct scene_shape {
    unsigned short opaque_first_word;
    short rotation_y;
    struct SHAPE3D *shape;
    struct SHAPE3D *low_detail_shape;
    unsigned char overlay;
    unsigned char surface;
    unsigned char ignore_z_bias;
    unsigned char multi_tile;
    unsigned char physical_model;
    unsigned char unknown;
};
#pragma pack()
struct track_object {
    struct track_object_info *info;
    short rotation_y;
    struct SHAPE3D *shape;
    struct SHAPE3D *low_detail_shape;
    unsigned char overlay;
    unsigned char surface;
    unsigned char ignore_z_bias;
    unsigned char multi_tile;
    unsigned char physical_model;
    unsigned char unknown;
};
#pragma pack()
struct track_object_info {
    unsigned char block_count;
    unsigned char entry_point;
    unsigned char exit_point;
    unsigned char entry_type;
    unsigned char exit_type;
    unsigned char arrow_type;
    short arrow_orientation;
    unsigned char *camera_data;
    unsigned char opponent1;
    unsigned char opponent2;
    unsigned char opponent3;
    unsigned char opponent_speed_code;
};
#pragma pack()
struct TrackNode {
    unsigned char column;
    unsigned char row;
    unsigned char element;
    unsigned char block;
    unsigned char connection;
    unsigned char terrain;
    unsigned char previousColumn;
    unsigned char previousRow;
    unsigned char previousElement;
    unsigned char previousBlock;
    unsigned char previousReversed;
    unsigned char previousExitType;
    short parent;
};
#pragma pack()
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    short has_opponent_link;
};
#pragma pack()
struct TRKOBJINFO_LINK_BYTES {
    char first;
    char second;
};
#pragma pack()
struct WALLREC {
    short orientation;
    short x;
    short z;
};
#endif
