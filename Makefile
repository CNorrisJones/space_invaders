CC 		 					= gcc
CPPFLAGS 					= -I.
CFLAGS 						= -Wall -Wextra -g
SDL_CFLAGS  				:= $(shell sdl2-config --cflags)
SDL_LIBS					:= $(shell sdl2-config --libs)
MIXER_CFLAGS				:= $(shell pkg-config --cflags SDL2_mixer)
MIXER_LIBS					:= $(shell pkg-config --libs SDL2_mixer)

RUN_TARGET						= run-emulate
RUN_DIAGNOSTIC_TARGET 			= run-diagnostic
ALL_TEST_TARGET					= test-all
AUDIO_TEST_TARGET				= test-audio
CPU_INTERRUPT_TEST_TARGET		= test-cpu-interrupt
CPU_IOBUS_TEST_TARGET 			= test-cpu-iobus
CPU_TIMING_TEST_TARGET			= test-cpu-timing
CPU_TEST_TARGET 				= test-cpu
DISPLAY_TEST_TARGET 			= test-display
INTERRUPT_SCHEDULER_TEST_TARGET	= test-interrupt-scheduler
IOBUS_TEST_TARGET				= test-iobus
SHIFT_REGISTER_TEST_TARGET 		= test-shift-register
VIDEO_TEST_TARGET 				= test-video


EMULATE_BIN						= emulate
DIAGNOSTIC_BIN					= diagnostic
ALL_TEST_BIN					= test_all
AUDIO_TEST_BIN					= test_audio
CPU_INTERRUPT_TEST_BIN			= test_cpu_interrupt
CPU_IOBUS_TEST_BIN				= test_cpu_iobus
CPU_TIMING_TEST_BIN				= test_cpu_timing
CPU_TEST_BIN					= test_cpu
DISPLAY_TEST_BIN				= test_display
INTERRUPT_SCHEDULER_TEST_BIN	= test_interrupt_scheduler
IOBUS_TEST_BIN					= test_iobus
SHIFT_REGISTER_TEST_BIN			= test_shift_register
VIDEO_TEST_BIN					= test_video


CORE_SRCS 						= disassembler.c cpu.c helper_funcs.c iobus.c
EMULATE_SRCS					= audio.c space_invaders_machine.c interrupt_scheduler.c emulate.c display.c video.c $(CORE_SRCS)
DIAGNOSTIC_SRCS					= diagnostic.c $(CORE_SRCS)
AUDIO_TEST_SRCS					= test/test_audio.c audio.c
CPU_INTERRUPT_TEST_SRCS			= test/test_cpu_interrupt.c $(CORE_SRCS)
CPU_IOBUS_TEST_SRCS				= test/test_cpu_iobus.c $(CORE_SRCS)
CPU_TIMING_TEST_SRCS			= test/test_cpu_timing.c $(CORE_SRCS)
CPU_TEST_SRCS 					= test/test_cpu.c $(CORE_SRCS)
DISPLAY_TEST_SRCS 				= test/test_display.c display.c
INTERRUPT_SCHEDULER_TEST_SRCS	= test/test_interrupt_scheduler.c interrupt_scheduler.c
IOBUS_TEST_SRCS					= test/test_iobus.c iobus.c
SHIFT_REGISTER_TEST_SRCS		= test/test_shift_register_hardware.c audio.c display.c space_invaders_machine.c $(CORE_SRCS)
VIDEO_TEST_SRCS 				= test/test_video.c video.c display.c
HARNESS_TEST_SRCS = \
    test/test_harness.c \
    test/test_audio.c \
    test/test_cpu_interrupt.c \
    test/test_cpu_iobus.c \
    test/test_cpu_timing.c \
    test/test_cpu.c \
    test/test_display.c \
    test/test_interrupt_scheduler.c \
    test/test_iobus.c \
    test/test_shift_register_hardware.c \
    test/test_video.c

EMULATE_OBJS					= $(EMULATE_SRCS:.c=.o)
DIAGNOSTIC_OBJS 				= $(DIAGNOSTIC_SRCS:.c=.o)
AUDIO_TEST_OBJS					= $(AUDIO_TEST_SRCS:.c=.o)
CPU_INTERRUPT_TEST_OBJS			= $(CPU_INTERRUPT_TEST_SRCS:.c=.o)
CPU_IOBUS_TEST_OBJS				= $(CPU_IOBUS_TEST_SRCS:.c=.o)
CPU_TIMING_TEST_OBJS 			= $(CPU_TIMING_TEST_SRCS:.c=.o)
CPU_TEST_OBJS 					= $(CPU_TEST_SRCS:.c=.o)
DISPLAY_TEST_OBJS 				= $(DISPLAY_TEST_SRCS:.c=.o)
INTERRUPT_SCHEDULER_TEST_OBJS	= $(INTERRUPT_SCHEDULER_TEST_SRCS:.c=.o)
IOBUS_TEST_OBJS 				= $(IOBUS_TEST_SRCS:.c=.o)
SHIFT_REGISTER_TEST_OBJS 		= $(SHIFT_REGISTER_TEST_SRCS:.c=.o)
VIDEO_TEST_OBJS 				= $(VIDEO_TEST_SRCS:.c=.o)
HARNESS_TEST_OBJS 				= $(HARNESS_TEST_SRCS:.c=.harness.o)
MIXER_OBJS						= audio.o test/test_audio.o
SDL_OBJS						= display.o test/test_display.o test/test_video.o
ALL_OBJS						:= $(sort \
	$(EMULATE_OBJS)						\
	$(DIAGNOSTIC_OBJS) 					\
	$(HARNESS_TEST_OBJS)				\
	$(AUDIO_TEST_OBJS)					\
	$(CPU_INTERRUPT_TEST_OBJS)			\
	$(CPU_IOBUS_TEST_OBJS)				\
	$(CPU_TIMING_TEST_OBJS)				\
	$(CPU_TEST_OBJS) 					\
	$(DISPLAY_TEST_OBJS) 				\
	$(INTERRUPT_SCHEDULER_TEST_OBJS)	\
	$(IOBUS_TEST_OBJS)					\
	$(SHIFT_REGISTER_TEST_OBJS)			\
	$(VIDEO_TEST_OBJS))
	
