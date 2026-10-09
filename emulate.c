#include <stdlib.h>
#include <stdio.h>
#include "disassembler.h"
#include "state.h"
#include "helper_funcs.h"

int main(__attribute__((unused)) int argc, char* argv[]) {
    long file_size = 0;
    int counter = 0;
    unsigned char *buffer = GetFileContents(argv[1], &file_size);
    const uint16_t MEM_START = 0x0;

    TestDisassembler(file_size, buffer);

    State8080 *demo_state = CreateTestState(MEM_START);
    LoadRomIntoState(demo_state, buffer, file_size);
    int emu_return_code;

    while(1) {
        if (counter == 37410) {
            printf("Debug statement here");
        }

        Disassemble8080Code(demo_state->memory, demo_state->pc);
        emu_return_code = Emulate8080Op(demo_state);
        if (emu_return_code == -1) {
            printf("CPU STATE FAILURE RETURNED\n");
            FreeState(demo_state);
            free(buffer);
            return 1;
        }

        counter += 1;
        printf("\tCY=%d,P=%d,S=%d,Z=%d,AC=%d\n", demo_state->cc.cy, demo_state->cc.p, demo_state->cc.s, demo_state->cc.z, demo_state->cc.ac);
        printf("\tA $%02x B $%02x C $%02x D $%02x E $%02x H $%02x L $%02x SP %04x\n", demo_state->a, demo_state->b, demo_state->c, demo_state->d, demo_state->e, demo_state->h, demo_state->l, demo_state->sp);
        printf("\tCounter is: %d\n", counter); 
    }

    FreeState(demo_state);
    free(buffer);
    return 0;
}
