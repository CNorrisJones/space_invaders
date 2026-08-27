#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "state.h"

void UnimplementedInstruction(State8080 *state) {
    //pc will have advanced one, so undo that    
    printf ("Error: Unimplemented instruction\n");    
    exit(1);    
}

uint8_t Parity(uint8_t input) {
    uint8_t bit_count = 0;

    for (uint8_t i = 0; i < 8; i++) {
        if ((input >> i) & 0x01) {
            bit_count++;
        }
    }
    
    return (bit_count % 2 == 0);
}

uint16_t GetMemoryAddress(uint8_t high, uint8_t low) {
    uint16_t address = ((uint16_t)high << 8) | low;
    return address;
}

void HandleFlags(State8080 *state, uint16_t value) {
    const uint8_t UPPER_BIT_MASK = 0xff;
    const uint8_t SIGN_BIT_MASK = 0x80;

    state->cc.z = ((value & UPPER_BIT_MASK) == 0); // Check Zero Flag
    state->cc.s = ((value & SIGN_BIT_MASK) != 0);  // Check Sign Flag
    state->cc.cy = (value > UPPER_BIT_MASK);       // Check Carry Flag
    state->cc.p = Parity((uint8_t)(value & UPPER_BIT_MASK));  // Check Parity Flag
    return;
}

void HandleFlagsNoCarryFlag(State8080 *state, uint16_t value) {
    const uint8_t UPPER_BIT_MASK = 0xff;
    const uint8_t SIGN_BIT_MASK = 0x80;

    state->cc.z = ((value & UPPER_BIT_MASK) == 0); // Check Zero Flag
    state->cc.s = ((value & SIGN_BIT_MASK) != 0);  // Check Sign Flag
    state->cc.p = Parity((uint8_t)(value & UPPER_BIT_MASK));  // Check Parity Flag
    return;
}

void HandleCarryFlag(State8080 *state, uint16_t value) {
    state->cc.cy = (value > 0xff);       // Check Carry Flag
    return;
}

// NOP: Do Nothing
// Flags: None
// Op Size: 1
void NopOpCode() {
    return;
}

// ADD: A <- A + (added_register)
// Flags: Z, S, P, CY, AC
// Op Size: 1
void AddOpCode(State8080 *state, uint8_t *added_register) { 
    // Cast to 16-bit to more easily check for a carry
    uint16_t answer = (uint16_t) state->a + (uint16_t) *added_register;    

    HandleFlags(state, answer);
    state->a = (uint8_t)(answer & 0xff);
    return;
}

// ADC: A <- A + (added_register) + CY
// Flags: Z, S, P, CY, AC
// Op Size: 1
void AdcOpCode(State8080 *state, uint8_t *added_register) {
    // Cast to 16-bit to more easily check for a carry
    uint16_t answer = (uint16_t) state->a + (uint16_t) *added_register + (uint16_t) state->cc.cy;    

    HandleFlags(state, answer);
    state->a = (uint8_t)(answer & 0xff);
    return;
}

// SUB: A <- A - (subtracted_register)
// Flags: Z, S, P, CY, AC
// Op Size: 1
void SubOpCode(State8080 *state, uint8_t *subtracted_register) {
    // Cast to 16-bit to more easily check for a carry
    uint16_t answer = (uint16_t) state->a - (uint16_t) *subtracted_register;

    HandleFlags(state, answer);
    state->a = (uint8_t)(answer & 0xff);
    return;
}

// SBB: A <- A - (subtracted_register) - CY
// Flags: Z, S, P, CY, AC
// Op Size: 1
void SbbOpCode(State8080 *state, uint8_t *subtracted_register) {
    // Cast to 16-bit to more easily check for a carry
    uint16_t answer = (uint16_t) state->a - (uint16_t) *subtracted_register - (uint16_t) state->cc.cy;

    HandleFlags(state, answer);
    state->a = (uint8_t)(answer & 0xff);
    return;
}

// ADI: A <- A + added_byte
// Flags: Z, S, P, CY, AC
// Op Size: 2
void AdiOpCode(State8080 *state, uint8_t added_byte) {
    // Cast to 16-bit to more easily check for a carry
    uint16_t answer = (uint16_t) state->a + (uint16_t) added_byte;    

    HandleFlags(state, answer);
    state->a = (uint8_t)(answer & 0xff);
    state->pc += 1;
    return;
}

