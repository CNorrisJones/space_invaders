#include <stdio.h>
#include <stdbool.h>

#include "cpu.h"

int main(void) {
    Cpu8080 *test_cpu = CreateCpu(0x0000);
    if (test_cpu == NULL) {
        printf("CPU Interrupt Test Failure on CPU Init\n");
        return 1;
    }

    int cycle_test = 0;
    uint16_t test_vector = 0xDEAD;
    uint16_t current_pc = 0x1234;
    test_cpu->pc = current_pc;
    uint16_t current_sp = 0x4321;
    test_cpu->sp = current_sp;

    cycle_test = InjectHardwareInterrupt(NULL, test_vector);
    if (cycle_test != 0) {
        printf("Inject Null CPU Test Fail\n");
        return 1;
    }

    cycle_test = InjectHardwareInterrupt(test_cpu, test_vector);
    if (cycle_test != 0 ||
        test_cpu->int_enable != false ||
        test_cpu->pc != current_pc ||
        test_cpu->sp != current_sp
    ) {
        printf("Inject Test with Interrupt Disabled Fail\n");
        FreeCpu(test_cpu);
        return 1;
    }

    test_cpu->int_enable = true;
    cycle_test = InjectHardwareInterrupt(test_cpu, test_vector);
    if (cycle_test != 11 ||
        test_cpu->pc != test_vector ||
        test_cpu->memory[test_cpu->sp + 1] != 0x12 ||
        test_cpu->memory[test_cpu->sp] != 0x34 ||
        test_cpu->sp != current_sp - 2 ||
        test_cpu->int_enable != false
    ) {
        printf("Inject Test with Interrupt Accepted Fail\n");
        FreeCpu(test_cpu);
        return 1;
    }

    unsigned char test_op_ei[] = {0xFB};
    unsigned char test_op_nop[] = {0x00};

    LoadRomIntoCpu(test_cpu, test_op_ei, sizeof(test_op_ei));
    Emulate8080Op(test_cpu);

    if (test_cpu->ei_enable_pending != true || test_cpu->int_enable != false) {
        printf("Inject Test EI Op Code Set Pending Fail\n");
        FreeCpu(test_cpu);
        return 1;
    }

    cycle_test = InjectHardwareInterrupt(test_cpu, test_vector);
    if (cycle_test != 0) {
        printf("Inject Test EI Op Code Not Enabling Int Fail\n");
        FreeCpu(test_cpu);
        return 1;
    }

    LoadRomIntoCpu(test_cpu, test_op_nop, sizeof(test_op_nop));
    Emulate8080Op(test_cpu);

    if (test_cpu->ei_enable_pending != false || test_cpu->int_enable != true) {
        printf("Inject Test EI Pending Setting Interrupt Fail\n");
        FreeCpu(test_cpu);
        return 1;
    }

    cycle_test = InjectHardwareInterrupt(test_cpu, test_vector);
    if (cycle_test != 11) {
        printf("Inject Test EI Op Code Enabling Interrupt After Additional Op Failed\n");
        FreeCpu(test_cpu);
        return 1;
    }

    printf("CPU Interrupt Test Successful\n");
    FreeCpu(test_cpu);
    return 0;
}
