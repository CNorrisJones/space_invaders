#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "interrupt_scheduler.h"

int main(void) {
    InterruptScheduler scheduler = {0};
    scheduler.next_vector = MID_VECTOR;

    CheckInterruptCycles(&scheduler, (CYCLE_INTERVAL - 1));
    if (scheduler.cycles_elapsed != (CYCLE_INTERVAL - 1) || scheduler.int_request_pending != false || scheduler.next_vector != MID_VECTOR) {
        printf("Interrupt Scheduler Test Failed\n");
        printf("Test Failure: Non Interval-Exceeding Cycles Added\n");
        return 1;
    }

    CheckInterruptCycles(&scheduler, 1);
    if (scheduler.cycles_elapsed != 0 || scheduler.int_request_pending != true || scheduler.next_vector != MID_VECTOR) {
        printf("Interrupt Scheduler Test Failed\n");
        printf("Test Failure: Interval-Exceeding Cycle Added\n");
        return 1;
    }

    UpdateSchedulerAfterInterrupt(NULL, 0);
    if (scheduler.cycles_elapsed != 0 || scheduler.int_request_pending != true || scheduler.next_vector != MID_VECTOR) {
        printf("Interrupt Scheduler Test Failed\n");
        printf("Test Failure: Null After Interrupt Check\n");
        return 1;
    }

    UpdateSchedulerAfterInterrupt(&scheduler, 0);
    if (scheduler.cycles_elapsed != 0 || scheduler.int_request_pending != true || scheduler.next_vector != MID_VECTOR) {
        printf("Interrupt Scheduler Test Failed\n");
        printf("Test Failure: Update Scheduler 0 Cycle Check\n");
        return 1;
    }

    UpdateSchedulerAfterInterrupt(&scheduler, 11);
    if (scheduler.cycles_elapsed != 11 || scheduler.total_cycle_count != (CYCLE_INTERVAL + 11) || scheduler.int_request_pending != false || scheduler.next_vector != END_VECTOR) {
        printf("Interrupt Scheduler Test Failed\n");
        printf("Test Failure: Update Scheduler 11 Cycle First Check\n");
        return 1;
    }

    scheduler.int_request_pending = true;
    UpdateSchedulerAfterInterrupt(&scheduler, 11);
    if (scheduler.cycles_elapsed != 22 || scheduler.total_cycle_count != (CYCLE_INTERVAL + 22) || scheduler.int_request_pending != false || scheduler.next_vector != MID_VECTOR) {
        printf("Interrupt Scheduler Test Failed\n");
        printf("Test Failure: Update Scheduler 22 Cycle Second Check\n");
        return 1;
    }

    scheduler.cycles_elapsed = (CYCLE_INTERVAL - 11);
    scheduler.total_cycle_count = (CYCLE_INTERVAL - 11);
    scheduler.int_request_pending = true;

    UpdateSchedulerAfterInterrupt(&scheduler, 11);
    if (scheduler.cycles_elapsed != 0 || scheduler.total_cycle_count != CYCLE_INTERVAL || scheduler.int_request_pending != true || scheduler.next_vector != END_VECTOR) {
        printf("Interrupt Scheduler Test Interval Overflow Check Failed\n");
        printf("Test Failure: Update Scheduler 11 Cycle Second Check\n");
        return 1;
    }

    printf("Interrupt Scheduler Test Successful\n");
    return 0;
}