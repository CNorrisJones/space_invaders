#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "state.h"
#include "disassembler.h"

uint8_t Parity(uint8_t input) {
    uint8_t bit_count = 0;

    for (uint8_t i = 0; i < 8; i++) {
        if ((input >> i) & 0x01) {
            bit_count++;
        }
    }
    
    return (bit_count % 2 == 0);
}

uint16_t GetMemoryAdr(uint8_t high, uint8_t low) {
    uint16_t adr = ((uint16_t) high << 8) | low;
    return adr;
}

void HandleZeroSignParityFlags(State8080 *state, uint16_t value) {
    const uint8_t UPPER_BIT_MASK = 0xff;
    const uint8_t SIGN_BIT_MASK = 0x80;

    state->cc.z = ((value & UPPER_BIT_MASK) == 0); // Check Zero Flag
    state->cc.s = ((value & SIGN_BIT_MASK) != 0);  // Check Sign Flag
    state->cc.p = Parity((uint8_t)(value & UPPER_BIT_MASK));  // Check Parity Flag
    return;
}

// --------------------------------
// ----- Data Transfer Funcs ------
// --------------------------------

// MOV: dest_reg <- send_reg
// Flags: None
// Op Size: 1
void MovOpCode(uint8_t *dest_reg, uint8_t *send_reg) {
    *dest_reg = *send_reg;
    return;
}

// MVI: dest_reg <- byte 2
// Flags: None
// Op Size: 2
void MviOpCode(State8080 *state, uint8_t *dest_reg, uint8_t byte_2) {
    *dest_reg = byte_2;
    state->pc += 1;
    return;
}

// LXI: high_reg <- byte 2, low_reg <- byte 3
// Flags: None
// Op Size: 3
void LxiOpCode(State8080 *state, uint8_t *high_reg, uint8_t *low_reg, uint8_t byte_2, uint8_t byte_3) {
    *high_reg = byte_3;
    *low_reg = byte_2;
    
    state->pc += 2;
    return;
}

// LXI Stack Pointer: SP.hi <- byte 3, SP.lo <- byte 2
// Flags: None
// Op Size: 3
void LxiStackPointerOpCode(State8080 *state, uint8_t byte_2, uint8_t byte_3) {
    uint16_t adr = GetMemoryAdr(byte_3, byte_2);
    state->sp = adr;
    state->pc += 2;
    return;
}

// LDA: A <- (adr)
// Flags: None
// Op Size: 3
void LdaOpCode(State8080 *state, uint16_t adr) {
    state->a = state->memory[adr];

    state->pc += 2;
    return;
}

// STA: (adr) <- A
// Flags: None
// Op Size: 3
void StaOpCode(State8080 *state, uint16_t adr) {
    state->memory[adr] = state->a;

    state->pc += 2;
    return;
}

// LHLD: L <- (adr); H<-(adr+1)
// Flags: None
// Op Size: 3
void LhldOpCode(State8080 *state, uint16_t adr) {
    state->l = state->memory[adr];
    state->h = state->memory[adr + 1];

    state->pc += 2;
    return;
}

// SHLD: (adr) <-L; (adr+1)<-H
// Flags: None
// Op Size: 3
void ShldOpCode(State8080 *state, uint16_t adr) {
    state->memory[adr] = state->l;
    state->memory[adr + 1] = state->h;

    state->pc += 2;
    return;
}

// LDAX: A <- (reg_pair)
// Flags: None
// Op Size: 1
void LdaxOpCode(State8080 *state, uint16_t reg_pair) {
    state->a = state->memory[reg_pair];
    return;
}

// STAX: ([high_reg][low_reg]) <- A
// Flags: None
// Op Size: 1
void StaxOpCode(State8080 *state, uint8_t high_reg, uint8_t low_reg) {
    uint16_t adr = GetMemoryAdr(high_reg, low_reg);

    state->memory[adr] = state->a;
    return;
}

// XCHG: H <-> D; L <-> E
// Flags: None
// Op Size: 1
void XchgOpCode(State8080 *state) {
    uint8_t temp_d_reg = state->d;
    uint8_t temp_e_reg = state->e;
    uint8_t temp_h_reg = state->h;
    uint8_t temp_l_reg = state->l;

    state->h = temp_d_reg;
    state->d = temp_h_reg;

    state->l = temp_e_reg;
    state->e = temp_l_reg;

    return;
}

// --------------------------------
// ------- Arithmetic Funcs -------
// --------------------------------

// ADD: A <- A + reg
// Flags: Z, S, P, CY, AC
// Op Size: 1
void AddOpCode(State8080 *state, uint8_t *reg) { 
    // Cast to 16-bit to more easily check for a carry
    uint16_t answer = (uint16_t) state->a + (uint16_t) *reg;    

    HandleZeroSignParityFlags(state, answer);
    state->cc.cy = (answer > 0xff);
    state->cc.ac = (((state->a & 0x0F) + (*reg & 0x0F)) > 0x0F);

    state->a = (uint8_t)(answer & 0xff);
    return;
}

// ADI: A <- A + added_byte
// Flags: Z, S, P, CY, AC
// Op Size: 2
void AdiOpCode(State8080 *state, uint8_t byte_2) {
    // Cast to 16-bit to more easily check for a carry
    uint16_t answer = (uint16_t) state->a + (uint16_t) byte_2;    

    HandleZeroSignParityFlags(state, answer);
    state->cc.cy = (answer > 0xff);
    state->cc.ac = (((state->a & 0x0F) + (byte_2 & 0x0F)) > 0x0F);

    state->a = (uint8_t)(answer & 0xff);
    state->pc += 1;
    return;
}

