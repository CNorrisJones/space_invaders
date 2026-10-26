#ifndef INTERRUPT_SCHEDULER_H
#define INTERRUPT_SCHEDULER_H

#include <stdint.h>
#include <stdbool.h>

enum InterruptVectors {
    MID_VECTOR = 0x0008,
    END_VECTOR = 0x0010  
};

enum { CYCLE_INTERVAL = 17067 };

typedef struct InterruptScheduler {
    uint32_t    cycles_elapsed;
    uint8_t     next_vector;
    bool        int_request_pending;
    uint64_t    total_cycle_count;
} InterruptScheduler;

void CheckInterruptCycles(InterruptScheduler *scheduler, int cycles_added);
void UpdateSchedulerAfterInterrupt(InterruptScheduler *scheduler, int interrupt_cycles);

#endif
