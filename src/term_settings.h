#ifndef TERM_SETTINGS_H
#define TERM_SETTINGS_H

void set_keypress(void);
void reset_keypress(void);
void getSizeTerm(int *, int *);
void set_invisible_kursor(void);
void gotoxy(int, int);
void clrscr(void);
void clearln(int);
char mygetch(unsigned long);
#endif