// LXI: lower_register <- byte 3, upper_register <- byte 2
// Flags: None
// Op Size: 3
void LxiOpCode(State8080 *state, uint8_t *lower_register, uint8_t *upper_register, uint8_t byte_2, uint8_t byte_3) {
    *lower_register = byte_3;
    *upper_register = byte_2;
    state->pc += 2;
    return;
}

// LXI: SP.hi <- byte 3, SP.lo <- byte 2
// Flags: None
// Op Size: 3
void LxiStackPointerOpCode(State8080 *state, uint8_t byte_2, uint8_t byte_3) {
    uint16_t address = GetMemoryAddress(byte_3, byte_2);
    state->sp = address;
    state->pc += 2;
    return;
}

// MOV: Dest <- Send
// Flags: None
// Op Size: 1
void MovOpCode(uint8_t *dest_register, uint8_t *send_register) {
    *dest_register = *send_register;
    return;
}

// MVI: dest_register <- byte 2
// Flags: None
// Op Size: 2
void MviOpCode(State8080 *state, uint8_t *dest_register, uint8_t byte_2) {
    *dest_register = byte_2;
    state->pc += 1;
    return;
}

// STAX: ([high_register][low_register]) <- A
// Flags: None
// Op Size: 1
void StaxOpCode(State8080 *state, uint8_t high_register, uint8_t low_register) {
    uint16_t address = GetMemoryAddress(high_register, low_register);

    state->memory[address] = state->a;
    return;
}

// INX: [Register Pair] <- [Register Pair] + 1
// Flags: None
// Op Size: 1
void InxOpCode(uint8_t *high_register, uint8_t *low_register) {
    uint16_t address = GetMemoryAddress(*high_register, *low_register);
    address += 1;

    *high_register = (address >> 8) & 0xFF;
    *low_register = (address & 0xFF);
    return;
}

// INX: Stack Pointer = Stack Pointer + 1
// Flags: None
// Op Size: 1
void InxStackPointerOpCode(State8080 *state) {
    state->sp += 1;
    return;
}

// INR: Register <- Register + 1
// Flags: Z, S, P, AC
// Op Size: 1
void InrOpCode(State8080 *state, uint8_t *reg) {
    uint16_t answer = (uint16_t) *reg + 1;

    HandleFlagsNoCarryFlag(state, answer);
    *reg = answer;
    return;
}

// DCR: Register <- Register - 1
// Flags: Z, S, P, AC
// Op Size: 1
void DcrOpCode(State8080 *state, uint8_t *reg) {
    uint16_t answer = (uint16_t) *reg - 1;

    HandleFlagsNoCarryFlag(state, answer);
    *reg = answer;
    return;
}

// RLC: A = A << 1; bit 0 = prev bit 7; CY = prev bit 7
// Flags: CY
// Op Size: 1
void RlcOpCode(State8080 *state) {
    uint8_t value = state->a;
    uint8_t bit_seven_value = value >> 7;

    value = value << 1;
    state->cc.cy = bit_seven_value;
    value = value | bit_seven_value;

    state->a = value;
    return;
}

// DAD: HL = HL + (Register Pair)
// Flags: CY
// Op Size: 1
void DadOpCode(State8080 *state, uint16_t register_pair) {
    uint32_t hl_address = (uint32_t) GetMemoryAddress(state->h, state->l);

    hl_address = hl_address + (uint32_t) register_pair;
    state->cc.cy = (hl_address > 0xFFFF);

    uint8_t final_upper_register = (uint8_t) ((hl_address & 0xFFFF) >> 8);
    uint8_t final_lower_register = (uint8_t) (hl_address & 0xFF);

    state->h = final_upper_register;
    state->l = final_lower_register;
    
    return;
}

// LDAX: A <- (Register Pair)
// Flags: None
// Op Size: 1
void LdaxOpCode(State8080 *state, uint16_t register_pair) {
    state->a = state->memory[register_pair];
    return;
}

// DCX: [Register Pair] = [Register Pair] - 1
// Flags: None
// Op Size: 1
void DcxOpCode(uint8_t *high_register, uint8_t *low_register) {
    uint16_t address = GetMemoryAddress(*high_register, *low_register);
    address -= 1;

    *high_register = (address >> 8) & 0xFF;
    *low_register = (address & 0xFF);
    return;
}

// DCX: Stack Pointer = Stack Pointer - 1
// Flags: None
// Op Size: 1
void DcxStackPointerOpCode(State8080 *state) {
    state->sp -= 1;
    return;
}

