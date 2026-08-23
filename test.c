#include <stdlib.h>
#include "disassembler.h"
#include "state.h"
#include "helper_funcs.h"

int main(int argc, char* argv[]) {
    // unsigned char test_rom[3] = {0x3A, 0x00, 0x20};

    long file_size = 0;
    unsigned char *buffer = get_file_contents(argv[1], &file_size);

    test_disassembler(file_size, buffer);

    State8080 *demo_state = create_test_state();
    unsigned char demo_command[3] = {0x01, 0x10, 0x23};

    test_state(demo_state, demo_command, 3);

    free_state(demo_state);
    free(buffer);
    return 0;
}
