extern char audio_filetemp[];
extern char unk_407AC[];
extern char *strrchr(const char *string, int ch);
extern char *strcpy(char *destination, const char *source);
extern char *strcat(char *destination, const char *source);
extern unsigned int strlen(const char *string);

char *audio_make_filename(char *filename, char *extension, char *prefix)
{
    char *slash;
    strcpy(audio_filetemp, filename);
    slash = strrchr(audio_filetemp, 0x5c);
    if (slash != 0)
        *++slash = 0;
    else
        audio_filetemp[0] = 0;
    strcat(audio_filetemp, prefix);
    slash = strrchr(filename, 0x5c);
    if (slash != 0)
        strcat(audio_filetemp, ++slash);
    else
        strcat(audio_filetemp, filename);
    if (audio_filetemp[strlen(audio_filetemp) - 4] != 0x2e || strlen(audio_filetemp) <= 4) {
        strcat(audio_filetemp, unk_407AC);
        strcat(audio_filetemp, extension);
    }
    return audio_filetemp;
}

