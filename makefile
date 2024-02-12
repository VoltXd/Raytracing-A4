CC=gcc
CFLAGS=-Wall -Ofast -march=native -mtune=native -lm

SRC=src/*.c
INCLUDE=-I include
BIN=raytracing-app

all:
	$(CC) $(SRC) -o $(BIN) $(INCLUDE) $(CFLAGS) 

clean:
	rm -f $(BIN) *.o *.i *.s *.png