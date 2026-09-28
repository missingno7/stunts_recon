#include "platform_hw.h"
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

extern struct TRACKOBJECT trklst[];
extern struct GAMEINFO globalgamesettings;
static unsigned char far *terrain_tile_shapes[19];
static struct SHAPE2D far *track_editor_cursors[4];
static struct SHAPE2D far *road_tile_shapes[4];
static unsigned char far *piece_mask_shapes[186];
static unsigned char far *piece_fill_shapes[186];
static unsigned char far *palette_piece_layout;
extern struct SPRITE far *g_wndspr;
unsigned char far *td14tb;
unsigned char far *td15p_9;
extern unsigned char far *g_column_of_trkdata21_pth;
extern unsigned char far *tdfrompathrow22;
extern unsigned char far *main_data_file_addr;
int gterrtrk[30];
int lnoffsets[30];
extern char resbuftext[];
unsigned char sampled_trk_column;
extern unsigned char g_cur_track_row;
extern int pixel_scales;
extern int performGraphColor;
extern int dialogarg2;
extern int dlg_colour;
extern int palette_window_line_color, palette_window_fill_color, palette_window_line_style;
extern int text_cursor_outline_color;
extern int menu_button_color_a, menu_button_color_b, menu_button_color_c;
extern int g_trackpiecescounter;
extern int msecoordx, pos_y_ms;
extern unsigned char flagsdown;
extern char buf_g_path[];
extern char track_file[];
extern unsigned char g_is_busy;

extern unsigned char far * far file_load_shape2d_fatal_thunk(char *name);
extern void far locate_many_resources(unsigned char far *data, char *names, char far **result);
extern unsigned char far * far file_load_resource_file(char *name);
extern unsigned char far * far locate_shape_alt(unsigned char far *data, char *name);
extern unsigned char far * far locate_shape_fatal(unsigned char far *data, char *name);
extern char far * far locate_text_resource(unsigned char far *data, char *name);
extern struct SPRITE far * far sprite_make_window(int width, int height, int flags);
extern void far sprite_copy_wnd_to_1_clear(void);
extern void far draw_button();
extern void far draw_lines_unknown();
extern void far sprite_copy_wnd_to_1(void);
extern void far sprset1size(int left, int right, int top, int bottom);
extern void far sprite_setup1_from_arg_pointer(struct SPRITE far *sprite);
extern int far mouse_track_op();
extern void far sprite_blit_to_video(struct SPRITE far *sprite, int mode);
extern void far preRender_line(int x1, int y1, int x2, int y2, int color);
extern void far sprcopy2to12(void);
extern void far sprite_shape_to_1(void far *shape, int x, int y);
extern void far sprite_clear_shape_alt(void far *shape, int x, int y);
extern void far sprite_putimage_and_alt(void far *shape, int x, int y);
extern void far sprite_putimage_and(void far *shape, int x, int y);
extern void far sprite_putimage_or(void far *shape, int x, int y);
extern void far putpixel_iconMask(void far *shape, int x, int y);
extern void far putpixel_iconFillings(void far *shape, int x, int y);
extern void far msdrawopaquechk(void);
extern void far msdrawtransparentchk(void);
extern void far font_setup_unknown(int colour, int mode);
extern void far copy_string(char *destination, char far *source);
extern int far font_op2(char *name);
extern void far draw_text_at(char *text, int x, int y);
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
extern void far draw_rect_outline(int x1, int y1, int x2, int y2, int color);
extern void far sprite_free_window(struct SPRITE far *sprite);
extern void far unload_resource(unsigned char far *data);
extern void far mmgr_free(unsigned char far *data);
extern unsigned char far subst_hillroad(unsigned int terrain, unsigned int element);

char validate_track_elements();
void clear_invalid_track_tiles();
void preRender_icons();
void draw_2DtrackMap();

char aEokenseieemseedewwefuenpestej[] = "eokenseieemseedewwefuenpestejsejdeteewaefteat";
char aTer0[] = "ter0";
unsigned int function_key_scan_codes[12] = { BIOS_KEY_F1, BIOS_KEY_F2, BIOS_KEY_F3, BIOS_KEY_F4, BIOS_KEY_F5, BIOS_KEY_F6, BIOS_KEY_F7, BIOS_KEY_F8, BIOS_KEY_F9, BIOS_KEY_F10, 0, 0 }; /* PLATFORM(input_kb): maps BIOS F1-F10 return values to menu shortcuts. */
int trackmenu2_buttons_x1[5] = { 9, 202, 220, 8, 220 };
int trackmenu2_buttons_x2[5] = { 199, 206, 315, 199, 315 };
int trackmenu2_buttons_y1[5] = { 181, 4, 132, 4, 36 };
int trackmenu2_buttons_y2[5] = { 187, 179, 139, 179, 187 };
unsigned char palette_column_limits[2] = { 30, 6 };
unsigned char palette_row_limits[2] = { 29, 9 };
char aFlatlakelak1lak2lak3lak4highg[] = "flatlakelak1lak2lak3lak4highgoungouwgousgouegou1gou2gou3gou4gou5gou6gou7gou8";
char aCrs0crs1crs2crs3[] = "crs0crs1crs2crs3";
char aUcr0ucr1ucr2ucr3[] = "ucr0ucr1ucr2ucr3";

