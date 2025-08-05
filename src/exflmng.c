#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "term_settings.h"
#include "list.h"
#include "drowing.h"
#include "colors.h"

#define LEFT  0
#define CENTER  1
#define RIGHT 2
#define SLEEP 100
#define SIZE_PATH 1024
#define TYPE_DIR 4
#define TYPE_FILE 8
#define PG_UP 'w'
#define PG_DN 's'
#define PG_LF 'a'
#define PG_RT 'd'
#define BACKSPACE 127 // символ backspace 
#define ESCAPE 27 // символ ESC
#define DEL 177
#define ENTER '\n'

void downDirList(int *, int *, int, int);
WindowInfo_t * reading_dir(char *, int);
void reading_and_drowing_file(char *);
char * getPathDone(char *);
char * getPathUp(char *, char *);
int isRootDir(char *);
long prev_line_in_file(FILE *);
long next_line_in_file(FILE *);
int print_file(FILE *, int, int, int);
char *rename_element(char *, const char *, int, int);
char * add_symbol_to_str(char *, char);
char * del_symbol_from_str(char *);
int warning_remove_file(int, int);

char exgetch(void);


int main(void)
{
	set_keypress();
	clrscr();
	set_invisible_kursor();
	int len_term, height_term; 
	int count = 0; // количество файлов в директории
	char ch = ' ';
	char * name_cur; // название файла, на который указывает курсор
	char * path_left = malloc(SIZE_PATH * sizeof(char)); // папке ниже текущей
	char * path_center = malloc(SIZE_PATH * sizeof(char)); // актуальный путь
	char * path_right = malloc(SIZE_PATH * sizeof(char)); // папка выше текущей
	
	strcpy(path_center, getenv("PWD"));
	if(strcmp(path_center, "/") != 0) strcpy(path_left, getPathDone(getenv("PWD")));
	WindowInfo_t * center, * right, * left;

	int cur_point = 1; // куда указывает курсор
	int offset = 0;
	
	center = reading_dir(path_center, CENTER);
	left = reading_dir(path_left, LEFT);
	right = reading_dir(path_right, RIGHT);
	count = count_nodes(center -> top);
	
	while(ch != ESCAPE){
	
		getSizeTerm(&len_term, &height_term);
		// len_term = (len_term / 3) * 3;
		name_cur = (peek_entry(center -> top, cur_point + offset) -> d_name);
		count = count_nodes(center -> top);
		
		if(ch == PG_DN) downDirList(&cur_point, &offset, height_term, count);

		else if(ch == PG_UP) {
			if(cur_point > 1) cur_point--;
			else if((cur_point + offset) > 1) offset --;
			else if((cur_point + offset) == 1) {
				cur_point = (height_term > count) ? count : height_term;
				offset = (cur_point == count) ? 0 : count - cur_point;
			}
		}
		else if((ch == PG_LF) && (!isRootDir(path_left))) { // переход на папку ниже
		
			// определение новых путей
			strcpy(path_right, path_center);
			strcpy(path_center, path_left);
			path_left = getPathDone(path_left);
			
			// освобождение памяти и изменение переменных контейнеров
			destroy_list(right -> top);
			free(right);
			right = init_list(RIGHT);
			right -> top = center -> top;
			center -> top = left -> top;
			free(left);
			left = reading_dir(path_left, LEFT);
			count = count_nodes(center -> top);
			cur_point = 1;
			offset = 0;
		}

		else if(ch == ENTER) {
			if(peek_entry(center -> top, cur_point + offset) -> d_type == TYPE_DIR) {
				// открытие другой папки
				strcpy(path_right, path_center);
				path_right = getPathUp(path_right, name_cur);
				right = reading_dir(path_right, RIGHT);				
			}
			else if(peek_entry(center -> top, cur_point + offset) -> d_type == TYPE_FILE) {
				// открытие файла
				strcpy(path_right, path_center);
				path_right = getPathUp(path_right, name_cur);
				reading_and_drowing_file(path_right);
				path_right = getPathDone(path_right);
			}
		}
		
		else if((ch == PG_RT) && right -> top != NULL){ 
			/* переход назад на папку выше (влево) */
			strcpy(path_left, path_center);
			strcpy(path_center, path_right);

			destroy_list(left -> top);
			free(left);
			left = init_list(LEFT);
			left -> top	= center -> top;
			center -> top = right -> top;
			right -> top = NULL;
			count = count_nodes(center -> top);
			cur_point = 1;
			offset = 0;
		}

		char * new_name = NULL;
		int yt = (height_term / 2) - (count / 2);
		yt = (yt < 1) ? 1: yt;
		yt += cur_point - 1;

		if((ch == 'r') || (ch == 'R')) {
			/* переименование элемента (файла, папки и тд) */

			new_name = rename_element(name_cur, path_center, len_term, yt);
			if(new_name != NULL && strlen(new_name) > 0){
				strcpy(peek_entry(center -> top, cur_point + offset) -> d_name, new_name);
				free(new_name);	
			}
		}
		
		else if(ch == 'n') {
			/* добавление нового файла */
			struct dirent newFileInfo;
			newFileInfo.d_type = TYPE_FILE;
			strcpy(newFileInfo.d_name, "file");
			char * path_temp = malloc(256 * sizeof(char));
			strcpy(path_temp, path_center);
			path_temp = getPathUp(path_temp, "file");
			fclose(fopen(path_temp, "w")); /* создаем файл */

			create_node(center, &newFileInfo, cur_point + offset);
			
			downDirList(&cur_point, &offset, height_term, count);
			drowing_pole_dir(center, cur_point, offset, CENTER);

			new_name = rename_element("file", path_center, len_term, yt + 1);

			if((new_name != NULL) && (strlen(new_name) > 0)){
				strcpy(peek_entry(center -> top, cur_point + offset) -> d_name, new_name);
				free(new_name);
			}
			else {
				remove(path_temp);
				delete_node(center, cur_point + offset + 1);
			}
			free(path_temp);

		}

		else if(ch == 'c' || ch == 'C'){
			// удаление файла
			char * path_temp = malloc(256 * sizeof(char));
			int change = warning_remove_file(len_term, yt);
			if(change){
				strcpy(path_temp, path_center);
				path_temp = getPathUp(path_temp, name_cur);
				delete_node(center, cur_point + offset);
				remove(path_temp);
			}
			free(path_temp);
		}
		
		drowing_pole_dir(center, cur_point, offset, CENTER);
		drowing_pole_dir(left, cur_point, 0, LEFT);
		drowing_pole_dir(right, cur_point, 0, RIGHT);

		gotoxy(1,1);
		printf("%s\n", path_center);
		printf("%s", path_left);	
		
		gotoxy(255, 255);
		ch = fgetc(stdin);
	}

	for(int i = 0; i < height_term + 1; i++) clearln(i);
	destroy_list(center -> top);
	free(center);
	free(path_left);
	free(path_center);
	free(path_right);
	gotoxy(1, 1);
	reset_keypress();
	return 0;
}

