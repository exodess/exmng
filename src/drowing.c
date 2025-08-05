#include <stdio.h>
#include "drowing.h"
#include "term_settings.h"
#include "colors.h"

void drowing_pole_file(int len_term, int height_term)
{
	int x = len_term * 0.1;
	int y = height_term * 0.1;
	printf("%s%s", WHITE_FRONT, WHITE);
	for(int i = 0; i < len_term * 1.6 + 3; i++) {
		gotoxy(x, y);
		printf("-");
		if(i % 2) {
			x++;
			y = height_term * 0.1;
		}
		else y = height_term * 0.9;
	}
	x = len_term * 0.1;
	y = height_term * 0.1 + 1;
	for(int i = 0; i < height_term * 1.6 - 1; i++) {
		gotoxy(x, y);
		printf("|");
		if(i % 2) {
			y++;
			x = len_term * 0.1;
		}
		else x = len_term * 0.9;
	}
	printf("%s", ESC);

	for(int i = height_term * 0.1 + 1; i < height_term * 0.9 - 1; i++){
		for(int j = len_term * 0.1 + 1; j < len_term * 0.9 - 1; j++){
			gotoxy(j, i);
			printf("%s %s", GREY_FRONT, ESC);
		}
	}
}

void drowing_pole_dir(WindowInfo_t * point, int current_point, int offset, int flag)
{
	int len_term = 0, height_term = 0; // размеры терминала
	int xl; // граничные значения отображения блока по оси x
	int yt; // граничные значения отображения блока по оси y
	int count = 0;
	DList_t * list = NULL;
	
	getSizeTerm(&len_term, &height_term);

	clear_pole(len_term, height_term, flag);
	if(point == NULL || point -> top == NULL) return ;	
	
	if(flag == 0) xl = 1;
	else if(flag == 1) xl = len_term / 3 + 1;
	else xl = 2 * (len_term / 3) + 2;
	
	list = point -> top;
	count = (count_nodes(list) > height_term) ? height_term : count_nodes(list);

	yt = (height_term / 2) - (count / 2);
	yt = (yt < 1) ? 1: yt;
	
	for(int i = 0; i < count; i++){
		gotoxy(xl, yt + i);
		if((current_point == i + 1) && (flag == 1)) {
			int type = (peek_entry(list, current_point) -> d_type);
			if(type == 4) printf("%s", ORANGE_FRONT);
			else printf("%s", BLUE_FRONT);
		}
		drowing_pole_names(peek_entry(list, i + 1 + offset), len_term / 3 - 1, flag);
		printf("%s", ESC);
	}
}

void drowing_pole_names(struct dirent * entry, int len_pole, int here)
{
	int flag = entry -> d_type;
	char * theme;
	if(here == 1){
		if(flag == 4) theme = ORANGE;
		else  theme = BLUE;
	}
	else theme = GREY;
	printf(" %s", theme);

	int j = 0;
	theme = entry -> d_name;
	while((j < (len_pole - 8)) && (theme[j] != '\0'))
		 printf("%c", theme[j++]);

	while(j++ < len_pole - 8) printf(" ");
	
	printf(" [%02d] ", entry -> d_type);
	

}

void clear_pole(int len_term, int height_term, int flag)
{	
	int xl = 0; // левая граница поля, которое нужно очистить
	int len = len_term / 3 - 1;
	if(flag == 0) xl = 1;
	else if(flag == 1) xl = len_term / 3;
	else xl = 2 * len_term / 3;

	for(int y = 0; y < height_term + 1; y++){
		gotoxy(xl, y);
		for(int j = 0; j < len + 1; j++) printf(" ");
	}
}
