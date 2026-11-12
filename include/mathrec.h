/* Shared mathrec declarations; DOS medium-model types. */
#ifndef STUNTS_MATHREC_H
#define STUNTS_MATHREC_H
struct MAT3;
#pragma pack()
struct MAT3 {
    int _11;
    int _21;
    int _31;
    int _12;
    int _22;
    int _32;
    int _13;
    int _23;
    int _33;
};
#endif