char exgetch(void)
{
	char ch = getchar();
	if(ch == ESCAPE) {
		if(mygetch(1)) return fgetc(stdin);
		
	}

	return ch;
}

int warning_remove_file(int len_term, int yt)
{
	// выдает предупреждение о том, точно ли пользователь хочет удалить файл
	char * warning_text = "Confirm file deletion";
	int xt = len_term / 2 - strlen(warning_text) / 2 - 1;
	int x_no = strlen(warning_text) / 4 - 1;
	int x_yes = 3 * strlen(warning_text) / 4 - 1;
	int change = 0;
	char * front_color = GREY_FRONT;
	char ch = ' ';
	
	printf("%s", WHITE_FRONT);

	gotoxy(xt, yt - 1);
	for(int i = 0; i < (int)strlen(warning_text) + 2; i++) printf(" ");

	gotoxy(xt, yt);
	printf("%s %s ", ORANGE, warning_text);

	while(ch != ENTER) {
		if(ch == PG_LF) change = 0;
		else if(ch == PG_RT) change = 1;
		else if(ch == ESCAPE) {
			change = 0;
			break;
		}
		
		gotoxy(xt, yt + 1);
		for(int i = 0; i < (int)strlen(warning_text) + 2; i++) printf("%s ", WHITE_FRONT);

		gotoxy(xt + x_no, yt + 1);
		front_color = (change) ? GREY_FRONT : ORANGE_FRONT;
		printf("%s%s X %s", front_color, WHITE, ESC);

		gotoxy(xt + x_yes, yt + 1);
		front_color = (change) ? ORANGE_FRONT : GREY_FRONT;
		printf("%s%s V %s", front_color, WHITE, ESC);

		gotoxy(255, 255);
		ch = fgetc(stdin);
	}
	
	return change;
}

void downDirList(int * cur_point, int * offset, int height_term, int count)
{
	if((*cur_point < height_term) && (*cur_point < count)) *cur_point += 1;
	else if((*cur_point + *offset) < count) *offset += 1;  
	else {
		*cur_point = 1;
		*offset = 0;
	}
}

char * rename_element(char * old_name, const char * path, int len_term, int yt)
{	
	char ch;
	int xl = len_term / 3 + 1;
	int len = strlen(old_name);
	char * new_name = malloc((xl - 9) * sizeof(char));

	strcpy(new_name, old_name);
	
	gotoxy(xl, yt);
	printf("%s%s%*s%s", WHITE_FRONT, BLACK, len_term / 3 - 7, old_name, ESC);
	
	while((ch = getchar()) != '\n') { 
		if(ch == ESCAPE) { // символ ESC
			free(new_name);
			new_name = NULL;
			break;
		}
		if(ch == BACKSPACE && (len > 0)) {
			new_name = del_symbol_from_str(new_name);
			len--;
		}
		else if(isgraph(ch) && (len < xl - 10)) {
			new_name = add_symbol_to_str(new_name, ch);
			len++;
		}

		gotoxy(xl, yt);
		printf("%s%s%*s%s", WHITE_FRONT, BLACK, len_term / 3 - 7, new_name, ESC);
	}

	if(new_name != NULL && strlen(new_name) > 0) {
		char * new_path = malloc(256 * sizeof(char));
		char * oldpath = malloc(256 * sizeof(char));
		strcpy(oldpath, path);
		strcpy(new_path, path);
		new_path = getPathUp(new_path, new_name);
		oldpath = getPathUp(oldpath, old_name);

		rename(oldpath, new_path);

		free(oldpath);
		free(new_path);		
	}

	return new_name;
}

