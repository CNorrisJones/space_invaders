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
uint8_t Parity(uint8_t input);
uint16_t GetMemoryAddress(uint8_t high, uint8_t low);
void HandleFlags(State8080 *state, uint16_t value);
void HandleFlagsNoCarryFlag(State8080 *state, uint16_t value);
void HandleCarryFlag(State8080 *state, uint16_t value);
void NopOpCode();
void AddOpCode(State8080 *state, uint8_t *added_register);
void AdcOpCode(State8080 *state, uint8_t *added_register);
void SubOpCode(State8080 *state, uint8_t *subtracted_register);
void SbbOpCode(State8080 *state, uint8_t *subtracted_register);
void AdiOpCode(State8080 *state, uint8_t added_byte);
void LxiOpCode(State8080 *state, uint8_t *lower_register, uint8_t *upper_register, uint8_t byte_2, uint8_t byte_3);
void LxiStackPointerOpCode(State8080 *state, uint8_t byte_2, uint8_t byte_3);
void MovOpCode(uint8_t *dest_register, uint8_t *send_register);
void MviOpCode(State8080 *state, uint8_t *dest_register, uint8_t byte_2);
void StaxOpCode(State8080 *state, uint8_t high_register, uint8_t low_register);
void InxOpCode(uint8_t *high_register, uint8_t *low_register);
void InxStackPointerOpCode(State8080 *state);
void InrOpCode(State8080 *state, uint8_t *reg);
void DcrOpCode(State8080 *state, uint8_t *reg);
void RlcOpCode(State8080 *state);
void DadOpCode(State8080 *state, uint16_t register_pair);
void LdaxOpCode(State8080 *state, uint16_t register_pair);
void DcxOpCode(uint8_t *high_register, uint8_t *low_register);
void DcxStackPointerOpCode(State8080 *state);
void RrcOpCode(State8080 *state);
void RalOpcode(State8080 *state);
void RarOpCode(State8080 *state);
int Emulate8080Op(State8080* state);
State8080 *CreateTestState();
void FreeState(State8080 *state);
void TestState(State8080* state, uint8_t *command, size_t command_len);

#endif
