int far flagchar(char ch) { char *p; for (p = (char *)0x37f8; *p; ++p) if (*p == ch) return 1; return 0; }
