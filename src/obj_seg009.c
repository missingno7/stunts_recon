struct SHAPE2D {
    int s2d_width;
    int s2d_height;
    unsigned s2d_unk1;
    unsigned s2d_unk2;
    unsigned s2d_pos_x;
    unsigned s2d_pos_y;
    unsigned char s2d_unk3;
    unsigned char s2d_unk4;
};
struct SPRITE {
    struct SHAPE2D far *sprite_bitmapptr;
    unsigned short sprite_unk1;
    unsigned short sprite_unk2;
};
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    short ss_rotY;
    struct SHAPE3D *ss_shapePtr;
    struct SHAPE3D *ss_loShapePtr;
    unsigned char ss_ssOvelay;
    char ss_surfaceType;
    char ss_ignoreZBias;
    char ss_multiTileFlag;
    char ss_physicalModel;
    char scene_unk5;
};
struct GAMEINFO {
    char game_playercarid[4];
    char game_playermaterial;
    char game_playertransmission;
    char game_opponenttype;
    char game_opponentcarid[4];
    char game_opponentmaterial;
    char game_opponenttransmission;
    char game_trackname[9];
    unsigned short game_framespersec;
    unsigned short game_recordedframes;
};

extern struct TRACKOBJECT trkObjectList[];
extern struct GAMEINFO gameconfig;
extern unsigned char far *word_3283C[];
extern struct SHAPE2D far *word_3282C[];
extern struct SHAPE2D far *word_32888[];
extern unsigned char far *word_32544[];
extern unsigned char far *tracksmenushape2dunk[];
extern unsigned char far *word_32540;
extern struct SPRITE far *wndsprite;
extern unsigned char far *td14_elem_map_main;
extern unsigned char far *td15_terr_map_main;
extern unsigned char far *td21_col_from_path;
extern unsigned char far *td22_row_from_path;
extern unsigned char far *mainresptr;
extern int terrainrows[];
extern int trackrows[];
extern int word_45D3E[];
extern int word_35D42[];
extern char resID_byte1[];
extern unsigned char byte_45D90;
extern unsigned char byte_45E16;
extern int video_flag1_is1;
extern int performGraphColor;
extern int dialogarg2;
extern int dialog_fnt_colour;
extern int word_407EC, word_407EE, word_407F0;
extern int word_407F2;
extern int word_407F4, word_407F6, word_407F8;
extern int track_pieces_counter;
extern int mouse_xpos, mouse_ypos;
extern unsigned char mouse_butstate;
extern char g_path_buf[];
extern char byte_3B80C[];
extern unsigned char g_is_busy;

extern unsigned char far * far file_load_shape2d_fatal_thunk(char *name);
extern void far locate_many_resources(unsigned char far *data, char *names, char far **result);
extern unsigned char far * far file_load_resfile(char *name);
extern unsigned char far * far locate_shape_alt(unsigned char far *data, char *name);
extern unsigned char far * far locate_shape_fatal(unsigned char far *data, char *name);
extern char far * far locate_text_res(unsigned char far *data, char *name);
extern struct SPRITE far * far sprite_make_wnd(int width, int height, int flags);
extern void far sprite_copy_wnd_to_1_clear(void);
extern void far draw_button();
extern void far draw_lines_unk();
extern void far sprite_copy_wnd_to_1(void);
extern void far sprite_set_1_size(int left, int right, int top, int bottom);
extern void far sprite_set_1_from_argptr(struct SPRITE far *sprite);
extern int far mouse_track_op();
extern void far sprite_blit_to_video(struct SPRITE far *sprite, int mode);
extern void far preRender_line(int x1, int y1, int x2, int y2, int color);
extern void far sprite_copy_2_to_1_2(void);
extern void far sprite_shape_to_1(void far *shape, int x, int y);
extern void far sprite_clear_shape_alt(void far *shape, int x, int y);
extern void far sprite_putimage_and_alt(void far *shape, int x, int y);
extern void far sprite_putimage_and(void far *shape, int x, int y);
extern void far sprite_putimage_or(void far *shape, int x, int y);
extern void far putpixel_iconMask(void far *shape, int x, int y);
extern void far putpixel_iconFillings(void far *shape, int x, int y);
extern void far mouse_draw_opaque_check(void);
extern void far mouse_draw_transparent_check(void);
extern void far font_set_unk(int colour, int mode);
extern void far copy_string(char *destination, char far *source);
extern int far font_op2(char *name);
extern void far sub_345BC(char *text, int x, int y);
extern void far sprite_1_unk(int x, int y, int width, int height, int color);
extern int far show_dialog();
extern int far timer_get_delta_alt(void);
extern int far input_checking(int delta);
extern char far mouse_multi_hittest(int count, int *x1, int *x2, int *y1, int *y2);
extern void far timer_get_counter_unk(long ticks);
extern char far track_setup(void);
extern void far check_input(void);
extern char far do_fileselect_dialog(char *dir, char *name, char *ext, char far *title);
extern char far do_savefile_dialog(char *dir, char *name, char far *title);
extern void far file_build_path(char *dir, char *name, char *ext, char *dst);
extern void far file_read_fatal(char *path, unsigned char far *buffer);
extern int far file_write_fatal(char *path, unsigned char far *buffer, long size);
extern int far file_find(char *query);
extern void far highscore_write_a(int mode);
extern void far sub_3702E(int x1, int y1, int x2, int y2, int color);
extern void far sprite_free_wnd(struct SPRITE far *sprite);
extern void far unload_resource(unsigned char far *data);
extern void far mmgr_free(unsigned char far *data);
extern unsigned char far subst_hillroad_track(unsigned int terrain, unsigned int element);

char sub_2C81C();
void sub_2C9B4();
void preRender_icons();
void draw_2DtrackMap();

extern char aEokenseieemseedewwefuenpestej[];
extern char aTer0[];
extern unsigned int word_3ECBE[];
extern int trackmenu2_buttons_x1[], trackmenu2_buttons_x2[];
extern int trackmenu2_buttons_y1[], trackmenu2_buttons_y2[];
extern unsigned char byte_3ECFE[];
extern unsigned char byte_3ED00[];
extern char aFlatlakelak1lak2lak3lak4highg[];
extern char aCrs0crs1crs2crs3[];
extern char aUcr0ucr1ucr2ucr3[];

