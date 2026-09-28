#include "platform_hw.h"
#define TRACK_MAP_ROW_COUNT 30
#define TRACK_MAP_COLUMN_COUNT 30
#define TRACK_MAP_CELL_COUNT 900
#define TRACK_PREVIEW_ROW_COUNT 11
#define TRACK_PREVIEW_COLUMN_COUNT 12
#define TRACK_EDITOR_CELL_PIXELS 16
#define TRACK_EDITOR_CELL_SHIFT 4
#define TRACK_EDITOR_CELL_ORIGIN_X 8
#define TRACK_EDITOR_CELL_ORIGIN_Y 4
#define TRACK_EDITOR_CELL_LEFT_X (-8)
#define TRACK_EDITOR_CELL_RIGHT_X 24
#define TRACK_EDITOR_CELL_ABOVE_Y (-12)
#define TRACK_EDITOR_CELL_BELOW_Y 20
#define TRACK_PALETTE_GRID_SIDE 6
#define TRACK_PALETTE_PAGE_CELL_COUNT 36
#define TRACK_EDITOR_GRID_X_ORIGIN 220
#define TRACK_EDITOR_GRID_Y_ORIGIN 36
#define TRACK_EDITOR_PALETTE_ROW_Y_ORIGIN 28
#define TRACK_ELEMENT_SPECIAL_BASE 0xfd /* First byte reserved for multi-cell continuation codes. */
#define TRACK_ELEMENT_CONTINUATION_BASE 0xfe /* Lowest palette continuation marker used in layout scans. */
#define TRACK_TILE_CONTINUE_DOWN_RIGHT 0xfd /* Multi-cell piece continues down and right. */
#define TRACK_TILE_CONTINUE_DOWN 0xfe /* Multi-cell piece continues into the row below. */
#define TRACK_TILE_CONTINUE_RIGHT 0xff /* Multi-cell piece continues into the next column. */
#define TRACK_CACHE_INVALID 0xff /* Cached map value has not been drawn yet. */
#define TRACK_SHAPE_CACHE_INVALID 0xff /* Cached hover shape has not been drawn yet. */
#define TRACK_PIECE_SHAPE_COUNT 186
#define TERRAIN_SHAPE_COUNT 19
#define TRACK_EDITOR_CURSOR_COUNT 4
#define ROAD_TILE_SHAPE_COUNT 4
#include "stunts_types.h"
/* PORT: Resource records contain far pointers and depend on MSC default 2-byte packing; never use the host layout as the file layout. */
struct SHAPE2D {
    I16 s2d_width;
    I16 s2d_height;
    unsigned s2d_unk1;
    unsigned s2d_unk2;
    unsigned s2d_pos_x;
    unsigned s2d_pos_y;
    U8  s2d_unk3;
    U8  s2d_unk4;
};
struct SPRITE {
    struct SHAPE2D far *sprite_bitmapptr;
    U16S  sprite_unk1;
    U16S  sprite_unk2;
};
/* PORT: Track records use 16-bit near pointers and MSC default structure alignment; keep verified offsets. */
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    struct SHAPE3D *ss_shapePtr;
    struct SHAPE3D *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType;
    I8 ss_ignoreZBias;
    I8 ss_multiTileFlag;
    I8 ss_physicalModel;
    I8 scene_unk5;
};
/* PORT: Game-info records are byte/word layouts from the original 16-bit executable; preserve packing and widths. */
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};

extern struct TRACKOBJECT trklst[];
extern struct GAMEINFO globalgamesettings;
static U8  far *terrain_tile_shapes[TERRAIN_SHAPE_COUNT];
static struct SHAPE2D far *track_editor_cursors[TRACK_EDITOR_CURSOR_COUNT];
static struct SHAPE2D far *road_tile_shapes[ROAD_TILE_SHAPE_COUNT];
static U8  far *piece_mask_shapes[TRACK_PIECE_SHAPE_COUNT];
static U8  far *piece_fill_shapes[TRACK_PIECE_SHAPE_COUNT];
static U8  far *palette_piece_layout;
extern struct SPRITE far *g_wndspr;
unsigned char far *td14tb;
unsigned char far *td15p_9;
extern U8  far *g_column_of_trkdata21_pth;
extern U8  far *tdfrompathrow22;
extern U8  far *main_data_file_addr;
int gterrtrk[30];
int lnoffsets[30];
extern I8 resbuftext[];
unsigned char sampled_trk_column;
extern U8  g_cur_track_row;
extern I16 pixel_scales;
extern I16 performGraphColor;
extern I16 dialogarg2;
extern I16 dlg_colour;
extern I16 palette_window_line_color, palette_window_fill_color, palette_window_line_style;
extern I16 text_cursor_outline_color;
extern I16 menu_button_color_a, menu_button_color_b, menu_button_color_c;
extern I16 g_trackpiecescounter;
extern I16 msecoordx, pos_y_ms;
extern U8  flagsdown;
extern I8 buf_g_path[];
extern I8 track_file[];
extern U8  g_is_busy;

