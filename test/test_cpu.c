#include <stdlib.h>
#include <stdio.h>
#include "disassembler.h"
#include "cpu.h"
#include "helper_funcs.h"

int main() {
    char DIAGNOSTIC_FILE[] = "./rom/cpudiag.bin";
    is_test_cpu = 1;

    long file_size = 0;
    unsigned char *buffer = GetFileContents(DIAGNOSTIC_FILE, &file_size);
    const uint16_t MEM_START = 0x100;

    Cpu8080 *demo_state = CreateTestState(MEM_START);
    LoadRomIntoState(demo_state, buffer, file_size);

    while(1) {
        Emulate8080Op(demo_state);
    }

    return 0;
}
