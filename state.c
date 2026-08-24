#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "state.h"

void UnimplementedInstruction(State8080 *state) {
    //pc will have advanced one, so undo that    
    printf ("Error: Unimplemented instruction\n");    
    exit(1);    
}

int Parity(uint16_t input) {
    return 0;
}

uint16_t GetMemoryAddress(uint8_t high, uint8_t low) {
    uint16_t address = ((uint16_t)high << 8) | low;

    return address;
}

uint8_t PullFromMemory(State8080 *state, uint16_t address) {
    uint8_t value = state->memory[address];

    return value;
}

void NopOpCode() {
    return;
}

void AddOpCode(State8080 *state, uint8_t *added_register) {
    const uint8_t UPPER_BIT_MASK = 0xff;
    const uint8_t SIGN_BIT_MASK = 0x80;

    // Cast to 16-bit to more easily check for a carry
    uint16_t answer = (uint16_t) state->a + (uint16_t) *added_register;    

    state->cc.z = ((answer & UPPER_BIT_MASK) == 0); // Check Zero Flag
    state->cc.s = ((answer & SIGN_BIT_MASK) != 0);  // Check Sign Flag
    state->cc.cy = (answer > UPPER_BIT_MASK);       // Check Carry Flag
    state->cc.p = Parity(answer & UPPER_BIT_MASK);  // Check Parity Flag
    state->a = answer & UPPER_BIT_MASK;

    return;
}

void AddMemoryOpCode(State8080 *state) {
    uint16_t address = GetMemoryAddress(state->h, state->l);  
    uint16_t answer = (uint16_t) state->a + (uint16_t) PullFromMemory(state, address);    
    state->cc.z = ((answer & 0xff) == 0);    
    state->cc.s = ((answer & 0x80) != 0);    
    state->cc.cy = (answer > 0xff);    
    state->cc.p = Parity(answer&0xff);    
    state->a = answer & 0xff;
}

void AdiOpCode(State8080 *state, uint8_t added_byte) {
    const uint8_t UPPER_BIT_MASK = 0xff;
    const uint8_t SIGN_BIT_MASK = 0x80;

    // Cast to 16-bit to more easily check for a carry
    uint16_t answer = (uint16_t) state->a + (uint16_t) added_byte;    

    state->cc.z = ((answer & UPPER_BIT_MASK) == 0); // Check Zero Flag
    state->cc.s = ((answer & SIGN_BIT_MASK) != 0);  // Check Sign Flag
    state->cc.cy = (answer > UPPER_BIT_MASK);       // Check Carry Flag
    state->cc.p = Parity(answer & UPPER_BIT_MASK);  // Check Parity Flag
    state->a = answer & UPPER_BIT_MASK;
    state->pc += 1;

    return;
}

void LxiOpCode(State8080 *state, uint8_t *lower_register, uint8_t *upper_register, uint8_t byte_2, uint8_t byte_3) {
    *lower_register = byte_3;
    *upper_register = byte_2;
    state->pc += 2;
    return;
}

void LxiStackPointerOpCode(State8080 *state, uint8_t byte_2, uint8_t byte_3) {
    uint16_t address = GetMemoryAddress(byte_3, byte_2);
    state->sp = address;
    state->pc += 2;
    return;
}

void MovOpCode(uint8_t *dest_register, uint8_t *send_register) {
    *dest_register = *send_register;

    return;
}

void MovMemSendOpCode(State8080 *state, uint8_t *dest_register) {
    uint16_t address = GetMemoryAddress(state->h, state->l);

    *dest_register = PullFromMemory(state, address);
    return;
}

