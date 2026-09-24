int far flagchar(char ch) { char *p = (char *)0x37f8; while (*p && *p != ch) ++p; return *p != 0; }
