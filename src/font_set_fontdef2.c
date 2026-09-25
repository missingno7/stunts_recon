struct FONTDEF_PREFIX { unsigned char bytes[14]; unsigned short value; };
extern unsigned int fontdef_value;
extern void far set_fontdefseg(void far *data);
void far font_set_fontdef2(void far *data)
{
    set_fontdefseg(data);
    fontdef_value = ((struct FONTDEF_PREFIX far *)data)->value;
}
