unsigned char subst_hillroad_track(unsigned char a, unsigned char b)
{
    switch (a) {
    case 7:
        switch (b) {
        case 4: return 0xb6;
        case 14: return 0xba;
        case 24: return 0xbe;
        case 39: case 59: case 98: return 0xc2;
        }
        break;
    case 8:
        switch (b) {
        case 5: return 0xb7;
        case 15: return 0xbb;
        case 25: return 0xbf;
        case 36: case 56: case 95: return 0xc3;
        }
        break;
    case 9:
        switch (b) {
        case 4: return 0xb8;
        case 14: return 0xbc;
        case 24: return 0xc0;
        case 38: case 58: case 97: return 0xc4;
        }
        break;
    case 10:
        switch (b) {
        case 5: return 0xb9;
        case 15: return 0xbd;
        case 25: return 0xc1;
        case 37: case 57: case 96: return 0xc5;
        }
        break;
    }
    return 0;
}

