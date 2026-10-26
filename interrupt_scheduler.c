#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "interrupt_scheduler.h"

static void UpdateInterruptVector(InterruptScheduler *scheduler) {
    if (scheduler == NULL) {
        return;
    }

    switch(scheduler->next_vector) {
        case MID_VECTOR:
            scheduler->next_vector = END_VECTOR;
            break;
        case END_VECTOR:
            scheduler->next_vector = MID_VECTOR;
            break;
        default:
            break;
    }
}

void CheckInterruptCycles(InterruptScheduler *scheduler, int cycles_added) {
    if (scheduler == NULL) {
        return;
    }

    scheduler->cycles_elapsed += cycles_added;
    scheduler->total_cycle_count += cycles_added;

    if (scheduler->cycles_elapsed >= CYCLE_INTERVAL) {
        scheduler->int_request_pending = true;
        scheduler->cycles_elapsed -= CYCLE_INTERVAL;

        return;
    }
}

void UpdateSchedulerAfterInterrupt(InterruptScheduler *scheduler, int interrupt_cycles) {
    if (scheduler == NULL) {
        return;
    }

    if (interrupt_cycles > 0 && scheduler->int_request_pending == true) {
        scheduler->int_request_pending = false;
        UpdateInterruptVector(scheduler);
        CheckInterruptCycles(scheduler, interrupt_cycles);
    }
}
