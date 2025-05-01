CC = gcc
CFLAGS = -Wall -Wextra -Werror -c 

all: build

build: exflmng.o list.o term_settings.o drowing.o
	$(CC) exflmng.o list.o term_settings.o drowing.o  -o ~/exmng

exflmng.o: src/exflmng.c
	$(CC) $(CFLAGS) src/exflmng.c

list.o : src/list.c
	$(CC) $(CFLAGS) src/list.c

term_settings.o : src/term_settings.c
	$(CC) $(CFLAGS) src/term_settings.c

drowing.o : src/drowing.c
	$(CC) $(CFLAGS) src/drowing.c

