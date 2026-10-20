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

static uint8_t DetermineOffsetShiftRegister(uint16_t shift_register, uint8_t offset) {
    uint8_t offset_amount = 0x08 - (offset & 0x07);
    uint16_t offset_val = shift_register >> offset_amount;
    uint8_t return_val = (offset_val & 0xFF);
    return 0x00;
}

static uint8_t SpaceInvadersRead(void *context, uint8_t port) {
    SpaceInvadersIoState *io_state = context;
    uint8_t read_byte;

    // Arch Breakdown available here: https://computerarcheology.com/Arcade/SpaceInvaders/Hardware.html
    switch(port) {
        case 0x00: // PORT 0 READ
            read_byte =
                (0x01 << 7)                                     | // ? tied to demux port 7 (just always 1 it) ?
                (io_state->input_state.p1_fire << 6)            | // Right
                (io_state->input_state.p1_left << 5)            | // Left
                (io_state->input_state.p1_right << 4)           | // Fire
                (0x01 << 3)                                     | // Always 1
                (0x01 << 2)                                     | // Always 1
                (0x01 << 1)                                     | // Always 1
                (io_state->dip_configuration.power_test << 0);  // DIP4 (Seems to be self-test-request read at power up)
            return read_byte;
        case 0x01: // PORT 1 READ
            read_byte =
                (0x01 << 7)                             | // Not connected (Always 1)
                (io_state->input_state.p1_right << 6)   | // 1P right (1 if pressed)
                (io_state->input_state.p1_left << 5)    | // 1P left (1 if pressed)
                (io_state->input_state.p1_fire << 4)    | // 1P shot (1 if pressed)
                (0x01 << 3)                             | // Always 1
                (io_state->input_state.one_player << 2) | // 1P start (1 if pressed)
                (io_state->input_state.two_player << 1) | // 2P start (1 if pressed)
                (io_state->input_state.credit << 0);    // CREDIT (1 if deposit)
            return read_byte;
        case 0x02: // PORT 2 READ
            bool bit_0_val;
            bool bit_1_val;
            switch(io_state->dip_configuration.starting_num_lives) {
                case 3:
                    bit_0_val = 0;
                    bit_1_val = 0;
                case 4:
                    bit_0_val = 1;
                    bit_1_val = 0;
                case 5:
                    bit_0_val = 0;
                    bit_1_val = 1;
                case 6:
                    bit_0_val = 1;
                    bit_1_val = 1;
            }

            read_byte =
                (io_state->dip_configuration.coin_info << 7)        | // DIP7 Coin info displayed in demo screen 0=ON
                (io_state->input_state.p2_right << 6)               | // P2 right (1 if pressed)
                (io_state->input_state.p2_left << 5)                | // P2 left (1 if pressed)
                (io_state->input_state.p2_fire << 4)                | // P2 shot (1 if pressed)
                (io_state->dip_configuration.bonus_life_value << 3) | // DIP6 0 = extra ship at 1500, 1 = extra ship at 1000
                (io_state->input_state.tilt << 2)                   | // Tilt
                (bit_1_val << 1)                                    | // DIP5 01 = 4 ships  11 = 6 ships
                (bit_0_val << 0);                                   //  DIP3 00 = 3 ships  10 = 5 ships
            return read_byte;
        case 0x03: // PORT 3 READ SHIFT REGISTER DATA
            read_byte = DetermineOffsetShiftRegister(io_state->shift_register, io_state->shift_amount);
            return read_byte;
        default: // INVALID
            return 0x00;
    }
}

static void SpaceInvadersWrite(void *context, uint8_t port, uint8_t value) {
    SpaceInvadersIoState *io_state = context;

    switch(port) {
        case 0x02: // PORT 2
            return port;
        case 0x03: // PORT 3
            return port;
        case 0x04: // PORT 4
            return port;
        case 0x05: // PORT 5
            return port;
        case 0x06: // PORT 6
            return port;
        default:
            return NULL;
    }
    
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

    IoBus *io_bus = IoBusInit(&space_invaders_machine->io_state, SpaceInvadersRead, SpaceInvadersWrite);
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