void load_tracks_menu_shapes(void)
{
    char answer;
    unsigned char paletteModified;
    signed char originX;
    signed char objectHeight;
    unsigned char far *mediumTextData;
    int trackStep;
    signed char drawnMode;
    signed char shapeWidth;
    signed char errorMsg;
    unsigned char far *terrainTemplate;
    unsigned char tmpShape;
    signed char boxHeight;
    unsigned char far *shortNames;
    unsigned char far *teditData;
    unsigned char savedShape;
    unsigned char far *sdtBuffer;
    signed char cursorPixW;
    signed char lastType;
    unsigned char pathFlag;
    signed char imageMode;
    signed char paletteArea;
    unsigned char mapChanged;
    int cursorLeft;
    unsigned int key;
    signed char activeGroup;
    signed char tileSize;
    unsigned char lastHoverShape;
    signed char droppedPosY;
    unsigned char terrainCache[132];
    int animationCount;
    int lastTextWidth;
    signed char destPosX;
    unsigned char mapDirty;
    unsigned char far *pieceNames;
    register int j;
    int blinkFlag;
    signed char lastScrollX;
    char far *textPtr;
    unsigned char boxMarker;
    signed char saveOutcome;
    signed char destPosY;
    struct SPRITE far *windows[4];
    unsigned char elementState[132];
    register int stepTime;
    int screenPosY;
    signed char lastPutCol;
    signed char prevViewTop;
    signed char selRow[2];
    unsigned char value;
    unsigned char sliderChanged;
    unsigned char hovered;
    signed char selectCol[2];
    signed char viewTop;
    signed char hitArea;
    unsigned char inEditor;
    unsigned char oldCell;
    unsigned char selectedPiece;


    sdtBuffer = file_load_shape2d_fatal_thunk("sdtedit");
    locate_many_resources(sdtBuffer, aFlatlakelak1lak2lak3lak4highg, word_3283C);
    locate_many_resources(sdtBuffer, aCrs0crs1crs2crs3, (char far **)word_3282C);
    locate_many_resources(sdtBuffer, aUcr0ucr1ucr2ucr3, (char far **)word_32888);
    windows[0] = sprite_make_wnd(word_3282C[0]->s2d_width * video_flag1_is1,
        word_3282C[0]->s2d_height, 15);
    windows[1] = sprite_make_wnd(word_3282C[1]->s2d_width * video_flag1_is1,
        word_3282C[1]->s2d_height, 15);
    windows[2] = sprite_make_wnd(word_3282C[2]->s2d_width * video_flag1_is1,
        word_3282C[2]->s2d_height, 15);
    windows[3] = sprite_make_wnd(word_3282C[3]->s2d_width * video_flag1_is1,
        word_3282C[3]->s2d_height, 15);
    teditData = file_load_resfile("tedit");
    wndsprite = sprite_make_wnd(320, 200, 15);
    word_32540 = locate_shape_alt(teditData, "pbox");
    shortNames = locate_shape_alt(teditData, "snam");
    mediumTextData = locate_shape_alt(teditData, "mnam");
    pieceNames = locate_shape_alt(teditData, "tnam");
    mapDirty = 0;
    for (j = 0; j < 132; ++j) {
        elementState[j] = 0xff;
        terrainCache[j] = 0xff;
    }
    for (j = 0; j < 186; ++j) {
        textPtr = shortNames + j * 4;
        resID_byte1[0] = textPtr[0];
        resID_byte1[1] = textPtr[1];
        resID_byte1[2] = textPtr[2];
        resID_byte1[3] = textPtr[3];
        tracksmenushape2dunk[j] = locate_shape_fatal(sdtBuffer, resID_byte1);
        textPtr = mediumTextData + j * 4;
        resID_byte1[0] = textPtr[0];
        resID_byte1[1] = textPtr[1];
        resID_byte1[2] = textPtr[2];
        resID_byte1[3] = textPtr[3];
        word_32544[j] = locate_shape_fatal(sdtBuffer, resID_byte1);
    }

    lastPutCol = -1;
    lastType = -1;
    mapChanged = 1;
    sliderChanged = 1;
    paletteModified = 1;
    imageMode = -1;
    inEditor = 1;
    activeGroup = 1;
    pathFlag = 1;
    lastTextWidth = 0;
    paletteArea = 0;
    selectedPiece = 0;
    trackStep = 0;
    selectCol[1] = 0;
    originX = 0;
    viewTop = 0;
    lastHoverShape = 0;
    errorMsg = 0;
    selectCol[0] = byte_45D90;
    selRow[0] = byte_45E16;
    selRow[1] = 7;

    sprite_copy_wnd_to_1_clear();
    draw_button(locate_text_res(teditData, "bti"), 0xd9, 3, 0x66, 0x16,
        word_407F4, word_407F6, word_407F8, 0);
    draw_lines_unk(5, 0, 0xce, 0xbe, word_407EC, word_407EE, word_407F0);
    draw_lines_unk(0xd9, 0x20, 0x66, 0x9e, word_407EC, word_407EE, word_407F0);
    draw_button(locate_text_res(teditData, "bsc"), 0xdd, 0x8c, 0x5e, 0x0e,
        word_407F4, word_407F6, word_407F8, 0);
    draw_button(locate_text_res(teditData, "blo"), 0xdd, 0x9c, 0x2e, 0x0e,
        word_407F4, word_407F6, word_407F8, 0);
    draw_button(locate_text_res(teditData, "bsa"), 0xdd, 0xac, 0x2e, 0x0e,
        word_407F4, word_407F6, word_407F8, 0);
    draw_button(locate_text_res(teditData, "bcl"), 0x10d, 0x9c, 0x2e, 0x0e,
        word_407F4, word_407F6, word_407F8, 0);
    draw_button(locate_text_res(teditData, "bex"), 0x10d, 0xac, 0x2e, 0x0e,
        word_407F4, word_407F6, word_407F8, 0);

    do {
nextFrame:
        if (paletteModified || activeGroup != lastType) {
            shapeWidth = 1;
            objectHeight = 1;
            tileSize = 0;
            if (activeGroup != 0) {
                switch (trkObjectList[selectedPiece].ss_multiTileFlag) {
                case 1:
                    objectHeight = 2;
                    tileSize = 1;
                    break;
                case 2:
                    tileSize = 2;
                    shapeWidth = 2;
                    break;
                case 3:
                    shapeWidth = 2;
                    objectHeight = 2;
                    tileSize = 3;
                    break;
                }
            }
        }
        if (!paletteArea) {
            if (selectCol[0] == 29 && shapeWidth == 2)
                --selectCol[0];
            if (selRow[0] == 29 && objectHeight == 2)
                --selRow[0];
            while (selectCol[0] - originX + shapeWidth > 12)
                ++originX;
            while (selectCol[0] - originX < 0)
                --originX;
            while (selRow[0] - viewTop + objectHeight > 11)
                ++viewTop;
            while (selRow[0] - viewTop < 0)
                --viewTop;
            if (originX != lastScrollX || viewTop != prevViewTop) {
                lastScrollX = originX;
                prevViewTop = viewTop;
                mapChanged = 1;
                sliderChanged = 1;
            }
        }
        if (lastType != activeGroup) {
            paletteModified = 1;
            lastType = activeGroup;
            while (word_32540[selRow[1] * 6 + activeGroup * 36 + selectCol[1]] >= 0xfe) {
                if (word_32540[selRow[1] * 6 + activeGroup * 36 + selectCol[1]] == 0xff)
                    --selectCol[1];
                else
                    --selRow[1];
            }
            sprite_copy_wnd_to_1();
            preRender_icons(activeGroup);
            if (activeGroup == 0)
                mouse_track_op(0, 0xdd, 0x5f, 0x85, 5, 0, 1, 1);
            else
                mouse_track_op(0, 0xdd, 0x5f, 0x85, 5, activeGroup - 1, 1, 10);
        }
        if (pathFlag) {
            pathFlag = 0;
            errorMsg = sub_2C81C();
        }
        if (mapChanged || paletteModified) {
            sprite_copy_wnd_to_1();
            if (mapChanged) {
                mapChanged = 0;
                if (sliderChanged) {
                    sliderChanged = 0;
                    mouse_track_op(0, 9, 0xc0, 0xb5, 5, originX, 0x0c, 0x1e);
                    mouse_track_op(0, 0xca, 5, 4, 0xb0, viewTop, 0x0b, 0x1e);
                }
                sprite_set_1_size(8, 0xc8, 4, 0xb3);
                draw_2DtrackMap(originX, viewTop, elementState, terrainCache);
                sprite_set_1_size(0, 0x140, 0, 0xc8);
            }
            if (paletteModified) {
                paletteModified = 0;
                sprite_set_1_from_argptr(windows[tileSize]);
                if (activeGroup == 0) {
                    sprite_shape_to_1(word_3283C[selectedPiece], 0, 0);
                    preRender_line(1, 0, 15, 0, performGraphColor);
                    preRender_line(1, 14, 15, 14, performGraphColor);
                    preRender_line(1, 0, 1, 14, performGraphColor);
                    preRender_line(15, 0, 15, 14, performGraphColor);
                } else {
                    sprite_shape_to_1(word_3282C[tileSize], 0, 0);
                    if (selectedPiece != 0) {
                        putpixel_iconMask(word_32544[selectedPiece], 0, 0);
                        putpixel_iconFillings(tracksmenushape2dunk[selectedPiece], 0, 0);
                    }
                }
            }
            sprite_blit_to_video(wndsprite, imageMode);
            imageMode = -2;
            lastHoverShape = 0xff;
        }

        sprite_copy_2_to_1_2();
        if (!paletteArea) {
            cursorPixW = shapeWidth << 4;
            boxHeight = objectHeight << 4;
            cursorLeft = ((selectCol[0] - originX) << 4) + 8;
            screenPosY = ((selRow[0] - viewTop) << 4) + 4;
            hovered = td14_elem_map_main[trackrows[selRow[0]] + selectCol[0]];
            switch (hovered) {
            case 0xfd:
                hovered = td14_elem_map_main[word_45D3E[selRow[0]] + selectCol[0] - 1];
                break;
            case 0xfe:
                hovered = td14_elem_map_main[word_45D3E[selRow[0]] + selectCol[0]];
                break;
            case 0xff:
                hovered = td14_elem_map_main[trackrows[selRow[0]] + selectCol[0] - 1];
                break;
            }
        } else {
            cursorPixW = 0x10;
            boxHeight = 0x10;
            screenPosY = (selRow[1] << 4) + 0x24;
            if (selRow[1] == 6) {
                cursorLeft = 0xdc;
                boxHeight = 8;
                cursorPixW = 0x60;
            } else if (selRow[1] == 7) {
                screenPosY -= 8;
                selectCol[1] = 0;
                cursorLeft = 0xdc;
                cursorPixW = 0x60;
                hovered = 0;
            } else if (selRow[1] > 7) {
                screenPosY -= 8;
                if (selectCol[1] < 3)
                    selectCol[1] = 0;
                else
                    selectCol[1] = 3;
                cursorLeft = (selectCol[1] << 4) + 0xdc;
                cursorPixW = 0x30;
                hovered = 0;
            } else {
                cursorLeft = (selectCol[1] << 4) + 0xdc;
                if (selRow[1] < 5 &&
                        word_32540[selRow[1] * 6 + activeGroup * 36 + selectCol[1] + 6] == 0xfe)
                    boxHeight = 0x20;
                if (selectCol[1] < 5 &&
                        word_32540[selRow[1] * 6 + activeGroup * 36 + selectCol[1] + 1] == 0xff)
                    cursorPixW = 0x20;
                hovered = word_32540[selRow[1] * 6 + activeGroup * 36 + selectCol[1]];
                if (hovered >= 0xfd)
                    hovered = 0;
            }
            if (activeGroup == 0)
                hovered = 0;
        }

        if (hovered != lastHoverShape) {
            mouse_draw_opaque_check();
            font_set_unk(dialog_fnt_colour, 0);
            textPtr = pieceNames + hovered * 3;
            resID_byte1[0] = textPtr[0];
            resID_byte1[1] = textPtr[1];
            resID_byte1[2] = textPtr[2];
            copy_string(resID_byte1, locate_text_res(teditData, resID_byte1));
            j = font_op2(resID_byte1);
            sub_345BC(resID_byte1, 8, 0xc0);
            if (lastTextWidth > j)
                sprite_1_unk(j + 8, 0xc0, lastTextWidth - j, 8, 0);
            mouse_draw_transparent_check();
            lastTextWidth = j;
            lastHoverShape = hovered;
        }
        if (errorMsg) {
            show_dialog(1, 1, locate_text_res(teditData, aEokenseieemseedewwefuenpestej + errorMsg * 3),
                -1, -1, performGraphColor, 0, 0);
            errorMsg = 0;
        }

        animationCount = 99;
        blinkFlag = 0;
        mouse_draw_opaque_check();
        if ((drawnMode = paletteArea) == 0)
            sprite_clear_shape_alt(word_32888[tileSize], cursorLeft, screenPosY);
        do {
            if (animationCount > 15) {
                mouse_draw_opaque_check();
                if (!paletteArea) {
                    if (blinkFlag)
                        sprite_shape_to_1(word_32888[tileSize], cursorLeft, screenPosY);
                    else
                        sprite_shape_to_1(windows[tileSize]->sprite_bitmapptr, cursorLeft, screenPosY);
                } else {
                    sub_3702E(cursorLeft, screenPosY - 1, cursorLeft + cursorPixW,
                        screenPosY + boxHeight - 1, word_407F2);
                }
                mouse_draw_transparent_check();
                blinkFlag ^= 1;
                animationCount = 0;
            }
            stepTime = timer_get_delta_alt();
            animationCount += stepTime;
            key = input_checking(stepTime);
            hitArea = mouse_multi_hittest(5, trackmenu2_buttons_x1, trackmenu2_buttons_x2,
                trackmenu2_buttons_y1, trackmenu2_buttons_y2);
            if (hitArea != -1) {
                switch (hitArea) {
                case 0:
                    if (mouse_butstate & 3) {
                        paletteArea = 0;
                        value = mouse_track_op(1, 9, 0xc0, 0xb5, 5, originX, 0x0c, 0x1e);
                        selectCol[0] += value - originX;
                        originX = value;
                        key = 1;
                    }
                    break;
                case 1:
                    if (mouse_butstate & 3) {
                        paletteArea = 0;
                        value = mouse_track_op(1, 0xca, 5, 4, 0xb0, viewTop, 0x0b, 0x1e);
                        selRow[0] += value - viewTop;
                        viewTop = value;
                        key = 1;
                    }
                    break;
                case 2:
                    if (paletteArea != 1 || selRow[1] != 6) {
                        paletteArea = 1;
                        selRow[1] = 6;
                        key = 1;
                    }
                    if (mouse_butstate & 3) {
                        activeGroup = mouse_track_op(1, 0xdd, 0x5f, 0x85, 5, activeGroup - 1, 1, 10) + 1;
                        key = 1;
                    }
                    break;
                case 3:
                    destPosX = (mouse_xpos - 8) / 16;
                    destPosY = (mouse_ypos - 4) / 16;
                    if (activeGroup != 0) {
                        if (destPosY == 10 && (trkObjectList[selectedPiece].ss_multiTileFlag & 1))
                            --destPosY;
                        if (destPosX == 11 && (trkObjectList[selectedPiece].ss_multiTileFlag & 2))
                            --destPosX;
                    }
                    destPosX += originX;
                    destPosY += viewTop;
                    if (paletteArea || selectCol[0] != destPosX || selRow[0] != destPosY) {
                        paletteArea = 0;
                        selectCol[0] = destPosX;
                        selRow[0] = destPosY;
                        key = 1;
                    }
                    if (key == 0x20)
                        key = 0x0d;
                    break;
                case 4:
                    destPosX = (mouse_xpos - 0xdc) / 16;
                    destPosY = (mouse_ypos - 0x24) / 16;
                    if (destPosY < 6) {
                        if (word_32540[activeGroup * 36 + destPosY * 6 + destPosX] == 0xfe)
                            --destPosY;
                        if (word_32540[activeGroup * 36 + destPosY * 6 + destPosX] == 0xff)
                            --destPosX;
                    } else {
                        destPosY = (mouse_ypos - 0x1c) / 16;
                        if (destPosY == 7) {
                            destPosX = 0;
                            goto paletteClick;
                        } else if (destPosX < 3) {
                            destPosX = 0;
                            goto paletteClick;
                        }
                        destPosX = 3;
                        goto paletteClick;
                    }
paletteClick:
                    if (!paletteArea || selectCol[1] != destPosX || selRow[1] != destPosY) {
                        selectCol[1] = destPosX;
                        selRow[1] = destPosY;
                        paletteArea = 1;
                        key = 1;
                    }
                    if (key == 0x20)
                        key = 0x0d;
                    break;
                }
                if (key == 1)
                    lastPutCol = -1;
            }
            if (key == 0 && trackStep != 0)
                key = 1;
        } while (key == 0);
        if (trackStep != 0)
            timer_get_counter_unk(10L);
        if (blinkFlag) {
            mouse_draw_opaque_check();
            if (!drawnMode)
                sprite_shape_to_1(word_32888[tileSize], cursorLeft, screenPosY);
            else
                sub_3702E(cursorLeft, screenPosY - 1, cursorLeft + cursorPixW,
                    screenPosY + boxHeight - 1, word_407F2);
            mouse_draw_transparent_check();
        }

        if (trackStep != 0) {
            if (key != 1 || paletteArea)
                trackStep = track_pieces_counter - 1;
            selectCol[0] = td21_col_from_path[trackStep];
            selRow[0] = td22_row_from_path[trackStep];
            selectedPiece = td14_elem_map_main[trackrows[selRow[0]] + selectCol[0]];
            mapChanged = 1;
            paletteModified = 1;
            if (++trackStep < track_pieces_counter)
                goto nextFrame;
            selectedPiece = savedShape;
            trackStep = 0;
            goto nextFrame;
        }
        trackStep = 0;
        for (j = 0; j < 10; ++j) {
            if (word_3ECBE[j] == key) {
                activeGroup = j + 1;
                key = 0;
                break;
            }
        }

        switch (key) {
        case 0x20:
        case 0x5200:
            paletteArea ^= 1;
            break;
        case '-':
            if (activeGroup > 1)
                --activeGroup;
            break;
        case '+':
            if (activeGroup < 10)
                ++activeGroup;
            break;
        case 0x5400:
            activeGroup = 0;
            selectedPiece = 0;
            break;
        case 'c':
        case 'C':
            j = track_setup();
            show_dialog(1, 1, locate_text_res(teditData, aEokenseieemseedewwefuenpestej + j * 3),
                -1, -1, performGraphColor, 0, 0);
            if (j > 1) {
                paletteArea = 0;
                if (track_pieces_counter == 0) {
                    selectCol[0] = byte_45D90;
                    selRow[0] = byte_45E16;
                } else {
                    selectCol[0] = td21_col_from_path[0];
                    selRow[0] = td22_row_from_path[0];
                    savedShape = selectedPiece;
                    selectedPiece = td14_elem_map_main[trackrows[selRow[0]] + selectCol[0]];
                    trackStep = 1;
                    paletteModified = 1;
                }
            }
            check_input();
            break;
        case 0x0d:
            if (paletteArea) {
                if (selRow[1] < 6) {
                    selectedPiece = word_32540[selRow[1] * 6 + activeGroup * 36 + selectCol[1]];
                    if (activeGroup != 0) {
                        if ((trkObjectList[selectedPiece].ss_multiTileFlag & 1) &&
                                selRow[0] - viewTop == 10)
                            --selRow[0];
                        if ((trkObjectList[selectedPiece].ss_multiTileFlag & 2) &&
                                selectCol[0] - originX == 11)
                            --selectCol[0];
                    }
                    ++paletteModified;
                    paletteArea = 0;
                } else {
                    pathFlag = 1;
                    if (selRow[1] == 6) {
                        if (++activeGroup > 10)
                            activeGroup = 1;
                    } else if (selRow[1] == 7) {
                        answer = show_dialog(2, 1, locate_text_res(teditData, "mss"),
                            -1, -1, dialogarg2, 0, td14_elem_map_main[0x384]);
                        if (answer != -1 && answer != 5) {
                            td14_elem_map_main[0x384] = answer;
                            ++mapChanged;
                            mapDirty = 1;
                        }
                    } else if (selRow[1] == 8 && selectCol[1] != 0) {
                        answer = show_dialog(2, 1, locate_text_res(teditData, "men"),
                            -1, -1, dialogarg2, 0, 0);
                        if (answer != -1 && answer != 5) {
                            for (j = 0; j < 0x384; ++j)
                                td14_elem_map_main[j] = 0;
                            aTer0[3] = answer + '0';
                            terrainTemplate = locate_shape_alt(teditData, aTer0);
                            for (j = 0; j < 0x385; ++j)
                                td15_terr_map_main[j] = terrainTemplate[j];
                            gameconfig.game_trackname[0] = 0;
                            ++mapChanged;
                            mapDirty = 1;
                        }
                    } else if (selRow[1] == 8 && selectCol[1] == 0) {
                        sprite_copy_2_to_1_2();
                        if (mapDirty && (j = show_dialog(2, 1, locate_text_res(teditData, "chl"),
                                -1, -1, performGraphColor, 0, 0)) == 0)
                            goto save;
                        j = 1;
                        g_is_busy = 1;
                        ++mapChanged;
                        j = do_fileselect_dialog(byte_3B80C, gameconfig.game_trackname, ".trk",
                            locate_text_res(mainresptr, "trk"));
                        file_build_path(byte_3B80C, gameconfig.game_trackname, ".trk", g_path_buf);
                        if (j > 0) {
                            file_read_fatal(g_path_buf, td14_elem_map_main);
                            track_setup();
                            paletteArea = 0;
                            selRow[0] = byte_45E16;
                            selectCol[0] = byte_45D90;
                            mapDirty = 0;
                            ++mapChanged;
                        }
                        g_is_busy = 0;
                    } else if (selectCol[1] == 0) {
save:
                        saveOutcome = 0;
                        g_is_busy = 1;
                        while (saveOutcome == 0) {
                            sprite_copy_2_to_1_2();
                            ++mapChanged;
                            if (do_savefile_dialog(byte_3B80C, gameconfig.game_trackname,
                                    locate_text_res(mainresptr, "trk"))) {
                                file_build_path(byte_3B80C, gameconfig.game_trackname, ".trk",
                                    g_path_buf);
                                saveOutcome = 1;
                                if (file_find(g_path_buf)) {
                                    j = show_dialog(2, 1, locate_text_res(mainresptr, "fex"),
                                        -1, -1, performGraphColor, 0, 0);
                                    if (j == -1)
                                        saveOutcome = -1;
                                    else if (j == 0)
                                        saveOutcome = 0;
                                }
                            } else
                                saveOutcome = -1;
                            if (saveOutcome == 1) {
                                j = file_write_fatal(g_path_buf, td14_elem_map_main, 0x70aL);
                                if (j == 0)
                                    highscore_write_a(1);
                                if (j != 0) {
                                    show_dialog(1, 1, locate_text_res(mainresptr, "ser"),
                                        -1, -1, performGraphColor, 0, 0);
                                    saveOutcome = 0;
                                } else
                                    mapDirty = 0;
                            }
                        }
                        g_is_busy = 0;
                    } else {
                        if (mapDirty && (j = show_dialog(2, 1, locate_text_res(teditData, "chx"),
                                -1, -1, performGraphColor, 0, 0)) == 0)
                            goto save;
                        inEditor = 0;
                    }
                }
            } else if (activeGroup == 0) {
                if (selectCol[0] == lastPutCol && selRow[0] == droppedPosY) {
                    tmpShape = selectedPiece;
                    selectedPiece = oldCell;
                    oldCell = tmpShape;
                    ++paletteModified;
                } else {
                    oldCell = td15_terr_map_main[terrainrows[selRow[0]] + selectCol[0]];
                    lastPutCol = selectCol[0];
                    droppedPosY = selRow[0];
                }
                td15_terr_map_main[terrainrows[droppedPosY] + lastPutCol] = selectedPiece;
                mapDirty = 1;
                pathFlag = 1;
                ++mapChanged;
            } else if (!((trkObjectList[selectedPiece].ss_multiTileFlag & 1) && selRow[0] > 28) &&
                    !((trkObjectList[selectedPiece].ss_multiTileFlag & 2) && selectCol[0] > 28)) {
                if (selectCol[0] == lastPutCol && selRow[0] == droppedPosY) {
                    tmpShape = selectedPiece;
                    selectedPiece = oldCell;
                    oldCell = tmpShape;
                    ++paletteModified;
                } else {
                    oldCell = td14_elem_map_main[trackrows[selRow[0]] + selectCol[0]];
                    if (oldCell >= 0xfd)
                        oldCell = 0;
                    lastPutCol = selectCol[0];
                    droppedPosY = selRow[0];
                }
                td14_elem_map_main[trackrows[droppedPosY] + lastPutCol] = selectedPiece;
                mapDirty = 1;
                pathFlag = 1;
                ++mapChanged;
                switch (trkObjectList[selectedPiece].ss_multiTileFlag) {
                case 1:
                    td14_elem_map_main[word_35D42[droppedPosY] + lastPutCol] = 0xfe;
                    break;
                case 2:
                    td14_elem_map_main[trackrows[droppedPosY] + lastPutCol + 1] = 0xff;
                    break;
                case 3:
                    td14_elem_map_main[trackrows[droppedPosY] + lastPutCol + 1] = 0xff;
                    td14_elem_map_main[word_35D42[droppedPosY] + lastPutCol] = 0xfe;
                    td14_elem_map_main[word_35D42[droppedPosY] + lastPutCol + 1] = 0xfd;
                    break;
                }
            }
            check_input();
            break;
        case 0x4700:
            if (paletteArea) {
                selRow[1] = 0;
                selectCol[1] = 0;
            } else {
                if (selRow[0] == viewTop && selectCol[0] == originX) {
                    originX = 0;
                    viewTop = 0;
                }
                selRow[0] = viewTop;
                selectCol[0] = originX;
            }
            break;
        case 0x4800:
            if (selRow[paletteArea] != 0) {
                lastPutCol = -1;
                --selRow[paletteArea];
                if (paletteArea && selRow[1] < 6) {
                    while (word_32540[selRow[1] * 6 + activeGroup * 36 + selectCol[1]] >= 0xfe) {
                        boxMarker = word_32540[selRow[1] * 6 + activeGroup * 36 + selectCol[1]];
                        if (boxMarker == 0xff)
                            --selectCol[1];
                        else if (boxMarker == 0xfe)
                            --selRow[1];
                    }
                }
            }
            break;
        case 0x5000:
            if (selRow[paletteArea] < byte_3ED00[paletteArea]) {
                lastPutCol = -1;
                ++selRow[paletteArea];
                if (paletteArea && selRow[1] < 6) {
                    boxMarker = word_32540[selRow[1] * 6 + activeGroup * 36 + selectCol[1]];
                    if (boxMarker == 0xff)
                        --selectCol[1];
                    else if (boxMarker == 0xfe)
                        ++selRow[1];
                }
            }
            break;
        case 0x4b00:
            if (paletteArea && selRow[1] == 6) {
                if (activeGroup > 1)
                    --activeGroup;
            } else if (selectCol[paletteArea] != 0) {
                lastPutCol = -1;
                --selectCol[paletteArea];
                if (paletteArea) {
                    if (selRow[1] > 5)
                        selectCol[1] = 0;
                    else
                        while (word_32540[selRow[1] * 6 + activeGroup * 36 + selectCol[1]] >= 0xfe) {
                            boxMarker = word_32540[selRow[1] * 6 + activeGroup * 36 + selectCol[1]];
                            if (boxMarker == 0xff)
                                --selectCol[1];
                            else if (boxMarker == 0xfe)
                                --selRow[1];
                        }
                }
            }
            break;
        case 0x4d00:
            if (paletteArea && selRow[1] == 6) {
                if (activeGroup < 10)
                    ++activeGroup;
            } else {
                boxMarker = 1;
                if (paletteArea) {
                    if (selRow[1] > 5)
                        boxMarker = 3;
                    else
                        while (selectCol[paletteArea] + boxMarker < byte_3ECFE[paletteArea] &&
                                word_32540[selRow[1] * 6 + activeGroup * 36 + selectCol[1] + boxMarker] >= 0xfe) {
                            value = word_32540[selRow[1] * 6 + activeGroup * 36 + selectCol[1] + boxMarker];
                            if (value == 0xff)
                                ++boxMarker;
                            else if (value == 0xfe)
                                --selRow[1];
                        }
                }
                if (selectCol[paletteArea] + boxMarker < byte_3ECFE[paletteArea]) {
                    lastPutCol = -1;
                    selectCol[paletteArea] += boxMarker;
                }
            }
            break;
        }
    } while (inEditor);

    sprite_free_wnd(wndsprite);
    sprite_free_wnd(windows[3]);
    sprite_free_wnd(windows[2]);
    sprite_free_wnd(windows[1]);
    sprite_free_wnd(windows[0]);
    unload_resource(teditData);
    mmgr_free(sdtBuffer);
}

