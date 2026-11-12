/* Shared carrec declarations; DOS medium-model types. */
#ifndef STUNTS_CARREC_H
#define STUNTS_CARREC_H
struct car_exp_name_table;
#pragma pack()
struct car_exp_name_table {
    char entry[15][5];
    char trailing_zero;
};
#endif
