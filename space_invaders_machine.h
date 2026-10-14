#ifndef SPACE_INVADERS_MACHINE_H
#define SPACE_INVADERS_MACHINE_H

#include <stdint.h>
#include <stdbool.h>

#include "iobus.h"
#include "cpu.h"
#include "display.h"
#include "input_state.h"
#include "space_invaders_interrupt_scheduler.h"

typedef struct SpaceInvadersSoundLatches {
    bool ufo_effect;
    bool shot_effect;
    bool player_die_effect;
    bool invader_die_effect;
    bool extended_play_effect;
    bool amplifier_enable_effect;
    bool fleet_move_1_effect;
    bool fleet_move_2_effect;
    bool fleet_move_3_effect;
    bool fleet_move_4_effect;
    bool ufo_hit_effect;
} SpaceInvadersSoundLatches;

typedef struct SpaceInvadersDipSettings {
    bool        power_test;
    bool        coin_info;
    uint8_t     starting_num_lives;
    bool        bonus_life_value;
    
} SpaceInvadersDipSettings;

typedef struct SpaceInvadersIoState {
    InputState                  input_state;
    uint16_t                    shift_register;
    uint8_t                     shift_amount;
    SpaceInvadersDipSettings    dip_configuration;
    SpaceInvadersSoundLatches  sound_latches;
} SpaceInvadersIoState;

typedef struct SpaceInvadersMachine {
    IoBus                               *io_bus;
    Cpu8080                             *cpu;
    DisplayContext                      *display;
    uint32_t                            *frame_buffer;
    SpaceInvadersIoState                io_state;
    SpaceInvadersInterruptScheduler     interrupt_scheduler;
} SpaceInvadersMachine;

SpaceInvadersMachine *InitSpaceInvadersMachine(
    IoBus *io_bus,
    Cpu8080 *cpu,
    DisplayContext *display,
    uint32_t *frame_buffer,
    uint8_t starting_lives,
    bool bonus_life_value
);


#endif
