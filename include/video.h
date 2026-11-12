/* Indexed 320 by 200 display geometry and primitive drawing. */
#ifndef STUNTS_VIDEO_H_INCLUDED
#define STUNTS_VIDEO_H_INCLUDED
#include "stunts_types.h"
#include "platform_hw.h"

extern void FAR xorrectc(I16, I16, I16, I16, I16);

#define CRACK_SOURCE_CANVAS_HEIGHT 200 /* I16, 200 */
#define HALF_SCREEN_HEIGHT_PIXELS 0x64 /* I16, 100 */
#define MCGA_LAST_X 0x13f /* I16, 319 */
#define MCGA_SCREEN_WIDTH_PIXELS 320 /* I16, 320 */
#define PREVIEW_VIEWPORT_HEIGHT 0x64 /* I16, 100 */
#define RECTANGLE_EDGE_THICKNESS_PIXELS 1 /* I16, 1 */
#define RECTLIST_SOURCE_FIRST 1 /* I16, 1 */
#define RECTLIST_SOURCE_SECOND 2 /* I16, 2 */
#define RECT_EMPTY_MAX_SENTINEL -1 /* I16, -1 */
#define RECT_EMPTY_MIN_SENTINEL 9999 /* I16, 9999 */
#define SKYBOX_HORIZONTAL_HALF_WRAP 0x200 /* I16, 512 */
#define SKYBOX_HORIZONTAL_WRAP_SIZE 0x400 /* I16, 1024 */
#define SKYBOX_PANEL3_X 0x200 /* I16, 512 */
#define SKYBOX_PANEL4_X 0x340 /* I16, 832 */
#define VIDEO_MATERIAL_TABLE_COUNT 129 /* I16, 129 */

#endif /* STUNTS_VIDEO_H_INCLUDED */
