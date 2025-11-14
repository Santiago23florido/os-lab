TARGET = program
CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude -D_POSIX_C_SOURCE=200809L

SRC_C = src/main.c src/mem.c src/fs.c src/sandbox.c src/protect.c src/threads.c src/coroutines.c src/terminal.c src/network.c
SRC_S = src/coroutines-asm.s

OBJ_C = $(SRC_C:.c=.o)
OBJ_S = $(SRC_S:.s=.o)
OBJ = $(OBJ_C) $(OBJ_S)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

# Compilar .c → .o
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compilar .s → .o (esta es la regla que faltaba en tu ejecución real)
src/%.o: src/%.s
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: all
	./$(TARGET)
