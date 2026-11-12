#include "stunts_types.h"
#include "platform_hw.h"
/* READABILITY: Implement the shared line editor, including key polling, cursor drawing, and text-width handling. */
#include "fontrec.h"

#include "font.h"
#include "game.h"
#include "input.h"
#include "sprite2d.h"
#include "timer.h"
#include "video.h"


static I8 *line_input_text_buffer;
static I16S line_input_cursor_vertical, line_edit_x, line_edit_y;
static I16S cursor_flash_state, line_text_max_width, line_input_cursor_slot;


/* Local view: strlen; return is signed word; another unit uses unsigned word. */
extern I16 strlen(I8 *text);
    /* PLATFORM(video): measure or draw the current line-edit display. */


    /* PLATFORM(video): measure or draw the current line-edit display. */

    /* PLATFORM(video): measure or draw the current line-edit display. */

    /* PLATFORM(video): measure or draw the current line-edit display. */




    /* PLATFORM(input_kb): poll the game input-abort state. */

    /* PLATFORM(input_kb): read the next DOS keyboard callback value. */

    /* PLATFORM(timer): reset the game timer countdown for this input wait. */
/* Local view: setticks; parameters.count is 2; another unit uses 1. */
extern void FAR setticks(I16 offset, I16 segment);
    /* PLATFORM(timer): set the cursor-blink timer interval. */

    /* PLATFORM(timer): check whether the timer countdown expired. */


/* Edit a bounded text buffer using keyboard input, blink timing, and the line-rendering helpers.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(input_kb): poll the keyboard and abort callback for edit keys. */
/* PLATFORM(video): copy the render surface before drawing line edits. */
/* PLATFORM(timer): refresh cursor blink timing while waiting for input. */
/* Local view: get_pstring; parameters[0] is signed byte; another unit uses signed word. */
I16 FAR get_pstring(I8 flags, I8 *buffer, I16 pendingKey, I16 bufferLimit,
                  I16 maxWidth, I16 x, I16 y,
                  I16 (FAR *readCallback)(void), I16 timerOffset, I16 timerSegment)
{
  I16S firstKey;
  I16S savedBlink;
  I16S inputKey;
  I16S cursorIndex;
  I16S insertMode;
    /* PLATFORM(video): copy the active render surface before editing. */
  displaywindow();
  line_edit_x = x;
  line_edit_y = y;
  line_input_text_buffer = buffer;
  line_text_max_width = maxWidth;
  buffer[bufferLimit] = 0;
  if (flags & 1)
    buffer[0] = 0;
  if (flags & 2)
    line_input_cursor_slot = 0;
  else
    line_input_cursor_slot = strlen(buffer);
  cursorIndex = strlen(buffer);
  while (cursorIndex < bufferLimit)
    buffer[cursorIndex++] = ' ';

  read_line_helper2();
  line_input_cursor_vertical = 1;
  cursor_flash_state = 1;
  insertMode = 0;
  read_line_helper();
    /* PLATFORM(timer): reset the game timer countdown for this input wait. */
  setticks(timerOffset, timerSegment);
    /* PLATFORM(timer): set the cursor-blink timer interval. */
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
    /* PLATFORM(input_kb): read the next DOS keyboard callback value. */
      while ((inputKey = testkey()) == 0) {
    /* PLATFORM(input_kb): poll the game input-abort state. */
        if (testbiosticks() != 0) { inputKey = 0; break; }
        readCallback();
      }
    }
    if (inputKey == 0)
    {
    /* PLATFORM(timer): set the cursor-blink timer interval. */
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
    /* PLATFORM(timer): check whether the timer countdown expired. */
      if ((timerOffset | timerSegment) != 0 && testticks() != 0)
      {
        break;
      }
      continue;
    }
    /* PLATFORM(timer): reset the game timer countdown for this input wait. */
    setticks(timerOffset, timerSegment);
    if (inputKey == KEY_ASCII_ENTER || inputKey == KEY_ASCII_ESCAPE || inputKey == KEY_SCAN_UP || inputKey == KEY_SCAN_DOWN && !(flags & 8) || inputKey == KEY_ASCII_TAB && !(flags & 0x10))
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
        line_input_cursor_vertical = 8;
      }
      else
      {
        insertMode = 0;
        line_input_cursor_vertical = 1;
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
      if (firstKey != 0 && (!(flags & 4)))
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
/* PLATFORM(video): measure and draw the text cursor using the renderer. */
void FAR read_line_helper(void)
{
    I16S width;
    I16S x;
    I16S y;

    if (cursor_flash_state == 0)
        return;

    if (strlen(line_input_text_buffer) < line_input_cursor_slot)
        line_input_cursor_slot = strlen(line_input_text_buffer);

    /* PLATFORM(video): measure or draw the current line-edit display. */
    width = textnpixels(line_input_text_buffer + line_input_cursor_slot, 1);
    if (width == 0)
    /* PLATFORM(video): measure displayed text using the game font renderer. */
        width = textpixels(" ");

    /* PLATFORM(video): measure or draw the current line-edit display. */
    x = textnpixels(line_input_text_buffer, line_input_cursor_slot) + line_edit_x;
    y = line_input_font_header->line_height + line_edit_y - line_input_cursor_vertical;
    /* PLATFORM(video): measure or draw the current line-edit display. */
    xorrectc(x, y, width, line_input_cursor_vertical, line_input_font_header->foreground);
}

/* Clamp text to the configured width, draw it, and update the remaining line background.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(video): clip and draw the editable text line. */
void FAR read_line_helper2(void)
{
    I16S length;
    I16S unusedWidth;
    I16S fontWidth;

    if (line_text_max_width != 0) {
        for (;;) {
    /* PLATFORM(video): measure displayed text using the game font renderer. */
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

    /* PLATFORM(video): measure or draw the current line-edit display. */
    movtxy(line_input_text_buffer, line_edit_x, line_edit_y);
    if (line_text_max_width != 0) {
    /* PLATFORM(video): measure displayed text using the game font renderer. */
        fontWidth = textpixels(line_input_text_buffer);
        unusedWidth = line_text_max_width - fontWidth;
        if (unusedWidth > 0)
    /* PLATFORM(video): measure or draw the current line-edit display. */
            drawrectc(fontWidth + line_edit_x, line_edit_y, unusedWidth,
                          line_input_font_header->line_height, line_input_font_header->background);
    }
}