void MovMemDestOpCode(State8080 *state, uint8_t *send_register) {
    uint16_t address = GetMemoryAddress(state->h, state->l);

    state->memory[address] = *send_register;
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
            UnimplementedInstruction(state);
            break;
        case 0x03: // INX B
            UnimplementedInstruction(state);
            break;
        case 0x04: // INR B
            UnimplementedInstruction(state);
            break;
        case 0x05: // DCR B
            UnimplementedInstruction(state);
            break;
        case 0x06: // MVI B, D8
            UnimplementedInstruction(state);
            break;
        case 0x07: // RLC
            UnimplementedInstruction(state);
            break;
        case 0x08: // NOP
            NopOpCode();
            break;
        case 0x09: // DAD B
            UnimplementedInstruction(state);
            break;
        case 0x0A: // LDAX B
            UnimplementedInstruction(state);
            break;
        case 0x0B: // DCX B
            UnimplementedInstruction(state);
            break;
        case 0x0C: // INR C
            UnimplementedInstruction(state);
            break;
        case 0x0D: // DCR C
            UnimplementedInstruction(state);
            break;
        case 0x0E: // MVI C, D8
            UnimplementedInstruction(state);
            break;
        case 0x0F: // RRC
            UnimplementedInstruction(state);
            break;
        case 0x10: // NOP
            NopOpCode();
            break;
        case 0x11: // LXI D, D16
            LxiOpCode(state, &state->d, &state->e, opcode[1], opcode[2]);
            break;
        case 0x12: // STAX D
            UnimplementedInstruction(state);
            break;
        case 0x13: // INX D
            UnimplementedInstruction(state);
            break;
        case 0x14: // INR D
            UnimplementedInstruction(state);
            break;
        case 0x15: // DCR D
            UnimplementedInstruction(state);
            break;
        case 0x16: // MVI D, D8
            UnimplementedInstruction(state);
            break;
        case 0x17: // RAL
            UnimplementedInstruction(state);
            break;
        case 0x18: // NOP
            NopOpCode();
            break;
        case 0x19: // DAD D
            UnimplementedInstruction(state);
            break;
        case 0x1A: // LDAX D
            UnimplementedInstruction(state);
            break;
        case 0x1B: // DCX D
            UnimplementedInstruction(state);
            break;
        case 0x1C: // INR E
            UnimplementedInstruction(state);
            break;
        case 0x1D: // DCR E
            UnimplementedInstruction(state);
            break;
        case 0x1E: // MVI E, D8
            UnimplementedInstruction(state);
            break;
        case 0x1F: // RAR
            UnimplementedInstruction(state);
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
            UnimplementedInstruction(state);
            break;
        case 0x24: // INR H
            UnimplementedInstruction(state);
            break;
        case 0x25: // DCR H
            UnimplementedInstruction(state);
            break;
        case 0x26: // MVI H, D8
            UnimplementedInstruction(state);
            break;
        case 0x27: // DAA
            UnimplementedInstruction(state);
            break;
        case 0x28: // NOP
            NopOpCode();
            break;
        case 0x29: // DAD H
            UnimplementedInstruction(state);
            break;
        case 0x2A: // LHLD adr
            UnimplementedInstruction(state);
            break;
        case 0x2B: // DCX H
            UnimplementedInstruction(state);
            break;
        case 0x2C: // INR L
            UnimplementedInstruction(state);
            break;
        case 0x2D: // DCR L
            UnimplementedInstruction(state);
            break;
        case 0x2E: // MVI L, D8
            UnimplementedInstruction(state);
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
            UnimplementedInstruction(state);
            break;
        case 0x34: // INR M
            UnimplementedInstruction(state);
            break;
        case 0x35: // DCR M
            UnimplementedInstruction(state);
            break;
        case 0x36: // MVI M, D8
            UnimplementedInstruction(state);
            break;
        case 0x37: // STC
            UnimplementedInstruction(state);
            break;
        case 0x38: // NOP
            NopOpCode();
            break;
        case 0x39: // DAD SP
            UnimplementedInstruction(state);
            break;
        case 0x3A: // LDA adr
            UnimplementedInstruction(state);
            break;
        case 0x3B: // DCX SP
            UnimplementedInstruction(state);
            break;
        case 0x3C: // INR A
            UnimplementedInstruction(state);
            break;
        case 0x3D: // DCR A
            UnimplementedInstruction(state);
            break;
        case 0x3E: // MVI A, D8
            UnimplementedInstruction(state);
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
            MovMemSendOpCode(state, &state->b);
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
            MovMemSendOpCode(state, &state->c);
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
            MovMemSendOpCode(state, &state->d);
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
            MovMemSendOpCode(state, &state->e);
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
            MovMemSendOpCode(state, &state->h);
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
            MovMemSendOpCode(state, &state->l);
            break;
        case 0x6F: // MOV L, A
            MovOpCode(&state->l, &state->a);
            break;
        case 0x70: // MOV M, B
            MovMemDestOpCode(state, &state->b);
            break;
        case 0x71: // MOV M, C
            MovMemDestOpCode(state, &state->c);
            break;
        case 0x72: // MOV M, D
            MovMemDestOpCode(state, &state->d);
            break;
        case 0x73: // MOV M, E
            MovMemDestOpCode(state, &state->e);
            break;
        case 0x74: // MOV M, H
            MovMemDestOpCode(state, &state->h);
            break;
        case 0x75: // MOV M, L
            MovMemDestOpCode(state, &state->l);
            break;
        case 0x76: // HLT
            UnimplementedInstruction(state);
            break;
        case 0x77: // MOV M, A
            MovMemDestOpCode(state, &state->a);
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
            MovMemSendOpCode(state, &state->a);
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
            AddMemoryOpCode(state);
            break;
        case 0x87: // ADD A
            AddOpCode(state, &state->a);
            break;
        case 0x88: // ADC B
            UnimplementedInstruction(state);
            break;
        case 0x89: // ADC C
            UnimplementedInstruction(state);
            break;
        case 0x8A: // ADC D
            UnimplementedInstruction(state);
            break;
        case 0x8B: // ADC E
            UnimplementedInstruction(state);
            break;
        case 0x8C: // ADC H
            UnimplementedInstruction(state);
            break;
        case 0x8D: // ADC L
            UnimplementedInstruction(state);
            break;
        case 0x8E: // ADC M
            UnimplementedInstruction(state);
            break;
        case 0x8F: // // ADC A
            UnimplementedInstruction(state);
            break;
        case 0x90: // SUB B
            UnimplementedInstruction(state);
            break;
        case 0x91: // SUB C
            UnimplementedInstruction(state);
            break;
        case 0x92: // SUB D
            UnimplementedInstruction(state);
            break;
        case 0x93: // SUB E
            UnimplementedInstruction(state);
            break;
        case 0x94: // SUB H
            UnimplementedInstruction(state);
            break;
        case 0x95: // SUB L
            UnimplementedInstruction(state);
            break;
        case 0x96: // SUB M
            UnimplementedInstruction(state);
            break;
        case 0x97: // SUB A
            UnimplementedInstruction(state);
            break;
        case 0x98: // SBB B
            UnimplementedInstruction(state);
            break;
        case 0x99: // SBB C
            UnimplementedInstruction(state);
            break;
        case 0x9A: // SBB D
            UnimplementedInstruction(state);
            break;
        case 0x9B: // SBB E
            UnimplementedInstruction(state);
            break;
        case 0x9C: // SBB H
            UnimplementedInstruction(state);
            break;
        case 0x9D: // SBB L
            UnimplementedInstruction(state);
            break;
        case 0x9E: // SBB M
            UnimplementedInstruction(state);
            break;
        case 0x9F: // SBB A
            UnimplementedInstruction(state);
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