// ADC: A <- A + (reg) + CY
// Flags: Z, S, P, CY, AC
// Op Size: 1
void AdcOpCode(State8080 *state, uint8_t *reg) {
    uint8_t carry_flag = state->cc.cy;

    // Cast to 16-bit to more easily check for a carry
    uint16_t answer = (uint16_t) state->a + (uint16_t) *reg + (uint16_t) carry_flag;    

    HandleZeroSignParityFlags(state, answer);
    state->cc.cy = (answer > 0xff);
    state->cc.ac = (((state->a & 0x0F) + (*reg & 0x0F) + carry_flag) > 0x0F);

    state->a = (uint8_t)(answer & 0xff);
    return;
}

// ACI: A <- A + data + CY
// Flags: Z, S, P, CY, AC
// Op Size: 2
void AciOpCode(State8080 *state, uint8_t byte_2) {
    uint8_t carry_flag = state->cc.cy;

    // Cast to 16-bit to more easily check for a carry
    uint16_t answer = (uint16_t) state->a + (uint16_t) byte_2 + (uint16_t) carry_flag;    

    HandleZeroSignParityFlags(state, answer);
    state->cc.cy = (answer > 0xff);
    state->cc.ac = (((state->a & 0x0F) + (byte_2 & 0x0F) + carry_flag) > 0x0F);

    state->a = (uint8_t)(answer & 0xff);
    state->pc += 1;
    return;
}

// SUB: A <- A - reg
// Flags: Z, S, P, CY, AC
// Op Size: 1
void SubOpCode(State8080 *state, uint8_t *reg) {
    // Cast to 16-bit to more easily check for a carry
    uint16_t answer = (uint16_t) state->a - (uint16_t) *reg;

    HandleZeroSignParityFlags(state, answer);
    state->cc.cy = ((uint16_t) *reg > (uint16_t) state->a);
    state->cc.ac = ((*reg & 0x0F) > (state->a & 0x0F));

    state->a = (uint8_t)(answer & 0xff);
    return;
}

// SUI: A <- A - data
// Flags: Z, S, P, CY, AC
// Op Size: 2
void SuiOpCode(State8080 *state, uint8_t byte_2) {
    // Cast to 16-bit to more easily check for a carry
    uint16_t answer = (uint16_t) state->a - (uint16_t) byte_2;

    HandleZeroSignParityFlags(state, answer);
    state->cc.cy = ((uint16_t) byte_2 > (uint16_t) state->a);
    state->cc.ac = ((byte_2 & 0x0F) > (state->a & 0x0F));

    state->a = (uint8_t)(answer & 0xff);
    state->pc += 1;
    return;
}

// SBB: A <- A - (reg) - CY
// Flags: Z, S, P, CY, AC
// Op Size: 1
void SbbOpCode(State8080 *state, uint8_t *reg) {
    uint8_t carry_flag = state->cc.cy;

    // Cast to 16-bit to more easily check for a carry
    uint16_t answer = (uint16_t) state->a - (uint16_t) *reg - (uint16_t) carry_flag;

    HandleZeroSignParityFlags(state, answer);
    state->cc.cy = ((uint16_t) *reg + (uint16_t) carry_flag) > (uint16_t) state->a;
    state->cc.ac = (((*reg & 0x0F) + carry_flag) > (state->a & 0x0F));

    state->a = (uint8_t)(answer & 0xff);
    return;
}

// SBI: A <- A - data - CY
// Flags: Z, S, P, CY, AC
// Op Size: 2
void SbiOpCode(State8080 *state, uint8_t byte_2) {
    uint8_t carry_flag = state->cc.cy;

    // Cast to 16-bit to more easily check for a carry
    uint16_t answer = (uint16_t) state->a - (uint16_t) byte_2 - (uint16_t) carry_flag;

    HandleZeroSignParityFlags(state, answer);
    state->cc.cy = ((uint16_t) byte_2 + (uint16_t) carry_flag) > (uint16_t) state->a;
    state->cc.ac = (((byte_2 & 0x0F) + carry_flag) > (state->a & 0x0F));

    state->a = (uint8_t)(answer & 0xff);
    state->pc += 1;
    return;
}

// INR: reg <- reg + 1
// Flags: Z, S, P, AC
// Op Size: 1
void InrOpCode(State8080 *state, uint8_t *reg) {
    uint16_t answer = (uint16_t) *reg + 1;

    HandleZeroSignParityFlags(state, answer);
    state->cc.ac = (((*reg & 0x0F) + 0x01) > 0x0F);

    *reg = answer;
    return;
}

// DCR: reg <- reg - 1
// Flags: Z, S, P, AC
// Op Size: 1
void DcrOpCode(State8080 *state, uint8_t *reg) {
    uint16_t answer = (uint16_t) *reg - 1;

    HandleZeroSignParityFlags(state, answer);
    state->cc.ac = ((*reg & 0x0F) == 0x0);

    *reg = answer;
    return;
}

// INX: [Register Pair] <- [Register Pair] + 1
// Flags: None
// Op Size: 1
void InxOpCode(uint8_t *high_reg, uint8_t *low_reg) {
    uint16_t adr = GetMemoryAdr(*high_reg, *low_reg);
    adr += 1;

    *high_reg = (adr >> 8) & 0xFF;
    *low_reg = (adr & 0xFF);
    return;
}

