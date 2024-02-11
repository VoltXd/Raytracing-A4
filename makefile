CC=gcc
CFLAGS=-Wall -O3 -march=native -mtune=native

SRC=src/*.c
INCLUDE=-I include
BIN=raytracing-app

all:
	$(CC) -o $(BIN) $(CFLAGS) $(INCLUDE) $(SRC)

clean:
	rm -f $(BIN) *.o *.i *.s *.png