void load_tracks_menu_shapes(void)
{ /* PURPOSE: Load track editor assets, draw the menu and map preview, then process selection and file actions. Params: none. Returns: void. Globals: reads track/map data, resource and tile caches, palette layout and menu settings; writes selection state, map/resource pointers and activity flags. */ /* PLATFORM(file): uses file and resource services. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(input_mouse): polls or updates mouse state. */ /* PLATFORM(timer): registers, removes, or reads the game timer. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(memory): allocates or releases game-managed memory. */
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


    sdtBuffer = file_load_shape2d_fatal_thunk("sdtedit") /* PLATFORM(file): load a 2D shape or enter the fatal-error path. */;
    locate_many_resources(sdtBuffer, aFlatlakelak1lak2lak3lak4highg, terrain_tile_shapes) /* PLATFORM(file): resolve the requested names in one resource block. */;
    locate_many_resources(sdtBuffer, aCrs0crs1crs2crs3, (char far **)track_editor_cursors) /* PLATFORM(file): resolve the requested names in one resource block. */;
    locate_many_resources(sdtBuffer, aUcr0ucr1ucr2ucr3, (char far **)road_tile_shapes) /* PLATFORM(file): resolve the requested names in one resource block. */;
    windows[0] = sprite_make_window(track_editor_cursors[0]->s2d_width * pixel_scales,
        track_editor_cursors[0]->s2d_height, 15) /* PLATFORM(video): allocate a temporary sprite window. */;
    windows[1] = sprite_make_window(track_editor_cursors[1]->s2d_width * pixel_scales,
        track_editor_cursors[1]->s2d_height, 15) /* PLATFORM(video): allocate a temporary sprite window. */;
    windows[2] = sprite_make_window(track_editor_cursors[2]->s2d_width * pixel_scales,
        track_editor_cursors[2]->s2d_height, 15) /* PLATFORM(video): allocate a temporary sprite window. */;
    windows[3] = sprite_make_window(track_editor_cursors[3]->s2d_width * pixel_scales,
        track_editor_cursors[3]->s2d_height, 15) /* PLATFORM(video): allocate a temporary sprite window. */;
    teditData = file_load_resource_file("tedit") /* PLATFORM(file): load a resource file. */;
    g_wndspr = sprite_make_window(320, 200, 15) /* PLATFORM(video): allocate a temporary sprite window. */;
    palette_piece_layout = locate_shape_alt(teditData, "pbox") /* PLATFORM(file): locate a named shape in resource data. */;
    shortNames = locate_shape_alt(teditData, "snam") /* PLATFORM(file): locate a named shape in resource data. */;
    mediumTextData = locate_shape_alt(teditData, "mnam") /* PLATFORM(file): locate a named shape in resource data. */;
    pieceNames = locate_shape_alt(teditData, "tnam") /* PLATFORM(file): locate a named shape in resource data. */;
    mapDirty = 0;
    for (j = 0; j < 132; ++j) {
        elementState[j] = 0xff;
        terrainCache[j] = 0xff;
    }
    for (j = 0; j < 186; ++j) {
        textPtr = shortNames + j * 4;
        resbuftext[0] = textPtr[0];
        resbuftext[1] = textPtr[1];
        resbuftext[2] = textPtr[2];
        resbuftext[3] = textPtr[3];
        piece_fill_shapes[j] = locate_shape_fatal(sdtBuffer, resbuftext) /* PLATFORM(file): locate a named shape in resource data. */;
        textPtr = mediumTextData + j * 4;
        resbuftext[0] = textPtr[0];
        resbuftext[1] = textPtr[1];
        resbuftext[2] = textPtr[2];
        resbuftext[3] = textPtr[3];
        piece_mask_shapes[j] = locate_shape_fatal(sdtBuffer, resbuftext) /* PLATFORM(file): locate a named shape in resource data. */;
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
    selectCol[0] = sampled_trk_column;
    selRow[0] = g_cur_track_row;
    selRow[1] = 7;

    sprite_copy_wnd_to_1_clear() /* PLATFORM(video): copy the active window sprite into buffer 1 and clear its color. */;
    draw_button(locate_text_resource(teditData, "bti") /* PLATFORM(file): locate a named text entry in resource data. */, 0xd9, 3, 0x66, 0x16,
        menu_button_color_a, menu_button_color_b, menu_button_color_c, 0) /* PLATFORM(video): draw a menu button. */;
    draw_lines_unknown(5, 0, 0xce, 0xbe, palette_window_line_color, palette_window_fill_color, palette_window_line_style) /* PLATFORM(video): draw menu line primitives. */;
    draw_lines_unknown(0xd9, 0x20, 0x66, 0x9e, palette_window_line_color, palette_window_fill_color, palette_window_line_style) /* PLATFORM(video): draw menu line primitives. */;
    draw_button(locate_text_resource(teditData, "bsc") /* PLATFORM(file): locate a named text entry in resource data. */, 0xdd, 0x8c, 0x5e, 0x0e,
        menu_button_color_a, menu_button_color_b, menu_button_color_c, 0) /* PLATFORM(video): draw a menu button. */;
    draw_button(locate_text_resource(teditData, "blo") /* PLATFORM(file): locate a named text entry in resource data. */, 0xdd, 0x9c, 0x2e, 0x0e,
        menu_button_color_a, menu_button_color_b, menu_button_color_c, 0) /* PLATFORM(video): draw a menu button. */;
    draw_button(locate_text_resource(teditData, "bsa") /* PLATFORM(file): locate a named text entry in resource data. */, 0xdd, 0xac, 0x2e, 0x0e,
        menu_button_color_a, menu_button_color_b, menu_button_color_c, 0) /* PLATFORM(video): draw a menu button. */;
    draw_button(locate_text_resource(teditData, "bcl") /* PLATFORM(file): locate a named text entry in resource data. */, 0x10d, 0x9c, 0x2e, 0x0e,
        menu_button_color_a, menu_button_color_b, menu_button_color_c, 0) /* PLATFORM(video): draw a menu button. */;
    draw_button(locate_text_resource(teditData, "bex") /* PLATFORM(file): locate a named text entry in resource data. */, 0x10d, 0xac, 0x2e, 0x0e,
        menu_button_color_a, menu_button_color_b, menu_button_color_c, 0) /* PLATFORM(video): draw a menu button. */;

    do {
nextFrame:
        if (paletteModified || activeGroup != lastType) {
            shapeWidth = 1;
            objectHeight = 1;
            tileSize = 0;
            if (activeGroup != 0) {
                switch (trklst[selectedPiece].ss_multiTileFlag) {
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
            while (palette_piece_layout[selRow[1] * 6 + activeGroup * 36 + selectCol[1]] >= 0xfe) {
                if (palette_piece_layout[selRow[1] * 6 + activeGroup * 36 + selectCol[1]] == 0xff)
                    --selectCol[1];
                else
                    --selRow[1];
            }
            sprite_copy_wnd_to_1() /* PLATFORM(video): copy the active window sprite into buffer 1. */;
            preRender_icons(activeGroup) /* PLATFORM(video): render terrain and track icons into the active sprite buffer. */;
            if (activeGroup == 0)
                mouse_track_op(0, 0xdd, 0x5f, 0x85, 5, 0, 1, 1) /* PLATFORM(input_mouse): track the mouse and process its current action. */;
            else
                mouse_track_op(0, 0xdd, 0x5f, 0x85, 5, activeGroup - 1, 1, 10) /* PLATFORM(input_mouse): track the mouse and process its current action. */;
        }
        if (pathFlag) {
            pathFlag = 0;
            errorMsg = validate_track_elements();
        }
        if (mapChanged || paletteModified) {
            sprite_copy_wnd_to_1() /* PLATFORM(video): copy the active window sprite into buffer 1. */;
            if (mapChanged) {
                mapChanged = 0;
                if (sliderChanged) {
                    sliderChanged = 0;
                    mouse_track_op(0, 9, 0xc0, 0xb5, 5, originX, 0x0c, 0x1e) /* PLATFORM(input_mouse): track the mouse and process its current action. */;
                    mouse_track_op(0, 0xca, 5, 4, 0xb0, viewTop, 0x0b, 0x1e) /* PLATFORM(input_mouse): track the mouse and process its current action. */;
                }
                sprset1size(8, 0xc8, 4, 0xb3) /* PLATFORM(video): set the active sprite buffer bounds. */;
                draw_2DtrackMap(originX, viewTop, elementState, terrainCache) /* PLATFORM(video): render the cached track map into the active sprite buffer. */;
                sprset1size(0, 0x140, 0, 0xc8) /* PLATFORM(video): set the active sprite buffer bounds. */;
            }
            if (paletteModified) {
                paletteModified = 0;
                sprite_setup1_from_arg_pointer(windows[tileSize]) /* PLATFORM(video): select the sprite described by the argument pointer. */;
                if (activeGroup == 0) {
                    sprite_shape_to_1(terrain_tile_shapes[selectedPiece], 0, 0) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                    preRender_line(1, 0, 15, 0, performGraphColor) /* PLATFORM(video): draw a line primitive. */;
                    preRender_line(1, 14, 15, 14, performGraphColor) /* PLATFORM(video): draw a line primitive. */;
                    preRender_line(1, 0, 1, 14, performGraphColor) /* PLATFORM(video): draw a line primitive. */;
                    preRender_line(15, 0, 15, 14, performGraphColor) /* PLATFORM(video): draw a line primitive. */;
                } else {
                    sprite_shape_to_1(track_editor_cursors[tileSize], 0, 0) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                    if (selectedPiece != 0) {
                        putpixel_iconMask(piece_mask_shapes[selectedPiece], 0, 0) /* PLATFORM(video): apply a track-piece icon mask. */;
                        putpixel_iconFillings(piece_fill_shapes[selectedPiece], 0, 0) /* PLATFORM(video): apply a track-piece icon fill. */;
                    }
                }
            }
            sprite_blit_to_video(g_wndspr, imageMode) /* PLATFORM(video): present the active sprite buffer on the display. */;
            imageMode = -2;
            lastHoverShape = 0xff;
        }

        sprcopy2to12() /* PLATFORM(video): copy the current sprite into the active buffer. */;
        if (!paletteArea) {
            cursorPixW = shapeWidth << 4;
            boxHeight = objectHeight << 4;
            cursorLeft = ((selectCol[0] - originX) << 4) + 8;
            screenPosY = ((selRow[0] - viewTop) << 4) + 4;
            hovered = td14tb[lnoffsets[selRow[0]] + selectCol[0]];
            switch (hovered) {
            case 0xfd:
                hovered = td14tb[lnoffsets[selRow[0] - 1] + selectCol[0] - 1];
                break;
            case 0xfe:
                hovered = td14tb[lnoffsets[selRow[0] - 1] + selectCol[0]];
                break;
            case 0xff:
                hovered = td14tb[lnoffsets[selRow[0]] + selectCol[0] - 1];
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
                        palette_piece_layout[selRow[1] * 6 + activeGroup * 36 + selectCol[1] + 6] == 0xfe)
                    boxHeight = 0x20;
                if (selectCol[1] < 5 &&
                        palette_piece_layout[selRow[1] * 6 + activeGroup * 36 + selectCol[1] + 1] == 0xff)
                    cursorPixW = 0x20;
                hovered = palette_piece_layout[selRow[1] * 6 + activeGroup * 36 + selectCol[1]];
                if (hovered >= 0xfd)
                    hovered = 0;
            }
            if (activeGroup == 0)
                hovered = 0;
        }

        if (hovered != lastHoverShape) {
            msdrawopaquechk() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
            font_setup_unknown(dlg_colour, 0) /* PLATFORM(video): select the font and color mode. */;
            textPtr = pieceNames + hovered * 3;
            resbuftext[0] = textPtr[0];
            resbuftext[1] = textPtr[1];
            resbuftext[2] = textPtr[2];
            copy_string(resbuftext, locate_text_resource(teditData, resbuftext) /* PLATFORM(file): locate a named text entry in resource data. */);
            j = font_op2(resbuftext) /* PLATFORM(video): load or select a named font. */;
            draw_text_at(resbuftext, 8, 0xc0) /* PLATFORM(video): draw a text string at screen coordinates. */;
            if (lastTextWidth > j)
                sprite_1_unk(j + 8, 0xc0, lastTextWidth - j, 8, 0) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
            msdrawtransparentchk() /* PLATFORM(video): redraw the pointer in transparent mode when needed. */;
            lastTextWidth = j;
            lastHoverShape = hovered;
        }
        if (errorMsg) {
            show_dialog(1, 1, locate_text_resource(teditData, aEokenseieemseedewwefuenpestej + errorMsg * 3) /* PLATFORM(file): locate a named text entry in resource data. */,
                -1, -1, performGraphColor, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
            errorMsg = 0;
        }

        animationCount = 99;
        blinkFlag = 0;
        msdrawopaquechk() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
        if ((drawnMode = paletteArea) == 0)
            sprite_clear_shape_alt(road_tile_shapes[tileSize], cursorLeft, screenPosY) /* PLATFORM(video): clear the requested shape in sprite buffer 1. */;
        do {
            if (animationCount > 15) {
                msdrawopaquechk() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
                if (!paletteArea) {
                    if (blinkFlag)
                        sprite_shape_to_1(road_tile_shapes[tileSize], cursorLeft, screenPosY) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                    else
                        sprite_shape_to_1(windows[tileSize]->sprite_bitmapptr, cursorLeft, screenPosY) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                } else {
                    draw_rect_outline(cursorLeft, screenPosY - 1, cursorLeft + cursorPixW,
                        screenPosY + boxHeight - 1, text_cursor_outline_color) /* PLATFORM(video): draw a rectangle outline. */;
                }
                msdrawtransparentchk() /* PLATFORM(video): redraw the pointer in transparent mode when needed. */;
                blinkFlag ^= 1;
                animationCount = 0;
            }
            stepTime = timer_get_delta_alt() /* PLATFORM(timer): read the elapsed timer delta through the far entry point. */;
            animationCount += stepTime;
            key = input_checking(stepTime) /* PLATFORM(input_kb): poll the configured input devices. */;
            hitArea = mouse_multi_hittest(5, trackmenu2_buttons_x1, trackmenu2_buttons_x2,
                trackmenu2_buttons_y1, trackmenu2_buttons_y2) /* PLATFORM(input_mouse): test the mouse against candidate rectangles. */;
            if (hitArea != -1) {
                switch (hitArea) {
                case 0:
                    if (flagsdown & 3) {
                        paletteArea = 0;
                        value = mouse_track_op(1, 9, 0xc0, 0xb5, 5, originX, 0x0c, 0x1e) /* PLATFORM(input_mouse): track the mouse and process its current action. */;
                        selectCol[0] += value - originX;
                        originX = value;
                        key = 1;
                    }
                    break;
                case 1:
                    if (flagsdown & 3) {
                        paletteArea = 0;
                        value = mouse_track_op(1, 0xca, 5, 4, 0xb0, viewTop, 0x0b, 0x1e) /* PLATFORM(input_mouse): track the mouse and process its current action. */;
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
                    if (flagsdown & 3) {
                        activeGroup = mouse_track_op(1, 0xdd, 0x5f, 0x85, 5, activeGroup - 1, 1, 10) /* PLATFORM(input_mouse): track the mouse and process its current action. */ + 1;
                        key = 1;
                    }
                    break;
                case 3:
                    destPosX = (msecoordx - 8) / 16;
                    destPosY = (pos_y_ms - 4) / 16;
                    if (activeGroup != 0) {
                        if (destPosY == 10 && (trklst[selectedPiece].ss_multiTileFlag & 1))
                            --destPosY;
                        if (destPosX == 11 && (trklst[selectedPiece].ss_multiTileFlag & 2))
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
                    destPosX = (msecoordx - 0xdc) / 16;
                    destPosY = (pos_y_ms - 0x24) / 16;
                    if (destPosY < 6) {
                        if (palette_piece_layout[activeGroup * 36 + destPosY * 6 + destPosX] == 0xfe)
                            --destPosY;
                        if (palette_piece_layout[activeGroup * 36 + destPosY * 6 + destPosX] == 0xff)
                            --destPosX;
                    } else {
                        destPosY = (pos_y_ms - 0x1c) / 16;
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
            timer_get_counter_unk(10L) /* PLATFORM(timer): wait or schedule against the requested timer ticks. */;
        if (blinkFlag) {
            msdrawopaquechk() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
            if (!drawnMode)
                sprite_shape_to_1(road_tile_shapes[tileSize], cursorLeft, screenPosY) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
            else
                draw_rect_outline(cursorLeft, screenPosY - 1, cursorLeft + cursorPixW,
                    screenPosY + boxHeight - 1, text_cursor_outline_color) /* PLATFORM(video): draw a rectangle outline. */;
            msdrawtransparentchk() /* PLATFORM(video): redraw the pointer in transparent mode when needed. */;
        }

        if (trackStep != 0) {
            if (key != 1 || paletteArea)
                trackStep = g_trackpiecescounter - 1;
            selectCol[0] = g_column_of_trkdata21_pth[trackStep];
            selRow[0] = tdfrompathrow22[trackStep];
            selectedPiece = td14tb[lnoffsets[selRow[0]] + selectCol[0]];
            mapChanged = 1;
            paletteModified = 1;
            if (++trackStep < g_trackpiecescounter)
                goto nextFrame;
            selectedPiece = savedShape;
            trackStep = 0;
            goto nextFrame;
        }
        trackStep = 0;
        for (j = 0; j < 10; ++j) {
            if (function_key_scan_codes[j] == key) {
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
            show_dialog(1, 1, locate_text_resource(teditData, aEokenseieemseedewwefuenpestej + j * 3) /* PLATFORM(file): locate a named text entry in resource data. */,
                -1, -1, performGraphColor, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
            if (j > 1) {
                paletteArea = 0;
                if (g_trackpiecescounter == 0) {
                    selectCol[0] = sampled_trk_column;
                    selRow[0] = g_cur_track_row;
                } else {
                    selectCol[0] = g_column_of_trkdata21_pth[0];
                    selRow[0] = tdfrompathrow22[0];
                    savedShape = selectedPiece;
                    selectedPiece = td14tb[lnoffsets[selRow[0]] + selectCol[0]];
                    trackStep = 1;
                    paletteModified = 1;
                }
            }
            check_input();
            break;
        case 0x0d:
            if (paletteArea) {
                if (selRow[1] < 6) {
                    selectedPiece = palette_piece_layout[selRow[1] * 6 + activeGroup * 36 + selectCol[1]];
                    if (activeGroup != 0) {
                        if ((trklst[selectedPiece].ss_multiTileFlag & 1) &&
                                selRow[0] - viewTop == 10)
                            --selRow[0];
                        if ((trklst[selectedPiece].ss_multiTileFlag & 2) &&
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
                        answer = show_dialog(2, 1, locate_text_resource(teditData, "mss") /* PLATFORM(file): locate a named text entry in resource data. */,
                            -1, -1, dialogarg2, 0, td14tb[0x384]) /* PLATFORM(video): present the interactive dialog renderer. */;
                        if (answer != -1 && answer != 5) {
                            td14tb[0x384] = answer;
                            ++mapChanged;
                            mapDirty = 1;
                        }
                    } else if (selRow[1] == 8 && selectCol[1] != 0) {
                        answer = show_dialog(2, 1, locate_text_resource(teditData, "men") /* PLATFORM(file): locate a named text entry in resource data. */,
                            -1, -1, dialogarg2, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
                        if (answer != -1 && answer != 5) {
                            for (j = 0; j < 0x384; ++j)
                                td14tb[j] = 0;
                            aTer0[3] = answer + '0';
                            terrainTemplate = locate_shape_alt(teditData, aTer0) /* PLATFORM(file): locate a named shape in resource data. */;
                            for (j = 0; j < 0x385; ++j)
                                td15p_9[j] = terrainTemplate[j];
                            globalgamesettings.game_trackname[0] = 0;
                            ++mapChanged;
                            mapDirty = 1;
                        }
                    } else if (selRow[1] == 8 && selectCol[1] == 0) {
                        sprcopy2to12() /* PLATFORM(video): copy the current sprite into the active buffer. */;
                        if (mapDirty && (j = show_dialog(2, 1, locate_text_resource(teditData, "chl") /* PLATFORM(file): locate a named text entry in resource data. */,
                                -1, -1, performGraphColor, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */) == 0)
                            goto save;
                        j = 1;
                        g_is_busy = 1;
                        ++mapChanged;
                        j = do_fileselect_dialog(track_file, globalgamesettings.game_trackname, ".trk",
                            locate_text_resource(main_data_file_addr, "trk") /* PLATFORM(file): locate a named text entry in resource data. */) /* PLATFORM(file): open the file-selection UI and return its selection. */;
                        file_build_path(track_file, globalgamesettings.game_trackname, ".trk", buf_g_path) /* PLATFORM(file): build the path used by the file service. */;
                        if (j > 0) {
                            file_read_fatal(buf_g_path, td14tb) /* PLATFORM(file): read a file or enter the fatal-error path. */;
                            track_setup();
                            paletteArea = 0;
                            selRow[0] = g_cur_track_row;
                            selectCol[0] = sampled_trk_column;
                            mapDirty = 0;
                            ++mapChanged;
                        }
                        g_is_busy = 0;
                    } else if (selectCol[1] == 0) {
save:
                        saveOutcome = 0;
                        g_is_busy = 1;
                        while (saveOutcome == 0) {
                            sprcopy2to12() /* PLATFORM(video): copy the current sprite into the active buffer. */;
                            ++mapChanged;
                            if (do_savefile_dialog(track_file, globalgamesettings.game_trackname,
                                    locate_text_resource(main_data_file_addr, "trk") /* PLATFORM(file): locate a named text entry in resource data. */) /* PLATFORM(file): open the save-file UI and return its selection. */) {
                                file_build_path(track_file, globalgamesettings.game_trackname, ".trk",
                                    buf_g_path) /* PLATFORM(file): build the path used by the file service. */;
                                saveOutcome = 1;
                                if (file_find(buf_g_path) /* PLATFORM(file): search for a file by name. */) {
                                    j = show_dialog(2, 1, locate_text_resource(main_data_file_addr, "fex") /* PLATFORM(file): locate a named text entry in resource data. */,
                                        -1, -1, performGraphColor, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
                                    if (j == -1)
                                        saveOutcome = -1;
                                    else if (j == 0)
                                        saveOutcome = 0;
                                }
                            } else
                                saveOutcome = -1;
                            if (saveOutcome == 1) {
                                j = file_write_fatal(buf_g_path, td14tb, 0x70aL) /* PLATFORM(file): write a file or enter the fatal-error path. */;
                                if (j == 0)
                                    highscore_write_a(1);
                                if (j != 0) {
                                    show_dialog(1, 1, locate_text_resource(main_data_file_addr, "ser") /* PLATFORM(file): locate a named text entry in resource data. */,
                                        -1, -1, performGraphColor, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
                                    saveOutcome = 0;
                                } else
                                    mapDirty = 0;
                            }
                        }
                        g_is_busy = 0;
                    } else {
                        if (mapDirty && (j = show_dialog(2, 1, locate_text_resource(teditData, "chx") /* PLATFORM(file): locate a named text entry in resource data. */,
                                -1, -1, performGraphColor, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */) == 0)
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
                    oldCell = td15p_9[gterrtrk[selRow[0]] + selectCol[0]];
                    lastPutCol = selectCol[0];
                    droppedPosY = selRow[0];
                }
                td15p_9[gterrtrk[droppedPosY] + lastPutCol] = selectedPiece;
                mapDirty = 1;
                pathFlag = 1;
                ++mapChanged;
            } else if (!((trklst[selectedPiece].ss_multiTileFlag & 1) && selRow[0] > 28) &&
                    !((trklst[selectedPiece].ss_multiTileFlag & 2) && selectCol[0] > 28)) {
                if (selectCol[0] == lastPutCol && selRow[0] == droppedPosY) {
                    tmpShape = selectedPiece;
                    selectedPiece = oldCell;
                    oldCell = tmpShape;
                    ++paletteModified;
                } else {
                    oldCell = td14tb[lnoffsets[selRow[0]] + selectCol[0]];
                    if (oldCell >= 0xfd)
                        oldCell = 0;
                    lastPutCol = selectCol[0];
                    droppedPosY = selRow[0];
                }
                td14tb[lnoffsets[droppedPosY] + lastPutCol] = selectedPiece;
                mapDirty = 1;
                pathFlag = 1;
                ++mapChanged;
                switch (trklst[selectedPiece].ss_multiTileFlag) {
                case 1:
                    td14tb[lnoffsets[droppedPosY + 1] + lastPutCol] = 0xfe;
                    break;
                case 2:
                    td14tb[lnoffsets[droppedPosY] + lastPutCol + 1] = 0xff;
                    break;
                case 3:
                    td14tb[lnoffsets[droppedPosY] + lastPutCol + 1] = 0xff;
                    td14tb[lnoffsets[droppedPosY + 1] + lastPutCol] = 0xfe;
                    td14tb[lnoffsets[droppedPosY + 1] + lastPutCol + 1] = 0xfd;
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
                    while (palette_piece_layout[selRow[1] * 6 + activeGroup * 36 + selectCol[1]] >= 0xfe) {
                        boxMarker = palette_piece_layout[selRow[1] * 6 + activeGroup * 36 + selectCol[1]];
                        if (boxMarker == 0xff)
                            --selectCol[1];
                        else if (boxMarker == 0xfe)
                            --selRow[1];
                    }
                }
            }
            break;
        case 0x5000:
            if (selRow[paletteArea] < palette_row_limits[paletteArea]) {
                lastPutCol = -1;
                ++selRow[paletteArea];
                if (paletteArea && selRow[1] < 6) {
                    boxMarker = palette_piece_layout[selRow[1] * 6 + activeGroup * 36 + selectCol[1]];
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
                        while (palette_piece_layout[selRow[1] * 6 + activeGroup * 36 + selectCol[1]] >= 0xfe) {
                            boxMarker = palette_piece_layout[selRow[1] * 6 + activeGroup * 36 + selectCol[1]];
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
                        while (selectCol[paletteArea] + boxMarker < palette_column_limits[paletteArea] &&
                                palette_piece_layout[selRow[1] * 6 + activeGroup * 36 + selectCol[1] + boxMarker] >= 0xfe) {
                            value = palette_piece_layout[selRow[1] * 6 + activeGroup * 36 + selectCol[1] + boxMarker];
                            if (value == 0xff)
                                ++boxMarker;
                            else if (value == 0xfe)
                                --selRow[1];
                        }
                }
                if (selectCol[paletteArea] + boxMarker < palette_column_limits[paletteArea]) {
                    lastPutCol = -1;
                    selectCol[paletteArea] += boxMarker;
                }
            }
            break;
        }
    } while (inEditor);

    sprite_free_window(g_wndspr) /* PLATFORM(video): release a temporary sprite window. */;
    sprite_free_window(windows[3]) /* PLATFORM(video): release a temporary sprite window. */;
    sprite_free_window(windows[2]) /* PLATFORM(video): release a temporary sprite window. */;
    sprite_free_window(windows[1]) /* PLATFORM(video): release a temporary sprite window. */;
    sprite_free_window(windows[0]) /* PLATFORM(video): release a temporary sprite window. */;
    unload_resource(teditData) /* PLATFORM(memory): release a loaded resource. */;
    mmgr_free(sdtBuffer) /* PLATFORM(memory): release the resource through game memory management. */;
}

void preRender_icons(unsigned char mode)
{ /* PURPOSE: Draw the six-by-six terrain and track-piece icon grid. Params: mode. Returns: void. Globals: reads palette_piece_layout, piece_fill_shapes, piece_mask_shapes, terrain_tile_shapes, trklst; writes none. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    unsigned char iconIndex;
    unsigned char row;
    unsigned char stateId;
    for (iconIndex = 0; iconIndex < 6; ++iconIndex) {
        for (row = 0; row < 6; ++row) {
            stateId = palette_piece_layout[mode * 36 + iconIndex * 6 + row];
            if (mode == 0) {
                sprite_shape_to_1(terrain_tile_shapes[stateId], 220 + (row << 4),
                    36 + (iconIndex << 4)) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
            } else if (stateId < 0xfd) {
                sprite_shape_to_1(terrain_tile_shapes[0], 220 + (row << 4),
                    36 + (iconIndex << 4)) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                switch (trklst[stateId].ss_multiTileFlag) {
                case 1:
                    sprite_shape_to_1(terrain_tile_shapes[0], 220 + (row << 4),
                        52 + (iconIndex << 4)) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                    break;
                case 2:
                    sprite_shape_to_1(terrain_tile_shapes[0], 236 + (row << 4),
                        36 + (iconIndex << 4)) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                    break;
                case 3:
                    sprite_shape_to_1(terrain_tile_shapes[0], 236 + (row << 4),
                        36 + (iconIndex << 4)) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                    sprite_shape_to_1(terrain_tile_shapes[0], 220 + (row << 4),
                        52 + (iconIndex << 4)) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                    sprite_shape_to_1(terrain_tile_shapes[0], 236 + (row << 4),
                        52 + (iconIndex << 4)) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                    break;
                }
                putpixel_iconMask(piece_mask_shapes[stateId], 220 + (row << 4),
                    36 + (iconIndex << 4)) /* PLATFORM(video): apply a track-piece icon mask. */;
                putpixel_iconFillings(piece_fill_shapes[stateId],
                    220 + (row << 4), 36 + (iconIndex << 4)) /* PLATFORM(video): apply a track-piece icon fill. */;
            }
        }
    }
}

void draw_2DtrackMap(unsigned char rowBase, unsigned char columnBase, unsigned char *lastElement, unsigned char *lastTerrain)
{ /* PURPOSE: Draw the cached twelve-by-eleven track map preview. Params: rowBase, columnBase, lastElement, lastTerrain. Returns: void. Globals: reads gterrtrk, lnoffsets, piece_fill_shapes, piece_mask_shapes, td14tb, td15p_9, terrain_tile_shapes, trklst; writes none. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    unsigned int rowIndex;
    signed char columnIndex;
    signed char mapRow;
    unsigned int mapIndex;
    unsigned char surface;
    unsigned char tileId;

    for (mapRow = 0; mapRow < 11; ++mapRow) {
        rowIndex = mapRow * 12;
        for (columnIndex = 0; columnIndex < 12; ++columnIndex) {
            tileId = td14tb[lnoffsets[columnBase + mapRow] + columnIndex + rowBase];
            surface = td15p_9[gterrtrk[columnBase + mapRow] + columnIndex + rowBase];
            mapIndex = rowIndex + columnIndex;

            if (tileId >= 0xfd && (mapRow == 0 || columnIndex == 0)) {
                lastElement[mapIndex] = 0xff;
                if (tileId == 0xff && columnIndex == 0) {
                    sprite_putimage_and_alt(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow] + columnIndex + rowBase]],
                        (columnIndex << 4) + 8, (mapRow << 4) + 4) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                    sprite_putimage_and_alt(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow + 1] + columnIndex + rowBase]],
                        (columnIndex << 4) + 8, (mapRow << 4) + 20) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                    sprite_putimage_and(piece_mask_shapes[td14tb[lnoffsets[columnBase + mapRow] + columnIndex + rowBase - 1]],
                        (columnIndex << 4) - 8, (mapRow << 4) + 4) /* PLATFORM(video): apply a sprite mask at the requested position. */;
                    sprite_putimage_or(piece_fill_shapes[td14tb[lnoffsets[columnBase + mapRow] + columnIndex + rowBase - 1]],
                        (columnIndex << 4) - 8, (mapRow << 4) + 4) /* PLATFORM(video): apply sprite pixels at the requested position. */;
                } else if (tileId == 0xfe && mapRow == 0) {
                    sprite_putimage_and_alt(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow] + columnIndex + rowBase]],
                        (columnIndex << 4) + 8, (mapRow << 4) + 4) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                    sprite_putimage_and_alt(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow] + columnIndex + rowBase + 1]],
                        (columnIndex << 4) + 24, (mapRow << 4) + 4) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                    sprite_putimage_and(piece_mask_shapes[td14tb[lnoffsets[columnBase + mapRow - 1] + columnIndex + rowBase]],
                        (columnIndex << 4) + 8, (mapRow << 4) - 12) /* PLATFORM(video): apply a sprite mask at the requested position. */;
                    sprite_putimage_or(piece_fill_shapes[td14tb[lnoffsets[columnBase + mapRow - 1] + columnIndex + rowBase]],
                        (columnIndex << 4) + 8, (mapRow << 4) - 12) /* PLATFORM(video): apply sprite pixels at the requested position. */;
                } else if (tileId == 0xfd && mapRow == 0 && columnIndex == 0) {
                    sprite_putimage_and_alt(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow] + columnIndex + rowBase]],
                        (columnIndex << 4) + 8, (mapRow << 4) + 4) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                    sprite_putimage_and(piece_mask_shapes[td14tb[lnoffsets[columnBase + mapRow - 1] + columnIndex + rowBase - 1]],
                        (columnIndex << 4) - 8, (mapRow << 4) - 12) /* PLATFORM(video): apply a sprite mask at the requested position. */;
                    sprite_putimage_or(piece_fill_shapes[td14tb[lnoffsets[columnBase + mapRow - 1] + columnIndex + rowBase - 1]],
                        (columnIndex << 4) - 8, (mapRow << 4) - 12) /* PLATFORM(video): apply sprite pixels at the requested position. */;
                }
            } else if (tileId == 0) {
                if (lastElement[mapIndex] != 0 || lastTerrain[mapIndex] != surface) {
                    sprite_shape_to_1(terrain_tile_shapes[surface], (columnIndex << 4) + 8, (mapRow << 4) + 4) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                    lastElement[mapIndex] = 0;
                    lastTerrain[mapIndex] = surface;
                }
            } else if (tileId < 0xfd) {
                if (lastElement[mapIndex] != tileId || lastTerrain[mapIndex] != surface) {
                    lastElement[mapIndex] = tileId;
                    lastTerrain[mapIndex] = surface;
                    sprite_shape_to_1(terrain_tile_shapes[surface], (columnIndex << 4) + 8, (mapRow << 4) + 4) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                    switch (trklst[tileId].ss_multiTileFlag) {
                    case 0:
                        putpixel_iconMask(piece_mask_shapes[tileId], (columnIndex << 4) + 8, (mapRow << 4) + 4) /* PLATFORM(video): apply a track-piece icon mask. */;
                        putpixel_iconFillings(piece_fill_shapes[tileId], (columnIndex << 4) + 8, (mapRow << 4) + 4) /* PLATFORM(video): apply a track-piece icon fill. */;
                        break;
                    case 1:
                        sprite_putimage_and_alt(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow + 1] + columnIndex + rowBase]],
                            (columnIndex << 4) + 8, (mapRow << 4) + 20) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                        sprite_putimage_and(piece_mask_shapes[tileId], (columnIndex << 4) + 8, (mapRow << 4) + 4) /* PLATFORM(video): apply a sprite mask at the requested position. */;
                        sprite_putimage_or(piece_fill_shapes[tileId], (columnIndex << 4) + 8, (mapRow << 4) + 4) /* PLATFORM(video): apply sprite pixels at the requested position. */;
                        break;
                    case 2:
                        sprite_putimage_and_alt(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow] + columnIndex + rowBase + 1]],
                            (columnIndex << 4) + 24, (mapRow << 4) + 4) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                        sprite_putimage_and(piece_mask_shapes[tileId], (columnIndex << 4) + 8, (mapRow << 4) + 4) /* PLATFORM(video): apply a sprite mask at the requested position. */;
                        sprite_putimage_or(piece_fill_shapes[tileId], (columnIndex << 4) + 8, (mapRow << 4) + 4) /* PLATFORM(video): apply sprite pixels at the requested position. */;
                        break;
                    case 3:
                        sprite_putimage_and_alt(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow] + columnIndex + rowBase + 1]],
                            (columnIndex << 4) + 24, (mapRow << 4) + 4) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                        sprite_putimage_and_alt(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow + 1] + columnIndex + rowBase]],
                            (columnIndex << 4) + 8, (mapRow << 4) + 20) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                        sprite_putimage_and_alt(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow + 1] + columnIndex + rowBase + 1]],
                            (columnIndex << 4) + 24, (mapRow << 4) + 20) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                        sprite_putimage_and(piece_mask_shapes[tileId], (columnIndex << 4) + 8, (mapRow << 4) + 4) /* PLATFORM(video): apply a sprite mask at the requested position. */;
                        sprite_putimage_or(piece_fill_shapes[tileId], (columnIndex << 4) + 8, (mapRow << 4) + 4) /* PLATFORM(video): apply sprite pixels at the requested position. */;
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

