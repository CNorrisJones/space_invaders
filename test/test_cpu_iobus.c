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
    Cpu8080 *demo_cpu = CreateCpu(0x00);
    if (demo_cpu == NULL) {
        printf("Demo CPU Creation Failure\n");
        return 1;
    }

    TestHardware *test_hardware = malloc(sizeof(*test_hardware));
    if (test_hardware == NULL) {
        printf("Failure allocating test_hardware\n");
        FreeCpu(demo_cpu);
        return 1;
    }
    test_hardware->return_value = 0xA5;

    IoBus *test_bus = IoBusInit(test_hardware, ReadTest, WriteTest);
    if (test_bus == NULL) {
        printf("Failure setting up test_bus\n");
        FreeCpu(demo_cpu);
        free(test_hardware);
        return 1;
    }

    demo_cpu->io_bus = test_bus;

    uint8_t test_val = 0x00;
    uint8_t port_val = 0x00;

    test_val = demo_cpu->io_bus->read(demo_cpu->io_bus->hardware_context, port_val);

    if (test_val == 0xFF) {
        printf("Read Value Inccorect Port Success\n");
    } else {
        printf("Read Value Incorrect Port Failure\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    port_val = 0x01;

    test_val = demo_cpu->io_bus->read(demo_cpu->io_bus->hardware_context, port_val);

    if (test_val == 0xA5) {
        printf("Read Value Correct Port Success\n");
    } else {
        printf("Read Value Correct Port Failure\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    test_val = 0xAB;
    demo_cpu->io_bus->write(demo_cpu->io_bus->hardware_context, port_val, test_val);

    if (test_hardware->return_value == 0xA5) {
        printf("Write Value Incorrect Port Success\n");
    } else {
        printf("Write Value Incorrect Port Failure\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    port_val = 0x02;
    demo_cpu->io_bus->write(demo_cpu->io_bus->hardware_context, port_val, test_val);

    if (test_hardware->return_value == 0xAB) {
        printf("Write Value Correct Port Success\n");
    } else {
        printf("Write Value Correct Port Failure\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char read_op_code[] = {0xDB, 0x01};
    LoadRomIntoCpu(demo_cpu, read_op_code, 2);
    test_hardware->return_value = 0xCC;
    uint16_t prog_counter = demo_cpu->pc;

    Emulate8080Op(demo_cpu);

    if (demo_cpu->a != 0xCC || demo_cpu->pc != (prog_counter + 2)) {
        printf("Emulate IN Op Code Test Failure\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    } else {
        printf("Emulate IN Op Code Test Success\n");
    }

    unsigned char write_op_code[] = {0xD3, 0x02};
    demo_cpu->a = 0xDD;
    LoadRomIntoCpu(demo_cpu, write_op_code, 2);
    prog_counter = demo_cpu->pc;

    Emulate8080Op(demo_cpu);

    if (test_hardware->return_value != 0xDD || demo_cpu->pc != (prog_counter + 2)) {
        printf("Emulate OUT Op Code Test Failure\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    } else {
        printf("Emulate OUT Op Code Test Success\n");
    }

    demo_cpu->io_bus = NULL;
    LoadRomIntoCpu(demo_cpu, read_op_code, 2);
    prog_counter = demo_cpu->pc;
    int check_read = demo_cpu->a;
    int check_fail = Emulate8080Op(demo_cpu);

    if (check_fail != -1 || prog_counter != demo_cpu->pc || check_read != demo_cpu->a) {
        printf("NULL IoBus Failing on IN Op Code Test Failure\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    LoadRomIntoCpu(demo_cpu, write_op_code, 2);
    prog_counter = demo_cpu->pc;
    int check_write = test_hardware->return_value;
    check_fail = Emulate8080Op(demo_cpu);

    if (check_fail != -1 || prog_counter != demo_cpu->pc || check_write != test_hardware->return_value) {
        printf("NULL IoBus Failing on OUT Op Code Test Failure\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    printf("CPU IoBus Tests Successful\n");
    FreeCpu(demo_cpu);
    IoBusShutdown(test_bus);
    free(test_hardware);
    return 0;
}
