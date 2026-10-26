#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "space_invaders_machine.h"
#include "helper_funcs.h"

static const int CPU_ERR = -1;
static const int MACHINE_FAILURE = 1;

int main(void) {
    SpaceInvadersMachine *machine = InitSpaceInvadersMachine(SPACE_INVADERS_LIVES_3, false);
    if (machine == NULL) {
        printf("Error Generating Space Invaders Machine. Closing\n");
        return MACHINE_FAILURE;
    }

    char rom_file[] = "./rom/invaders";
    long file_size = 0;
    unsigned char *rom_buffer = GetFileContents(rom_file, &file_size);
    if (rom_buffer == NULL) {
        printf("Error Setting Space Invaders Rom into Memory. Closing\n");
        SpaceInvadersMachineShutdown(machine);
        return MACHINE_FAILURE;
    }

    LoadRomIntoCpu(machine->cpu, rom_buffer, file_size);
    int cpu_op_cycles;
    int inject_cycles;

    int frame_status = DisplayFrame(machine->display, machine->frame_buffer);
        if (frame_status != 0) {
            printf("Error Initializing the Graphics Window\n");
            free(rom_buffer);
            SpaceInvadersMachineShutdown(machine);
            return MACHINE_FAILURE;
        }

    int quit = 0;

    while(!quit) {
        quit = DisplayPollEvents(&machine->io_state.input_state);
        if (quit == DISPLAY_QUIT) {
            break;
        }

        cpu_op_cycles = Emulate8080Op(machine->cpu);
        if (cpu_op_cycles == CPU_ERR) {
            printf("Error occured during CPU Run\n");
            free(rom_buffer);
            SpaceInvadersMachineShutdown(machine);
            exit(MACHINE_FAILURE);
        }

        CheckInterruptCycles(&machine->interrupt_scheduler, cpu_op_cycles);

        if (machine->interrupt_scheduler.int_request_pending) {
            inject_cycles = InjectHardwareInterrupt(machine->cpu, machine->interrupt_scheduler.next_vector);
            UpdateSchedulerAfterInterrupt(&machine->interrupt_scheduler, inject_cycles);
        }
    }

    printf("Shutting Down Machine\n");
    free(rom_buffer);
    SpaceInvadersMachineShutdown(machine);
    return 0;
}
