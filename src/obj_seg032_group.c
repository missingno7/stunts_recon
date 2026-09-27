struct SCREEN_RECT {
    short width;
    short height;
    short reserved[7];
    short bottom;
};

extern char *off_42A1E;
extern short word_42A16, word_42A18, word_42A1A;
extern short word_42A1C, word_42A20, word_42A22;
extern struct SCREEN_RECT far *word_405FE;

extern int strlen(char *text);
extern int far font_op(char *text, int count);
extern int far font_op2(char *text);
extern void far sub_35B76(int x, int y, int width, int height, int right);
extern void far sub_345BC(char *text, int x, int y);
extern void far sprite_1_unk2(int x, int y, int width, int height, int right);
extern void far sprite_copy_2_to_1(void);
extern void far read_line_helper(void);
extern void far read_line_helper2(void);
extern int far sub_2EB07(void);
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
  word_42A18 = x;
  word_42A1A = y;
  off_42A1E = buffer;
  word_42A20 = maxWidth;
  buffer[bufferLimit] = 0;
  if (flags & 1)
    buffer[0] = 0;
  if (flags & 2)
    word_42A22 = 0;
  else
    word_42A22 = strlen(buffer);
  cursorIndex = strlen(buffer);
  while (cursorIndex < bufferLimit)
    buffer[cursorIndex++] = ' ';

  read_line_helper2();
  word_42A16 = 1;
  word_42A1C = 1;
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
        if (sub_2EB07() != 0) { inputKey = 0; break; }
        readCallback();
      }
    }
    if (inputKey == 0)
    {
      set_add_value(4L);
      savedBlink = word_42A1C;
      word_42A1C = 1;
      read_line_helper();
      if (savedBlink != 0)
      {
        word_42A1C = 0;
      }
      else
      {
        word_42A1C = 1;
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
      if (word_42A22 < bufferLimit)
        ++word_42A22;
    }
    else
      if (inputKey == 0x4B00)
    {
      read_line_helper();
      if (word_42A22 != 0)
        --word_42A22;
    }
    else
      if (inputKey == 0x4700)
    {
      read_line_helper();
      word_42A22 = 0;
    }
    else
      if (inputKey == 0x4F00)
    {
      read_line_helper();
      word_42A22 = strlen(buffer);
    }
    else
      if (inputKey == 0x5200)
    {
      read_line_helper();
      if (insertMode == 0)
      {
        insertMode = 1;
        word_42A16 = 8;
      }
      else
      {
        insertMode = 0;
        word_42A16 = 1;
      }
    }
    else
      if (inputKey == 0x5300)
    {
      if (!(word_42A22 >= bufferLimit || 0 == buffer[word_42A22]))
      {
        read_line_helper();
        cursorIndex = word_42A22;
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
      if (word_42A22 == 0)
      {
        firstKey = 0;
        continue;
      }
      read_line_helper();
      --word_42A22;
      cursorIndex = word_42A22;
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
      if (word_42A22 >= bufferLimit)
      {
        firstKey = 0;
        continue;
      }
      read_line_helper();
      if (firstKey != 0 && (!(flags & 4)))
      {
        word_42A22 = 0;
        cursorIndex = 0;
        while (cursorIndex < bufferLimit)
        {
          buffer[cursorIndex] = ' ';
          ++cursorIndex;
        }

      }
      if (buffer[word_42A22] == 0)
        buffer[word_42A22 + 1] = 0;
      if (insertMode != 0)
      {
        cursorIndex = bufferLimit - 2;
        while (cursorIndex >= word_42A22)
        {
          buffer[cursorIndex + 1] = buffer[cursorIndex];
          --cursorIndex;
        }

      }
      buffer[word_42A22] = (char) inputKey;
      if (word_42A22 < bufferLimit)
        ++word_42A22;
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

    if (word_42A1C == 0)
        return;

    if (strlen(off_42A1E) < word_42A22)
        word_42A22 = strlen(off_42A1E);

    width = font_op(off_42A1E + word_42A22, 1);
    if (width == 0)
        width = font_op2(" ");

    x = font_op(off_42A1E, word_42A22) + word_42A18;
    y = word_405FE->bottom + word_42A1A - word_42A16;
    sub_35B76(x, y, width, word_42A16, word_405FE->width);
}

void far read_line_helper2(void)
{
    short length;
    short unusedWidth;
    short fontWidth;

    if (word_42A20 != 0) {
        for (;;) {
            if (font_op2(off_42A1E) <= word_42A20)
                break;
            if (strlen(off_42A1E) == 0)
                break;
            off_42A1E[strlen(off_42A1E) - 1] = 0;
        }
    }

    length = strlen(off_42A1E);
    if (word_42A22 > length)
        word_42A22 = length;

    sub_345BC(off_42A1E, word_42A18, word_42A1A);
    if (word_42A20 != 0) {
        fontWidth = font_op2(off_42A1E);
        unusedWidth = word_42A20 - fontWidth;
        if (unusedWidth > 0)
            sprite_1_unk2(fontWidth + word_42A18, word_42A1A, unusedWidth,
                          word_405FE->bottom, word_405FE->height);
    }
}
