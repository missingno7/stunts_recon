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
/* Local view: SHAPE2D; struct.count is 8; another unit uses 6. */
struct SHAPE2D {
    I16 s2d_width;
    I16 s2d_height;
    unsigned s2d_hotspot_x;
    unsigned s2d_hotspot_y;
    unsigned s2d_pos_x;
    unsigned s2d_pos_y;
    U8  s2d_plane_color0;
    U8  s2d_plane_color1_bg;
};
/* Local view: SPRITE; struct.count is 3; another unit uses 2. */
struct SPRITE {
    struct SHAPE2D far *sprite_bitmapptr;
    U16S  sprite_unk1;
    U16S  sprite_unk2;
};
/* PORT: Track records use 16-bit near pointers and MSC default structure alignment; keep verified offsets. */
#include "trkobj.h"

/* PORT: Game-info records are byte/word layouts from the original 16-bit executable; preserve packing and widths. */
/* Local view: GAMEINFO; struct[8].type is unsigned word; another unit uses signed byte. */
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
#include "font.h"
#include "game.h"
#include "gfxapi.h"
#include "input.h"
#include "prim3d.h"
#include "resapi.h"
#include "shape2d.h"
#include "track.h"
#include "uiapi.h"


/* Local view: trklst; array is None; another unit uses 215. */
extern struct TRACKOBJECT trklst[];

static U8  far *terrain_tile_shapes[TERRAIN_SHAPE_COUNT];
static struct SHAPE2D far *track_editor_cursors[TRACK_EDITOR_CURSOR_COUNT];
static struct SHAPE2D far *road_tile_shapes[ROAD_TILE_SHAPE_COUNT];
static U8  far *piece_mask_shapes[TRACK_PIECE_SHAPE_COUNT];
static U8  far *piece_fill_shapes[TRACK_PIECE_SHAPE_COUNT];
static U8  far *palette_piece_layout;

/* Local view: td14tb; pointer is unsigned byte; another unit uses signed byte. */
unsigned char far *td14tb;
unsigned char far *td15p_9;
/* Local view: g_column_of_trkdata21_pth; pointer is unsigned byte; another unit uses signed byte. */
extern U8  far *g_column_of_trkdata21_pth;
/* Local view: tdfrompathrow22; pointer is unsigned byte; another unit uses signed byte. */
extern U8  far *tdfrompathrow22;
/* Local view: main_data_file_addr; pointer is unsigned byte; another unit uses signed byte. */
extern U8  far *main_data_file_addr;
int gterrtrk[30];
int lnoffsets[30];
/* Local view: resbuftext; array is None; another unit uses 80. */
extern I8 resbuftext[];
unsigned char sampled_trk_column;



/* Local view: dialog_frame_color; type is signed word; another unit uses unsigned word. */
extern I16 dialog_frame_color;
/* Local view: dlg_colour; type is signed word; another unit uses unsigned word. */
extern I16 dlg_colour;





/* Local view: flagsdown; type is unsigned byte; another unit uses signed word. */
extern U8  flagsdown;


/* Local view: g_is_busy; type is unsigned byte; another unit uses signed byte. */
extern U8  g_is_busy;

/* Local view: file_load_shape2d_fatal_thunk; return.pointer is unsigned byte; another unit uses void. */
extern U8  far * far file_load_shape2d_fatal_thunk(I8 *name);
/* Local view: locate_many_resources; parameters[0].pointer is unsigned byte; another unit uses void. */
extern void far locateshapes(U8  far *data, I8 *names, I8 far **result);
/* Local view: file_load_resource_file; return.pointer is unsigned byte; another unit uses void. */
extern U8  far * far openresource(I8 *name);
/* Local view: locate_shape_alt; return.pointer is unsigned byte; another unit uses void. */
extern U8  far * far getres(U8  far *data, I8 *name);
/* Local view: locate_shape_fatal; return.pointer is unsigned byte; another unit uses struct SHAPE2D. */
extern U8  far * far locate_shape_fatal(U8  far *data, I8 *name);
/* Local view: locate_text_resource; parameters[0].pointer is unsigned byte; another unit uses signed byte. */
extern I8 far * far getresl(U8  far *data, I8 *name);
/* Local view: sprite_make_window; return.pointer is struct SPRITE; another unit uses void. */
extern struct SPRITE far * far sprite_make_window(I16 width, I16 height, I16 flags);




