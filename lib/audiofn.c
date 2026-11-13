#include "audio.h"
#include "chartab.h"
#include "ui.h"
#include "video.h"
#include "dlgcolor.h"
#include "math3d.h"
#include "viewstat.h"
#include "files.h"
#include "types.h"
/* READABILITY: Construct an audio resource path and define the initialized palette, material, and UI color tables owned by this module. */
char audiofiletmp[UI_AUDIOFILETMP_CAPACITY];
#include "memory.h"

/* Build a path from its directory, optional prefix, filename, and extension.
 * Params and return follow the declared C signature. */
I8 *audio_make_filename(I8 *filename, I8 *extension, I8 *prefix)
{
    I8 *slash;
    strcpy(audiofiletmp, filename);
    slash = strrchr(audiofiletmp, DOS_PATH_SEPARATOR);
    if (slash != 0)
        *++slash = 0;
    else
        audiofiletmp[0] = 0;
    strcat(audiofiletmp, prefix);
    slash = strrchr(filename, DOS_PATH_SEPARATOR);
    if (slash != 0)
        strcat(audiofiletmp, ++slash);
    else
        strcat(audiofiletmp, filename);
    if (audiofiletmp[strlen(audiofiletmp) - FILE_EXTENSION_SUFFIX_BYTES] != FILE_EXTENSION_DOT || strlen(audiofiletmp) <= FILE_EXTENSION_SUFFIX_BYTES) {
        strcat(audiofiletmp, ".");
        strcat(audiofiletmp, extension);
    }
    return audiofiletmp;
}

U16S one_through_fourteen_table[UI_ONE_THROUGH_FOURTEEN_TABLE_CAPACITY] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14
};
I16 dlg_colour = 15;
I16 intro_color_max = 16;
I16 menu_hover_color_a = 5;
I16 menu_hover_color_b = 14;
I16 animation_outline_color = 8;
U16S  intro_text_color_a = 15;
U16S  intro_text_style_a = 8;
U16S  intro_text_color_b = 11;
U16S  intro_text_style_b = 3;
U16S  intro_text_color_c = 12;
U16S  intro_text_style_c = 4;
U16S  intro_text_color_d = 9;
U16S  intro_text_style_d = 1;
U16S  intro_text_color_e = 10;
U16S  intro_text_style_e = 2;
U16S  intro_text_color_f = 13;
U16S  intro_text_style_f = 5;
I16 palette_window_line_color = 11;
I16 palette_window_fill_color = 9;
I16 palette_window_line_style = 1;
I16 text_cursor_outline_color = 12;
I16 menu_button_color_a = 15;
I16 menu_button_color_b = 8;
I16 menu_button_color_c = 7;
I16 menu_clear_color = 9;
I16 camera_select_fill_color = 1;
I16 camera_select_outline_color = 4;
I16 performGraphColor = 1;
U16S  dialog_frame_color = 4;
U16S SDPAL_mcgacol1[VIDEO_MATERIAL_TABLE_COUNT] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
    12, 13, 14, 15, 108, 116, 15, 28, 29, 14, 28, 31,
    14, 200, 198, 196, 112, 114, 116, 194, 197, 200, 146, 37,
    35, 181, 29, 31, 19, 3, 11, 27, 0, 4, 4, 12,
    156, 154, 152, 150, 42, 40, 38, 37, 27, 26, 25, 24,
    72, 70, 68, 66, 123, 121, 120, 117, 92, 90, 88, 87,
    173, 171, 169, 167, 20, 19, 18, 17, 77, 76, 74, 73,
    45, 44, 42, 41, 159, 175, 174, 172, 29, 28, 18, 90,
    15, 7, 200, 219, 136, 99, 101, 103, 104, 106, 17, 20,
    60, 77, 46, 61, 45, 202, 190, 186, 183, 180, 0, 28,
    30, 16, 20, 68, 54, 39, 43, 12, 17
};
U16S SDPAL_mcgatype[VIDEO_MATERIAL_TABLE_COUNT] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 0, 0
};
U16S SDPAL_mcgadither[VIDEO_MATERIAL_TABLE_COUNT] = {
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0x77DD, 0xDD77,
    0x77DD, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0x77DD, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x77DD, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xCC33,
    0x33CC, 0xB66D, 0x4992, 0xB66D, 0x4992, 0xB66D, 0x4992, 0x0000,
    0x0000
};
/* Local view: material_clrlist_ptr; renderer retains its excluded MATERIALCLRLIST pointer. */
extern I16* material_clrlist_ptr;
I16* material_clrlist_ptr = SDPAL_mcgacol1;
U16S *material_color_table_pointer = SDPAL_mcgacol1;
U16S *material_pattern_table_pointer = SDPAL_mcgatype;
U16S *material_pattern2_table_ptr = SDPAL_mcgadither;
