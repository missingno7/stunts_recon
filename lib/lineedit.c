#include "types.h"
/* Implement the shared line editor, including key polling, cursor drawing, and text-width handling. */
#include "ui.h"
#include "chartab.h"
#include "math3d.h"
#include "viewstat.h"
#include "game.h"
#include "input.h"
#include "video.h"
#include "timer.h"

static I8 *line_input_text_buffer;
static I16S line_input_cursor_vertical, line_edit_x, line_edit_y;
static I16S cursor_flash_state, line_text_max_width, line_input_cursor_slot;
#include "memory.h"






/* Edit a bounded text buffer using keyboard input, blink timing, and the line-rendering helpers.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
I16 FAR get_pstring(I8 flags, I8 *buffer, I16 pendingKey, I16 bufferLimit,
                  I16 maxWidth, I16 x, I16 y,
                  I16 (FAR *readCallback)(void), I16 timerOffset, I16 timerSegment)
{
  I16S firstKey;
  I16S savedBlink;
  I16S inputKey;
  I16S cursorIndex;
  I16S insertMode;
  displaywindow();
  line_edit_x = x;
  line_edit_y = y;
  line_input_text_buffer = buffer;
  line_text_max_width = maxWidth;
  buffer[bufferLimit] = 0;
  if (flags & UI_LINE_EDIT_CLEAR_BUFFER)
    buffer[0] = 0;
  if (flags & UI_LINE_EDIT_CURSOR_AT_START)
    line_input_cursor_slot = 0;
  else
    line_input_cursor_slot = strlen(buffer);
  cursorIndex = strlen(buffer);
  while (cursorIndex < bufferLimit)
    buffer[cursorIndex++] = ' ';

  read_line_helper2();
  line_input_cursor_vertical = UI_LINE_CURSOR_BAR_PIXELS;
  cursor_flash_state = 1;
  insertMode = 0;
  read_line_helper();
  setticks(timerOffset, timerSegment);
  setbiosticks(4L);
  firstKey = 1;
  for (;;)
  {
    if (pendingKey != 0)
    {
      inputKey = pendingKey;
      pendingKey = 0;
    }
    else
    {
      while ((inputKey = testkey()) == 0) {
        if (testbiosticks() != 0) { inputKey = 0; break; }
        readCallback();
      }
    }
    if (inputKey == 0)
    {
      setbiosticks(4L);
      savedBlink = cursor_flash_state;
      cursor_flash_state = 1;
      read_line_helper();
      if (savedBlink != 0)
      {
        cursor_flash_state = 0;
      }
      else
      {
        cursor_flash_state = 1;
      }
      if ((timerOffset | timerSegment) != 0 && testticks() != 0)
      {
        break;
      }
      continue;
    }
    setticks(timerOffset, timerSegment);
    if (inputKey == KEY_ASCII_ENTER || inputKey == KEY_ASCII_ESCAPE || inputKey == KEY_SCAN_UP || inputKey == KEY_SCAN_DOWN && !(flags & UI_LINE_EDIT_KEEP_DOWN_KEY) || inputKey == KEY_ASCII_TAB && !(flags & UI_LINE_EDIT_KEEP_TAB_KEY))
      break;
    if (inputKey == KEY_SCAN_RIGHT)
    {
      read_line_helper();
      if (line_input_cursor_slot < bufferLimit)
        ++line_input_cursor_slot;
    }
    else
      if (inputKey == KEY_SCAN_LEFT)
    {
      read_line_helper();
      if (line_input_cursor_slot != 0)
        --line_input_cursor_slot;
    }
    else
      if (inputKey == KEY_SCAN_HOME)
    {
      read_line_helper();
      line_input_cursor_slot = 0;
    }
    else
      if (inputKey == KEY_SCAN_END)
    {
      read_line_helper();
      line_input_cursor_slot = strlen(buffer);
    }
    else
      if (inputKey == KEY_SCAN_INSERT)
    {
      read_line_helper();
      if (insertMode == 0)
      {
        insertMode = 1;
        line_input_cursor_vertical = UI_LINE_CURSOR_BLOCK_PIXELS;
      }
      else
      {
        insertMode = 0;
        line_input_cursor_vertical = UI_LINE_CURSOR_BAR_PIXELS;
      }
    }
    else
      if (inputKey == KEY_SCAN_DELETE)
    {
      if (!(line_input_cursor_slot >= bufferLimit || 0 == buffer[line_input_cursor_slot]))
      {
        read_line_helper();
        cursorIndex = line_input_cursor_slot;
        while (cursorIndex < bufferLimit)
        {
          buffer[cursorIndex] = buffer[cursorIndex + 1];
          ++cursorIndex;
        }

        buffer[bufferLimit - 1] = ' ';
        read_line_helper2();
      }
      else
      {
        firstKey = 0;
        continue;
      }
    }
    else
      if (inputKey == KEY_ASCII_BACKSPACE)
    {
      if (line_input_cursor_slot == 0)
      {
        firstKey = 0;
        continue;
      }
      read_line_helper();
      --line_input_cursor_slot;
      cursorIndex = line_input_cursor_slot;
      while (cursorIndex < bufferLimit)
      {
        buffer[cursorIndex] = buffer[cursorIndex + 1];
        ++cursorIndex;
      }

      buffer[bufferLimit - 1] = ' ';
      read_line_helper2();
    }
    else
      if (inputKey >= KEY_ASCII_SPACE && inputKey <= KEY_ASCII_Z)
    {
      if (line_input_cursor_slot >= bufferLimit)
      {
        firstKey = 0;
        continue;
      }
      read_line_helper();
      if (firstKey != 0 && (!(flags & UI_LINE_EDIT_KEEP_FIRST_TEXT)))
      {
        line_input_cursor_slot = 0;
        cursorIndex = 0;
        while (cursorIndex < bufferLimit)
        {
          buffer[cursorIndex] = ' ';
          ++cursorIndex;
        }

      }
      if (buffer[line_input_cursor_slot] == 0)
        buffer[line_input_cursor_slot + 1] = 0;
      if (insertMode != 0)
      {
        cursorIndex = bufferLimit - 2;
        while (cursorIndex >= line_input_cursor_slot)
        {
          buffer[cursorIndex + 1] = buffer[cursorIndex];
          --cursorIndex;
        }

      }
      buffer[line_input_cursor_slot] = inputKey;
      if (line_input_cursor_slot < bufferLimit)
        ++line_input_cursor_slot;
      read_line_helper2();
    }
    else
    {
      firstKey = 0;
      continue;
    }
    read_line_helper();
    firstKey = 0;
  }
  read_line_helper();
  return inputKey;
}

/* Measure the text prefix and draw the cursor block at the current edit position.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
void FAR read_line_helper(void)
{
    I16S width;
    I16S x;
    I16S y;

    if (cursor_flash_state == 0)
        return;

    if ((I16)strlen(line_input_text_buffer) < line_input_cursor_slot)
        line_input_cursor_slot = (I16)strlen(line_input_text_buffer);

    width = textnpixels(line_input_text_buffer + line_input_cursor_slot, 1);
    if (width == 0)
        width = textpixels(" ");

    x = textnpixels(line_input_text_buffer, line_input_cursor_slot) + line_edit_x;
    y = line_input_font_header->line_height + line_edit_y - line_input_cursor_vertical;
    xorrectc(x, y, width, line_input_cursor_vertical, line_input_font_header->foreground);
}

/* Clamp text to the configured width, draw it, and update the remaining line background.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
void FAR read_line_helper2(void)
{
    I16S length;
    I16S unusedWidth;
    I16S fontWidth;

    if (line_text_max_width != 0) {
        for (;;) {
            if (textpixels(line_input_text_buffer) <= line_text_max_width)
                break;
            if (strlen(line_input_text_buffer) == 0)
                break;
            line_input_text_buffer[strlen(line_input_text_buffer) - 1] = 0;
        }
    }

    length = strlen(line_input_text_buffer);
    if (line_input_cursor_slot > length)
        line_input_cursor_slot = length;

    movtxy(line_input_text_buffer, line_edit_x, line_edit_y);
    if (line_text_max_width != 0) {
        fontWidth = textpixels(line_input_text_buffer);
        unusedWidth = line_text_max_width - fontWidth;
        if (unusedWidth > 0)
            drawrectc(fontWidth + line_edit_x, line_edit_y, unusedWidth,
                          line_input_font_header->line_height, line_input_font_header->background);
    }
}