char validate_track_elements(void)
{ /* PURPOSE: Check track/terrain pairs, clear invalid entries, and return the error code. Params: none. Returns: char. Globals: reads gterrtrk, lnoffsets, td14tb, td15p_9; writes none. */
    unsigned char elem, colidx;
    unsigned char terrain, rowno;
    char error;

    clear_invalid_track_tiles();
    error = 0;
    for (rowno = 0; rowno < 30; ++rowno) {
        for (colidx = 0; colidx < 30; ++colidx) {
            terrain = td15p_9[gterrtrk[rowno] + colidx];
            elem = td14tb[lnoffsets[rowno] + colidx];
            if (elem != 0 && terrain != 0 && terrain != 6) {
                switch (terrain) {
                case 1: case 2: case 3: case 4: case 5:
                    if (elem == 0xff)
                        elem = td14tb[lnoffsets[rowno] + colidx - 1];
                    else if (elem == 0xfe)
                        elem = td14tb[lnoffsets[rowno - 1] + colidx];
                    else if (elem == 0xfd)
                        elem = td14tb[lnoffsets[rowno - 1] + colidx - 1];
                    switch (elem) {
                    case 0x22: case 0x23:
                    case 0x67: case 0x68: case 0x69: case 0x6a: case 0x6b: case 0x6c:
                    case 0xab: case 0xac: case 0xad: case 0xae:
                        break;
                    default:
                        td14tb[lnoffsets[rowno] + colidx] = 0;
                        error = 12;
                    }
                    break;
                case 7: case 8: case 9: case 10:
                    if (!subst_hillroad(terrain, elem)) {
                        td14tb[lnoffsets[rowno] + colidx] = 0;
                        error = 13;
                    }
                    break;
                default:
                    error = 14;
                    td14tb[lnoffsets[rowno] + colidx] = 0;
                }
            }
        }
    }
    if (error != 0) clear_invalid_track_tiles();
    return error;
}

