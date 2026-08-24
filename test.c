#include <stdlib.h>
#include "disassembler.h"
#include "state.h"
#include "helper_funcs.h"

int main(int argc, char* argv[]) {
    // unsigned char test_rom[3] = {0x3A, 0x00, 0x20};

    long file_size = 0;
    unsigned char *buffer = GetFileContents(argv[1], &file_size);

    TestDisassembler(file_size, buffer);

    State8080 *demo_state = CreateTestState();
    unsigned char demo_command[3] = {0x01, 0x10, 0x23};

    TestState(demo_state, demo_command, 3);

    FreeState(demo_state);
    free(buffer);
    return 0;
}
