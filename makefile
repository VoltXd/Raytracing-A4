CC=gcc
CFLAGS=-Wall -Ofast -march=native -mtune=native -lm -lOpenCL -DCL_TARGET_OPENCL_VERSION=300
SRC=src/*.c
INCLUDE=-I include
BIN=raytracing-app

all:
	$(CC) -fopenmp $(SRC) -o $(BIN) $(INCLUDE) $(CFLAGS) 

vtune:
	$(CC) -fopenmp $(SRC) -o $(BIN) $(INCLUDE) $(CFLAGS) -g

asm:
	$(CC) $(SRC) -S $(INCLUDE) $(CFLAGS) -fverbose-asm

clean:
	rm -f $(BIN) *.o *.i *.s *.png