char * add_symbol_to_str(char * str, char ch)
{
	int j = 0;
	while(str[j] != '\0') j++;

	str[j] = ch;
	j++;
	str[j] = '\0';

	return str;
}

char * del_symbol_from_str(char * str)
{
	int j = 0;
	if(*str != '\0'){
		while(str[j] != '\0') j++;
			j--;
			str[j] = '\0';	
	}
	return str;
}

char * getPathDone(char * path)
{
	char * res = strrchr(path, '/');
	if(res == path && *(res + 1) == '\0') *res = '\0';
	else if(res == path && *(res + 1) != '\0') *(res + 1) = '\0';
	else *res = '\0';

	return path;
}

char * getPathUp(char * path, char * filename)
{
	char * res = NULL;
	if(strcmp(path, "/") != 0) res = strcat(path, "/");
	res = strcat(res, filename);

	return res;
	
}

int isRootDir(char * path)
{
	return (strcmp(path, "/") == 0);
}

WindowInfo_t * reading_dir(char * dirname, int flag)
{
	if(dirname == NULL) return NULL;
	
	struct dirent * entry = NULL;
	int num = 1;
	WindowInfo_t * point = init_list(flag);
	DList_t * list = point -> top;
	
	DIR * cur_dir = opendir(dirname);

	if(cur_dir) { 
		while((entry = readdir(cur_dir)) != NULL){
			if(strcmp(entry -> d_name, ".") == 0 || strcmp(entry -> d_name, "..") == 0) continue;
			list = push_entry(list, entry, num);
			if(point -> top == NULL) point -> top = list;
			num ++;
		}
		closedir(cur_dir);
	}
	return point;
}

void reading_and_drowing_file(char * path_to_file)
{
	int len_term = 0, height_term = 0;
	int mode = 1; // возможна ли прокрутка файла вниз
	long offset = 0;
	char ch = ' ';
	
	getSizeTerm(&len_term, &height_term);
	drowing_pole_file(len_term, height_term);
	FILE * fp = fopen(path_to_file, "r");

	if(fp) {
		mode = print_file(fp, offset, len_term, height_term);

		gotoxy(len_term / 2 - strlen(path_to_file) / 2, height_term / 10);
		printf("%s%s%s%s", WHITE_FRONT, BLACK, path_to_file, ESC);

		while((ch = getchar()) != ESCAPE) {
			getSizeTerm(&len_term, &height_term);
			if((ch == 'w') && offset > 0) offset = prev_line_in_file(fp);
			else if((ch == 's') && mode) offset = next_line_in_file(fp);
			
			drowing_pole_file(len_term, height_term);
			gotoxy(len_term / 2 - strlen(path_to_file) / 2, height_term / 10);
			printf("%s%s%s%s", WHITE_FRONT, BLACK, path_to_file, ESC);
			mode = print_file(fp, offset, len_term, height_term);
			gotoxy(len_term / 10, height_term / 10 + 1);
			
			ch = ' ';
		}
	}
	fclose(fp);
}

int print_file(FILE * fp, int offset, int len_term, int height_term)
{
	char ch = ' ';

	int x = len_term / 10 + 1;
	int y = height_term / 10 + 1;
	
	fseek(fp, offset, SEEK_SET);
	while((ch = fgetc(fp)) != EOF) {
		if(ch == '\t') x += 2;
		if(x >= len_term * 0.9 - 1) while(ch != '\n') ch = fgetc(fp);
		if(ch == '\n') {
			x = len_term / 10 + 1;
			y++;
			continue;
		}
		if(y > (height_term * 0.9 - 1)) break;
		
		gotoxy(x, y);
		printf("%s%s%c%s", GREY_FRONT, BLACK, ch, ESC);
		x++;
	}
	fseek(fp, offset, SEEK_SET);
	return (ch != EOF) ? 1 : 0; // чтобы нельзя было опускаться ниже конца файла
}

long prev_line_in_file(FILE * fp)
{
	long prev_ofsset = ftell(fp);
	char ch = ' ';
	int count = 0;
	while((prev_ofsset > 0) && count < 2) {
		prev_ofsset--;
		fseek(fp, prev_ofsset, SEEK_SET);
		ch = fgetc(fp);
		count += (ch == '\n');
	}
	return (prev_ofsset == 0) ? prev_ofsset : prev_ofsset + 1;
	
}

long next_line_in_file(FILE * fp)
{
	char ch;
	long offset = ftell(fp);
	while(((ch = fgetc(fp)) != '\n') && (ch != EOF)) ;
	return (ch == EOF) ? offset : ftell(fp);
}
