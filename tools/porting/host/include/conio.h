#ifndef H1_HOST_CONIO_H
#define H1_HOST_CONIO_H
int inp(unsigned short port);
unsigned int inpw(unsigned short port);
int outp(unsigned short port, int value);
unsigned int outpw(unsigned short port, unsigned int value);
int getch(void);
int getche(void);
int kbhit(void);
#endif
