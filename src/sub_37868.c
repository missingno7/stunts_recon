extern unsigned char byte_44290;
extern void far audio_unk2(int index, int value);
void far sub_37868(int value) { int index; index=0; while(index<byte_44290) { audio_unk2(index,value); ++index; } }
