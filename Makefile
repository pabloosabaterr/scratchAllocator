CC = gcc
FLAGS = -Wall -Werror -pedantic
TARGET = output
SOURCES = src/main.c src/allocator.c

all:
	$(CC) $(FLAGS) -o $(TARGET) $(SOURCES)

tests:
	$(CC) $(FLAGS) -o test test.c && ./test