HARNESS_SUPPORT_OBJS = \
    audio.o display.o video.o interrupt_scheduler.o \
    space_invaders_machine.o disassembler.o cpu.o \
    helper_funcs.o iobus.o

.PHONY: all run-emulate run-diagnostic test-all test-audio test-cpu-interrupt test-cpu-iobus test-cpu-timing test-cpu test-display test-interrupt-scheduler test-iobus test-shift-register test-video clean valgrind

all: $(EMULATE_BIN)

$(RUN_TARGET): $(EMULATE_BIN)
	./$(EMULATE_BIN)

$(RUN_DIAGNOSTIC_TARGET): $(DIAGNOSTIC_BIN)
	./$(DIAGNOSTIC_BIN) ./rom/invaders

$(ALL_TEST_TARGET): $(ALL_TEST_BIN)
	./$(ALL_TEST_BIN)

$(AUDIO_TEST_TARGET): $(AUDIO_TEST_BIN)
	./$(AUDIO_TEST_BIN)

$(CPU_INTERRUPT_TEST_TARGET): $(CPU_INTERRUPT_TEST_BIN)
	./$(CPU_INTERRUPT_TEST_BIN)

$(CPU_IOBUS_TEST_TARGET): $(CPU_IOBUS_TEST_BIN)
	./$(CPU_IOBUS_TEST_BIN)

$(CPU_TIMING_TEST_TARGET): $(CPU_TIMING_TEST_BIN)
	./$(CPU_TIMING_TEST_BIN)

$(CPU_TEST_TARGET): $(CPU_TEST_BIN)
	./$(CPU_TEST_BIN)

$(DISPLAY_TEST_TARGET): $(DISPLAY_TEST_BIN)
	./$(DISPLAY_TEST_BIN)

$(INTERRUPT_SCHEDULER_TEST_TARGET): $(INTERRUPT_SCHEDULER_TEST_BIN)
	./$(INTERRUPT_SCHEDULER_TEST_BIN)

$(IOBUS_TEST_TARGET): $(IOBUS_TEST_BIN)
	./$(IOBUS_TEST_BIN)

$(SHIFT_REGISTER_TEST_TARGET): $(SHIFT_REGISTER_TEST_BIN)
	./$(SHIFT_REGISTER_TEST_BIN)

$(VIDEO_TEST_TARGET): $(VIDEO_TEST_BIN)
	./$(VIDEO_TEST_BIN)

clean:
	rm -f $(EMULATE_BIN) $(DIAGNOSTIC_BIN) $(ALL_TEST_BIN) $(AUDIO_TEST_BIN) $(CPU_INTERRUPT_TEST_BIN) $(CPU_IOBUS_TEST_BIN) $(CPU_TIMING_TEST_BIN) $(CPU_TEST_BIN) $(DISPLAY_TEST_BIN) $(INTERRUPT_SCHEDULER_TEST_BIN) $(IOBUS_TEST_BIN) $(SHIFT_REGISTER_TEST_BIN) $(VIDEO_TEST_BIN) $(ALL_OBJS)

valgrind: $(EMULATE_BIN)
	valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(EMULATE_BIN)

$(EMULATE_BIN): $(EMULATE_OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^ $(SDL_LIBS) $(MIXER_LIBS)

$(DIAGNOSTIC_BIN): $(DIAGNOSTIC_OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

$(ALL_TEST_BIN): $(HARNESS_TEST_OBJS) $(HARNESS_SUPPORT_OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^ $(SDL_LIBS) $(MIXER_LIBS)

$(AUDIO_TEST_BIN): $(AUDIO_TEST_OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^ $(MIXER_LIBS)

$(CPU_INTERRUPT_TEST_BIN): $(CPU_INTERRUPT_TEST_OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

$(CPU_IOBUS_TEST_BIN): $(CPU_IOBUS_TEST_OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

$(CPU_TIMING_TEST_BIN): $(CPU_TIMING_TEST_OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

$(CPU_TEST_BIN): $(CPU_TEST_OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

$(DISPLAY_TEST_BIN): $(DISPLAY_TEST_OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^ $(SDL_LIBS)

$(INTERRUPT_SCHEDULER_TEST_BIN): $(INTERRUPT_SCHEDULER_TEST_OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

$(IOBUS_TEST_BIN): $(IOBUS_TEST_OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

$(SHIFT_REGISTER_TEST_BIN): $(SHIFT_REGISTER_TEST_OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^ $(SDL_LIBS) $(MIXER_LIBS)

$(VIDEO_TEST_BIN): $(VIDEO_TEST_OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^ $(SDL_LIBS)

$(MIXER_OBJS): CPPFLAGS += $(MIXER_CFLAGS)

$(SDL_OBJS): CPPFLAGS += $(SDL_CFLAGS)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

%.harness.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DTEST_HARNESS -c $< -o $@
