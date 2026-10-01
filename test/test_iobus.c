#include <stdint.h>

#include "iobus.h"

typedef struct TestHardware {
    uint8_t return_value;
} TestHardware;

uint8_t ReadTest(TestHardware *test_hardware, uint8_t port) {
    if (port == 0x01) {
        return 0xA5;
    } else {
        return 0xFF;
    }
}

void WriteTest(TestHardware *test_hardware, uint8_t port, uint8_t value) {
    if (port == 0x02) {
        test_hardware->return_value = value;
    }

    return;
}

int main(void) {
    TestHardware *test_hardware = malloc(sizeof(TestHardware));
    test_hardware->return_value = 0x00;

    IoBus *test_bus = IoBusInit(test_hardware, ReadTest, WriteTest);

    
}