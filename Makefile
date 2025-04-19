CC = gcc
CFLAGS = -Wall -Wextra -Werror
TARGET = sh
SRC = sh.c

default: build run

build:
	$(CC) $(SRC) -o $(TARGET)

run: build
	./grade.sh

clean:
	rm -f $(TARGET)