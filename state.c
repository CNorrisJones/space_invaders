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

void nop_op_code() {
    return;
}

void add_op_code(State8080 *state, uint8_t *added_register) {
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

void add_memory_op_code(State8080 *state) {
    uint16_t offset = (state->h<<8) | (state->l);    
    uint16_t answer = (uint16_t) state->a + state->memory[offset];    
    state->cc.z = ((answer & 0xff) == 0);    
    state->cc.s = ((answer & 0x80) != 0);    
    state->cc.cy = (answer > 0xff);    
    state->cc.p = Parity(answer&0xff);    
    state->a = answer & 0xff;
}

void adi_op_code(State8080 *state, uint8_t added_byte) {
    const uint8_t UPPER_BIT_MASK = 0xff;
    const uint8_t SIGN_BIT_MASK = 0x80;

    // Cast to 16-bit to more easily check for a carry
    uint16_t answer = (uint16_t) state->a + (uint16_t) added_byte;    

    state->cc.z = ((answer & UPPER_BIT_MASK) == 0); // Check Zero Flag
    state->cc.s = ((answer & SIGN_BIT_MASK) != 0);  // Check Sign Flag
    state->cc.cy = (answer > UPPER_BIT_MASK);       // Check Carry Flag
    state->cc.p = Parity(answer & UPPER_BIT_MASK);  // Check Parity Flag
    state->a = answer & UPPER_BIT_MASK;

    return;
}

void lxi_op_code(State8080 *state, uint8_t *lower_register, uint8_t *upper_register, uint8_t byte_1, uint8_t byte_2) {
    *upper_register = byte_1;
    *lower_register = byte_2;
    state->pc += 2;
    return;
}

int Emulate8080Op(State8080* state) {
    unsigned char *opcode = &state->memory[state->pc];

    switch(*opcode) {
        case 0x00: // NOP
            nop_op_code();
            break;
        case 0x01: // LXI B D16
            lxi_op_code(state, &state->b, &state->c, opcode[1], opcode[2]);
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
            UnimplementedInstruction(state);
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
            UnimplementedInstruction(state);
            break;
        case 0x11: // LXI D, D16
            UnimplementedInstruction(state);
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
            UnimplementedInstruction(state);
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
            UnimplementedInstruction(state);
            break;
        case 0x21: // LXI H, D16
            UnimplementedInstruction(state);
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
            UnimplementedInstruction(state);
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
            UnimplementedInstruction(state);
            break;
        case 0x31: // LXI SP, D16
            UnimplementedInstruction(state);
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
            UnimplementedInstruction(state);
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
            UnimplementedInstruction(state);
            break;
        case 0x41: // MOV B, C
            UnimplementedInstruction(state);
            break;
        case 0x42: // MOV B, D
            UnimplementedInstruction(state);
            break;
        case 0x43: // MOV B, E
            UnimplementedInstruction(state);
            break;
        case 0x44: // MOV B, H
            UnimplementedInstruction(state);
            break;
        case 0x45: // MOV B, L
            UnimplementedInstruction(state);
            break;
        case 0x46: // MOV B, M
            UnimplementedInstruction(state);
            break;
        case 0x47: // MOV B, A
            UnimplementedInstruction(state);
            break;
        case 0x48: // MOV C, B
            UnimplementedInstruction(state);
            break;
        case 0x49: // MOV C, C
            UnimplementedInstruction(state);
            break;
        case 0x4A: // MOV C, D
            UnimplementedInstruction(state);
            break;
        case 0x4B: // MOV C, E
            UnimplementedInstruction(state);
            break;
        case 0x4C: // MOV C, H
            UnimplementedInstruction(state);
            break;
        case 0x4D: // MOV C, L
            UnimplementedInstruction(state);
            break;
        case 0x4E: // MOV C, M
            UnimplementedInstruction(state);
            break;
        case 0x4F: // MOV C, A
            UnimplementedInstruction(state);
            break;
        case 0x50: // MOV D, B
            UnimplementedInstruction(state);
            break;
        case 0x51: // MOV D, C
            UnimplementedInstruction(state);
            break;
        case 0x52: // MOV D, D
            UnimplementedInstruction(state);
            break;
        case 0x53: // MOV D, E
            UnimplementedInstruction(state);
            break;
        case 0x54: // MOV D, H
            UnimplementedInstruction(state);
            break;
        case 0x55: // MOV D, L
            UnimplementedInstruction(state);
            break;
        case 0x56: // MOV D, M
            UnimplementedInstruction(state);
            break;
        case 0x57: // MOV D, A
            UnimplementedInstruction(state);
            break;
        case 0x58: // MOV E, B
            UnimplementedInstruction(state);
            break;
        case 0x59: // MOV E, C
            UnimplementedInstruction(state);
            break;
        case 0x5A: // MOV E, D
            UnimplementedInstruction(state);
            break;
        case 0x5B: // MOV E, E
            UnimplementedInstruction(state);
            break;
        case 0x5C: // MOV E, H
            UnimplementedInstruction(state);
            break;
        case 0x5D: // MOV E, L
            UnimplementedInstruction(state);
            break;
        case 0x5E: // MOV E, M
            UnimplementedInstruction(state);
            break;
        case 0x5F: // MOV E, A
            UnimplementedInstruction(state);
            break;
        case 0x60: // MOV H, B
            UnimplementedInstruction(state);
            break;
        case 0x61: // MOV H, C
            UnimplementedInstruction(state);
            break;
        case 0x62: // MOV H, D
            UnimplementedInstruction(state);
            break;
        case 0x63: // MOV H, E
            UnimplementedInstruction(state);
            break;
        case 0x64: // MOV H, H
            UnimplementedInstruction(state);
            break;
        case 0x65: // MOV H, L
            UnimplementedInstruction(state);
            break;
        case 0x66: // MOV H, M
            UnimplementedInstruction(state);
            break;
        case 0x67: // MOV H, A
            UnimplementedInstruction(state);
            break;
        case 0x68: // MOV L, B
            UnimplementedInstruction(state);
            break;
        case 0x69: // MOV L, C
            UnimplementedInstruction(state);
            break;
        case 0x6A: // MOV L, D
            UnimplementedInstruction(state);
            break;
        case 0x6B: // MOV L, E
            UnimplementedInstruction(state);
            break;
        case 0x6C: // MOV L, H
            UnimplementedInstruction(state);
            break;
        case 0x6D: // MOV L, L
            UnimplementedInstruction(state);
            break;
        case 0x6E: // MOV L, M
            UnimplementedInstruction(state);
            break;
        case 0x6F: // MOV L, A
            UnimplementedInstruction(state);
            break;
        case 0x70: // MOV M, B
            UnimplementedInstruction(state);
            break;
        case 0x71: // MOV M, C
            UnimplementedInstruction(state);
            break;
        case 0x72: // MOV M, D
            UnimplementedInstruction(state);
            break;
        case 0x73: // MOV M, E
            UnimplementedInstruction(state);
            break;
        case 0x74: // MOV M, H
            UnimplementedInstruction(state);
            break;
        case 0x75: // MOV M, L
            UnimplementedInstruction(state);
            break;
        case 0x76: // HLT
            UnimplementedInstruction(state);
            break;
        case 0x77: // MOV M, A
            UnimplementedInstruction(state);
            break;
        case 0x78: // MOV A, B
            UnimplementedInstruction(state);
            break;
        case 0x79: // MOV A, C
            UnimplementedInstruction(state);
            break;
        case 0x7A: // MOV A, D
            UnimplementedInstruction(state);
            break;
        case 0x7B: // MOV A, E
            UnimplementedInstruction(state);
            break;
        case 0x7C: // MOV A, H
            UnimplementedInstruction(state);
            break;
        case 0x7D: // MOV A, L
            UnimplementedInstruction(state);
            break;
        case 0x7E: // MOV A, M
            UnimplementedInstruction(state);
            break;
        case 0x7F: // MOV A, A
            UnimplementedInstruction(state);
            break;
        case 0x80: // ADD B
            add_op_code(state, &state->b);
            break;
        case 0x81: // ADD C
            add_op_code(state, &state->c);
            break;
        case 0x82: // ADD D
            add_op_code(state, &state->d);
            break;
        case 0x83: // ADD E
            add_op_code(state, &state->e);
            break;
        case 0x84: // ADD H
            add_op_code(state, &state->h);
            break;
        case 0x85: // ADD L
            add_op_code(state, &state->l);
            break;
        case 0x86: // ADD M
            add_memory_op_code(state);
            break;
        case 0x87: // ADD A
            add_op_code(state, &state->a);
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
            UnimplementedInstruction(state);
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
        case 0xCB: // NOP
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
        case 0xD9: // NOP
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
        case 0xDD: // NOP
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
        case 0xED: // NOP
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
        case 0xFD: // NOP
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

State8080 *create_test_state() {
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

void free_state(State8080 *state) {
    if (state != NULL) {
        free(state->memory);
        free(state);
    }
}

void test_state(State8080* state, uint8_t *command, size_t command_len) {
    memcpy(&state->memory[state->pc], command, command_len);
    Emulate8080Op(state);
    return;
}