// INX: Stack Pointer = Stack Pointer + 1
// Flags: None
// Op Size: 1
void InxStackPointerOpCode(State8080 *state) {
    state->sp += 1;
    return;
}

// DCX: [Register Pair] = [Register Pair] - 1
// Flags: None
// Op Size: 1
void DcxOpCode(uint8_t *high_reg, uint8_t *low_reg) {
    uint16_t adr = GetMemoryAdr(*high_reg, *low_reg);
    adr -= 1;

    *high_reg = (adr >> 8) & 0xFF;
    *low_reg = (adr & 0xFF);
    return;
}

// DCX: Stack Pointer = Stack Pointer - 1
// Flags: None
// Op Size: 1
void DcxStackPointerOpCode(State8080 *state) {
    state->sp -= 1;
    return;
}

// DAD: HL = HL + (reg_pair)
// Flags: CY
// Op Size: 1
void DadOpCode(State8080 *state, uint16_t reg_pair) {
    uint32_t hl_adr = (uint32_t) GetMemoryAdr(state->h, state->l);

    hl_adr = hl_adr + (uint32_t) reg_pair;
    state->cc.cy = (hl_adr > 0xFFFF);

    state->h = (uint8_t) ((hl_adr & 0xFFFF) >> 8);
    state->l = (uint8_t) (hl_adr & 0xFF);
    
    return;
}

// DAA: special
// Flags: Z, S, P, CY, AC
// Op Size: 1
void DaaOpCode(State8080 *state) {
    // If 4 least bits are > 9, or AC flag == 1, add 6 to state->a
    // If 4 greatest bits are now > 9, or CY flag == 1, add 6 to most significant 4 bits
    // If CY flag was 1, it stays 1, if it was 0, then it flips to 1 if we go over
    uint16_t answer = state->a;

    uint8_t lower_bits = (answer & 0x0F);

    if ((lower_bits > 0x09) | (state->cc.ac != 0)) {
        answer += 0x06;
        state->cc.ac = 0x1;
    } else {
        state->cc.ac = 0x0;
    }

    uint8_t upper_bits = ((answer >> 4) & 0x0F);

    if ((upper_bits > 0x09) | (state->cc.cy != 0)) {
        answer += 0x60;  
    } 

    HandleZeroSignParityFlags(state, answer);
    state->cc.cy = ((state->cc.cy != 0) | (answer > 0xff));

    state->a = (uint8_t)(answer & 0xff);
    return;
}

// --------------------------------
// --------- Logical Funcs --------
// --------------------------------

// ANA: A <- A & reg
// Flags: Z, S, P, CY, AC
// Op Size: 1
void AnaOpCode(State8080 *state, uint8_t *reg) {
    uint8_t answer = state->a & *reg;    

    HandleZeroSignParityFlags(state, (uint16_t) answer);
    state->cc.cy = 0; // Clears out CY Flag
    state->cc.ac = ((state->a & 0x8) >> 3) | ((*reg & 0x8) >> 3); // AC is the OR of 3rd-bit from both initial operands

    state->a = (uint8_t) answer;
    return;
}

// ANI: A <- A & byte_2
// Flags: Z, S, P, CY, AC
// Op Size: 2
void AniOpCode(State8080 *state, uint8_t byte_2) {
    uint8_t answer = state->a & byte_2;    

    HandleZeroSignParityFlags(state, (uint16_t) answer);
    state->cc.cy = 0; // Clears out CY flag
    state->cc.ac = ((state->a & 0x8) >> 3) | ((byte_2 & 0x8) >> 3); // AC is the OR of 3rd-bit from both initial operands

    state->a = (uint8_t) answer;
    state->pc += 1;
    return;
}

// XRA: A <- A ^ reg
// Flags: Z, S, P, CY, AC
// Op Size: 1
void XraOpCode(State8080 *state, uint8_t *reg) {
    uint8_t answer = state->a ^ *reg;    

    HandleZeroSignParityFlags(state, (uint16_t) answer);
    state->cc.cy = 0; // Clears out CY Flag
    state->cc.ac = 0; // Clears out AC Flag

    state->a = (uint8_t) answer;
    return;
}

// 	XRI: A <- A ^ data
// Flags: Z, S, P, CY, AC
// Op Size: 2
void XriOpCode(State8080 *state, uint8_t byte_2) {
    uint8_t answer = state->a ^ byte_2;    

    HandleZeroSignParityFlags(state, (uint16_t) answer);
    state->cc.cy = 0; // Clears out CY Flag
    state->cc.ac = 0; // Clears out AC Flag

    state->a = (uint8_t) answer;
    state->pc += 1;
    return;
}

// ORA: A <- A | reg
// Flags: Z, S, P, CY, AC
// Op Size: 1
void OraOpCode(State8080 *state, uint8_t *reg) {
    uint8_t answer = state->a | *reg;

    HandleZeroSignParityFlags(state, (uint16_t) answer);
    state->cc.cy = 0; // Clears out CY Flag
    state->cc.ac = 0; // Clears out AC Flag

    state->a = (uint8_t) answer;
    return;
}