void preRender_icons(unsigned char mode)
{
    unsigned char iconIndex;
    unsigned char row;
    unsigned char stateId;
    for (iconIndex = 0; iconIndex < 6; ++iconIndex) {
        for (row = 0; row < 6; ++row) {
            stateId = word_32540[mode * 36 + iconIndex * 6 + row];
            if (mode == 0) {
                sprite_shape_to_1(word_3283C[stateId], 220 + (row << 4),
                    36 + (iconIndex << 4));
            } else if (stateId < 0xfd) {
                sprite_shape_to_1(word_3283C[0], 220 + (row << 4),
                    36 + (iconIndex << 4));
                switch (trkObjectList[stateId].ss_multiTileFlag) {
                case 1:
                    sprite_shape_to_1(word_3283C[0], 220 + (row << 4),
                        52 + (iconIndex << 4));
                    break;
                case 2:
                    sprite_shape_to_1(word_3283C[0], 236 + (row << 4),
                        36 + (iconIndex << 4));
                    break;
                case 3:
                    sprite_shape_to_1(word_3283C[0], 236 + (row << 4),
                        36 + (iconIndex << 4));
                    sprite_shape_to_1(word_3283C[0], 220 + (row << 4),
                        52 + (iconIndex << 4));
                    sprite_shape_to_1(word_3283C[0], 236 + (row << 4),
                        52 + (iconIndex << 4));
                    break;
                }
                putpixel_iconMask(word_32544[stateId], 220 + (row << 4),
                    36 + (iconIndex << 4));
                putpixel_iconFillings(tracksmenushape2dunk[stateId],
                    220 + (row << 4), 36 + (iconIndex << 4));
            }
        }
    }
}

