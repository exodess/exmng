#include "list.h"
#include <stdlib.h>
ListInfo_t * init_list(int flags)
{
	// создается переменная, которая будет указывать на вершину списка
	ListInfo_t * new = malloc(sizeof(ListInfo_t));

	new -> top = NULL;
	new -> flags = flags;
	return new;
}

DList_t * push_entry(DList_t * list, struct dirent * entry, int number)
{
	// list указывает в конец списка
	DList_t * new = malloc(sizeof(DList_t));

	if(list != NULL) list -> next = new;
	new -> cur_entry = *entry;
	new -> next = NULL;
	new -> prev = list;
	new -> num = number;

	return new;
}

struct dirent * peek_entry(DList_t * list, int number)
{
	DList_t * temp = list;

	while((temp -> num) != number) {
		list = list -> next;
		temp = list;
	}

	return &(temp -> cur_entry);
}

void delete_node(ListInfo_t * root, int number)
{
	DList_t * temp = root -> top;
	if(number == 1) {
		// удаление корневого элемента
		root -> top = (root -> top -> next);
		free(temp);
	}
	else {
		DList_t * prev = NULL;
		while((temp -> num) != number) {
			prev = temp;
			temp = temp -> next;
		}
		prev -> next = temp -> next;
		free(temp);
		temp = prev -> next;

		while(temp != NULL) {
			temp -> num --;
			temp = temp -> next;
		}
	}
}

void create_node(ListInfo_t * root, struct dirent info, int number)
{
	DList_t * point = root -> top;
	while(point -> num != number) point = point -> next;

	DList_t * new = malloc(sizeof(DList_t));

	new -> cur_entry = info;
	new -> next = point -> next;
	new -> prev = point;
	new -> num = number;

	point -> next = new;

	while(new != NULL) {
		new -> num ++;
		new = new -> next;
	}
	
}

void destroy_list(DList_t * list)
{
	DList_t * temp = list;
	while(temp != NULL){
		list = list -> next;
		free(temp);
		temp = list;
	}
}

int count_nodes(DList_t * list)
{
	int count = 0;
	DList_t * temp = list;

	while(temp != NULL) {
		list = list -> next;
		temp = list;
		count ++;
	}

	return count;
}
