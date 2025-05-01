#include <termios.h>
#include <stdio.h>
#include <sys/types.h>
#include "term_settings.h"

static struct termios stored_settings;

void set_keypress(void)
{
	struct termios new_settings;
	tcgetattr(0,&stored_settings);
	new_settings = stored_settings;
	new_settings.c_lflag &= (~ICANON & ~ECHO);
	new_settings.c_cc[VTIME] = 0;
	new_settings.c_cc[VMIN] = 1;
	tcsetattr(0,TCSANOW,&new_settings);
	return;
}
void reset_keypress(void)
{
	tcsetattr(0,TCSANOW,&stored_settings);
	return;
}

void getSizeTerm(int * width, int *height)
{
	// попытка установить курсор в (255, 255)
	// терминал установит курсор в правый нижний угол
	fputs("\x1B[255;255H\x1B[6n", stdout); 

	// чтение координат курсора
	scanf("\x1B[%d;%dR", height, width);
}

void set_invisible_kursor(void)
{
	printf("\033[=00");
}

void gotoxy(int x, int y)
{
	printf("\033[%d;%dH", y, x);
}

void clrscr(void)
{
	fputs("\x1B[2J\x1B[H", stdout);
	fflush(stdout);
}

void clearln(int y)
{
	gotoxy(1, y);
	printf("\033[0K");
	fflush(stdout);
}

char mygetch(unsigned long mcsec) {
    fd_set rfds;
    struct timeval tv;
    int retval;

    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = mcsec;
    retval = select(1, &rfds, NULL, NULL, &tv);
    return retval;
}