// RRC: A = A >> 1; bit 7 = prev bit 0; CY = prev bit 0
// Flags: CY
// Op Size: 1
void RrcOpCode(State8080 *state) {
    uint8_t value = state->a;
    uint8_t bit_zero_value = value & 0x1;

    value = value >> 1;
    state->cc.cy = bit_zero_value;
    value = value | (uint8_t) (bit_zero_value << 7);

    state->a = value;
    return;
}

// RAL: A = A << 1; bit 0 = prev CY; CY = prev bit 7
// Flags: CY
// Op Size: 1
void RalOpcode(State8080 *state) {
    uint8_t value = state->a;
    uint8_t bit_seven_value = value >> 7;

    value = value << 1;
    value = value | state->cc.cy;
    state->cc.cy = bit_seven_value;

    state->a = value;
    return;
}

// RAR: A = A >> 1; bit 7 = prev CY; CY = prev bit 0
// Flags: CY
// Op Size: 1
void RarOpCode(State8080 *state) {
    uint8_t value = state->a;
    uint8_t bit_zero_value = value & 0x1;

    value = value >> 1;
    value = value | (uint8_t) (state->cc.cy << 7);
    state->cc.cy = bit_zero_value;

    state->a = value;
    return;
}

int Emulate8080Op(State8080* state) {
    unsigned char *opcode = &state->memory[state->pc];

    switch(*opcode) {
        case 0x00: // NOP
            NopOpCode();
            break;
        case 0x01: // LXI B D16
            LxiOpCode(state, &state->b, &state->c, opcode[1], opcode[2]);
            break;
        case 0x02: // STAX B
            StaxOpCode(state, state->b, state->c);
            break;
        case 0x03: // INX B
            InxOpCode(&state->b, &state->c);
            break;
        case 0x04: // INR B
            InrOpCode(state, &state->b);
            break;
        case 0x05: // DCR B
            DcrOpCode(state, &state->b);
            break;
        case 0x06: // MVI B, D8
            MviOpCode(state, &state->b, opcode[1]);
            break;
        case 0x07: // RLC
            RlcOpCode(state);
            break;
        case 0x08: // NOP
            NopOpCode();
            break;
        case 0x09: // DAD B
            DadOpCode(state, GetMemoryAddress(state->b, state->c));
            break;
        case 0x0A: // LDAX B
            LdaxOpCode(state, GetMemoryAddress(state->b, state->c));
            break;
        case 0x0B: // DCX B
            DcxOpCode(&state->b, &state->c);
            break;
        case 0x0C: // INR C
            InrOpCode(state, &state->c);
            break;
        case 0x0D: // DCR C
            DcrOpCode(state, &state->c);
            break;
        case 0x0E: // MVI C, D8
            MviOpCode(state, &state->c, opcode[1]);
            break;
        case 0x0F: // RRC
            RrcOpCode(state);
            break;
        case 0x10: // NOP
            NopOpCode();
            break;
        case 0x11: // LXI D, D16
            LxiOpCode(state, &state->d, &state->e, opcode[1], opcode[2]);
            break;
        case 0x12: // STAX D
            StaxOpCode(state, state->d, state->e);
            break;
        case 0x13: // INX D
            InxOpCode(&state->d, &state->e);
            break;
        case 0x14: // INR D
            InrOpCode(state, &state->d);
            break;
        case 0x15: // DCR D
            DcrOpCode(state, &state->d);
            break;
        case 0x16: // MVI D, D8
            MviOpCode(state, &state->d, opcode[1]);
            break;
        case 0x17: // RAL
            RalOpcode(state);
            break;
        case 0x18: // NOP
            NopOpCode();
            break;
        case 0x19: // DAD D
            DadOpCode(state, GetMemoryAddress(state->d, state->e));
            break;
        case 0x1A: // LDAX D
            LdaxOpCode(state, GetMemoryAddress(state->d, state->e));
            break;
        case 0x1B: // DCX D
            DcxOpCode(&state->d, &state->e);
            break;
        case 0x1C: // INR E
            InrOpCode(state, &state->e);
            break;
        case 0x1D: // DCR E
            DcrOpCode(state, &state->e);
            break;
        case 0x1E: // MVI E, D8
            MviOpCode(state, &state->e, opcode[1]);
            break;
        case 0x1F: // RAR
            RarOpCode(state);
            break;
        case 0x20: // NOP
            NopOpCode();
            break;
        case 0x21: // LXI H, D16
            LxiOpCode(state, &state->h, &state->l, opcode[1], opcode[2]);
            break;
        case 0x22: // SHLD adr
            UnimplementedInstruction(state);
            break;
        case 0x23: // INX H
            InxOpCode(&state->h, &state->l);
            break;
        case 0x24: // INR H
            InrOpCode(state, &state->h);
            break;
        case 0x25: // DCR H
            DcrOpCode(state, &state->h);
            break;
        case 0x26: // MVI H, D8
            MviOpCode(state, &state->h, opcode[1]);
            break;
        case 0x27: // DAA
            UnimplementedInstruction(state);
            break;
        case 0x28: // NOP
            NopOpCode();
            break;
        case 0x29: // DAD H
            DadOpCode(state, GetMemoryAddress(state->h, state->l));
            break;
        case 0x2A: // LHLD adr
            UnimplementedInstruction(state);
            break;
        case 0x2B: // DCX H
            DcxOpCode(&state->h, &state->l);
            break;
        case 0x2C: // INR L
            InrOpCode(state, &state->l);
            break;
        case 0x2D: // DCR L
            DcrOpCode(state, &state->l);
            break;
        case 0x2E: // MVI L, D8
            MviOpCode(state, &state->l, opcode[1]);
            break;
        case 0x2F: // CMA
            UnimplementedInstruction(state);
            break;
        case 0x30: // NOP
            NopOpCode();
            break;
        case 0x31: // LXI SP, D16
            LxiStackPointerOpCode(state, opcode[1], opcode[2]);
            break;
        case 0x32: // STA adr
            UnimplementedInstruction(state);
            break;
        case 0x33: // INX SP
            InxStackPointerOpCode(state);
            break;
        case 0x34: // INR M
            InrOpCode(state, &state->memory[GetMemoryAddress(state->h, state->l)]);
            break;
        case 0x35: // DCR M
            DcrOpCode(state, &state->memory[GetMemoryAddress(state->h, state->l)]);
            break;
        case 0x36: // MVI M, D8
            MviOpCode(state, &state->memory[GetMemoryAddress(state->h, state->l)], opcode[1]);
            break;
        case 0x37: // STC
            UnimplementedInstruction(state);
            break;
        case 0x38: // NOP
            NopOpCode();
            break;
        case 0x39: // DAD SP
            DadOpCode(state, state->sp);
            break;
        case 0x3A: // LDA adr
            UnimplementedInstruction(state);
            break;
        case 0x3B: // DCX SP
            DcxStackPointerOpCode(state);
            break;
        case 0x3C: // INR A
            InrOpCode(state, &state->a);
            break;
        case 0x3D: // DCR A
            DcrOpCode(state, &state->a);
            break;
        case 0x3E: // MVI A, D8
            MviOpCode(state, &state->a, opcode[1]);
            break;
        case 0x3F: // CMC
            UnimplementedInstruction(state);
            break;
        case 0x40: // MOV B, B
            MovOpCode(&state->b, &state->b);
            break;
        case 0x41: // MOV B, C
            MovOpCode(&state->b, &state->c);
            break;
        case 0x42: // MOV B, D
            MovOpCode(&state->b, &state->d);
            break;
        case 0x43: // MOV B, E
            MovOpCode(&state->b, &state->e);
            break;
        case 0x44: // MOV B, H
            MovOpCode(&state->b, &state->h);
            break;
        case 0x45: // MOV B, L
            MovOpCode(&state->b, &state->l);
            break;
        case 0x46: // MOV B, M
            MovOpCode(&state->b, &state->memory[GetMemoryAddress(state->h, state->l)]);
            break;
        case 0x47: // MOV B, A
            MovOpCode(&state->b, &state->a);
            break;
        case 0x48: // MOV C, B
            MovOpCode(&state->c, &state->b);
            break;
        case 0x49: // MOV C, C
            MovOpCode(&state->c, &state->c);
            break;
        case 0x4A: // MOV C, D
            MovOpCode(&state->c, &state->d);
            break;
        case 0x4B: // MOV C, E
            MovOpCode(&state->c, &state->e);
            break;
        case 0x4C: // MOV C, H
            MovOpCode(&state->c, &state->h);
            break;
        case 0x4D: // MOV C, L
            MovOpCode(&state->c, &state->l);
            break;
        case 0x4E: // MOV C, M
            MovOpCode(&state->c, &state->memory[GetMemoryAddress(state->h, state->l)]);
            break;
        case 0x4F: // MOV C, A
            MovOpCode(&state->c, &state->a);
            break;
        case 0x50: // MOV D, B
            MovOpCode(&state->d, &state->b);
            break;
        case 0x51: // MOV D, C
            MovOpCode(&state->d, &state->c);
            break;
        case 0x52: // MOV D, D
            MovOpCode(&state->d, &state->d);
            break;
        case 0x53: // MOV D, E
            MovOpCode(&state->d, &state->e);
            break;
        case 0x54: // MOV D, H
            MovOpCode(&state->d, &state->h);
            break;
        case 0x55: // MOV D, L
            MovOpCode(&state->d, &state->l);
            break;
        case 0x56: // MOV D, M
            MovOpCode(&state->d, &state->memory[GetMemoryAddress(state->h, state->l)]);
            break;
        case 0x57: // MOV D, A
            MovOpCode(&state->d, &state->a);
            break;
        case 0x58: // MOV E, B
            MovOpCode(&state->e, &state->b);
            break;
        case 0x59: // MOV E, C
            MovOpCode(&state->e, &state->c);
            break;
        case 0x5A: // MOV E, D
            MovOpCode(&state->e, &state->d);
            break;
        case 0x5B: // MOV E, E
            MovOpCode(&state->e, &state->e);
            break;
        case 0x5C: // MOV E, H
            MovOpCode(&state->e, &state->h);
            break;
        case 0x5D: // MOV E, L
            MovOpCode(&state->e, &state->l);
            break;
        case 0x5E: // MOV E, M
            MovOpCode(&state->e, &state->memory[GetMemoryAddress(state->h, state->l)]);
            break;
        case 0x5F: // MOV E, A
            MovOpCode(&state->e, &state->a);
            break;
        case 0x60: // MOV H, B
            MovOpCode(&state->h, &state->b);
            break;
        case 0x61: // MOV H, C
            MovOpCode(&state->h, &state->c);
            break;
        case 0x62: // MOV H, D
            MovOpCode(&state->h, &state->d);
            break;
        case 0x63: // MOV H, E
            MovOpCode(&state->h, &state->e);
            break;
        case 0x64: // MOV H, H
            MovOpCode(&state->h, &state->h);
            break;
        case 0x65: // MOV H, L
            MovOpCode(&state->h, &state->l);
            break;
        case 0x66: // MOV H, M
            MovOpCode(&state->h, &state->memory[GetMemoryAddress(state->h, state->l)]);
            break;
        case 0x67: // MOV H, A
            MovOpCode(&state->h, &state->a);
            break;
        case 0x68: // MOV L, B
            MovOpCode(&state->l, &state->b);
            break;
        case 0x69: // MOV L, C
            MovOpCode(&state->l, &state->c);
            break;
        case 0x6A: // MOV L, D
            MovOpCode(&state->l, &state->d);
            break;
        case 0x6B: // MOV L, E
            MovOpCode(&state->l, &state->e);
            break;
        case 0x6C: // MOV L, H
            MovOpCode(&state->l, &state->h);
            break;
        case 0x6D: // MOV L, L
            MovOpCode(&state->l, &state->l);
            break;
        case 0x6E: // MOV L, M
            MovOpCode(&state->l, &state->memory[GetMemoryAddress(state->h, state->l)]);
            break;
        case 0x6F: // MOV L, A
            MovOpCode(&state->l, &state->a);
            break;
        case 0x70: // MOV M, B
            MovOpCode(&state->memory[GetMemoryAddress(state->h, state->l)], &state->b);
            break;
        case 0x71: // MOV M, C
            MovOpCode(&state->memory[GetMemoryAddress(state->h, state->l)], &state->c);
            break;
        case 0x72: // MOV M, D
            MovOpCode(&state->memory[GetMemoryAddress(state->h, state->l)], &state->d);
            break;
        case 0x73: // MOV M, E
            MovOpCode(&state->memory[GetMemoryAddress(state->h, state->l)], &state->e);
            break;
        case 0x74: // MOV M, H
            MovOpCode(&state->memory[GetMemoryAddress(state->h, state->l)], &state->h);
            break;
        case 0x75: // MOV M, L
            MovOpCode(&state->memory[GetMemoryAddress(state->h, state->l)], &state->l);
            break;
        case 0x76: // HLT
            UnimplementedInstruction(state);
            break;
        case 0x77: // MOV M, A
            MovOpCode(&state->memory[GetMemoryAddress(state->h, state->l)], &state->a);
            break;
        case 0x78: // MOV A, B
            MovOpCode(&state->a, &state->b);
            break;
        case 0x79: // MOV A, C
            MovOpCode(&state->a, &state->c);
            break;
        case 0x7A: // MOV A, D
            MovOpCode(&state->a, &state->d);
            break;
        case 0x7B: // MOV A, E
            MovOpCode(&state->a, &state->e);
            break;
        case 0x7C: // MOV A, H
            MovOpCode(&state->a, &state->h);
            break;
        case 0x7D: // MOV A, L
            MovOpCode(&state->a, &state->l);
            break;
        case 0x7E: // MOV A, M
            MovOpCode(&state->a, &state->memory[GetMemoryAddress(state->h, state->l)]);
            break;
        case 0x7F: // MOV A, A
            MovOpCode(&state->a, &state->a);
            break;
        case 0x80: // ADD B
            AddOpCode(state, &state->b);
            break;
        case 0x81: // ADD C
            AddOpCode(state, &state->c);
            break;
        case 0x82: // ADD D
            AddOpCode(state, &state->d);
            break;
        case 0x83: // ADD E
            AddOpCode(state, &state->e);
            break;
        case 0x84: // ADD H
            AddOpCode(state, &state->h);
            break;
        case 0x85: // ADD L
            AddOpCode(state, &state->l);
            break;
        case 0x86: // ADD M
            AddOpCode(state, &state->memory[GetMemoryAddress(state->h, state->l)]);
            break;
        case 0x87: // ADD A
            AddOpCode(state, &state->a);
            break;
        case 0x88: // ADC B
            AdcOpCode(state, &state->b);
            break;
        case 0x89: // ADC C
            AdcOpCode(state, &state->c);
            break;
        case 0x8A: // ADC D
            AdcOpCode(state, &state->d);
            break;
        case 0x8B: // ADC E
            AdcOpCode(state, &state->e);
            break;
        case 0x8C: // ADC H
            AdcOpCode(state, &state->h);
            break;
        case 0x8D: // ADC L
            AdcOpCode(state, &state->l);
            break;
        case 0x8E: // ADC M
            AdcOpCode(state, &state->memory[GetMemoryAddress(state->h, state->l)]);
            break;
        case 0x8F: // // ADC A
            AdcOpCode(state, &state->a);
            break;
        case 0x90: // SUB B
            SubOpCode(state, &state->b);
            break;
        case 0x91: // SUB C
            SubOpCode(state, &state->c);
            break;
        case 0x92: // SUB D
            SubOpCode(state, &state->d);
            break;
        case 0x93: // SUB E
            SubOpCode(state, &state->e);
            break;
        case 0x94: // SUB H
            SubOpCode(state, &state->h);
            break;
        case 0x95: // SUB L
            SubOpCode(state, &state->l);
            break;
        case 0x96: // SUB M
            SubOpCode(state, &state->memory[GetMemoryAddress(state->h, state->l)]);
            break;
        case 0x97: // SUB A
            SubOpCode(state, &state->a);
            break;
        case 0x98: // SBB B
            SbbOpCode(state, &state->b);
            break;
        case 0x99: // SBB C
            SbbOpCode(state, &state->c);
            break;
        case 0x9A: // SBB D
            SbbOpCode(state, &state->d);
            break;
        case 0x9B: // SBB E
            SbbOpCode(state, &state->e);
            break;
        case 0x9C: // SBB H
            SbbOpCode(state, &state->h);
            break;
        case 0x9D: // SBB L
            SbbOpCode(state, &state->l);
            break;
        case 0x9E: // SBB M
            SbbOpCode(state, &state->memory[GetMemoryAddress(state->h, state->l)]);
            break;
        case 0x9F: // SBB A
            SbbOpCode(state, &state->a);
            break;
        case 0xA0: // ANA B
            UnimplementedInstruction(state);
            break;
        case 0xA1: // ANA C
            UnimplementedInstruction(state);
            break;
        case 0xA2: // ANA D
            UnimplementedInstruction(state);
            break;
        case 0xA3: // ANA E
            UnimplementedInstruction(state);
            break;
        case 0xA4: // ANA H
            UnimplementedInstruction(state);
            break;
        case 0xA5: // ANA L
            UnimplementedInstruction(state);
            break;
        case 0xA6: // ANA M
            UnimplementedInstruction(state);
            break;
        case 0xA7: // ANA A
            UnimplementedInstruction(state);
            break;
        case 0xA8: // XRA B
            UnimplementedInstruction(state);
            break;
        case 0xA9: // XRA C
            UnimplementedInstruction(state);
            break;
        case 0xAA: // XRA D
            UnimplementedInstruction(state);
            break;
        case 0xAB: // XRA E
            UnimplementedInstruction(state);
            break;
        case 0xAC: // XRA H
            UnimplementedInstruction(state);
            break;
        case 0xAD: // XRA L
            UnimplementedInstruction(state);
            break;
        case 0xAE: // XRA M
            UnimplementedInstruction(state);
            break;
        case 0xAF: // XRA A
            UnimplementedInstruction(state);
            break;
        case 0xB0: // ORA B
            UnimplementedInstruction(state);
            break;
        case 0xB1: // ORA C
            UnimplementedInstruction(state);
            break;
        case 0xB2: // ORA D
            UnimplementedInstruction(state);
            break;
        case 0xB3: // ORA E
            UnimplementedInstruction(state);
            break;
        case 0xB4: // ORA H
            UnimplementedInstruction(state);
            break;
        case 0xB5: // ORA L
            UnimplementedInstruction(state);
            break;
        case 0xB6: // ORA M
            UnimplementedInstruction(state);
            break;
        case 0xB7: // ORA A
            UnimplementedInstruction(state);
            break;
        case 0xB8: // CMP B
            UnimplementedInstruction(state);
            break;
        case 0xB9: // CMP C
            UnimplementedInstruction(state);
            break;
        case 0xBA: // CMP D
            UnimplementedInstruction(state);
            break;
        case 0xBB: // CMP E
            UnimplementedInstruction(state);
            break;
        case 0xBC: // CMP H
            UnimplementedInstruction(state);
            break;
        case 0xBD: // CMP L
            UnimplementedInstruction(state);
            break;
        case 0xBE: // CMP M
            UnimplementedInstruction(state);
            break;
        case 0xBF: // CMP A
            UnimplementedInstruction(state);
            break;
        case 0xC0: // RNZ
            UnimplementedInstruction(state);
            break;
        case 0xC1: // POP B
            UnimplementedInstruction(state);
            break;
        case 0xC2: // JNZ adr
            UnimplementedInstruction(state);
            break;
        case 0xC3: // JMP adr
            UnimplementedInstruction(state);
            break;
        case 0xC4: // CNZ adr
            UnimplementedInstruction(state);
            break;
        case 0xC5: // PUSH B
            UnimplementedInstruction(state);
            break;
        case 0xC6: // ADI D8
            AdiOpCode(state, opcode[1]);
            break;
        case 0xC7: // RST 0
            UnimplementedInstruction(state);
            break;
        case 0xC8: // RZ
            UnimplementedInstruction(state);
            break;
        case 0xC9: // RET
            UnimplementedInstruction(state);
            break;
        case 0xCA: // JZ adr
            UnimplementedInstruction(state);
            break;
        case 0xCB: // JMP adr
            UnimplementedInstruction(state);
            break;
        case 0xCC: // CZ adr
            UnimplementedInstruction(state);
            break;
        case 0xCD: // CALL adr
            UnimplementedInstruction(state);
            break;
        case 0xCE: // ACI D8
            UnimplementedInstruction(state);
            break;
        case 0xCF: // RST 1
            UnimplementedInstruction(state);
            break;
        case 0xD0: // RNC
            UnimplementedInstruction(state); 
            break;
        case 0xD1: // POP D
            UnimplementedInstruction(state);
            break;
        case 0xD2: // JNC adr
            UnimplementedInstruction(state);
            break;
        case 0xD3: // OUT D8
            UnimplementedInstruction(state);
            break;
        case 0xD4: // CNC adr
            UnimplementedInstruction(state);
            break;
        case 0xD5: // PUSH D
            UnimplementedInstruction(state);
            break;
        case 0xD6: // SUI D8
            UnimplementedInstruction(state);
            break;
        case 0xD7: // RST 2
            UnimplementedInstruction(state);
            break;
        case 0xD8: // RC
            UnimplementedInstruction(state);
            break;
        case 0xD9: // RET
            UnimplementedInstruction(state);
            break;
        case 0xDA: // JC adr
            UnimplementedInstruction(state);
            break;
        case 0xDB: // IN D8
            UnimplementedInstruction(state);
            break;
        case 0xDC: // CC adr
            UnimplementedInstruction(state);
            break;
        case 0xDD: // CALL adr
            UnimplementedInstruction(state);
            break;
        case 0xDE: // SBI D8
            UnimplementedInstruction(state);
            break;
        case 0xDF: // RST 3
            UnimplementedInstruction(state);
            break;
        case 0xE0: // RPO
            UnimplementedInstruction(state);
            break;
        case 0xE1: // POP H
            UnimplementedInstruction(state);
            break;
        case 0xE2: // JPO adr
            UnimplementedInstruction(state);
            break;
        case 0xE3: // XTHL
            UnimplementedInstruction(state);
            break;
        case 0xE4: // CPO adr
            UnimplementedInstruction(state);
            break;
        case 0xE5: // PUSH H
            UnimplementedInstruction(state);
            break;
        case 0xE6: // ANI D8
            UnimplementedInstruction(state);
            break;
        case 0xE7: // RST 4
            UnimplementedInstruction(state);
            break;
        case 0xE8: // RPE
            UnimplementedInstruction(state);
            break;
        case 0xE9: // PCHL
            UnimplementedInstruction(state);
            break;
        case 0xEA: // JPE adr
            UnimplementedInstruction(state);
            break;
        case 0xEB: // XCHG
            UnimplementedInstruction(state);
            break;
        case 0xEC: // CPE adr
            UnimplementedInstruction(state);
            break;
        case 0xED: // CALL adr
            UnimplementedInstruction(state);
            break;
        case 0xEE: // XRI D8
            UnimplementedInstruction(state);
            break;
        case 0xEF: // RST 5
            UnimplementedInstruction(state);
            break;
        case 0xF0: // RP
            UnimplementedInstruction(state);
            break;
        case 0xF1: // POP PSW
            UnimplementedInstruction(state);
            break;
        case 0xF2: // JP adr
            UnimplementedInstruction(state);
            break;
        case 0xF3: // DI
            UnimplementedInstruction(state);
            break;
        case 0xF4: // CP adr
            UnimplementedInstruction(state);
            break;
        case 0xF5: // PUSH PSW
            UnimplementedInstruction(state);
            break;
        case 0xF6: // ORI D8
            UnimplementedInstruction(state);
            break;
        case 0xF7: // RST 6
            UnimplementedInstruction(state);
            break;
        case 0xF8: // RM
            UnimplementedInstruction(state);
            break;
        case 0xF9: // SPHL
            UnimplementedInstruction(state);
            break;
        case 0xFA: // JM adr
            UnimplementedInstruction(state);
            break;
        case 0xFB: // EI
            UnimplementedInstruction(state);
            break;
        case 0xFC: // CM adr
            UnimplementedInstruction(state);
            break;
        case 0xFD: // CALL adr
            UnimplementedInstruction(state);
            break;
        case 0xFE: // CPI D8
            UnimplementedInstruction(state);
            break;
        case 0xFF: // RST 7
            UnimplementedInstruction(state);
            break;
    }
    state->pc+=1;  //for the opcode

    return 0;
}

