#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "iobus.h"
#include "cpu.h"
#include "display.h"
#include "input_state.h"
#include "space_invaders_interrupt_scheduler.h"
#include "space_invaders_machine.h"

SpaceInvadersMachine *InitSpaceInvadersMachine(
    IoBus *io_bus,
    Cpu8080 *cpu,
    DisplayContext *display,
    uint32_t *frame_buffer,
    uint8_t starting_lives,
    bool bonus_life_value
) {
    SpaceInvadersMachine *space_invaders_machine = malloc(sizeof(SpaceInvadersMachine));
    if (space_invaders_machine == NULL) {
        return NULL;
    }

    if (cpu == NULL || io_bus == NULL || display == NULL || frame_buffer == NULL) {
        free(space_invaders_machine);
        return NULL;
    }

    space_invaders_machine->cpu = cpu;
    space_invaders_machine->io_bus = io_bus;
    space_invaders_machine->display = display;
    space_invaders_machine->frame_buffer = frame_buffer;

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

    return space_invaders_machine;
}