void draw_2DtrackMap(unsigned char rowBase, unsigned char columnBase, unsigned char *lastElement, unsigned char *lastTerrain)
{
    unsigned int rowIndex;
    signed char columnIndex;
    signed char mapRow;
    unsigned int mapIndex;
    unsigned char surface;
    unsigned char tileId;

    for (mapRow = 0; mapRow < 11; ++mapRow) {
        rowIndex = mapRow * 12;
        for (columnIndex = 0; columnIndex < 12; ++columnIndex) {
            tileId = td14_elem_map_main[trackrows[columnBase + mapRow] + columnIndex + rowBase];
            surface = td15_terr_map_main[terrainrows[columnBase + mapRow] + columnIndex + rowBase];
            mapIndex = rowIndex + columnIndex;

            if (tileId >= 0xfd && (mapRow == 0 || columnIndex == 0)) {
                lastElement[mapIndex] = 0xff;
                if (tileId == 0xff && columnIndex == 0) {
                    sprite_putimage_and_alt(word_3283C[td15_terr_map_main[terrainrows[columnBase + mapRow] + columnIndex + rowBase]],
                        (columnIndex << 4) + 8, (mapRow << 4) + 4);
                    sprite_putimage_and_alt(word_3283C[td15_terr_map_main[terrainrows[columnBase + mapRow + 1] + columnIndex + rowBase]],
                        (columnIndex << 4) + 8, (mapRow << 4) + 20);
                    sprite_putimage_and(word_32544[td14_elem_map_main[trackrows[columnBase + mapRow] + columnIndex + rowBase - 1]],
                        (columnIndex << 4) - 8, (mapRow << 4) + 4);
                    sprite_putimage_or(tracksmenushape2dunk[td14_elem_map_main[trackrows[columnBase + mapRow] + columnIndex + rowBase - 1]],
                        (columnIndex << 4) - 8, (mapRow << 4) + 4);
                } else if (tileId == 0xfe && mapRow == 0) {
                    sprite_putimage_and_alt(word_3283C[td15_terr_map_main[terrainrows[columnBase + mapRow] + columnIndex + rowBase]],
                        (columnIndex << 4) + 8, (mapRow << 4) + 4);
                    sprite_putimage_and_alt(word_3283C[td15_terr_map_main[terrainrows[columnBase + mapRow] + columnIndex + rowBase + 1]],
                        (columnIndex << 4) + 24, (mapRow << 4) + 4);
                    sprite_putimage_and(word_32544[td14_elem_map_main[word_45D3E[columnBase + mapRow] + columnIndex + rowBase]],
                        (columnIndex << 4) + 8, (mapRow << 4) - 12);
                    sprite_putimage_or(tracksmenushape2dunk[td14_elem_map_main[word_45D3E[columnBase + mapRow] + columnIndex + rowBase]],
                        (columnIndex << 4) + 8, (mapRow << 4) - 12);
                } else if (tileId == 0xfd && mapRow == 0 && columnIndex == 0) {
                    sprite_putimage_and_alt(word_3283C[td15_terr_map_main[terrainrows[columnBase + mapRow] + columnIndex + rowBase]],
                        (columnIndex << 4) + 8, (mapRow << 4) + 4);
                    sprite_putimage_and(word_32544[td14_elem_map_main[word_45D3E[columnBase + mapRow] + columnIndex + rowBase - 1]],
                        (columnIndex << 4) - 8, (mapRow << 4) - 12);
                    sprite_putimage_or(tracksmenushape2dunk[td14_elem_map_main[word_45D3E[columnBase + mapRow] + columnIndex + rowBase - 1]],
                        (columnIndex << 4) - 8, (mapRow << 4) - 12);
                }
            } else if (tileId == 0) {
                if (lastElement[mapIndex] != 0 || lastTerrain[mapIndex] != surface) {
                    sprite_shape_to_1(word_3283C[surface], (columnIndex << 4) + 8, (mapRow << 4) + 4);
                    lastElement[mapIndex] = 0;
                    lastTerrain[mapIndex] = surface;
                }
            } else if (tileId < 0xfd) {
                if (lastElement[mapIndex] != tileId || lastTerrain[mapIndex] != surface) {
                    lastElement[mapIndex] = tileId;
                    lastTerrain[mapIndex] = surface;
                    sprite_shape_to_1(word_3283C[surface], (columnIndex << 4) + 8, (mapRow << 4) + 4);
                    switch (trkObjectList[tileId].ss_multiTileFlag) {
                    case 0:
                        putpixel_iconMask(word_32544[tileId], (columnIndex << 4) + 8, (mapRow << 4) + 4);
                        putpixel_iconFillings(tracksmenushape2dunk[tileId], (columnIndex << 4) + 8, (mapRow << 4) + 4);
                        break;
                    case 1:
                        sprite_putimage_and_alt(word_3283C[td15_terr_map_main[terrainrows[columnBase + mapRow + 1] + columnIndex + rowBase]],
                            (columnIndex << 4) + 8, (mapRow << 4) + 20);
                        sprite_putimage_and(word_32544[tileId], (columnIndex << 4) + 8, (mapRow << 4) + 4);
                        sprite_putimage_or(tracksmenushape2dunk[tileId], (columnIndex << 4) + 8, (mapRow << 4) + 4);
                        break;
                    case 2:
                        sprite_putimage_and_alt(word_3283C[td15_terr_map_main[terrainrows[columnBase + mapRow] + columnIndex + rowBase + 1]],
                            (columnIndex << 4) + 24, (mapRow << 4) + 4);
                        sprite_putimage_and(word_32544[tileId], (columnIndex << 4) + 8, (mapRow << 4) + 4);
                        sprite_putimage_or(tracksmenushape2dunk[tileId], (columnIndex << 4) + 8, (mapRow << 4) + 4);
                        break;
                    case 3:
                        sprite_putimage_and_alt(word_3283C[td15_terr_map_main[terrainrows[columnBase + mapRow] + columnIndex + rowBase + 1]],
                            (columnIndex << 4) + 24, (mapRow << 4) + 4);
                        sprite_putimage_and_alt(word_3283C[td15_terr_map_main[terrainrows[columnBase + mapRow + 1] + columnIndex + rowBase]],
                            (columnIndex << 4) + 8, (mapRow << 4) + 20);
                        sprite_putimage_and_alt(word_3283C[td15_terr_map_main[terrainrows[columnBase + mapRow + 1] + columnIndex + rowBase + 1]],
                            (columnIndex << 4) + 24, (mapRow << 4) + 20);
                        sprite_putimage_and(word_32544[tileId], (columnIndex << 4) + 8, (mapRow << 4) + 4);
                        sprite_putimage_or(tracksmenushape2dunk[tileId], (columnIndex << 4) + 8, (mapRow << 4) + 4);
                        break;
                    }
                }
            } else {
                lastElement[mapIndex] = 0xff;
                lastTerrain[mapIndex] = 0xff;
            }
        }
    }
}

