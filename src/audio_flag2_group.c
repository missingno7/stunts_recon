extern unsigned char audioflag2, byte_8b20;
extern unsigned int word_4eca;
extern void far audio_driver_func1E(unsigned int, unsigned int);
extern void far sub_39700(void);
void far audio_enable_flag2(void) { audioflag2 = 1; }
void far audio_disable_flag2(void) { audioflag2 = 0; word_4eca = 1; if (byte_8b20) { audio_driver_func1E(0, (unsigned int)byte_8b20 - 1); } sub_39700(); word_4eca = 0; }
int far audio_toggle_flag2(void) { if (audioflag2 == 1) { audio_disable_flag2(); return 0; } audio_enable_flag2(); return 1; }

