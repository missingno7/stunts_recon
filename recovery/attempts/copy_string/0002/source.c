void copy_string(char *destination, char far *source)
{
    char far *current = source;

    do {
        *destination++ = *current++;
    } while (*current != '\0');

    *destination = '\0';
}
