#include <stdlib.h>
#include <stdio.h>
#include "disassembler.h"
#include "cpu.h"
#include "helper_funcs.h"

int main(__attribute__((unused)) int argc, char* argv[]) {
    long file_size = 0;
    int counter = 0;
    unsigned char *buffer = GetFileContents(argv[1], &file_size);
    const uint16_t MEM_START = 0x0;

    TestDisassembler(file_size, buffer);

    Cpu8080 *demo_cpu = CreateTestCpu(MEM_START);
    LoadRomIntoCpu(demo_cpu, buffer, file_size);
    int emu_return_code;

    while(1) {
        if (counter == 37410) {
            printf("Debug statement here");
        }

        Disassemble8080Code(demo_cpu->memory, demo_cpu->pc);
        emu_return_code = Emulate8080Op(demo_cpu);
        if (emu_return_code == -1) {
            printf("CPU STATE FAILURE RETURNED\n");
            FreeCpu(demo_cpu);
            free(buffer);
            return 1;
        }

        counter += 1;
        printf("\tCY=%d,P=%d,S=%d,Z=%d,AC=%d\n", demo_cpu->cc.cy, demo_cpu->cc.p, demo_cpu->cc.s, demo_cpu->cc.z, demo_cpu->cc.ac);
        printf("\tA $%02x B $%02x C $%02x D $%02x E $%02x H $%02x L $%02x SP %04x\n", demo_cpu->a, demo_cpu->b, demo_cpu->c, demo_cpu->d, demo_cpu->e, demo_cpu->h, demo_cpu->l, demo_cpu->sp);
        printf("\tCounter is: %d\n", counter); 
    }

    FreCpu(demo_cpu);
    free(buffer);
    return 0;
}