/* Local view: sprset1size; parameters[0] is signed word; another unit uses unsigned word. */
extern void far clippix(I16 left, I16 right, I16 top, I16 bottom);
/* Local view: sprite_setup1_from_arg_pointer; parameters[0].pointer is struct SPRITE; another unit uses void. */
extern void far sprite_setup1_from_arg_pointer(struct SPRITE far *sprite);

/* Local view: sprite_blit_to_video; return is void; another unit uses signed word. */
extern void far sprite_blit_to_video(struct SPRITE far *sprite, I16 mode);


/* Local view: movfxya; parameters[0].pointer is void; another unit uses struct SHAPE2D. */
extern void far movfxya(void far *shape, I16 x, I16 y);
/* Local view: sprite_clear_shape_alt; parameters[0].pointer is void; another unit uses struct SHAPE2D. */
extern void far readfxya(void far *shape, I16 x, I16 y);
/* Local view: sprite_putimage_and_alt; parameters[0].pointer is void; another unit uses struct SHAPE2D. */
extern void far movcfxya(void far *shape, I16 x, I16 y);
/* Local view: andcfxya; parameters[0].pointer is void; another unit uses struct SHAPE2D. */
extern void far andcfxya(void far *shape, I16 x, I16 y);
/* Local view: orcfxya; parameters[0].pointer is void; another unit uses struct SHAPE2D. */
extern void far orcfxya(void far *shape, I16 x, I16 y);




/* Local view: font_setup_unknown; parameters[0] is signed word; another unit uses unsigned word. */
extern void far textcolor(I16 colour, I16 mode);



/* Local view: drawrect; return is void; another unit uses signed word. */
extern void far drawrect(I16 x, I16 y, I16 width, I16 height, I16 color);
/* Local view: messagebox; return is signed word; another unit uses unsigned word. */
extern I16 far messagebox();
/* Local view: timer_get_delta_alt; return is signed word; another unit uses unsigned long. */
extern I16 far elticks(void);

/* Local view: mousecursor; return is signed byte; another unit uses signed word. */
extern I8 far mousecursor(I16 count, I16 *x1, I16 *x2, I16 *y1, I16 *y2);
/* Local view: timedwait; return is void; another unit uses unsigned long. */
extern void far timedwait(I32 ticks);
/* Local view: compiletrack; return is signed byte; another unit uses signed word. */
extern I8 far compiletrack(void);

/* Local view: do_fileselect_dialog; parameters[3].pointer is signed byte; another unit uses void. */
extern I8 far floadreq(I8 *dir, I8 *name, I8 *ext, I8 far *title);
/* Local view: fsavereq; parameters[2].pointer is signed byte; another unit uses void. */
extern I8 far fsavereq(I8 *dir, I8 *name, I8 far *title);


/* Local view: file_write_fatal; parameters[1].pointer is unsigned byte; another unit uses struct HighScoreRecord. */
extern I16 far savefilez(I8 *path, U8  far *buffer, I32 size);
/* Local view: findfirst; return is signed word; another unit uses near pointer. */
extern I16 far findfirst(I8 *query);
/* Local view: highscore_write_a; return is void; another unit uses signed byte. */
extern void far readhighs(I16 mode);

/* Local view: sprite_free_window; parameters[0].pointer is struct SPRITE; another unit uses void. */
extern void far removewindow(struct SPRITE far *sprite);
/* Local view: closeresource; parameters[0].pointer is unsigned byte; another unit uses void. */
extern void far closeresource(U8  far *data);
/* Local view: freememory; parameters[0].pointer is unsigned byte; another unit uses void. */
extern void far freememory(U8  far *data);
/* Local view: roadupcheck; parameters[0] is unsigned word; another unit uses unsigned byte. */
extern U8  far roadupcheck(U16  terrain, U16  element);



/* Local view: drawpiecebox; parameters is None; another unit uses ['u8']. */
void drawpiecebox();
/* Local view: drawtrackbox; parameters is None; another unit uses ['u8', 'u8', {'pointer': 'u8', 'qualifiers': [], 'segment': 'near'}, {'pointer': 'u8', 'qualifiers': [], 'segment': 'near'}]. */
void drawtrackbox();

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

