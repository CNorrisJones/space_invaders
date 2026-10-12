#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "iobus.h"
#include "cpu.h"

typedef struct TestHardware {
    uint8_t return_value;
} TestHardware;

static uint8_t ReadTest(void *context, uint8_t port) {
    TestHardware *test_hardware = context;

    if (port == 0x01) {
        return test_hardware->return_value;
    } else {
        return 0xFF;
    }
}

static void WriteTest(void *context, uint8_t port, uint8_t value) {
    TestHardware *test_hardware = context;
    if (port == 0x02) {
        test_hardware->return_value = value;
    }
}

int main(void) {
    Cpu8080 *demo_state = CreateTestState(0x00);
    if (demo_state == NULL) {
        printf("Demo State Creation Failure\n");
        return 1;
    }

    TestHardware *test_hardware = malloc(sizeof(*test_hardware));
    if (test_hardware == NULL) {
        printf("Failure allocating test_hardware\n");
        FreeState(demo_state);
        return 1;
    }
    test_hardware->return_value = 0xA5;

    IoBus *test_bus = IoBusInit(test_hardware, ReadTest, WriteTest);
    if (test_bus == NULL) {
        printf("Failure setting up test_bus\n");
        FreeState(demo_state);
        free(test_hardware);
        return 1;
    }

    demo_state->io_bus = test_bus;

    uint8_t test_val = 0x00;
    uint8_t port_val = 0x00;

    test_val = demo_state->io_bus->read(demo_state->io_bus->hardware_context, port_val);

    if (test_val == 0xFF) {
        printf("Read Value Inccorect Port Success\n");
    } else {
        printf("Read Value Incorrect Port Failure\n");
        FreeState(demo_state);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    port_val = 0x01;

    test_val = demo_state->io_bus->read(demo_state->io_bus->hardware_context, port_val);

    if (test_val == 0xA5) {
        printf("Read Value Correct Port Success\n");
    } else {
        printf("Read Value Correct Port Failure\n");
        FreeState(demo_state);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    test_val = 0xAB;
    demo_state->io_bus->write(demo_state->io_bus->hardware_context, port_val, test_val);

    if (test_hardware->return_value == 0xA5) {
        printf("Write Value Incorrect Port Success\n");
    } else {
        printf("Write Value Incorrect Port Failure\n");
        FreeState(demo_state);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    port_val = 0x02;
    demo_state->io_bus->write(demo_state->io_bus->hardware_context, port_val, test_val);

    if (test_hardware->return_value == 0xAB) {
        printf("Write Value Correct Port Success\n");
    } else {
        printf("Write Value Correct Port Failure\n");
        FreeState(demo_state);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char read_op_code[] = {0xDB, 0x01};
    LoadRomIntoState(demo_state, read_op_code, 2);
    test_hardware->return_value = 0xCC;
    uint16_t pc_state = demo_state->pc;

    Emulate8080Op(demo_state);

    if (demo_state->a != 0xCC || demo_state->pc != (pc_state + 2)) {
        printf("Emulate IN Op Code Test Failure\n");
        FreeState(demo_state);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    } else {
        printf("Emulate IN Op Code Test Success\n");
    }

    unsigned char write_op_code[] = {0xD3, 0x02};
    demo_state->a = 0xDD;
    LoadRomIntoState(demo_state, write_op_code, 2);
    pc_state = demo_state->pc;

    Emulate8080Op(demo_state);

    if (test_hardware->return_value != 0xDD || demo_state->pc != (pc_state + 2)) {
        printf("Emulate OUT Op Code Test Failure\n");
        FreeState(demo_state);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    } else {
        printf("Emulate OUT Op Code Test Success\n");
    }

    demo_state->io_bus = NULL;
    LoadRomIntoState(demo_state, read_op_code, 2);
    pc_state = demo_state->pc;
    int check_read = demo_state->a;
    int check_fail = Emulate8080Op(demo_state);

    if (check_fail != -1 || pc_state != demo_state->pc || check_read != demo_state->a) {
        printf("NULL IoBus Failing on IN Op Code Test Failure\n");
        FreeState(demo_state);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    LoadRomIntoState(demo_state, write_op_code, 2);
    pc_state = demo_state->pc;
    int check_write = test_hardware->return_value;
    check_fail = Emulate8080Op(demo_state);

    if (check_fail != -1 || pc_state != demo_state->pc || check_write != test_hardware->return_value) {
        printf("NULL IoBus Failing on OUT Op Code Test Failure\n");
        FreeState(demo_state);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    printf("CPU IoBus Tests Successful\n");
    FreeState(demo_state);
    IoBusShutdown(test_bus);
    free(test_hardware);
    return 0;
}
