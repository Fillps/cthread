
CC=gcc
LIB_DIR=./lib
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src

all: regra1 regra2

regra1: $(SRC_DIR)/*.c
	$(CC) -c $(SRC_DIR)/*.c -L$(LIB_DIR) -I$(INC_DIR) -Wall
	mv *.o $(BIN_DIR)
	
regra2:  $(BIN_DIR)/*.o
	ar crs $(LIB_DIR)/libcthread.a $(BIN_DIR)/*.o

clean:
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/cthread.o $(BIN_DIR)/insert.o


