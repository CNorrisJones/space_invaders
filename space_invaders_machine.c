#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "iobus.h"
#include "cpu.h"
#include "display.h"
#include "video.h"
#include "input_state.h"
#include "space_invaders_interrupt_scheduler.h"
#include "space_invaders_machine.h"

static uint8_t ToReplaceRead(void *context, uint8_t port) {
    SpaceInvadersIoState *io_state = context;

    if (port == 0x01) {
        return io_state->shift_amount;
    } else {
        return 0xFF;
    }
}

static void ToReplaceWrite(void *context, uint8_t port, uint8_t value) {
    SpaceInvadersIoState *io_state = context;
    if (port == 0x02) {
        io_state->shift_amount = value;
    }
}

SpaceInvadersMachine *InitSpaceInvadersMachine(SpaceInvadersStartingLives starting_lives, bool bonus_life_value) {
    switch(starting_lives) {
        case SPACE_INVADERS_LIVES_3: break;
        case SPACE_INVADERS_LIVES_4: break;
        case SPACE_INVADERS_LIVES_5: break;
        case SPACE_INVADERS_LIVES_6: break;
        default: return NULL;
    }
    
    SpaceInvadersMachine *space_invaders_machine = malloc(sizeof(SpaceInvadersMachine));
    if (space_invaders_machine == NULL) {
        return NULL;
    }

    space_invaders_machine->cpu = NULL;
    space_invaders_machine->io_bus = NULL;
    space_invaders_machine->display = NULL;
    space_invaders_machine->frame_buffer = NULL;

    space_invaders_machine->interrupt_scheduler.cycles_elapsed = 0;
    space_invaders_machine->interrupt_scheduler.int_request_pending = 0;
    space_invaders_machine->interrupt_scheduler.next_vector = 0x08;
    space_invaders_machine->interrupt_scheduler.total_cycle_count = 0;

    space_invaders_machine->io_state.dip_configuration.starting_num_lives = starting_lives;
    space_invaders_machine->io_state.dip_configuration.bonus_life_value = bonus_life_value;
    space_invaders_machine->io_state.dip_configuration.coin_info = 0;
    space_invaders_machine->io_state.dip_configuration.power_test = 0;
    
    space_invaders_machine->io_state.input_state.credit = 0;
    space_invaders_machine->io_state.input_state.one_player = 0;
    space_invaders_machine->io_state.input_state.two_player = 0;
    space_invaders_machine->io_state.input_state.p1_fire = 0;
    space_invaders_machine->io_state.input_state.p1_left = 0;
    space_invaders_machine->io_state.input_state.p1_right = 0;
    space_invaders_machine->io_state.input_state.p2_fire = 0;
    space_invaders_machine->io_state.input_state.p2_left = 0;
    space_invaders_machine->io_state.input_state.p2_right = 0;
    space_invaders_machine->io_state.input_state.tilt = 0;

    space_invaders_machine->io_state.shift_amount = 0x00;
    space_invaders_machine->io_state.shift_register = 0x0000;

    space_invaders_machine->io_state.sound_latches.amplifier_enable_effect = 0;
    space_invaders_machine->io_state.sound_latches.extended_play_effect = 0;
    space_invaders_machine->io_state.sound_latches.fleet_move_1_effect = 0;
    space_invaders_machine->io_state.sound_latches.fleet_move_2_effect = 0;
    space_invaders_machine->io_state.sound_latches.fleet_move_3_effect = 0;
    space_invaders_machine->io_state.sound_latches.fleet_move_4_effect = 0;
    space_invaders_machine->io_state.sound_latches.invader_die_effect = 0;
    space_invaders_machine->io_state.sound_latches.player_die_effect = 0;
    space_invaders_machine->io_state.sound_latches.shot_effect = 0;
    space_invaders_machine->io_state.sound_latches.ufo_effect = 0;
    space_invaders_machine->io_state.sound_latches.ufo_hit_effect = 0;

    Cpu8080 *cpu = CreateCpu(0x00);
    if (cpu == NULL) {
        free(space_invaders_machine);
        return NULL;
    }

    DisplayContext *display = DisplayInit(VIDEO_OUTPUT_WIDTH, VIDEO_OUTPUT_HEIGHT);
    if (display == NULL) {
        FreeCpu(cpu);
        free(space_invaders_machine);
        return NULL;
    }

    uint32_t *frame_buffer = malloc(VIDEO_OUTPUT_WIDTH * VIDEO_OUTPUT_HEIGHT * sizeof(uint32_t));

    if (frame_buffer == NULL) {
        DisplayShutdown(display);
        FreeCpu(cpu);
        free(space_invaders_machine);
        return NULL;
    }

    IoBus *io_bus = IoBusInit(&space_invaders_machine->io_state, ToReplaceRead, ToReplaceWrite);
    if (io_bus == NULL) {
        free(frame_buffer);
        DisplayShutdown(display);
        FreeCpu(cpu);
        free(space_invaders_machine);
        return NULL;
    }

    space_invaders_machine->cpu = cpu;
    space_invaders_machine->cpu->io_bus = io_bus;
    space_invaders_machine->io_bus = io_bus;
    space_invaders_machine->display = display;
    space_invaders_machine->frame_buffer = frame_buffer;

    return space_invaders_machine;
}

void SpaceInvadersMachineShutdown(SpaceInvadersMachine *machine) {
    if (machine == NULL) {
        return;
    }

    if (machine->cpu != NULL) {
        Cpu8080 *cpu_to_delete = machine->cpu;
        machine->cpu = NULL;
        FreeCpu(cpu_to_delete);
    }

    if (machine->io_bus != NULL) {
        IoBus *io_bus_to_delete = machine->io_bus;
        machine->io_bus = NULL;
        IoBusShutdown(io_bus_to_delete);
    }

    if (machine->display != NULL) {
        DisplayContext *display_to_delete = machine->display;
        machine->display = NULL;
        DisplayShutdown(display_to_delete);
    }

    if (machine->frame_buffer != NULL) {
        uint32_t *buffer_to_delete = machine->frame_buffer;
        machine->frame_buffer = NULL;
        free(buffer_to_delete);
    }

    free(machine);
}
