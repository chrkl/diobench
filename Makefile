CC	:= mpicc
CFLAGS := -g -Wall
LDFLAGS = -L/usr/lib
LDLIBS = -largtable2
OBJECTS  := bench.o diobench.o
INCLUDES = -Iinclude

diobench: bench.o diobench.o
	$(CC) $(LDFLAGS) bench.o diobench.o -o diobench $(LDLIBS)

bench.o:
	$(CC) -c $(INCLUDES) $(CFLAGS) src/bench.c -o bench.o

diobench.o:
	$(CC) -c $(INCLUDES) $(CFLAGS) src/diobench.c -o diobench.o

all: diobench

clean:
	rm -rf *.o
	rm -rf diobench
