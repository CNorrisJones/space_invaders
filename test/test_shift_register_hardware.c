#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "space_invaders_machine.h"

int main(void) {
    bool new_life_at_1000 = true;
    SpaceInvadersStartingLives three_lives = SPACE_INVADERS_LIVES_3;
    SpaceInvadersMachine *test_machine = InitSpaceInvadersMachine(three_lives, new_life_at_1000);
    if (test_machine == NULL) {
        printf("Shift Register Test Failure: Failed to Generate Machine\n");
        return 1;
    }

    if (test_machine->io_state.shift_register != 0x0000) {
        printf("Shift Register Hardware Failure\n");
        printf("Initial Value of Shift Register Wrong\n");
        SpaceInvadersMachineShutdown(test_machine);
        return 1;
    } else {
        printf("Initial Value of Shift Register Correct\n");
    }

    if (test_machine->io_state.shift_amount != 0x00) {
        printf("Shift Register Hardware Failure\n");
        printf("Initial Value of Shift Amount Wrong\n");
        SpaceInvadersMachineShutdown(test_machine);
        return 1;
    } else {
        printf("Initial Value of Shift Amount Correct\n");
    }

    test_machine->io_bus->write(test_machine->io_bus->hardware_context, 0x02, 0xFD);
    if (test_machine->io_state.shift_amount != 0x05) {
        printf("Shift Register Hardware Failure\n");
        printf("Port 2 Write Masking Test Failure\n");
        SpaceInvadersMachineShutdown(test_machine);
        return 1;
    } else {
        printf("Port 2 Write Masking Test Success\n");
    }

    test_machine->io_bus->write(test_machine->io_bus->hardware_context, 0x04, 0xAA);
    if (test_machine->io_state.shift_register != 0xAA00) {
        printf("Shift Register Hardware Failure\n");
        printf("Port 4 Write 1 Failure\n");
        SpaceInvadersMachineShutdown(test_machine);
        return 1;
    } else {
        printf("Port 4 Write 1 Test Success\n");
    }

    test_machine->io_bus->write(test_machine->io_bus->hardware_context, 0x04, 0xFF);
    if (test_machine->io_state.shift_register != 0xFFAA) {
        printf("Shift Register Hardware Failure\n");
        printf("Port 4 Write 2 Failure\n");
        SpaceInvadersMachineShutdown(test_machine);
        return 1;
    } else {
        printf("Port 4 Write 2 Test Success\n");
    }

    test_machine->io_bus->write(test_machine->io_bus->hardware_context, 0x04, 0x12);
    if (test_machine->io_state.shift_register != 0x12FF) {
        printf("Shift Register Hardware Failure\n");
        printf("Port 4 Write 3 Failure\n");
        SpaceInvadersMachineShutdown(test_machine);
        return 1;
    } else {
        printf("Port 4 Write 3 Test Success\n");
    }

    test_machine->io_bus->write(test_machine->io_bus->hardware_context, 0x04, 0x5A);
    test_machine->io_bus->write(test_machine->io_bus->hardware_context, 0x04, 0xA5);
    uint8_t read_test_val;

    test_machine->io_bus->write(test_machine->io_bus->hardware_context, 0x02, 0x00);
    read_test_val = test_machine->io_bus->read(test_machine->io_bus->hardware_context, 0x03);
    if (read_test_val != 0xA5) {
        printf("Shift Register Hardware Failure\n");
        printf("Port 3 Read Offset 0 Failure\n");
        SpaceInvadersMachineShutdown(test_machine);
        return 1;
    } else {
        printf("Port 3 Read Offset 0 Success\n");
    }

    test_machine->io_bus->write(test_machine->io_bus->hardware_context, 0x02, 0x01);
    read_test_val = test_machine->io_bus->read(test_machine->io_bus->hardware_context, 0x03);
    if (read_test_val != 0x4A) {
        printf("Shift Register Hardware Failure\n");
        printf("Port 3 Read Offset 1 Failure\n");
        SpaceInvadersMachineShutdown(test_machine);
        return 1;
    } else {
        printf("Port 3 Read Offset 1 Success\n");
    }

    test_machine->io_bus->write(test_machine->io_bus->hardware_context, 0x02, 0x02);
    read_test_val = test_machine->io_bus->read(test_machine->io_bus->hardware_context, 0x03);
    if (read_test_val != 0x95) {
        printf("Shift Register Hardware Failure\n");
        printf("Port 3 Read Offset 2 Failure\n");
        SpaceInvadersMachineShutdown(test_machine);
        return 1;
    } else {
        printf("Port 3 Read Offset 2 Success\n");
    }

    test_machine->io_bus->write(test_machine->io_bus->hardware_context, 0x02, 0x03);
    read_test_val = test_machine->io_bus->read(test_machine->io_bus->hardware_context, 0x03);
    if (read_test_val != 0x2A) {
        printf("Shift Register Hardware Failure\n");
        printf("Port 3 Read Offset 3 Failure\n");
        SpaceInvadersMachineShutdown(test_machine);
        return 1;
    } else {
        printf("Port 3 Read Offset 3 Success\n");
    }

    test_machine->io_bus->write(test_machine->io_bus->hardware_context, 0x02, 0x04);
    read_test_val = test_machine->io_bus->read(test_machine->io_bus->hardware_context, 0x03);
    if (read_test_val != 0x55) {
        printf("Shift Register Hardware Failure\n");
        printf("Port 3 Read Offset 4 Failure\n");
        SpaceInvadersMachineShutdown(test_machine);
        return 1;
    } else {
        printf("Port 3 Read Offset 4 Success\n");
    }

    test_machine->io_bus->write(test_machine->io_bus->hardware_context, 0x02, 0x05);
    read_test_val = test_machine->io_bus->read(test_machine->io_bus->hardware_context, 0x03);
    if (read_test_val != 0xAB) {
        printf("Shift Register Hardware Failure\n");
        printf("Port 3 Read Offset 5 Failure\n");
        SpaceInvadersMachineShutdown(test_machine);
        return 1;
    } else {
        printf("Port 3 Read Offset 5 Success\n");
    }

    test_machine->io_bus->write(test_machine->io_bus->hardware_context, 0x02, 0x06);
    read_test_val = test_machine->io_bus->read(test_machine->io_bus->hardware_context, 0x03);
    if (read_test_val != 0x56) {
        printf("Shift Register Hardware Failure\n");
        printf("Port 3 Read Offset 6 Failure\n");
        SpaceInvadersMachineShutdown(test_machine);
        return 1;
    } else {
        printf("Port 3 Read Offset 6 Success\n");
    }

    test_machine->io_bus->write(test_machine->io_bus->hardware_context, 0x02, 0x07);
    read_test_val = test_machine->io_bus->read(test_machine->io_bus->hardware_context, 0x03);
    if (read_test_val != 0xAD) {
        printf("Shift Register Hardware Failure\n");
        printf("Port 3 Read Offset 7 Failure\n");
        SpaceInvadersMachineShutdown(test_machine);
        return 1;
    } else {
        printf("Port 3 Read Offset 7 Success\n");
    }

    printf("Shift Register Hardware Test Successful\n");
    SpaceInvadersMachineShutdown(test_machine);
    return 0;
}
