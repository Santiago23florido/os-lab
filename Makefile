CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude

SRC = src/main.c src/mem.c src/fs.c src/sandbox.c src/protect.c src/threads.c
OBJ = $(SRC:.c=.o)

TARGET = program

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)

run: all
	./$(TARGET)
