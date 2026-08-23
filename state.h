#ifndef STATE_H
#define STATE_H

#include <stdint.h>

typedef struct ConditionCodes {    
    uint8_t    z:1;    
    uint8_t    s:1;    
    uint8_t    p:1;    
    uint8_t    cy:1;    
    uint8_t    ac:1;    
    uint8_t    pad:3;    
} ConditionCodes;

typedef struct State8080 {    
    uint8_t     a;    
    uint8_t     b;    
    uint8_t     c;    
    uint8_t     d;    
    uint8_t     e;    
    uint8_t     h;    
    uint8_t     l;    
    uint16_t    sp;    
    uint16_t    pc;    
    uint8_t     *memory;    
    struct      ConditionCodes      cc;    
    uint8_t     int_enable;    
} State8080;

void UnimplementedInstruction(State8080 *state);
int Parity(uint16_t input);
void nop_op_code();
void add_op_code(State8080 *state, uint8_t *added_register);
void add_memory_op_code(State8080 *state);
void adi_op_code(State8080 *state, uint8_t added_byte);
void lxi_op_code(State8080 *state, uint8_t *lower_register, uint8_t *upper_register, uint8_t byte_1, uint8_t byte_2);
int Emulate8080Op(State8080* state);
State8080 *create_test_state();
void free_state(State8080 *state);
void test_state(State8080* state, uint8_t *command, size_t command_len);

#endif
