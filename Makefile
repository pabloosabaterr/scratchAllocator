CC = gcc
FLAGS = -Wall -Werror -pedantic
TARGET = output
SOURCES = src/main.c

all:
	$(CC) $(FLAGS) -o $(TARGET) $(SOURCES)