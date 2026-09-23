void copy_string(char *destination, char far *source)
{
    do {
        *destination++ = *source++;
    } while (*source != '\0');

    *destination = '\0';
}
