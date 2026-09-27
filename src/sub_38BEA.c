struct AudioChunk { unsigned char b[0x26]; unsigned short value26; unsigned char r[0x1f]; unsigned char driverChannel; unsigned char tail[4]; };
extern struct AudioChunk word_3396C[];
extern char far *word_3060A;
void far _loadds sub_38BEA(int voiceIndex, int value) {
 struct AudioChunk *voice;
 voice = &word_3396C[voiceIndex];
 if (value & 0x100) value |= 0x80;
 value = ((value & 0xff00) >> 1) + ((signed char)value - 0x2000);
 voice->value26 = value;
 ((void (far *)(struct AudioChunk *, int, int))(word_3060A + 0x1b))(voice,value,voice->driverChannel);
}
