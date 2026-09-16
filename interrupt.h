#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdint.h>
#include "state.h"

uint8_t InterruptIn(State8080 *state, uint8_t port);
void InterruptOut(State8080 *state, uint8_t port);

#endif