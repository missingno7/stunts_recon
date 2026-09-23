int toupper(int ch)
{
    if (ch >= 'a' && ch <= 'z') {
        ch -= ' ';
    }
    return ch;
}