extern U8  far * far file_load_shape2d_fatal_thunk(I8 *name);
extern void far locate_many_resources(U8  far *data, I8 *names, I8 far **result);
extern U8  far * far file_load_resource_file(I8 *name);
extern U8  far * far locate_shape_alt(U8  far *data, I8 *name);
extern U8  far * far locate_shape_fatal(U8  far *data, I8 *name);
extern I8 far * far locate_text_resource(U8  far *data, I8 *name);
extern struct SPRITE far * far sprite_make_window(I16 width, I16 height, I16 flags);
extern void far sprite_copy_wnd_to_1_clear(void);
extern void far draw_button();
extern void far draw_lines_unknown();
extern void far sprite_copy_wnd_to_1(void);
extern void far sprset1size(I16 left, I16 right, I16 top, I16 bottom);
extern void far sprite_setup1_from_arg_pointer(struct SPRITE far *sprite);
extern I16 far mouse_track_op();
extern void far sprite_blit_to_video(struct SPRITE far *sprite, I16 mode);
extern void far preRender_line(I16 x1, I16 y1, I16 x2, I16 y2, I16 color);
extern void far sprcopy2to12(void);
extern void far sprite_shape_to_1(void far *shape, I16 x, I16 y);
extern void far sprite_clear_shape_alt(void far *shape, I16 x, I16 y);
extern void far sprite_putimage_and_alt(void far *shape, I16 x, I16 y);
extern void far sprite_putimage_and(void far *shape, I16 x, I16 y);
extern void far sprite_putimage_or(void far *shape, I16 x, I16 y);
extern void far putpixel_iconMask(void far *shape, I16 x, I16 y);
extern void far putpixel_iconFillings(void far *shape, I16 x, I16 y);
extern void far msdrawopaquechk(void);
extern void far msdrawtransparentchk(void);
extern void far font_setup_unknown(I16 colour, I16 mode);
extern void far copy_string(I8 *destination, I8 far *source);
extern I16 far font_op2(I8 *name);
extern void far draw_text_at(I8 *text, I16 x, I16 y);
extern void far sprite_1_unk(I16 x, I16 y, I16 width, I16 height, I16 color);
extern I16 far show_dialog();
extern I16 far timer_get_delta_alt(void);
extern I16 far input_checking(I16 delta);
extern I8 far mouse_multi_hittest(I16 count, I16 *x1, I16 *x2, I16 *y1, I16 *y2);
extern void far timer_get_counter_unk(I32 ticks);
extern I8 far track_setup(void);
extern void far check_input(void);
extern I8 far do_fileselect_dialog(I8 *dir, I8 *name, I8 *ext, I8 far *title);
extern I8 far do_savefile_dialog(I8 *dir, I8 *name, I8 far *title);
extern void far file_build_path(I8 *dir, I8 *name, I8 *ext, I8 *dst);
extern void far file_read_fatal(I8 *path, U8  far *buffer);
extern I16 far file_write_fatal(I8 *path, U8  far *buffer, I32 size);
extern I16 far file_find(I8 *query);
extern void far highscore_write_a(I16 mode);
extern void far draw_rect_outline(I16 x1, I16 y1, I16 x2, I16 y2, I16 color);
extern void far sprite_free_window(struct SPRITE far *sprite);
extern void far unload_resource(U8  far *data);
extern void far mmgr_free(U8  far *data);
extern U8  far subst_hillroad(U16  terrain, U16  element);

I8 validate_track_elements();
void clear_invalid_track_tiles();
void preRender_icons();
void draw_2DtrackMap();

I8 aEokenseieemseedewwefuenpestej[] = "eokenseieemseedewwefuenpestejsejdeteewaefteat";
I8 aTer0[] = "ter0";
U16  function_key_scan_codes[12] = { BIOS_KEY_F1, BIOS_KEY_F2, BIOS_KEY_F3, BIOS_KEY_F4, BIOS_KEY_F5, BIOS_KEY_F6, BIOS_KEY_F7, BIOS_KEY_F8, BIOS_KEY_F9, BIOS_KEY_F10, 0, 0 }; /* PLATFORM(input_kb): maps BIOS F1-F10 return values to menu shortcuts. */
I16 trackmenu2_buttons_x1[5] = { 9, 202, 220, 8, 220 };
I16 trackmenu2_buttons_x2[5] = { 199, 206, 315, 199, 315 };
I16 trackmenu2_buttons_y1[5] = { 181, 4, 132, 4, 36 };
I16 trackmenu2_buttons_y2[5] = { 187, 179, 139, 179, 187 };
U8  palette_column_limits[2] = { TRACK_MAP_COLUMN_COUNT, 6 };
U8  palette_row_limits[2] = { 29, 9 };
I8 aFlatlakelak1lak2lak3lak4highg[] = "flatlakelak1lak2lak3lak4highgoungouwgousgouegou1gou2gou3gou4gou5gou6gou7gou8";
I8 aCrs0crs1crs2crs3[] = "crs0crs1crs2crs3";
I8 aUcr0ucr1ucr2ucr3[] = "ucr0ucr1ucr2ucr3";

