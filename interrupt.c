#include "state.h"

uint8_t InterruptIn(State8080 *state, uint8_t port) {
    return 0x00;
}

void InterruptOut(State8080 *state, uint8_t port) {
    return;
}
