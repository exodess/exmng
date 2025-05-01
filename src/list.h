#ifndef LIST_H
#define LIST_H

#include <dirent.h>

typedef struct dir_list {
	struct dirent cur_entry;
	struct dir_list * next;
	struct dir_list * prev; 
	int num; // номер в списке
} DList_t; 

typedef struct {
	DList_t * top;
	enum {
		LEFT,
		CENTER,
		RIGHT
	} flags;
} ListInfo_t; 

ListInfo_t * init_list(int); 	// создание нового списка 		DList_t * top = init_list();
DList_t * push_entry(DList_t * list, struct dirent * entry, int number);	// добавляем новый файл в список 
struct dirent * peek_entry(DList_t * list, int num); // получение необходимого файла по его номеру (координата курсора)
void create_node(ListInfo_t *, struct dirent, int);
void delete_node(ListInfo_t * , int);
void destroy_list(DList_t * ); 	// удаление списка
int count_nodes(DList_t * list);

#endif
