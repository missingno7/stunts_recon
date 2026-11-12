/* Canonical TRKOBJINFO view; incompatible operational views stay local. */
#ifndef STUNTS_TRKINFO_H
#define STUNTS_TRKINFO_H
struct TRKOBJINFO_LINK_BYTES;
#pragma pack()
struct TRKOBJINFO {
    char si_noOfBlocks;
    char si_entryPoint;
    char si_exitPoint;
    char si_entryType;
    char si_exitType;
    char si_arrowType;
    short si_arrowOrient;
    short *si_cameraDataOffset;
    union  { short *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    char si_opp3;
    char si_oppSpedCode;
};
#endif
