CC 		 			= gcc
CPPFLAGS 			= -I.
CFLAGS 				= -Wall -Wextra -g
SDL_CFLAGS  		:= $(shell sdl2-config --cflags)
SDL_LIBS			:= $(shell sdl2-config --libs)

RUN_TARGET 			= run-emulator
CPU_TEST_TARGET 	= test-cpu
DISPLAY_TEST_TARGET = test-display
VIDEO_TEST_TARGET 	= test-video

EMULATOR_BIN		= emulator
CPU_TEST_BIN		= test_cpu
DISPLAY_TEST_BIN	= test_display
VIDEO_TEST_BIN		= test_video

CORE_SRCS 			= disassembler.c state.c interrupt.c helper_funcs.c
EMULATE_SRCS		= emulate.c display.c video.c $(CORE_SRCS)
CPU_TEST_SRCS 		= test/test_cpu.c $(CORE_SRCS)
DISPLAY_TEST_SRCS 	= test/test_display.c display.c
VIDEO_TEST_SRCS 	= test/test_video.c video.c display.c

EMULATE_OBJS 		= $(EMULATE_SRCS:.c=.o)
CPU_TEST_OBJS 		= $(CPU_TEST_SRCS:.c=.o)
DISPLAY_TEST_OBJS 	= $(DISPLAY_TEST_SRCS:.c=.o)
VIDEO_TEST_OBJS 	= $(VIDEO_TEST_SRCS:.c=.o)
SDL_OBJS			= display.o test/test_display.o test/test_video.o
ALL_OBJS			:= $(sort \
	$(EMULATE_OBJS) \
	$(CPU_TEST_OBJS) \
	$(DISPLAY_TEST_OBJS) \
	$(VIDEO_TEST_OBJS))

.PHONY: all run-emulator test-cpu test-display test-video clean valgrind

all: $(EMULATOR_BIN)

$(RUN_TARGET): $(EMULATOR_BIN)
	./$(EMULATOR_BIN) ./rom/invaders

$(CPU_TEST_TARGET): $(CPU_TEST_BIN)
	./$(CPU_TEST_BIN)

$(DISPLAY_TEST_TARGET): $(DISPLAY_TEST_BIN)
	./$(DISPLAY_TEST_BIN)

$(VIDEO_TEST_TARGET): $(VIDEO_TEST_BIN)
	./$(VIDEO_TEST_BIN)

clean:
	rm -f $(EMULATOR_BIN) $(CPU_TEST_BIN) $(DISPLAY_TEST_BIN) $(VIDEO_TEST_BIN) $(ALL_OBJS)

valgrind: $(EMULATOR_BIN)
	valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(EMULATOR_BIN) ./rom/invaders

$(EMULATOR_BIN): $(EMULATE_OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^ $(SDL_LIBS)

$(CPU_TEST_BIN): $(CPU_TEST_OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

$(DISPLAY_TEST_BIN): $(DISPLAY_TEST_OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^ $(SDL_LIBS)

$(VIDEO_TEST_BIN): $(VIDEO_TEST_OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^ $(SDL_LIBS)

$(SDL_OBJS): CPPFLAGS += $(SDL_CFLAGS)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
