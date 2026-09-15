# 1. Variables - Makes it easy to change compiler options later
CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = emulator
TEST_TARGET = test_runner

SHARED_SRCS = disassembler.c state.c helper_funcs.c
EMULATE_SRCS = emulate.c $(SHARED_SRCS)
TEST_SRCS = test.c $(SHARED_SRCS)

EMULATE_OBJS = $(EMULATE_SRCS:.c=.o)
TEST_OBJS = $(TEST_SRCS:.c=.o)
ALL_OBJS = emulate.o test.o disassembler.o state.o helper_funcs.o

all: $(TARGET)

$(TARGET): $(EMULATE_OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(EMULATE_OBJS)

$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(TEST_TARGET) $(ALL_OBJS)

run: $(TARGET)
	./$(TARGET) ./rom/invaders

test: $(TEST_TARGET)
	./$(TEST_TARGET)

valgrind: $(TARGET)
	valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET) ./rom/invaders