void load_tracks_menu_shapes(void)
{ /* PURPOSE: Load track editor assets, draw the menu and map preview, then process selection and file actions. Params: none. Returns: void. Globals: reads track/map data, resource and tile caches, palette layout and menu settings; writes selection state, map/resource pointers and activity flags. */ /* PLATFORM(file): uses file and resource services. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(input_mouse): polls or updates mouse state. */ /* PLATFORM(timer): registers, removes, or reads the game timer. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(memory): allocates or releases game-managed memory. */
    I8 answer;
    U8  paletteModified;
    I8S  originX;
    I8S  objectHeight;
    U8  far *mediumTextData;
    I16 trackStep;
    I8S  drawnMode;
    I8S  shapeWidth;
    I8S  errorMsg;
    U8  far *terrainTemplate;
    U8  tmpShape;
    I8S  boxHeight;
    U8  far *shortNames;
    U8  far *teditData;
    U8  savedShape;
    U8  far *sdtBuffer;
    I8S  cursorPixW;
    I8S  lastType;
    U8  pathFlag;
    I8S  imageMode;
    I8S  paletteArea;
    U8  mapChanged;
    I16 cursorLeft;
    U16  key;
    I8S  activeGroup;
    I8S  tileSize;
    U8  lastHoverShape;
    I8S  droppedPosY;
    U8  terrainCache[132];
    I16 animationCount;
    I16 lastTextWidth;
    I8S  destPosX;
    U8  mapDirty;
    U8  far *pieceNames;
    register I16 j;
    I16 blinkFlag;
    I8S  lastScrollX;
    I8 far *textPtr;
    U8  boxMarker;
    I8S  saveOutcome;
    I8S  destPosY;
    struct SPRITE far *windows[4];
    U8  elementState[132];
    register I16 stepTime;
    I16 screenPosY;
    I8S  lastPutCol;
    I8S  prevViewTop;
    I8S  selRow[2];
    U8  value;
    U8  sliderChanged;
    U8  hovered;
    I8S  selectCol[2];
    I8S  viewTop;
    I8S  hitArea;
    U8  inEditor;
    U8  oldCell;
    U8  selectedPiece;


    sdtBuffer = file_load_shape2d_fatal_thunk("sdtedit") /* PLATFORM(file): load a 2D shape or enter the fatal-error path. */;
    locate_many_resources(sdtBuffer, aFlatlakelak1lak2lak3lak4highg, terrain_tile_shapes) /* PLATFORM(file): resolve the requested names in one resource block. */;
    locate_many_resources(sdtBuffer, aCrs0crs1crs2crs3, (I8 far **)track_editor_cursors) /* PLATFORM(file): resolve the requested names in one resource block. */;
    locate_many_resources(sdtBuffer, aUcr0ucr1ucr2ucr3, (I8 far **)road_tile_shapes) /* PLATFORM(file): resolve the requested names in one resource block. */;
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
        elementState[j] = TRACK_CACHE_INVALID;
        terrainCache[j] = TRACK_CACHE_INVALID;
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
            while (palette_piece_layout[selRow[1] * TRACK_PALETTE_GRID_SIDE + activeGroup * TRACK_PALETTE_PAGE_CELL_COUNT + selectCol[1]] >= TRACK_ELEMENT_CONTINUATION_BASE) {
                if (palette_piece_layout[selRow[1] * TRACK_PALETTE_GRID_SIDE + activeGroup * TRACK_PALETTE_PAGE_CELL_COUNT + selectCol[1]] == TRACK_TILE_CONTINUE_RIGHT)
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
            lastHoverShape = TRACK_SHAPE_CACHE_INVALID;
        }

        sprcopy2to12() /* PLATFORM(video): copy the current sprite into the active buffer. */;
        if (!paletteArea) {
            cursorPixW = shapeWidth << TRACK_EDITOR_CELL_SHIFT;
            boxHeight = objectHeight << TRACK_EDITOR_CELL_SHIFT;
            cursorLeft = ((selectCol[0] - originX) << TRACK_EDITOR_CELL_SHIFT) + 8;
            screenPosY = ((selRow[0] - viewTop) << TRACK_EDITOR_CELL_SHIFT) + 4;
            hovered = td14tb[lnoffsets[selRow[0]] + selectCol[0]];
            switch (hovered) {
            case TRACK_TILE_CONTINUE_DOWN_RIGHT:
                hovered = td14tb[lnoffsets[selRow[0] - 1] + selectCol[0] - 1];
                break;
            case TRACK_TILE_CONTINUE_DOWN:
                hovered = td14tb[lnoffsets[selRow[0] - 1] + selectCol[0]];
                break;
            case TRACK_TILE_CONTINUE_RIGHT:
                hovered = td14tb[lnoffsets[selRow[0]] + selectCol[0] - 1];
                break;
            }
        } else {
            cursorPixW = TRACK_EDITOR_CELL_PIXELS;
            boxHeight = TRACK_EDITOR_CELL_PIXELS;
            screenPosY = (selRow[1] << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_GRID_Y_ORIGIN;
            if (selRow[1] == TRACK_PALETTE_GRID_SIDE) {
                cursorLeft = TRACK_EDITOR_GRID_X_ORIGIN;
                boxHeight = 8;
                cursorPixW = (TRACK_PALETTE_GRID_SIDE * TRACK_EDITOR_CELL_PIXELS);
            } else if (selRow[1] == 7) {
                screenPosY -= 8;
                selectCol[1] = 0;
                cursorLeft = TRACK_EDITOR_GRID_X_ORIGIN;
                cursorPixW = (TRACK_PALETTE_GRID_SIDE * TRACK_EDITOR_CELL_PIXELS);
                hovered = 0;
            } else if (selRow[1] > 7) {
                screenPosY -= 8;
                if (selectCol[1] < 3)
                    selectCol[1] = 0;
                else
                    selectCol[1] = 3;
                cursorLeft = (selectCol[1] << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_GRID_X_ORIGIN;
                cursorPixW = (3 * TRACK_EDITOR_CELL_PIXELS);
                hovered = 0;
            } else {
                cursorLeft = (selectCol[1] << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_GRID_X_ORIGIN;
                if (selRow[1] < 5 &&
                        palette_piece_layout[selRow[1] * TRACK_PALETTE_GRID_SIDE + activeGroup * TRACK_PALETTE_PAGE_CELL_COUNT + selectCol[1] + TRACK_PALETTE_GRID_SIDE] == TRACK_TILE_CONTINUE_DOWN)
                    boxHeight = (2 * TRACK_EDITOR_CELL_PIXELS);
                if (selectCol[1] < 5 &&
                        palette_piece_layout[selRow[1] * TRACK_PALETTE_GRID_SIDE + activeGroup * TRACK_PALETTE_PAGE_CELL_COUNT + selectCol[1] + 1] == TRACK_TILE_CONTINUE_RIGHT)
                    cursorPixW = (2 * TRACK_EDITOR_CELL_PIXELS);
                hovered = palette_piece_layout[selRow[1] * TRACK_PALETTE_GRID_SIDE + activeGroup * TRACK_PALETTE_PAGE_CELL_COUNT + selectCol[1]];
                if (hovered >= TRACK_ELEMENT_SPECIAL_BASE)
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
                    destPosX = (msecoordx - TRACK_EDITOR_CELL_ORIGIN_X) / TRACK_EDITOR_CELL_PIXELS;
                    destPosY = (pos_y_ms - TRACK_EDITOR_CELL_ORIGIN_Y) / TRACK_EDITOR_CELL_PIXELS;
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
                    destPosX = (msecoordx - TRACK_EDITOR_GRID_X_ORIGIN) / TRACK_EDITOR_CELL_PIXELS;
                    destPosY = (pos_y_ms - TRACK_EDITOR_GRID_Y_ORIGIN) / TRACK_EDITOR_CELL_PIXELS;
                    if (destPosY < 6) {
                        if (palette_piece_layout[activeGroup * TRACK_PALETTE_PAGE_CELL_COUNT + destPosY * TRACK_PALETTE_GRID_SIDE + destPosX] == TRACK_TILE_CONTINUE_DOWN)
                            --destPosY;
                        if (palette_piece_layout[activeGroup * TRACK_PALETTE_PAGE_CELL_COUNT + destPosY * TRACK_PALETTE_GRID_SIDE + destPosX] == TRACK_TILE_CONTINUE_RIGHT)
                            --destPosX;
                    } else {
                        destPosY = (pos_y_ms - TRACK_EDITOR_PALETTE_ROW_Y_ORIGIN) / TRACK_EDITOR_CELL_PIXELS;
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
        case KEY_ASCII_SPACE:
        case KEY_SCAN_INSERT:
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
                if (selRow[1] < TRACK_PALETTE_GRID_SIDE) {
                    selectedPiece = palette_piece_layout[selRow[1] * TRACK_PALETTE_GRID_SIDE + activeGroup * TRACK_PALETTE_PAGE_CELL_COUNT + selectCol[1]];
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
                    if (selRow[1] == TRACK_PALETTE_GRID_SIDE) {
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
                    if (oldCell >= TRACK_ELEMENT_SPECIAL_BASE)
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
                    td14tb[lnoffsets[droppedPosY + 1] + lastPutCol] = TRACK_TILE_CONTINUE_DOWN;
                    break;
                case 2:
                    td14tb[lnoffsets[droppedPosY] + lastPutCol + 1] = TRACK_TILE_CONTINUE_RIGHT;
                    break;
                case 3:
                    td14tb[lnoffsets[droppedPosY] + lastPutCol + 1] = TRACK_TILE_CONTINUE_RIGHT;
                    td14tb[lnoffsets[droppedPosY + 1] + lastPutCol] = TRACK_TILE_CONTINUE_DOWN;
                    td14tb[lnoffsets[droppedPosY + 1] + lastPutCol + 1] = TRACK_TILE_CONTINUE_DOWN_RIGHT;
                    break;
                }
            }
            check_input();
            break;
        case KEY_SCAN_HOME:
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
        case KEY_SCAN_UP:
            if (selRow[paletteArea] != 0) {
                lastPutCol = -1;
                --selRow[paletteArea];
                if (paletteArea && selRow[1] < TRACK_PALETTE_GRID_SIDE) {
                    while (palette_piece_layout[selRow[1] * TRACK_PALETTE_GRID_SIDE + activeGroup * TRACK_PALETTE_PAGE_CELL_COUNT + selectCol[1]] >= TRACK_ELEMENT_CONTINUATION_BASE) {
                        boxMarker = palette_piece_layout[selRow[1] * TRACK_PALETTE_GRID_SIDE + activeGroup * TRACK_PALETTE_PAGE_CELL_COUNT + selectCol[1]];
                        if (boxMarker == TRACK_TILE_CONTINUE_RIGHT)
                            --selectCol[1];
                        else if (boxMarker == TRACK_TILE_CONTINUE_DOWN)
                            --selRow[1];
                    }
                }
            }
            break;
        case KEY_SCAN_DOWN:
            if (selRow[paletteArea] < palette_row_limits[paletteArea]) {
                lastPutCol = -1;
                ++selRow[paletteArea];
                if (paletteArea && selRow[1] < TRACK_PALETTE_GRID_SIDE) {
                    boxMarker = palette_piece_layout[selRow[1] * TRACK_PALETTE_GRID_SIDE + activeGroup * TRACK_PALETTE_PAGE_CELL_COUNT + selectCol[1]];
                    if (boxMarker == TRACK_TILE_CONTINUE_RIGHT)
                        --selectCol[1];
                    else if (boxMarker == TRACK_TILE_CONTINUE_DOWN)
                        ++selRow[1];
                }
            }
            break;
        case KEY_SCAN_LEFT:
            if (paletteArea && selRow[1] == TRACK_PALETTE_GRID_SIDE) {
                if (activeGroup > 1)
                    --activeGroup;
            } else if (selectCol[paletteArea] != 0) {
                lastPutCol = -1;
                --selectCol[paletteArea];
                if (paletteArea) {
                    if (selRow[1] > 5)
                        selectCol[1] = 0;
                    else
                        while (palette_piece_layout[selRow[1] * TRACK_PALETTE_GRID_SIDE + activeGroup * TRACK_PALETTE_PAGE_CELL_COUNT + selectCol[1]] >= TRACK_ELEMENT_CONTINUATION_BASE) {
                            boxMarker = palette_piece_layout[selRow[1] * TRACK_PALETTE_GRID_SIDE + activeGroup * TRACK_PALETTE_PAGE_CELL_COUNT + selectCol[1]];
                            if (boxMarker == TRACK_TILE_CONTINUE_RIGHT)
                                --selectCol[1];
                            else if (boxMarker == TRACK_TILE_CONTINUE_DOWN)
                                --selRow[1];
                        }
                }
            }
            break;
        case KEY_SCAN_RIGHT:
            if (paletteArea && selRow[1] == TRACK_PALETTE_GRID_SIDE) {
                if (activeGroup < 10)
                    ++activeGroup;
            } else {
                boxMarker = 1;
                if (paletteArea) {
                    if (selRow[1] > 5)
                        boxMarker = 3;
                    else
                        while (selectCol[paletteArea] + boxMarker < palette_column_limits[paletteArea] &&
                                palette_piece_layout[selRow[1] * TRACK_PALETTE_GRID_SIDE + activeGroup * TRACK_PALETTE_PAGE_CELL_COUNT + selectCol[1] + boxMarker] >= TRACK_ELEMENT_CONTINUATION_BASE) {
                            value = palette_piece_layout[selRow[1] * TRACK_PALETTE_GRID_SIDE + activeGroup * TRACK_PALETTE_PAGE_CELL_COUNT + selectCol[1] + boxMarker];
                            if (value == TRACK_TILE_CONTINUE_RIGHT)
                                ++boxMarker;
                            else if (value == TRACK_TILE_CONTINUE_DOWN)
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

void preRender_icons(U8  mode)
{ /* PURPOSE: Draw the six-by-six terrain and track-piece icon grid. Params: mode. Returns: void. Globals: reads palette_piece_layout, piece_fill_shapes, piece_mask_shapes, terrain_tile_shapes, trklst; writes none. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    U8  iconIndex;
    U8  row;
    U8  stateId;
    for (iconIndex = 0; iconIndex < TRACK_PALETTE_GRID_SIDE; ++iconIndex) {
        for (row = 0; row < TRACK_PALETTE_GRID_SIDE; ++row) {
            stateId = palette_piece_layout[mode * TRACK_PALETTE_PAGE_CELL_COUNT + iconIndex * TRACK_PALETTE_GRID_SIDE + row];
            if (mode == 0) {
                sprite_shape_to_1(terrain_tile_shapes[stateId], TRACK_EDITOR_GRID_X_ORIGIN + (row << TRACK_EDITOR_CELL_SHIFT),
                    TRACK_EDITOR_GRID_Y_ORIGIN + (iconIndex << TRACK_EDITOR_CELL_SHIFT)) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
            } else if (stateId < TRACK_ELEMENT_SPECIAL_BASE) {
                sprite_shape_to_1(terrain_tile_shapes[0], TRACK_EDITOR_GRID_X_ORIGIN + (row << TRACK_EDITOR_CELL_SHIFT),
                    TRACK_EDITOR_GRID_Y_ORIGIN + (iconIndex << TRACK_EDITOR_CELL_SHIFT)) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                switch (trklst[stateId].ss_multiTileFlag) {
                case 1:
                    sprite_shape_to_1(terrain_tile_shapes[0], TRACK_EDITOR_GRID_X_ORIGIN + (row << TRACK_EDITOR_CELL_SHIFT),
                        TRACK_EDITOR_GRID_Y_ORIGIN + TRACK_EDITOR_CELL_PIXELS + (iconIndex << TRACK_EDITOR_CELL_SHIFT)) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                    break;
                case 2:
                    sprite_shape_to_1(terrain_tile_shapes[0], TRACK_EDITOR_GRID_X_ORIGIN + TRACK_EDITOR_CELL_PIXELS + (row << TRACK_EDITOR_CELL_SHIFT),
                        TRACK_EDITOR_GRID_Y_ORIGIN + (iconIndex << TRACK_EDITOR_CELL_SHIFT)) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                    break;
                case 3:
                    sprite_shape_to_1(terrain_tile_shapes[0], TRACK_EDITOR_GRID_X_ORIGIN + TRACK_EDITOR_CELL_PIXELS + (row << TRACK_EDITOR_CELL_SHIFT),
                        TRACK_EDITOR_GRID_Y_ORIGIN + (iconIndex << TRACK_EDITOR_CELL_SHIFT)) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                    sprite_shape_to_1(terrain_tile_shapes[0], TRACK_EDITOR_GRID_X_ORIGIN + (row << TRACK_EDITOR_CELL_SHIFT),
                        TRACK_EDITOR_GRID_Y_ORIGIN + TRACK_EDITOR_CELL_PIXELS + (iconIndex << TRACK_EDITOR_CELL_SHIFT)) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                    sprite_shape_to_1(terrain_tile_shapes[0], TRACK_EDITOR_GRID_X_ORIGIN + TRACK_EDITOR_CELL_PIXELS + (row << TRACK_EDITOR_CELL_SHIFT),
                        TRACK_EDITOR_GRID_Y_ORIGIN + TRACK_EDITOR_CELL_PIXELS + (iconIndex << TRACK_EDITOR_CELL_SHIFT)) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                    break;
                }
                putpixel_iconMask(piece_mask_shapes[stateId], TRACK_EDITOR_GRID_X_ORIGIN + (row << TRACK_EDITOR_CELL_SHIFT),
                    TRACK_EDITOR_GRID_Y_ORIGIN + (iconIndex << TRACK_EDITOR_CELL_SHIFT)) /* PLATFORM(video): apply a track-piece icon mask. */;
                putpixel_iconFillings(piece_fill_shapes[stateId],
                    TRACK_EDITOR_GRID_X_ORIGIN + (row << TRACK_EDITOR_CELL_SHIFT), TRACK_EDITOR_GRID_Y_ORIGIN + (iconIndex << TRACK_EDITOR_CELL_SHIFT)) /* PLATFORM(video): apply a track-piece icon fill. */;
            }
        }
    }
}

void draw_2DtrackMap(U8  rowBase, U8  columnBase, U8  *lastElement, U8  *lastTerrain)
{ /* PURPOSE: Draw the cached twelve-by-eleven track map preview. Params: rowBase, columnBase, lastElement, lastTerrain. Returns: void. Globals: reads gterrtrk, lnoffsets, piece_fill_shapes, piece_mask_shapes, td14tb, td15p_9, terrain_tile_shapes, trklst; writes none. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    U16  rowIndex;
    I8S  columnIndex;
    I8S  mapRow;
    U16  mapIndex;
    U8  surface;
    U8  tileId;

    for (mapRow = 0; mapRow < TRACK_PREVIEW_ROW_COUNT; ++mapRow) {
        rowIndex = mapRow * TRACK_PREVIEW_COLUMN_COUNT; /* PORT: signed 8-bit mapRow is promoted to 16-bit int; keep the target row-major index width. */
        for (columnIndex = 0; columnIndex < TRACK_PREVIEW_COLUMN_COUNT; ++columnIndex) {
            tileId = td14tb[lnoffsets[columnBase + mapRow] + columnIndex + rowBase]; /* PORT: this index arithmetic is 16-bit int over a far resource table; preserve segment-bounded access. */
            surface = td15p_9[gterrtrk[columnBase + mapRow] + columnIndex + rowBase];
            mapIndex = rowIndex + columnIndex;

            if (tileId >= TRACK_ELEMENT_SPECIAL_BASE && (mapRow == 0 || columnIndex == 0)) {
                lastElement[mapIndex] = TRACK_CACHE_INVALID; /* PORT: 0xff is the byte-cache invalid sentinel, distinct from the serialized continuation codes. */
                if (tileId == TRACK_TILE_CONTINUE_RIGHT && columnIndex == 0) {
                    sprite_putimage_and_alt(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow] + columnIndex + rowBase]],
                        (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                    sprite_putimage_and_alt(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow + 1] + columnIndex + rowBase]],
                        (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_BELOW_Y) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                    sprite_putimage_and(piece_mask_shapes[td14tb[lnoffsets[columnBase + mapRow] + columnIndex + rowBase - 1]],
                        (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_LEFT_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply a sprite mask at the requested position. */;
                    sprite_putimage_or(piece_fill_shapes[td14tb[lnoffsets[columnBase + mapRow] + columnIndex + rowBase - 1]],
                        (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_LEFT_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply sprite pixels at the requested position. */;
                } else if (tileId == TRACK_TILE_CONTINUE_DOWN && mapRow == 0) {
                    sprite_putimage_and_alt(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow] + columnIndex + rowBase]],
                        (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                    sprite_putimage_and_alt(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow] + columnIndex + rowBase + 1]],
                        (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_RIGHT_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                    sprite_putimage_and(piece_mask_shapes[td14tb[lnoffsets[columnBase + mapRow - 1] + columnIndex + rowBase]],
                        (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ABOVE_Y) /* PLATFORM(video): apply a sprite mask at the requested position. */;
                    sprite_putimage_or(piece_fill_shapes[td14tb[lnoffsets[columnBase + mapRow - 1] + columnIndex + rowBase]],
                        (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ABOVE_Y) /* PLATFORM(video): apply sprite pixels at the requested position. */;
                } else if (tileId == TRACK_TILE_CONTINUE_DOWN_RIGHT && mapRow == 0 && columnIndex == 0) {
                    sprite_putimage_and_alt(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow] + columnIndex + rowBase]],
                        (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                    sprite_putimage_and(piece_mask_shapes[td14tb[lnoffsets[columnBase + mapRow - 1] + columnIndex + rowBase - 1]],
                        (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_LEFT_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ABOVE_Y) /* PLATFORM(video): apply a sprite mask at the requested position. */;
                    sprite_putimage_or(piece_fill_shapes[td14tb[lnoffsets[columnBase + mapRow - 1] + columnIndex + rowBase - 1]],
                        (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_LEFT_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ABOVE_Y) /* PLATFORM(video): apply sprite pixels at the requested position. */;
                }
            } else if (tileId == 0) {
                if (lastElement[mapIndex] != 0 || lastTerrain[mapIndex] != surface) {
                    sprite_shape_to_1(terrain_tile_shapes[surface], (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                    lastElement[mapIndex] = 0;
                    lastTerrain[mapIndex] = surface;
                }
            } else if (tileId < TRACK_ELEMENT_SPECIAL_BASE) {
                if (lastElement[mapIndex] != tileId || lastTerrain[mapIndex] != surface) {
                    lastElement[mapIndex] = tileId;
                    lastTerrain[mapIndex] = surface;
                    sprite_shape_to_1(terrain_tile_shapes[surface], (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                    switch (trklst[tileId].ss_multiTileFlag) {
                    case 0:
                        putpixel_iconMask(piece_mask_shapes[tileId], (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply a track-piece icon mask. */;
                        putpixel_iconFillings(piece_fill_shapes[tileId], (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply a track-piece icon fill. */;
                        break;
                    case 1:
                        sprite_putimage_and_alt(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow + 1] + columnIndex + rowBase]],
                            (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_BELOW_Y) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                        sprite_putimage_and(piece_mask_shapes[tileId], (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply a sprite mask at the requested position. */;
                        sprite_putimage_or(piece_fill_shapes[tileId], (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply sprite pixels at the requested position. */;
                        break;
                    case 2:
                        sprite_putimage_and_alt(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow] + columnIndex + rowBase + 1]],
                            (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_RIGHT_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                        sprite_putimage_and(piece_mask_shapes[tileId], (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply a sprite mask at the requested position. */;
                        sprite_putimage_or(piece_fill_shapes[tileId], (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply sprite pixels at the requested position. */;
                        break;
                    case 3:
                        sprite_putimage_and_alt(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow] + columnIndex + rowBase + 1]],
                            (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_RIGHT_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                        sprite_putimage_and_alt(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow + 1] + columnIndex + rowBase]],
                            (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_BELOW_Y) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                        sprite_putimage_and_alt(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow + 1] + columnIndex + rowBase + 1]],
                            (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_RIGHT_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_BELOW_Y) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                        sprite_putimage_and(piece_mask_shapes[tileId], (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply a sprite mask at the requested position. */;
                        sprite_putimage_or(piece_fill_shapes[tileId], (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply sprite pixels at the requested position. */;
                        break;
                    }
                }
            } else {
                lastElement[mapIndex] = TRACK_CACHE_INVALID; /* PORT: 0xff is the byte-cache invalid sentinel, distinct from the serialized continuation codes. */
                lastTerrain[mapIndex] = TRACK_CACHE_INVALID;
            }
        }
    }
}

I8 validate_track_elements(void)
{ /* PURPOSE: Check track/terrain pairs, clear invalid entries, and return the error code. Params: none. Returns: char. Globals: reads gterrtrk, lnoffsets, td14tb, td15p_9; writes none. */
    U8  elem, colidx;
    U8  terrain, rowno;
    I8 error;

    clear_invalid_track_tiles();
    error = 0;
    for (rowno = 0; rowno < TRACK_MAP_ROW_COUNT; ++rowno) {
        for (colidx = 0; colidx < TRACK_MAP_COLUMN_COUNT; ++colidx) {
            terrain = td15p_9[gterrtrk[rowno] + colidx];
            elem = td14tb[lnoffsets[rowno] + colidx];
            if (elem != 0 && terrain != 0 && terrain != 6) {
                switch (terrain) {
                case 1: case 2: case 3: case 4: case 5:
                    if (elem == TRACK_TILE_CONTINUE_RIGHT)
                        elem = td14tb[lnoffsets[rowno] + colidx - 1];
                    else if (elem == TRACK_TILE_CONTINUE_DOWN)
                        elem = td14tb[lnoffsets[rowno - 1] + colidx];
                    else if (elem == TRACK_TILE_CONTINUE_DOWN_RIGHT)
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
    U8  used[TRACK_MAP_CELL_COUNT];
    U8  rowIdx;
    U8  x;
    U8  element;
    register I16 clear;

    for (clear = 0; clear < TRACK_MAP_CELL_COUNT; ++clear)
        used[clear] = 0;

    for (rowIdx = 0; rowIdx < TRACK_MAP_ROW_COUNT; ++rowIdx) {
        for (x = 0; x < TRACK_MAP_COLUMN_COUNT; ++x) {
            element = td14tb[lnoffsets[rowIdx] + x];
            if (element != 0) {
                if (element >= TRACK_ELEMENT_SPECIAL_BASE) {
                    if (used[lnoffsets[rowIdx] + x] == 0)
                        td14tb[lnoffsets[rowIdx] + x] = 0;
                } else {
                    switch (trklst[element].ss_multiTileFlag) {
                    case 1:
                        if (used[lnoffsets[rowIdx + 1] + x] != 0)
                            td14tb[lnoffsets[rowIdx] + x] = 0;
                        else if (td14tb[lnoffsets[rowIdx + 1] + x] != TRACK_TILE_CONTINUE_DOWN)
                            td14tb[lnoffsets[rowIdx] + x] = 0;
                        else
                            used[lnoffsets[rowIdx + 1] + x] = 1;
                        break;
                    case 2:
                        if (used[lnoffsets[rowIdx] + x + 1] != 0) {
                            td14tb[lnoffsets[rowIdx] + x] = 0;
                        } else {
                            if (td14tb[lnoffsets[rowIdx] + x + 1] != TRACK_TILE_CONTINUE_RIGHT)
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
                        } else if (td14tb[lnoffsets[rowIdx] + x + 1] != TRACK_TILE_CONTINUE_RIGHT ||
                                   td14tb[lnoffsets[rowIdx + 1] + x] != TRACK_TILE_CONTINUE_DOWN ||
                                   td14tb[lnoffsets[rowIdx + 1] + x + 1] != TRACK_ELEMENT_SPECIAL_BASE) {
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