State8080 *CreateTestState() {
    const int KB_64 = 65536;
    const uint8_t NULL_8_BIT = 0x00;

    State8080 *state = malloc(sizeof(State8080));
    if (state == NULL) {
        return NULL;
    }

    state->memory = calloc(KB_64, sizeof(uint8_t)); // calloc zeroes out the ROM/RAM space
    if (state->memory == NULL) {
        free(state); // Clean up container if memory array allocation fails
        return NULL;
    }

    // 3. Initialize General Purpose Registers (8-bit)
    state->a = NULL_8_BIT;
    state->b = 0x01;
    state->c = 0x01;
    state->d = NULL_8_BIT;
    state->e = NULL_8_BIT;
    state->h = NULL_8_BIT;
    state->l = NULL_8_BIT;

    // 4. Initialize Address Registers (16-bit)
    state->pc = 0x0000;  // Code execution normally starts at address 0
    state->sp = 0xF000;  // Space Invaders RAM stack area typically starts high

    // 5. Initialize Condition Code Flags (Bit-fields)
    state->cc.z  = 0;
    state->cc.s  = 0;
    state->cc.p  = 0;
    state->cc.cy = 0;
    state->cc.ac = 0;
    state->cc.pad = 0; // Clear the alignment padding

    // 6. Interrupt System Control
    state->int_enable = 0;

    return state;
}

void FreeState(State8080 *state) {
    if (state != NULL) {
        free(state->memory);
        free(state);
    }
}

void TestState(State8080* state, uint8_t *command, size_t command_len) {
    memcpy(&state->memory[state->pc], command, command_len);
    Emulate8080Op(state);
    return;
}