char sub_2C81C(void)
{
    unsigned char elem, colidx;
    unsigned char terrain, rowno;
    char error;

    sub_2C9B4();
    error = 0;
    for (rowno = 0; rowno < 30; ++rowno) {
        for (colidx = 0; colidx < 30; ++colidx) {
            terrain = td15_terr_map_main[terrainrows[rowno] + colidx];
            elem = td14_elem_map_main[trackrows[rowno] + colidx];
            if (elem != 0 && terrain != 0 && terrain != 6) {
                switch (terrain) {
                case 1: case 2: case 3: case 4: case 5:
                    if (elem == 0xff)
                        elem = td14_elem_map_main[trackrows[rowno] + colidx - 1];
                    else if (elem == 0xfe)
                        elem = td14_elem_map_main[word_45D3E[rowno] + colidx];
                    else if (elem == 0xfd)
                        elem = td14_elem_map_main[word_45D3E[rowno] + colidx - 1];
                    switch (elem) {
                    case 0x22: case 0x23:
                    case 0x67: case 0x68: case 0x69: case 0x6a: case 0x6b: case 0x6c:
                    case 0xab: case 0xac: case 0xad: case 0xae:
                        break;
                    default:
                        td14_elem_map_main[trackrows[rowno] + colidx] = 0;
                        error = 12;
                    }
                    break;
                case 7: case 8: case 9: case 10:
                    if (!subst_hillroad_track(terrain, elem)) {
                        td14_elem_map_main[trackrows[rowno] + colidx] = 0;
                        error = 13;
                    }
                    break;
                default:
                    error = 14;
                    td14_elem_map_main[trackrows[rowno] + colidx] = 0;
                }
            }
        }
    }
    if (error != 0) sub_2C9B4();
    return error;
}