void clear_invalid_track_tiles(void)
{ /* PURPOSE: Remove dangling cells from multi-tile track pieces. Params: none. Returns: void. Globals: reads lnoffsets, td14tb, trklst; writes none. */
    unsigned char used[900];
    unsigned char rowIdx;
    unsigned char x;
    unsigned char element;
    register int clear;

    for (clear = 0; clear < 900; ++clear)
        used[clear] = 0;

    for (rowIdx = 0; rowIdx < 30; ++rowIdx) {
        for (x = 0; x < 30; ++x) {
            element = td14tb[lnoffsets[rowIdx] + x];
            if (element != 0) {
                if (element >= 0xfd) {
                    if (used[lnoffsets[rowIdx] + x] == 0)
                        td14tb[lnoffsets[rowIdx] + x] = 0;
                } else {
                    switch (trklst[element].ss_multiTileFlag) {
                    case 1:
                        if (used[lnoffsets[rowIdx + 1] + x] != 0)
                            td14tb[lnoffsets[rowIdx] + x] = 0;
                        else if (td14tb[lnoffsets[rowIdx + 1] + x] != 0xfe)
                            td14tb[lnoffsets[rowIdx] + x] = 0;
                        else
                            used[lnoffsets[rowIdx + 1] + x] = 1;
                        break;
                    case 2:
                        if (used[lnoffsets[rowIdx] + x + 1] != 0) {
                            td14tb[lnoffsets[rowIdx] + x] = 0;
                        } else {
                            if (td14tb[lnoffsets[rowIdx] + x + 1] != 0xff)
                                td14tb[lnoffsets[rowIdx] + x] = 0;
                            else
                                used[lnoffsets[rowIdx] + x + 1] = 1;
                        }
                        break;
                    case 3:
                        if (used[lnoffsets[rowIdx + 1] + x + 1] +
                            used[lnoffsets[rowIdx] + x + 1] +
                            used[lnoffsets[rowIdx + 1] + x] != 0) {
                            td14tb[lnoffsets[rowIdx] + x] = 0;
                        } else if (td14tb[lnoffsets[rowIdx] + x + 1] != 0xff ||
                                   td14tb[lnoffsets[rowIdx + 1] + x] != 0xfe ||
                                   td14tb[lnoffsets[rowIdx + 1] + x + 1] != 0xfd) {
                            td14tb[lnoffsets[rowIdx] + x] = 0;
                        } else {
                            used[lnoffsets[rowIdx] + x + 1] = 1;
                            used[lnoffsets[rowIdx + 1] + x] = 1;
                            used[lnoffsets[rowIdx + 1] + x + 1] = 1;
                        }
                        break;
                    }
                }
            }
        }
    }
}
