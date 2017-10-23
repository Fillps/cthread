#
# Makefile para programas exemplo
# 

CC=gcc
LIB_DIR=../lib
FLAGS = -L$(LIB_DIR) -lcthread -Wall -m32

all: exemplo mandel teste_vetor barbeiro filosofos prodcons series

exemplo: exemplo.c $(LIB_DIR)/libcthread.a
	$(CC) -o exemplo exemplo.c $(FLAGS)

teste_vetor: teste_vetor.c $(LIB_DIR)/libcthread.a
	$(CC) -o teste_vetor teste_vetor.c $(FLAGS)

mandel: mandel.c $(LIB_DIR)/libcthread.a
	$(CC) -o mandel mandel.c $(FLAGS)

barbeiro: barbeiro.c $(LIB_DIR)/libcthread.a
	$(CC) -o barbeiro barbeiro.c $(FLAGS)

filosofos: filosofos.c $(LIB_DIR)/libcthread.a
	$(CC) -o filosofos filosofos.c $(FLAGS)

prodcons: prodcons.c $(LIB_DIR)/libcthread.a
	$(CC) -o prodcons prodcons.c $(FLAGS)

series: series.c $(LIB_DIR)/libcthread.a
	$(CC) -o series series.c $(FLAGS)
	
clean:
	rm -rf exemplo mandel teste_vetor barbeiro filosofos prodcons series *.o *~