void sub_2C9B4(void)
{
    unsigned char used[900];
    unsigned char rowIdx;
    unsigned char x;
    unsigned char element;
    register int clear;

    for (clear = 0; clear < 900; ++clear)
        used[clear] = 0;

    for (rowIdx = 0; rowIdx < 30; ++rowIdx) {
        for (x = 0; x < 30; ++x) {
            element = td14_elem_map_main[trackrows[rowIdx] + x];
            if (element != 0) {
                if (element >= 0xfd) {
                    if (used[trackrows[rowIdx] + x] == 0)
                        td14_elem_map_main[trackrows[rowIdx] + x] = 0;
                } else {
                    switch (trkObjectList[element].ss_multiTileFlag) {
                    case 1:
                        if (used[word_35D42[rowIdx] + x] != 0)
                            td14_elem_map_main[trackrows[rowIdx] + x] = 0;
                        else if (td14_elem_map_main[word_35D42[rowIdx] + x] != 0xfe)
                            td14_elem_map_main[trackrows[rowIdx] + x] = 0;
                        else
                            used[word_35D42[rowIdx] + x] = 1;
                        break;
                    case 2:
                        if (used[trackrows[rowIdx] + x + 1] != 0) {
                            td14_elem_map_main[trackrows[rowIdx] + x] = 0;
                        } else {
                            if (td14_elem_map_main[trackrows[rowIdx] + x + 1] != 0xff)
                                td14_elem_map_main[trackrows[rowIdx] + x] = 0;
                            else
                                used[trackrows[rowIdx] + x + 1] = 1;
                        }
                        break;
                    case 3:
                        if (used[word_35D42[rowIdx] + x + 1] +
                            used[trackrows[rowIdx] + x + 1] +
                            used[word_35D42[rowIdx] + x] != 0) {
                            td14_elem_map_main[trackrows[rowIdx] + x] = 0;
                        } else if (td14_elem_map_main[trackrows[rowIdx] + x + 1] != 0xff ||
                                   td14_elem_map_main[word_35D42[rowIdx] + x] != 0xfe ||
                                   td14_elem_map_main[word_35D42[rowIdx] + x + 1] != 0xfd) {
                            td14_elem_map_main[trackrows[rowIdx] + x] = 0;
                        } else {
                            used[trackrows[rowIdx] + x + 1] = 1;
                            used[word_35D42[rowIdx] + x] = 1;
                            used[word_35D42[rowIdx] + x + 1] = 1;
                        }
                        break;
                    }
                }
            }
        }
    }
}
