#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "iobus.h"

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
    TestHardware *test_hardware = malloc(sizeof(*test_hardware));
    if (test_hardware == NULL) {
        printf("Failure allocating test_hardware\n");
        return 1;
    }
    test_hardware->return_value = 0xA5;

    IoBus *fail_context_bus = IoBusInit(NULL, ReadTest, WriteTest);
    if (fail_context_bus != NULL) {
        printf("IoBus Init Did not NULL on invalid Hardware Context. Failure\n");
        free(test_hardware);
        return 1;
    } else {
        printf("Init Fails Properly on Null Hardware Context\n");
    }

    IoBus *fail_read_bus = IoBusInit(test_hardware, NULL, WriteTest);
    if (fail_read_bus != NULL) {
        printf("IoBus Init Did not NULL on invalid Read function. Failure\n");
        free(test_hardware);
        return 1;
    } else {
        printf("Init Fails Properly on Null Read Function\n");
    }

    IoBus *fail_write_bus = IoBusInit(test_hardware, ReadTest, NULL);
    if (fail_write_bus != NULL) {
        printf("IoBus Init Did not NULL on invalid Write function. Failure\n");
        free(test_hardware);
        return 1;
    } else {
        printf("Init Fails Properly on Null Write Function\n");
    }

    IoBus *test_bus = IoBusInit(test_hardware, ReadTest, WriteTest);
    if (test_bus == NULL) {
        printf("Failure setting up test_bus\n");
        free(test_hardware);
        return 1;
    }

    uint8_t read_val = test_bus->read(test_bus->hardware_context, 0x00);

    if (read_val == 0xFF) {
        printf("Read on Bad Port Success\n");
    } else {
        printf("Failure\n");
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    read_val = test_bus->read(test_bus->hardware_context, 0x01);

    if (read_val == 0xA5) {
        printf("Read on Good Port Success\n");
    } else {
        printf("Failure\n");
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    read_val = 0x33;
    test_bus->write(test_bus->hardware_context, 0x01, read_val);

    if (test_hardware->return_value == 0xA5) {
        printf("Write on Bad Port Success\n");
    } else {
        printf("Failure\n");
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    test_bus->write(test_bus->hardware_context, 0x02, read_val);

    if (test_hardware->return_value == read_val) {
        printf("Write on Good Port Success\n");
    } else {
        printf("Failure\n");
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    IoBusShutdown(test_bus);
    free(test_hardware);

    return 0;
}
