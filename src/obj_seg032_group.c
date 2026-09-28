struct SCREEN_RECT {
    short width;
    short height;
    short reserved[7];
    short bottom;
};

static char *line_input_text_buffer;
static short line_input_cursor_vertical, line_edit_x, line_edit_y;
static short cursor_flash_state, line_text_max_width, line_input_cursor_slot;
extern struct SCREEN_RECT far *line_input_screen_rect;

extern int strlen(char *text);
extern int far font_op(char *text, int count);
extern int far font_op2(char *text);
extern void far draw_filled_rect(int x, int y, int width, int height, int right);
extern void far draw_text_at(char *text, int x, int y);
extern void far sprite1_unknown2(int x, int y, int width, int height, int right);
extern void far sprite_copy_2_to_1(void);
extern void far read_line_helper(void);
extern void far read_line_helper2(void);
extern int far poll_input_abort(void);
extern int far kb_call_readchar_callback(void);
extern void far timer_copy_counter(int offset, int segment);
extern void far set_add_value(long ticks);
extern int far timer_compare_dx(void);

int far read_line(char flags, char *buffer, int pendingKey, int bufferLimit,
                  int maxWidth, int x, int y,
                  int (far *readCallback)(void), int timerOffset, int timerSegment)
{
  short firstKey;
  short savedBlink;
  short inputKey;
  short cursorIndex;
  short insertMode;
  sprite_copy_2_to_1();
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
  timer_copy_counter(timerOffset, timerSegment);
  set_add_value(4L);
  firstKey = 1;
  for (;;)
  {
    if (pendingKey != 0)
    {
      inputKey = (short) pendingKey;
      pendingKey = 0;
    }
    else
    {
      while ((inputKey = kb_call_readchar_callback()) == 0) {
        if (poll_input_abort() != 0) { inputKey = 0; break; }
        readCallback();
      }
    }
    if (inputKey == 0)
    {
      set_add_value(4L);
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
      if ((timerOffset | timerSegment) != 0 && timer_compare_dx() != 0)
      {
done:
        read_line_helper();
        return inputKey;
      }
      continue;
    }
    timer_copy_counter(timerOffset, timerSegment);
    if (inputKey == 13 || inputKey == 27 || inputKey == 0x4800 || inputKey == 0x5000 && !(flags & 8) || inputKey == 9 && !(flags & 0x10))
      goto done;
    if (inputKey == 0x4D00)
    {
      read_line_helper();
      if (line_input_cursor_slot < bufferLimit)
        ++line_input_cursor_slot;
    }
    else
      if (inputKey == 0x4B00)
    {
      read_line_helper();
      if (line_input_cursor_slot != 0)
        --line_input_cursor_slot;
    }
    else
      if (inputKey == 0x4700)
    {
      read_line_helper();
      line_input_cursor_slot = 0;
    }
    else
      if (inputKey == 0x4F00)
    {
      read_line_helper();
      line_input_cursor_slot = strlen(buffer);
    }
    else
      if (inputKey == 0x5200)
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
      if (inputKey == 0x5300)
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
      if (inputKey == 8)
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
      if (inputKey >= 0x20 && inputKey <= 0x7A)
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
      buffer[line_input_cursor_slot] = (char) inputKey;
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
}


void far read_line_helper(void)
{
    short width;
    short x;
    short y;

    if (cursor_flash_state == 0)
        return;

    if (strlen(line_input_text_buffer) < line_input_cursor_slot)
        line_input_cursor_slot = strlen(line_input_text_buffer);

    width = font_op(line_input_text_buffer + line_input_cursor_slot, 1);
    if (width == 0)
        width = font_op2(" ");

    x = font_op(line_input_text_buffer, line_input_cursor_slot) + line_edit_x;
    y = line_input_screen_rect->bottom + line_edit_y - line_input_cursor_vertical;
    draw_filled_rect(x, y, width, line_input_cursor_vertical, line_input_screen_rect->width);
}

void far read_line_helper2(void)
{
    short length;
    short unusedWidth;
    short fontWidth;

    if (line_text_max_width != 0) {
        for (;;) {
            if (font_op2(line_input_text_buffer) <= line_text_max_width)
                break;
            if (strlen(line_input_text_buffer) == 0)
                break;
            line_input_text_buffer[strlen(line_input_text_buffer) - 1] = 0;
        }
    }

    length = strlen(line_input_text_buffer);
    if (line_input_cursor_slot > length)
        line_input_cursor_slot = length;

    draw_text_at(line_input_text_buffer, line_edit_x, line_edit_y);
    if (line_text_max_width != 0) {
        fontWidth = font_op2(line_input_text_buffer);
        unusedWidth = line_text_max_width - fontWidth;
        if (unusedWidth > 0)
            sprite1_unknown2(fontWidth + line_edit_x, line_edit_y, unusedWidth,
                          line_input_screen_rect->bottom, line_input_screen_rect->height);
    }
}
