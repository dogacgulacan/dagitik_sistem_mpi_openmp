CC = mpicc
CFLAGS = -Wall -fopenmp
LDFLAGS = -fopenmp

.PHONY: all clean

all: data_processor

data_processor: src/main.c src/data_processor.c src/data_processor.h
	$(CC) $(CFLAGS) -o data_processor src/main.c src/data_processor.c $(LDFLAGS)

clean:
	rm -f data_processor