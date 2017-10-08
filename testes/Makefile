
CC=gcc
LIB_DIR=../lib
CTH_DIR=../

all:  meusTestes 



meusTestes: meusTestes.c $(LIB_DIR)/libcthread.a
	$(CC) -o meusTestes meusTestes.c -L$(LIB_DIR) -lcthread -lm -Wall -m32

clean: 
	rm -rf *.o