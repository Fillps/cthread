
CC=gcc
LIB_DIR=../lib
CTH_DIR=../
FLAGS = -L$(LIB_DIR) -lcthread -Wall -m32

all:  meusTestes 

meusTestes: meusTestes.c $(LIB_DIR)/libcthread.a
	$(CC) -o meusTestes meusTestes.c $(FLAGS)

clean: 
	rm -rf meusTestes *.o *~