void trackedit(void)
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
    locateshapes(sdtBuffer, aFlatlakelak1lak2lak3lak4highg, terrain_tile_shapes) /* PLATFORM(file): resolve the requested names in one resource block. */;
    locateshapes(sdtBuffer, aCrs0crs1crs2crs3, (I8 far **)track_editor_cursors) /* PLATFORM(file): resolve the requested names in one resource block. */;
    locateshapes(sdtBuffer, aUcr0ucr1ucr2ucr3, (I8 far **)road_tile_shapes) /* PLATFORM(file): resolve the requested names in one resource block. */;
    windows[0] = sprite_make_window(track_editor_cursors[0]->s2d_width * pixel_scales,
        track_editor_cursors[0]->s2d_height, 15) /* PLATFORM(video): allocate a temporary sprite window. */;
    windows[1] = sprite_make_window(track_editor_cursors[1]->s2d_width * pixel_scales,
        track_editor_cursors[1]->s2d_height, 15) /* PLATFORM(video): allocate a temporary sprite window. */;
    windows[2] = sprite_make_window(track_editor_cursors[2]->s2d_width * pixel_scales,
        track_editor_cursors[2]->s2d_height, 15) /* PLATFORM(video): allocate a temporary sprite window. */;
    windows[3] = sprite_make_window(track_editor_cursors[3]->s2d_width * pixel_scales,
        track_editor_cursors[3]->s2d_height, 15) /* PLATFORM(video): allocate a temporary sprite window. */;
    teditData = openresource("tedit") /* PLATFORM(file): load a resource file. */;
    g_wndspr = sprite_make_window(320, 200, 15) /* PLATFORM(video): allocate a temporary sprite window. */;
    palette_piece_layout = getres(teditData, "pbox") /* PLATFORM(file): locate a named shape in resource data. */;
    shortNames = getres(teditData, "snam") /* PLATFORM(file): locate a named shape in resource data. */;
    mediumTextData = getres(teditData, "mnam") /* PLATFORM(file): locate a named shape in resource data. */;
    pieceNames = getres(teditData, "tnam") /* PLATFORM(file): locate a named shape in resource data. */;
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

    usebufferc() /* PLATFORM(video): copy the active window sprite into buffer 1 and clear its color. */;
    draw3dbutton(getresl(teditData, "bti") /* PLATFORM(file): locate a named text entry in resource data. */, 0xd9, 3, 0x66, 0x16,
        menu_button_color_a, menu_button_color_b, menu_button_color_c, 0) /* PLATFORM(video): draw a menu button. */;
    sprite_draw_beveled_frame(5, 0, 0xce, 0xbe, palette_window_line_color, palette_window_fill_color, palette_window_line_style) /* PLATFORM(video): draw menu line primitives. */;
    sprite_draw_beveled_frame(0xd9, 0x20, 0x66, 0x9e, palette_window_line_color, palette_window_fill_color, palette_window_line_style) /* PLATFORM(video): draw menu line primitives. */;
    draw3dbutton(getresl(teditData, "bsc") /* PLATFORM(file): locate a named text entry in resource data. */, 0xdd, 0x8c, 0x5e, 0x0e,
        menu_button_color_a, menu_button_color_b, menu_button_color_c, 0) /* PLATFORM(video): draw a menu button. */;
    draw3dbutton(getresl(teditData, "blo") /* PLATFORM(file): locate a named text entry in resource data. */, 0xdd, 0x9c, 0x2e, 0x0e,
        menu_button_color_a, menu_button_color_b, menu_button_color_c, 0) /* PLATFORM(video): draw a menu button. */;
    draw3dbutton(getresl(teditData, "bsa") /* PLATFORM(file): locate a named text entry in resource data. */, 0xdd, 0xac, 0x2e, 0x0e,
        menu_button_color_a, menu_button_color_b, menu_button_color_c, 0) /* PLATFORM(video): draw a menu button. */;
    draw3dbutton(getresl(teditData, "bcl") /* PLATFORM(file): locate a named text entry in resource data. */, 0x10d, 0x9c, 0x2e, 0x0e,
        menu_button_color_a, menu_button_color_b, menu_button_color_c, 0) /* PLATFORM(video): draw a menu button. */;
    draw3dbutton(getresl(teditData, "bex") /* PLATFORM(file): locate a named text entry in resource data. */, 0x10d, 0xac, 0x2e, 0x0e,
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
            usebuffer() /* PLATFORM(video): copy the active window sprite into buffer 1. */;
            drawpiecebox(activeGroup) /* PLATFORM(video): render terrain and track icons into the active sprite buffer. */;
            if (activeGroup == 0)
                doslider(0, 0xdd, 0x5f, 0x85, 5, 0, 1, 1) /* PLATFORM(input_mouse): track the mouse and process its current action. */;
            else
                doslider(0, 0xdd, 0x5f, 0x85, 5, activeGroup - 1, 1, 10) /* PLATFORM(input_mouse): track the mouse and process its current action. */;
        }
        if (pathFlag) {
            pathFlag = 0;
            errorMsg = checktracksyntax();
        }
        if (mapChanged || paletteModified) {
            usebuffer() /* PLATFORM(video): copy the active window sprite into buffer 1. */;
            if (mapChanged) {
                mapChanged = 0;
                if (sliderChanged) {
                    sliderChanged = 0;
                    doslider(0, 9, 0xc0, 0xb5, 5, originX, 0x0c, 0x1e) /* PLATFORM(input_mouse): track the mouse and process its current action. */;
                    doslider(0, 0xca, 5, 4, 0xb0, viewTop, 0x0b, 0x1e) /* PLATFORM(input_mouse): track the mouse and process its current action. */;
                }
                clippix(8, 0xc8, 4, 0xb3) /* PLATFORM(video): set the active sprite buffer bounds. */;
                drawtrackbox(originX, viewTop, elementState, terrainCache) /* PLATFORM(video): render the cached track map into the active sprite buffer. */;
                clippix(0, 0x140, 0, 0xc8) /* PLATFORM(video): set the active sprite buffer bounds. */;
            }
            if (paletteModified) {
                paletteModified = 0;
                sprite_setup1_from_arg_pointer(windows[tileSize]) /* PLATFORM(video): select the sprite described by the argument pointer. */;
                if (activeGroup == 0) {
                    movfxya(terrain_tile_shapes[selectedPiece], 0, 0) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                    preRender_line(1, 0, 15, 0, performGraphColor) /* PLATFORM(video): draw a line primitive. */;
                    preRender_line(1, 14, 15, 14, performGraphColor) /* PLATFORM(video): draw a line primitive. */;
                    preRender_line(1, 0, 1, 14, performGraphColor) /* PLATFORM(video): draw a line primitive. */;
                    preRender_line(15, 0, 15, 14, performGraphColor) /* PLATFORM(video): draw a line primitive. */;
                } else {
                    movfxya(track_editor_cursors[tileSize], 0, 0) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                    if (selectedPiece != 0) {
                        andfxya(piece_mask_shapes[selectedPiece], 0, 0) /* PLATFORM(video): apply a track-piece icon mask. */;
                        orfxya(piece_fill_shapes[selectedPiece], 0, 0) /* PLATFORM(video): apply a track-piece icon fill. */;
                    }
                }
            }
            sprite_blit_to_video(g_wndspr, imageMode) /* PLATFORM(video): present the active sprite buffer on the display. */;
            imageMode = -2;
            lastHoverShape = TRACK_SHAPE_CACHE_INVALID;
        }

        usescreen() /* PLATFORM(video): copy the current sprite into the active buffer. */;
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
            mouseoff() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
            textcolor(dlg_colour, 0) /* PLATFORM(video): select the font and color mode. */;
            textPtr = pieceNames + hovered * 3;
            resbuftext[0] = textPtr[0];
            resbuftext[1] = textPtr[1];
            resbuftext[2] = textPtr[2];
            strcpyfar(resbuftext, getresl(teditData, resbuftext) /* PLATFORM(file): locate a named text entry in resource data. */);
            j = textpixels(resbuftext) /* PLATFORM(video): load or select a named font. */;
            movtxy(resbuftext, 8, 0xc0) /* PLATFORM(video): draw a text string at screen coordinates. */;
            if (lastTextWidth > j)
                drawrect(j + 8, 0xc0, lastTextWidth - j, 8, 0) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
            mouseon() /* PLATFORM(video): redraw the pointer in transparent mode when needed. */;
            lastTextWidth = j;
            lastHoverShape = hovered;
        }
        if (errorMsg) {
            messagebox(1, 1, getresl(teditData, aEokenseieemseedewwefuenpestej + errorMsg * 3) /* PLATFORM(file): locate a named text entry in resource data. */,
                -1, -1, performGraphColor, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
            errorMsg = 0;
        }

        animationCount = 99;
        blinkFlag = 0;
        mouseoff() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
        if ((drawnMode = paletteArea) == 0)
            readfxya(road_tile_shapes[tileSize], cursorLeft, screenPosY) /* PLATFORM(video): clear the requested shape in sprite buffer 1. */;
        do {
            if (animationCount > 15) {
                mouseoff() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
                if (!paletteArea) {
                    if (blinkFlag)
                        movfxya(road_tile_shapes[tileSize], cursorLeft, screenPosY) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                    else
                        movfxya(windows[tileSize]->sprite_bitmapptr, cursorLeft, screenPosY) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                } else {
                    xorbox(cursorLeft, screenPosY - 1, cursorLeft + cursorPixW,
                        screenPosY + boxHeight - 1, text_cursor_outline_color) /* PLATFORM(video): draw a rectangle outline. */;
                }
                mouseon() /* PLATFORM(video): redraw the pointer in transparent mode when needed. */;
                blinkFlag ^= 1;
                animationCount = 0;
            }
            stepTime = elticks() /* PLATFORM(timer): read the elapsed timer delta through the far entry point. */;
            animationCount += stepTime;
            key = mykey(stepTime) /* PLATFORM(input_kb): poll the configured input devices. */;
            hitArea = mousecursor(5, trackmenu2_buttons_x1, trackmenu2_buttons_x2,
                trackmenu2_buttons_y1, trackmenu2_buttons_y2) /* PLATFORM(input_mouse): test the mouse against candidate rectangles. */;
            if (hitArea != -1) {
                switch (hitArea) {
                case 0:
                    if (flagsdown & 3) {
                        paletteArea = 0;
                        value = doslider(1, 9, 0xc0, 0xb5, 5, originX, 0x0c, 0x1e) /* PLATFORM(input_mouse): track the mouse and process its current action. */;
                        selectCol[0] += value - originX;
                        originX = value;
                        key = 1;
                    }
                    break;
                case 1:
                    if (flagsdown & 3) {
                        paletteArea = 0;
                        value = doslider(1, 0xca, 5, 4, 0xb0, viewTop, 0x0b, 0x1e) /* PLATFORM(input_mouse): track the mouse and process its current action. */;
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
                        activeGroup = doslider(1, 0xdd, 0x5f, 0x85, 5, activeGroup - 1, 1, 10) /* PLATFORM(input_mouse): track the mouse and process its current action. */ + 1;
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
            timedwait(10L) /* PLATFORM(timer): wait or schedule against the requested timer ticks. */;
        if (blinkFlag) {
            mouseoff() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
            if (!drawnMode)
                movfxya(road_tile_shapes[tileSize], cursorLeft, screenPosY) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
            else
                xorbox(cursorLeft, screenPosY - 1, cursorLeft + cursorPixW,
                    screenPosY + boxHeight - 1, text_cursor_outline_color) /* PLATFORM(video): draw a rectangle outline. */;
            mouseon() /* PLATFORM(video): redraw the pointer in transparent mode when needed. */;
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
            j = compiletrack();
            messagebox(1, 1, getresl(teditData, aEokenseieemseedewwefuenpestej + j * 3) /* PLATFORM(file): locate a named text entry in resource data. */,
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
            clear_keyboardbuffer();
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
                        answer = messagebox(2, 1, getresl(teditData, "mss") /* PLATFORM(file): locate a named text entry in resource data. */,
                            -1, -1, dialog_frame_color, 0, td14tb[0x384]) /* PLATFORM(video): present the interactive dialog renderer. */;
                        if (answer != -1 && answer != 5) {
                            td14tb[0x384] = answer;
                            ++mapChanged;
                            mapDirty = 1;
                        }
                    } else if (selRow[1] == 8 && selectCol[1] != 0) {
                        answer = messagebox(2, 1, getresl(teditData, "men") /* PLATFORM(file): locate a named text entry in resource data. */,
                            -1, -1, dialog_frame_color, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
                        if (answer != -1 && answer != 5) {
                            for (j = 0; j < 0x384; ++j)
                                td14tb[j] = 0;
                            aTer0[3] = answer + '0';
                            terrainTemplate = getres(teditData, aTer0) /* PLATFORM(file): locate a named shape in resource data. */;
                            for (j = 0; j < 0x385; ++j)
                                td15p_9[j] = terrainTemplate[j];
                            globalgamesettings.game_trackname[0] = 0;
                            ++mapChanged;
                            mapDirty = 1;
                        }
                    } else if (selRow[1] == 8 && selectCol[1] == 0) {
                        usescreen() /* PLATFORM(video): copy the current sprite into the active buffer. */;
                        if (mapDirty && (j = messagebox(2, 1, getresl(teditData, "chl") /* PLATFORM(file): locate a named text entry in resource data. */,
                                -1, -1, performGraphColor, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */) == 0)
                            goto save;
                        j = 1;
                        g_is_busy = 1;
                        ++mapChanged;
                        j = floadreq(track_file, globalgamesettings.game_trackname, ".trk",
                            getresl(main_data_file_addr, "trk") /* PLATFORM(file): locate a named text entry in resource data. */) /* PLATFORM(file): open the file-selection UI and return its selection. */;
                        makename(track_file, globalgamesettings.game_trackname, ".trk", buf_g_path) /* PLATFORM(file): build the path used by the file service. */;
                        if (j > 0) {
                            loadfileat(buf_g_path, td14tb) /* PLATFORM(file): read a file or enter the fatal-error path. */;
                            compiletrack();
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
                            usescreen() /* PLATFORM(video): copy the current sprite into the active buffer. */;
                            ++mapChanged;
                            if (fsavereq(track_file, globalgamesettings.game_trackname,
                                    getresl(main_data_file_addr, "trk") /* PLATFORM(file): locate a named text entry in resource data. */) /* PLATFORM(file): open the save-file UI and return its selection. */) {
                                makename(track_file, globalgamesettings.game_trackname, ".trk",
                                    buf_g_path) /* PLATFORM(file): build the path used by the file service. */;
                                saveOutcome = 1;
                                if (findfirst(buf_g_path) /* PLATFORM(file): search for a file by name. */) {
                                    j = messagebox(2, 1, getresl(main_data_file_addr, "fex") /* PLATFORM(file): locate a named text entry in resource data. */,
                                        -1, -1, performGraphColor, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
                                    if (j == -1)
                                        saveOutcome = -1;
                                    else if (j == 0)
                                        saveOutcome = 0;
                                }
                            } else
                                saveOutcome = -1;
                            if (saveOutcome == 1) {
                                j = savefilez(buf_g_path, td14tb, 0x70aL) /* PLATFORM(file): write a file or enter the fatal-error path. */;
                                if (j == 0)
                                    readhighs(1);
                                if (j != 0) {
                                    messagebox(1, 1, getresl(main_data_file_addr, "ser") /* PLATFORM(file): locate a named text entry in resource data. */,
                                        -1, -1, performGraphColor, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
                                    saveOutcome = 0;
                                } else
                                    mapDirty = 0;
                            }
                        }
                        g_is_busy = 0;
                    } else {
                        if (mapDirty && (j = messagebox(2, 1, getresl(teditData, "chx") /* PLATFORM(file): locate a named text entry in resource data. */,
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
            clear_keyboardbuffer();
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

    removewindow(g_wndspr) /* PLATFORM(video): release a temporary sprite window. */;
    removewindow(windows[3]) /* PLATFORM(video): release a temporary sprite window. */;
    removewindow(windows[2]) /* PLATFORM(video): release a temporary sprite window. */;
    removewindow(windows[1]) /* PLATFORM(video): release a temporary sprite window. */;
    removewindow(windows[0]) /* PLATFORM(video): release a temporary sprite window. */;
    closeresource(teditData) /* PLATFORM(memory): release a loaded resource. */;
    freememory(sdtBuffer) /* PLATFORM(memory): release the resource through game memory management. */;
}

/* Local view: drawpiecebox; parameters is ['u8']; another unit uses None. */
void drawpiecebox(U8  mode)
{ /* PURPOSE: Draw the six-by-six terrain and track-piece icon grid. Params: mode. Returns: void. Globals: reads palette_piece_layout, piece_fill_shapes, piece_mask_shapes, terrain_tile_shapes, trklst; writes none. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    U8  iconIndex;
    U8  row;
    U8  stateId;
    for (iconIndex = 0; iconIndex < TRACK_PALETTE_GRID_SIDE; ++iconIndex) {
        for (row = 0; row < TRACK_PALETTE_GRID_SIDE; ++row) {
            stateId = palette_piece_layout[mode * TRACK_PALETTE_PAGE_CELL_COUNT + iconIndex * TRACK_PALETTE_GRID_SIDE + row];
            if (mode == 0) {
                movfxya(terrain_tile_shapes[stateId], TRACK_EDITOR_GRID_X_ORIGIN + (row << TRACK_EDITOR_CELL_SHIFT),
                    TRACK_EDITOR_GRID_Y_ORIGIN + (iconIndex << TRACK_EDITOR_CELL_SHIFT)) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
            } else if (stateId < TRACK_ELEMENT_SPECIAL_BASE) {
                movfxya(terrain_tile_shapes[0], TRACK_EDITOR_GRID_X_ORIGIN + (row << TRACK_EDITOR_CELL_SHIFT),
                    TRACK_EDITOR_GRID_Y_ORIGIN + (iconIndex << TRACK_EDITOR_CELL_SHIFT)) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                switch (trklst[stateId].ss_multiTileFlag) {
                case 1:
                    movfxya(terrain_tile_shapes[0], TRACK_EDITOR_GRID_X_ORIGIN + (row << TRACK_EDITOR_CELL_SHIFT),
                        TRACK_EDITOR_GRID_Y_ORIGIN + TRACK_EDITOR_CELL_PIXELS + (iconIndex << TRACK_EDITOR_CELL_SHIFT)) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                    break;
                case 2:
                    movfxya(terrain_tile_shapes[0], TRACK_EDITOR_GRID_X_ORIGIN + TRACK_EDITOR_CELL_PIXELS + (row << TRACK_EDITOR_CELL_SHIFT),
                        TRACK_EDITOR_GRID_Y_ORIGIN + (iconIndex << TRACK_EDITOR_CELL_SHIFT)) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                    break;
                case 3:
                    movfxya(terrain_tile_shapes[0], TRACK_EDITOR_GRID_X_ORIGIN + TRACK_EDITOR_CELL_PIXELS + (row << TRACK_EDITOR_CELL_SHIFT),
                        TRACK_EDITOR_GRID_Y_ORIGIN + (iconIndex << TRACK_EDITOR_CELL_SHIFT)) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                    movfxya(terrain_tile_shapes[0], TRACK_EDITOR_GRID_X_ORIGIN + (row << TRACK_EDITOR_CELL_SHIFT),
                        TRACK_EDITOR_GRID_Y_ORIGIN + TRACK_EDITOR_CELL_PIXELS + (iconIndex << TRACK_EDITOR_CELL_SHIFT)) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                    movfxya(terrain_tile_shapes[0], TRACK_EDITOR_GRID_X_ORIGIN + TRACK_EDITOR_CELL_PIXELS + (row << TRACK_EDITOR_CELL_SHIFT),
                        TRACK_EDITOR_GRID_Y_ORIGIN + TRACK_EDITOR_CELL_PIXELS + (iconIndex << TRACK_EDITOR_CELL_SHIFT)) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                    break;
                }
                andfxya(piece_mask_shapes[stateId], TRACK_EDITOR_GRID_X_ORIGIN + (row << TRACK_EDITOR_CELL_SHIFT),
                    TRACK_EDITOR_GRID_Y_ORIGIN + (iconIndex << TRACK_EDITOR_CELL_SHIFT)) /* PLATFORM(video): apply a track-piece icon mask. */;
                orfxya(piece_fill_shapes[stateId],
                    TRACK_EDITOR_GRID_X_ORIGIN + (row << TRACK_EDITOR_CELL_SHIFT), TRACK_EDITOR_GRID_Y_ORIGIN + (iconIndex << TRACK_EDITOR_CELL_SHIFT)) /* PLATFORM(video): apply a track-piece icon fill. */;
            }
        }
    }
}

/* Local view: drawtrackbox; parameters is ['u8', 'u8', {'pointer': 'u8', 'qualifiers': [], 'segment': 'near'}, {'pointer': 'u8', 'qualifiers': [], 'segment': 'near'}]; another unit uses None. */
void drawtrackbox(U8  rowBase, U8  columnBase, U8  *lastElement, U8  *lastTerrain)
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
                    movcfxya(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow] + columnIndex + rowBase]],
                        (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                    movcfxya(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow + 1] + columnIndex + rowBase]],
                        (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_BELOW_Y) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                    andcfxya(piece_mask_shapes[td14tb[lnoffsets[columnBase + mapRow] + columnIndex + rowBase - 1]],
                        (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_LEFT_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply a sprite mask at the requested position. */;
                    orcfxya(piece_fill_shapes[td14tb[lnoffsets[columnBase + mapRow] + columnIndex + rowBase - 1]],
                        (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_LEFT_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply sprite pixels at the requested position. */;
                } else if (tileId == TRACK_TILE_CONTINUE_DOWN && mapRow == 0) {
                    movcfxya(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow] + columnIndex + rowBase]],
                        (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                    movcfxya(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow] + columnIndex + rowBase + 1]],
                        (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_RIGHT_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                    andcfxya(piece_mask_shapes[td14tb[lnoffsets[columnBase + mapRow - 1] + columnIndex + rowBase]],
                        (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ABOVE_Y) /* PLATFORM(video): apply a sprite mask at the requested position. */;
                    orcfxya(piece_fill_shapes[td14tb[lnoffsets[columnBase + mapRow - 1] + columnIndex + rowBase]],
                        (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ABOVE_Y) /* PLATFORM(video): apply sprite pixels at the requested position. */;
                } else if (tileId == TRACK_TILE_CONTINUE_DOWN_RIGHT && mapRow == 0 && columnIndex == 0) {
                    movcfxya(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow] + columnIndex + rowBase]],
                        (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                    andcfxya(piece_mask_shapes[td14tb[lnoffsets[columnBase + mapRow - 1] + columnIndex + rowBase - 1]],
                        (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_LEFT_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ABOVE_Y) /* PLATFORM(video): apply a sprite mask at the requested position. */;
                    orcfxya(piece_fill_shapes[td14tb[lnoffsets[columnBase + mapRow - 1] + columnIndex + rowBase - 1]],
                        (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_LEFT_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ABOVE_Y) /* PLATFORM(video): apply sprite pixels at the requested position. */;
                }
            } else if (tileId == 0) {
                if (lastElement[mapIndex] != 0 || lastTerrain[mapIndex] != surface) {
                    movfxya(terrain_tile_shapes[surface], (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                    lastElement[mapIndex] = 0;
                    lastTerrain[mapIndex] = surface;
                }
            } else if (tileId < TRACK_ELEMENT_SPECIAL_BASE) {
                if (lastElement[mapIndex] != tileId || lastTerrain[mapIndex] != surface) {
                    lastElement[mapIndex] = tileId;
                    lastTerrain[mapIndex] = surface;
                    movfxya(terrain_tile_shapes[surface], (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
                    switch (trklst[tileId].ss_multiTileFlag) {
                    case 0:
                        andfxya(piece_mask_shapes[tileId], (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply a track-piece icon mask. */;
                        orfxya(piece_fill_shapes[tileId], (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply a track-piece icon fill. */;
                        break;
                    case 1:
                        movcfxya(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow + 1] + columnIndex + rowBase]],
                            (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_BELOW_Y) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                        andcfxya(piece_mask_shapes[tileId], (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply a sprite mask at the requested position. */;
                        orcfxya(piece_fill_shapes[tileId], (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply sprite pixels at the requested position. */;
                        break;
                    case 2:
                        movcfxya(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow] + columnIndex + rowBase + 1]],
                            (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_RIGHT_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                        andcfxya(piece_mask_shapes[tileId], (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply a sprite mask at the requested position. */;
                        orcfxya(piece_fill_shapes[tileId], (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply sprite pixels at the requested position. */;
                        break;
                    case 3:
                        movcfxya(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow] + columnIndex + rowBase + 1]],
                            (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_RIGHT_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                        movcfxya(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow + 1] + columnIndex + rowBase]],
                            (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_BELOW_Y) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                        movcfxya(terrain_tile_shapes[td15p_9[gterrtrk[columnBase + mapRow + 1] + columnIndex + rowBase + 1]],
                            (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_RIGHT_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_BELOW_Y) /* PLATFORM(video): apply the alternate sprite mask at the requested position. */;
                        andcfxya(piece_mask_shapes[tileId], (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply a sprite mask at the requested position. */;
                        orcfxya(piece_fill_shapes[tileId], (columnIndex << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_X, (mapRow << TRACK_EDITOR_CELL_SHIFT) + TRACK_EDITOR_CELL_ORIGIN_Y) /* PLATFORM(video): apply sprite pixels at the requested position. */;
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

I8 checktracksyntax(void)
{ /* PURPOSE: Check track/terrain pairs, clear invalid entries, and return the error code. Params: none. Returns: char. Globals: reads gterrtrk, lnoffsets, td14tb, td15p_9; writes none. */
    U8  elem, colidx;
    U8  terrain, rowno;
    I8 error;

    trackoverlapcheck();
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
                    if (!roadupcheck(terrain, elem)) {
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
    if (error != 0) trackoverlapcheck();
    return error;
}

void trackoverlapcheck(void)
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
