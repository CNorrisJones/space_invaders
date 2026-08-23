# 1. Variables - Makes it easy to change compiler options later
CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = emulator
SRCS = test.c disassembler.c state.c helper_funcs.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

# Rule to link the object files into the final executable binary
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Rule to compile each .c file into a temporary .o object file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean Target - Run 'make clean' to wipe out compiled binaries and start fresh
clean:
	rm -f $(TARGET) $(OBJS)

run: $(TARGET)
	./$(TARGET) ./rom/invaders

valgrind: $(TARGET)
	valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET) ./rom/invaders