#ifndef SPACE_INVADERS_INTERRUPT_SCHEDULER_H
#define SPACE_INVADERS_INTERRUPT_SCHEDULER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct SpaceInvadersInterruptScheduler {
    uint32_t    cycles_elapsed;
    uint8_t     next_vector;
    bool        int_request_pending;
    uint64_t    total_cycle_count;
} SpaceInvadersInterruptScheduler;

#endif