// ORI: A <- A | byte_2
// Flags: Z, S, P, CY, AC
// Op Size: 2
void OriOpCode(State8080 *state, uint8_t byte_2) {
    uint8_t answer = state->a | byte_2;

    HandleZeroSignParityFlags(state, (uint16_t) answer);
    state->cc.cy = 0; // Clears out CY Flag
    state->cc.ac = 0; // Clears out AC Flag

    state->a = (uint8_t) answer;
    state->pc += 1;
    return;
}

// CMP: A - reg
// Flags: Z, S, P, CY, AC
// Op Size: 1
void CmpOpCode(State8080 *state, uint8_t reg) {
    uint8_t answer = state->a - reg;

    HandleZeroSignParityFlags(state, (uint16_t) answer);
    state->cc.cy = (reg > state->a);
    state->cc.ac = ((reg & 0x0F) > (state->a & 0x0F));

    return;
}

// CPI: A - byte_2
// Flags: Z, S, P, CY, AC
// Op Size: 2
void CpiOpCode(State8080 *state, uint8_t byte_2) {
    uint8_t answer = state->a - byte_2;

    HandleZeroSignParityFlags(state, (uint16_t) answer);
    state->cc.cy = (byte_2 > state->a);
    state->cc.ac = ((byte_2 & 0x0F) > (state->a & 0x0F));

    state->pc +=1;
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

// CMA: A <- !A
// Flags: None
// Op Size: 1
void CmaOpCode(State8080 *state) {
    state->a = ~state->a;
    return;
}

// CMC: CY=!CY
// Flags: CY
// Op Size: 1
void CmcOpCode(State8080 *state) {
    state->cc.cy = !state->cc.cy;
    return;
}

// STC: CY = 1
// Flags: CY
// Op Size: 1
void StcOpCode(State8080 *state) {
    state->cc.cy = 1;
    return;
}

// --------------------------------
// --------- Branch Funcs ---------
// --------------------------------

// JMP: PC <= adr
// Flags: None
// Op Size: 3
void JmpOpCode(State8080 *state, uint16_t adr) {
    state->pc = adr - 1;
    return;
}

// JNZ: if Zero Flag == 0, PC <- adr
// Flags: None
// Op Size: 3
void JnzOpCode(State8080 *state, uint16_t adr) {
    if (!state->cc.z) {
        state->pc = adr - 1;
    } else {
        state->pc += 2;
    }

    return;
}

// JZ: if Z, PC <- adr
// Flags: None
// Op Size: 3
void JzOpCode(State8080 *state, uint16_t adr) {
    if (state->cc.z) {
        state->pc = adr - 1;
    } else {
        state->pc += 2;
    }

    return;
}

// JNC: if NCY, PC<-adr
// Flags: None
// Op Size: 3
void JncOpCode(State8080 *state, uint16_t adr) {
    if (!state->cc.cy) {
        state->pc = adr - 1;
    } else {
        state->pc += 2;
    }

    return;
}

// JC: if CY, PC<-adr
// Flags: None
// Op Size: 3
void JcOpCode(State8080 *state, uint16_t adr) {
    if (state->cc.cy) {
        state->pc = adr - 1;
    } else {
        state->pc += 2;
    }

    return;
}

// JPO: If Parity Odd (cc.p == 0) PC <- adr
// Flags: None
// Op Size: 3
void JpoOpCode(State8080 *state, uint16_t adr) {
    if (!state->cc.p) {
        state->pc = adr - 1;
    } else {
        state->pc += 2;
    }

    return;
}

// JPE: If Parity Even (cc.p == 1) PC <- adr
// Flags: None
// Op Size: 3
void JpeOpCode(State8080 *state, uint16_t adr) {
    if (state->cc.p) {
        state->pc = adr - 1;
    } else {
        state->pc += 2;
    }

    return;
}

// JP: If Positive (cc.s == 0), PC <- adr
// Flags: None
// Op Size: 3
void JpOpCode(State8080 *state, uint16_t adr) {
    if (!state->cc.s) {
        state->pc = adr - 1;
    } else {
        state->pc += 2;
    }

    return;
}

// JM: If Negative (cc.s == 1), PC <- adr
// Flags: None
// Op Size: 3
void JmOpCode(State8080 *state, uint16_t adr) {
    if (state->cc.s) {
        state->pc = adr - 1;
    } else {
        state->pc += 2;
    }

    return;
}

// CALL: (SP-1)<-PC.hi;(SP-2)<-PC.lo;SP<-SP-2;PC=adr
// Flags: None
// Op Size: 3
void CallOpCode(State8080 *state, uint16_t adr) {
    uint16_t ret_adr = state->pc + 3;

    uint8_t pc_high = (uint8_t) ((ret_adr >> 8) & 0xFF);
    uint8_t pc_low = (uint8_t) (ret_adr & 0xFF);

    state->memory[state->sp - 1] = pc_high;
    state->memory[state->sp - 2] = pc_low;

    state->sp -= 2;
    state->pc = adr - 1;
    return;
}

// CNZ: if state->cc.z == 0, CALL adr
// Flags: None
// Op Size: 3
void CnzOpCode(State8080 *state, uint16_t adr) {
    if (!state->cc.z) {
        CallOpCode(state, adr);
    } else {
        state->pc += 2;
    }

    return;
}

// CZ: if state->cc.z == 1, CALL adr
// Flags: None
// Op Size: 3
void CzOpCode(State8080 *state, uint16_t adr) {
    if (state->cc.z) {
        CallOpCode(state, adr);
    } else {
        state->pc += 2;
    }

    return;
}

// CNC: if NCY, CALL adr
// Flags: None
// Op Size: 3
void CncOpCode(State8080 *state, uint16_t adr) {
    if (!state->cc.cy) {
        CallOpCode(state, adr);
    } else {
        state->pc += 2;
    }

    return;
}

// CC: if CY, CALL adr
// Flags: None
// Op Size: 3
void CcOpCode(State8080 *state, uint16_t adr) {
    if (state->cc.cy) {
        CallOpCode(state, adr);
    } else {
        state->pc += 2;
    }

    return;
}

// CPO: if Parity Odd, CALL adr
// Flags: None
// Op Size: 3
void CpoOpCode(State8080 *state, uint16_t adr) {
    if (!state->cc.p) {
        CallOpCode(state, adr);
    } else {
        state->pc += 2;
    }

    return;
}

// CPE: If Parity Even, CALL adr
// Flags: None
// Op Size: 3
void CpeOpCode(State8080 *state, uint16_t adr) {
    if (state->cc.p) {
        CallOpCode(state, adr);
    } else {
        state->pc += 2;
    }

    return;
}

// CP: if Positive (cc.s == 0), PC <- adr
// Flags: None
// Op Size: 3
void CpOpCode(State8080 *state, uint16_t adr) {
    if (!state->cc.s) {
        CallOpCode(state, adr);
    } else {
        state->pc += 2;
    }

    return;
}

// CM: If Negative (cc.s == 1), PC <- adr
// Flags: None
// Op Size: 3
void CmOpCode(State8080 *state, uint16_t adr) {
    if (state->cc.s) {
        CallOpCode(state, adr);
    } else {
        state->pc += 2;
    }

    return;
}

// RET: PC.lo <- (sp); PC.hi<-(sp+1); SP <- SP+2
// Flags: None
// Op Size: 1
void RetOpCode(State8080 *state) {
    uint8_t pc_low = state->memory[state->sp];
    uint8_t pc_high = state->memory[state->sp + 1];
    uint16_t new_pc_adr = GetMemoryAdr(pc_high, pc_low);

    state->pc = new_pc_adr - 1;
    state->sp += 2;
    return;
}

// RNZ: if NZ, RET
// Flags: None
// Op Size: 1
void RnzOpCode(State8080 *state) {
    if (!state->cc.z) {
        RetOpCode(state);
    }

    return;
}

// RZ: if Z, RET
// Flags: None
// Op Size: 1
void RzOpCode(State8080 *state) {
    if (state->cc.z) {
        RetOpCode(state);
    }

    return;
}

// RNC:	if NCY, RET
// Flags: None
// Op Size: 1
void RncOpCode(State8080 *state) {
    if (!state->cc.cy) {
        RetOpCode(state);
    }

    return;
}

// RC: if CY, RET
// Flags: None
// Op Size: 1
void RcOpCode(State8080 *state) {
    if (state->cc.cy) {
        RetOpCode(state);
    }

    return;
}

// RPO: if Parity Odd (cc.p == 0), RET
// Flags: None
// Op Size: 1
void RpoOpCode(State8080 *state) {
    if (!state->cc.p) {
        RetOpCode(state);
    }

    return;
}

// RPE: if Parity Even (cc.p == 1), RET
// Flags: None
// Op Size: 1
void RpeOpCode(State8080 *state) {
    if (state->cc.p) {
        RetOpCode(state);
    }

    return;
}

// RP: If Sign flag Positive (cc.s == 0), RET
// Flags: None
// Op Size: 1
void RpOpCode(State8080 *state) {
    if (!state->cc.s) {
        RetOpCode(state);
    }

    return;
}

// RM: if negative (cc.s == 1), RET
// Flags: None
// Op Size: 1
void RmOpCode(State8080 *state) {
    if (state->cc.s) {
        RetOpCode(state);
    }

    return;
}

// RST: CALL hard-coded adr
// Flags: None
// Op Size: 1
void RstOpCode(State8080 *state, uint16_t adr) {
    uint16_t ret_adr = state->pc;

    uint8_t pc_high = (uint8_t) ((ret_adr >> 8) & 0xFF);
    uint8_t pc_low = (uint8_t) (ret_adr & 0xFF);
    
    state->memory[state->sp - 1] = pc_high;
    state->memory[state->sp - 2] = pc_low;

    state->sp -= 2;
    state->pc = adr - 1;

    return;
}

// PCHL: PC.hi <- H; PC.lo <- L
// Flags: None
// Op Size: 1
void PchlOpCode(State8080 *state) {
    uint16_t new_pc = GetMemoryAdr(state->h, state->l);
    state->pc = new_pc - 1;

    return;
}

// ----------------------------------------
// --- Stack, IO, Machine Control Funcs ---
// ----------------------------------------

// PUSH: (sp-2)<-low_reg; (sp-1)<-high_reg; sp <- sp - 2
// Flags: None
// Op Size: 1
void PushOpCode(State8080 *state, uint8_t high_reg, uint8_t low_reg) {
    uint8_t *high_reg_sink = &state->memory[state->sp-1];
    uint8_t *low_reg_sink = &state->memory[state->sp-2];

    *high_reg_sink = high_reg;
    *low_reg_sink = low_reg;

    state->sp -= 2;
    return;
}

// PUSH PSW: (sp-2)<-flags; (sp-1)<-A; sp <- sp - 2
// Flags: None
// Op Size: 1
void PushPswOpCode(State8080 *state) {
    uint8_t flags_byte =
        (state->cc.s  << 7) |
        (state->cc.z  << 6) |
        (0            << 5) |
        (state->cc.ac << 4) |
        (0            << 3) |
        (state->cc.p  << 2) |
        (1            << 1) |
        (state->cc.cy << 0);

    state->memory[state->sp - 2] = flags_byte;
    state->memory[state->sp - 1] = state->a;    
    state->sp -= 2;
    return;
}

// POP: low_reg <- (sp); high_reg <- (sp+1); sp <- sp+2
// Flags: None
// Op Size: 1
void PopOpCode(State8080 *state, uint8_t *high_reg, uint8_t *low_reg) {
    *high_reg = state->memory[state->sp + 1];
    *low_reg = state->memory[state->sp];

    state->sp += 2;
    return;
}

// POP PSW: flags <- (sp); A <- (sp+1); sp <- sp+2
// Flags: None
// Op Size: 1
void PopPswOpCode(State8080 *state) {
    uint8_t flag_byte = state->memory[state->sp];

    state->cc.cy = (flag_byte & 0x01) != 0;  // SP0 -> CY, 0th-bit bitmask = 0x01
    state->cc.p  = (flag_byte & 0x04) != 0;  // SP2 -> P,  2nd-bit bitmask = 0x04
    state->cc.ac = (flag_byte & 0x10) != 0;  // SP4 -> AC, 4th-bit bitmask = 0x10
    state->cc.z  = (flag_byte & 0x40) != 0;  // SP6 -> Z,  6th-bit bitmask = 0x40
    state->cc.s  = (flag_byte & 0x80) != 0;  // SP7 -> S,  7th-bit bitmask = 0x80
    
    state->a = state->memory[state->sp + 1];
    state->sp += 2;
    return;
}

// XTHL: L <-> (SP); H <-> (SP+1)
// Flags: None
// Op Size: 1
void XthlOpCode(State8080 *state) {
    uint8_t temp_l_reg = state->l;
    uint8_t temp_h_reg = state->h;

    uint8_t sp_low = state->memory[state->sp];
    uint8_t sp_high = state->memory[state->sp + 1];

    state->l = sp_low;
    state->h = sp_high;
    state->memory[state->sp] = temp_l_reg;
    state->memory[state->sp + 1] = temp_h_reg;
    return;
}

// SPHL: SP=HL
// Flags: None
// Op Size: 1
void SphlOpCode(State8080 *state) {
    uint16_t hl_adr = GetMemoryAdr(state->h, state->l);
    state->sp = hl_adr;

    return;
}

// IN: special
// Flags: None
// Op Size: 2
void InOpCode(State8080 *state, uint8_t byte_2) {
    // TODO
    printf("%x", byte_2);
    return;
}

// OUT: special
// Flags: None
// Op Size: 2
void OutOpCode(State8080 *state, uint8_t byte_2) {
    // TODO
    printf("%x", byte_2);
    return;
}

// EI: Enable Interrupt Flag
// Flags: None
// Op Size: 1
void EiOpCode(State8080 *state) {
    state->int_enable = 1;
    return;
}

// DI: Clear Interrupt Flag
// Flags: None
// Op Size: 1
void DiOpCode(State8080 *state) {
    state->int_enable = 0;
    return;
}

// HLT: special
// Flags: None
// Op Size: 1
void HltOpCode() {
    exit(0);
}

// NOP: Do Nothing
// Flags: None
// Op Size: 1
void NopOpCode() {
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
            DadOpCode(state, GetMemoryAdr(state->b, state->c));
            break;
        case 0x0A: // LDAX B
            LdaxOpCode(state, GetMemoryAdr(state->b, state->c));
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
            DadOpCode(state, GetMemoryAdr(state->d, state->e));
            break;
        case 0x1A: // LDAX D
            LdaxOpCode(state, GetMemoryAdr(state->d, state->e));
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
            ShldOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
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
            DaaOpCode(state);
            break;
        case 0x28: // NOP
            NopOpCode();
            break;
        case 0x29: // DAD H
            DadOpCode(state, GetMemoryAdr(state->h, state->l));
            break;
        case 0x2A: // LHLD adr
            LhldOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
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
            CmaOpCode(state);
            break;
        case 0x30: // NOP
            NopOpCode();
            break;
        case 0x31: // LXI SP, D16
            LxiStackPointerOpCode(state, opcode[1], opcode[2]);
            break;
        case 0x32: // STA adr
            StaOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0x33: // INX SP
            InxStackPointerOpCode(state);
            break;
        case 0x34: // INR M
            InrOpCode(state, &state->memory[GetMemoryAdr(state->h, state->l)]);
            break;
        case 0x35: // DCR M
            DcrOpCode(state, &state->memory[GetMemoryAdr(state->h, state->l)]);
            break;
        case 0x36: // MVI M, D8
            MviOpCode(state, &state->memory[GetMemoryAdr(state->h, state->l)], opcode[1]);
            break;
        case 0x37: // STC
            StcOpCode(state);
            break;
        case 0x38: // NOP
            NopOpCode();
            break;
        case 0x39: // DAD SP
            DadOpCode(state, state->sp);
            break;
        case 0x3A: // LDA adr
            LdaOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
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
            CmcOpCode(state);
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
            MovOpCode(&state->b, &state->memory[GetMemoryAdr(state->h, state->l)]);
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
            MovOpCode(&state->c, &state->memory[GetMemoryAdr(state->h, state->l)]);
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
            MovOpCode(&state->d, &state->memory[GetMemoryAdr(state->h, state->l)]);
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
            MovOpCode(&state->e, &state->memory[GetMemoryAdr(state->h, state->l)]);
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
            MovOpCode(&state->h, &state->memory[GetMemoryAdr(state->h, state->l)]);
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
            MovOpCode(&state->l, &state->memory[GetMemoryAdr(state->h, state->l)]);
            break;
        case 0x6F: // MOV L, A
            MovOpCode(&state->l, &state->a);
            break;
        case 0x70: // MOV M, B
            MovOpCode(&state->memory[GetMemoryAdr(state->h, state->l)], &state->b);
            break;
        case 0x71: // MOV M, C
            MovOpCode(&state->memory[GetMemoryAdr(state->h, state->l)], &state->c);
            break;
        case 0x72: // MOV M, D
            MovOpCode(&state->memory[GetMemoryAdr(state->h, state->l)], &state->d);
            break;
        case 0x73: // MOV M, E
            MovOpCode(&state->memory[GetMemoryAdr(state->h, state->l)], &state->e);
            break;
        case 0x74: // MOV M, H
            MovOpCode(&state->memory[GetMemoryAdr(state->h, state->l)], &state->h);
            break;
        case 0x75: // MOV M, L
            MovOpCode(&state->memory[GetMemoryAdr(state->h, state->l)], &state->l);
            break;
        case 0x76: // HLT
            HltOpCode();
            break;
        case 0x77: // MOV M, A
            MovOpCode(&state->memory[GetMemoryAdr(state->h, state->l)], &state->a);
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
            MovOpCode(&state->a, &state->memory[GetMemoryAdr(state->h, state->l)]);
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
            AddOpCode(state, &state->memory[GetMemoryAdr(state->h, state->l)]);
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
            AdcOpCode(state, &state->memory[GetMemoryAdr(state->h, state->l)]);
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
            SubOpCode(state, &state->memory[GetMemoryAdr(state->h, state->l)]);
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
            SbbOpCode(state, &state->memory[GetMemoryAdr(state->h, state->l)]);
            break;
        case 0x9F: // SBB A
            SbbOpCode(state, &state->a);
            break;
        case 0xA0: // ANA B
            AnaOpCode(state, &state->b);
            break;
        case 0xA1: // ANA C
            AnaOpCode(state, &state->c);
            break;
        case 0xA2: // ANA D
            AnaOpCode(state, &state->d);
            break;
        case 0xA3: // ANA E
            AnaOpCode(state, &state->e);
            break;
        case 0xA4: // ANA H
            AnaOpCode(state, &state->h);
            break;
        case 0xA5: // ANA L
            AnaOpCode(state, &state->l);
            break;
        case 0xA6: // ANA M
            AnaOpCode(state, &state->memory[GetMemoryAdr(state->h, state->l)]);
            break;
        case 0xA7: // ANA A
            AnaOpCode(state, &state->a);
            break;
        case 0xA8: // XRA B
            XraOpCode(state, &state->b);
            break;
        case 0xA9: // XRA C
            XraOpCode(state, &state->c);
            break;
        case 0xAA: // XRA D
            XraOpCode(state, &state->d);
            break;
        case 0xAB: // XRA E
            XraOpCode(state, &state->e);
            break;
        case 0xAC: // XRA H
            XraOpCode(state, &state->h);
            break;
        case 0xAD: // XRA L
            XraOpCode(state, &state->l);
            break;
        case 0xAE: // XRA M
            XraOpCode(state, &state->memory[GetMemoryAdr(state->h, state->l)]);
            break;
        case 0xAF: // XRA A
            XraOpCode(state, &state->a);
            break;
        case 0xB0: // ORA B
            OraOpCode(state, &state->b);
            break;
        case 0xB1: // ORA C
            OraOpCode(state, &state->c);
            break;
        case 0xB2: // ORA D
            OraOpCode(state, &state->d);
            break;
        case 0xB3: // ORA E
            OraOpCode(state, &state->e);
            break;
        case 0xB4: // ORA H
            OraOpCode(state, &state->h);
            break;
        case 0xB5: // ORA L
            OraOpCode(state, &state->l);
            break;
        case 0xB6: // ORA M
            OraOpCode(state, &state->memory[GetMemoryAdr(state->h, state->l)]);
            break;
        case 0xB7: // ORA A
            OraOpCode(state, &state->a);
            break;
        case 0xB8: // CMP B
            CmpOpCode(state, state->b);
            break;
        case 0xB9: // CMP C
            CmpOpCode(state, state->c);
            break;
        case 0xBA: // CMP D
            CmpOpCode(state, state->d);
            break;
        case 0xBB: // CMP E
            CmpOpCode(state, state->e);
            break;
        case 0xBC: // CMP H
            CmpOpCode(state, state->h);
            break;
        case 0xBD: // CMP L
            CmpOpCode(state, state->l);
            break;
        case 0xBE: // CMP M
            CmpOpCode(state, state->memory[GetMemoryAdr(state->h, state->l)]);
            break;
        case 0xBF: // CMP A
            CmpOpCode(state, state->a);
            break;
        case 0xC0: // RNZ
            RnzOpCode(state);
            break;
        case 0xC1: // POP B
            PopOpCode(state, &state->b, &state->c);
            break;
        case 0xC2: // JNZ adr
            JnzOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xC3: // JMP adr
            JmpOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xC4: // CNZ adr
            CnzOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xC5: // PUSH B
            PushOpCode(state, state->b, state->c);
            break;
        case 0xC6: // ADI D8
            AdiOpCode(state, opcode[1]);
            break;
        case 0xC7: // RST 0
            RstOpCode(state, 0x00);
            break;
        case 0xC8: // RZ
            RzOpCode(state);
            break;
        case 0xC9: // RET
            RetOpCode(state);
            break;
        case 0xCA: // JZ adr
            JzOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xCB: // JMP adr
            JmpOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xCC: // CZ adr
            CzOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xCD: // CALL adr
            CallOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xCE: // ACI D8
            AciOpCode(state, opcode[1]);
            break;
        case 0xCF: // RST 1
            RstOpCode(state, 0x08);
            break;
        case 0xD0: // RNC
            RncOpCode(state);
            break;
        case 0xD1: // POP D
            PopOpCode(state, &state->d, &state->e);
            break;
        case 0xD2: // JNC adr
            JncOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xD3: // OUT D8
            OutOpCode(state, opcode[1]);
            break;
        case 0xD4: // CNC adr
            CncOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xD5: // PUSH D
            PushOpCode(state, state->d, state->e);
            break;
        case 0xD6: // SUI D8
            SuiOpCode(state, opcode[1]);
            break;
        case 0xD7: // RST 2
            RstOpCode(state, 0x10);
            break;
        case 0xD8: // RC
            RcOpCode(state);
            break;
        case 0xD9: // RET
            RetOpCode(state);
            break;
        case 0xDA: // JC adr
            JcOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xDB: // IN D8
            InOpCode(state, opcode[1]);
            break;
        case 0xDC: // CC adr
            CcOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xDD: // CALL adr
            CallOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xDE: // SBI D8
            SbiOpCode(state, opcode[1]);
            break;
        case 0xDF: // RST 3
            RstOpCode(state, 0x18);
            break;
        case 0xE0: // RPO
            RpoOpCode(state);
            break;
        case 0xE1: // POP H
            PopOpCode(state, &state->h, &state->l);
            break;
        case 0xE2: // JPO adr
            JpoOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xE3: // XTHL
            XthlOpCode(state);
            break;
        case 0xE4: // CPO adr
            CpoOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xE5: // PUSH H
            PushOpCode(state, state->h, state->l);
            break;
        case 0xE6: // ANI D8
            AniOpCode(state, opcode[1]);
            break;
        case 0xE7: // RST 4
            RstOpCode(state, 0x20);
            break;
        case 0xE8: // RPE
            RpeOpCode(state);
            break;
        case 0xE9: // PCHL
            PchlOpCode(state);
            break;
        case 0xEA: // JPE adr
            JpeOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xEB: // XCHG
            XchgOpCode(state);
            break;
        case 0xEC: // CPE adr
            CpeOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xED: // CALL adr
            CallOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xEE: // XRI D8
            XriOpCode(state, opcode[1]);
            break;
        case 0xEF: // RST 5
            RstOpCode(state, 0x28);
            break;
        case 0xF0: // RP
            RpOpCode(state);
            break;
        case 0xF1: // POP PSW
            PopPswOpCode(state);
            break;
        case 0xF2: // JP adr
            JpOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xF3: // DI
            DiOpCode(state);
            break;
        case 0xF4: // CP adr
            CpOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xF5: // PUSH PSW
            PushPswOpCode(state);
            break;
        case 0xF6: // ORI D8
            OriOpCode(state, opcode[1]);
            break;
        case 0xF7: // RST 6
            RstOpCode(state, 0x30);
            break;
        case 0xF8: // RM
            RmOpCode(state);
            break;
        case 0xF9: // SPHL
            SphlOpCode(state);
            break;
        case 0xFA: // JM adr
            JmOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xFB: // EI
            EiOpCode(state);
            break;
        case 0xFC: // CM adr
            CmOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xFD: // CALL adr
            CallOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xFE: // CPI D8
            CpiOpCode(state, opcode[1]);
            break;
        case 0xFF: // RST 7
            RstOpCode(state, 0x38);
            break;
    }
    state->pc+=1;  //for the opcode

    return 0;
}

State8080 *CreateTestState(uint16_t pc_init) {
    const uint8_t NULL_8_BIT = 0x00;

    State8080 *state = malloc(sizeof(State8080));
    if (state == NULL) {
        return NULL;
    }

    state->memory = calloc(65536, sizeof(uint8_t)); // calloc zeroes out the ROM/RAM space
    if (state->memory == NULL) {
        free(state); // Clean up container if memory array allocation fails
        return NULL;
    }

    // 3. Initialize General Purpose Registers (8-bit)
    state->a = NULL_8_BIT;
    state->b = NULL_8_BIT;
    state->c = NULL_8_BIT;
    state->d = NULL_8_BIT;
    state->e = NULL_8_BIT;
    state->h = NULL_8_BIT;
    state->l = NULL_8_BIT;

    // 4. Initialize Address Registers (16-bit)
    state->pc = pc_init;  // Code execution normally starts at address 0
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

void LoadRomIntoState(State8080 *state, char *buffer, int buffer_len) {
    int b = 0;
    for (int i = state->pc; i < (state->pc + buffer_len); i++) {
        state->memory[i] = buffer[b];
        b++;
    }

    return;
}

void FreeState(State8080 *state) {
    if (state != NULL) {
        free(state->memory);
        free(state);
    }
}
