
CC=gcc
LIB_DIR=./lib
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src
TST_DIR=./testes

all: regra1 regra2

regra1: $(SRC_DIR)/*.c
	$(CC) -c $(SRC_DIR)/*.c -L$(LIB_DIR) -I$(INC_DIR) -Wall -m32
	mv *.o $(BIN_DIR)
	
regra2:  $(BIN_DIR)/cthread.o $(BIN_DIR)/support.o $(BIN_DIR)/util.o
	ar crs $(LIB_DIR)/libcthread.a $(BIN_DIR)/cthread.o $(BIN_DIR)/support.o $(BIN_DIR)/util.o
	
clean:
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/cthread.o $(BIN_DIR)/util.o


