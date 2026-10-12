#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "cpu.h"
#include "disassembler.h"

int is_test_cpu = 0;

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

void HandleZeroSignParityFlags(Cpu8080 *state, uint16_t value) {
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
// Cycles: 5
int MovOpCode(uint8_t *dest_reg, uint8_t *send_reg) {
    *dest_reg = *send_reg;
    return 5;
}

// MOV: dest_reg <- send_reg (using a memory address)
// Flags: None
// Op Size: 1
// Cycles: 7
int MovMemOpCode(uint8_t *dest_reg, uint8_t *send_reg) {
    *dest_reg = *send_reg;
    return 7;
}

// MVI: dest_reg <- byte 2
// Flags: None
// Op Size: 2
// Cycles: 7
int MviOpCode(Cpu8080 *state, uint8_t *dest_reg, uint8_t byte_2) {
    *dest_reg = byte_2;
    state->pc += 1;
    return 7;
}

// MVI: dest_reg <- byte 2 (With Memory Address)
// Flags: None
// Op Size: 2
// Cycles: 10
int MviMemOpCode(Cpu8080 *state, uint8_t *dest_reg, uint8_t byte_2) {
    *dest_reg = byte_2;
    state->pc += 1;
    return 10;
}

// LXI: high_reg <- byte 2, low_reg <- byte 3
// Flags: None
// Op Size: 3
// Cycles: 10
int LxiOpCode(Cpu8080 *state, uint8_t *high_reg, uint8_t *low_reg, uint8_t byte_2, uint8_t byte_3) {
    *high_reg = byte_3;
    *low_reg = byte_2;
    
    state->pc += 2;
    return 10;
}

// LXI Stack Pointer: SP.hi <- byte 3, SP.lo <- byte 2
// Flags: None
// Op Size: 3
// Cycles: 10
int LxiStackPointerOpCode(Cpu8080 *state, uint8_t byte_2, uint8_t byte_3) {
    uint16_t adr = GetMemoryAdr(byte_3, byte_2);
    state->sp = adr;
    state->pc += 2;
    return 10;
}

// LDA: A <- (adr)
// Flags: None
// Op Size: 3
// Cycles: 13
int LdaOpCode(Cpu8080 *state, uint16_t adr) {
    state->a = state->memory[adr];

    state->pc += 2;
    return 13;
}

// STA: (adr) <- A
// Flags: None
// Op Size: 3
// Cycles: 13
int StaOpCode(Cpu8080 *state, uint16_t adr) {
    state->memory[adr] = state->a;

    state->pc += 2;
    return 13;
}

// LHLD: L <- (adr); H<-(adr+1)
// Flags: None
// Op Size: 3
// Cycles: 16
int LhldOpCode(Cpu8080 *state, uint16_t adr) {
    state->l = state->memory[adr];
    state->h = state->memory[adr + 1];

    state->pc += 2;
    return 16;
}

// SHLD: (adr) <-L; (adr+1)<-H
// Flags: None
// Op Size: 3
// Cycles: 16
int ShldOpCode(Cpu8080 *state, uint16_t adr) {
    state->memory[adr] = state->l;
    state->memory[adr + 1] = state->h;

    state->pc += 2;
    return 16;
}

// LDAX: A <- (reg_pair)
// Flags: None
// Op Size: 1
// Cycles: 7
int LdaxOpCode(Cpu8080 *state, uint16_t reg_pair) {
    state->a = state->memory[reg_pair];
    return 7;
}

// STAX: ([high_reg][low_reg]) <- A
// Flags: None
// Op Size: 1
// Cycles: 7
int StaxOpCode(Cpu8080 *state, uint8_t high_reg, uint8_t low_reg) {
    uint16_t adr = GetMemoryAdr(high_reg, low_reg);

    state->memory[adr] = state->a;
    return 7;
}

// XCHG: H <-> D; L <-> E
// Flags: None
// Op Size: 1
// Cycles: 4
int XchgOpCode(Cpu8080 *state) {
    uint8_t temp_d_reg = state->d;
    uint8_t temp_e_reg = state->e;
    uint8_t temp_h_reg = state->h;
    uint8_t temp_l_reg = state->l;

    state->h = temp_d_reg;
    state->d = temp_h_reg;

    state->l = temp_e_reg;
    state->e = temp_l_reg;

    return 4;
}

// --------------------------------
// ------- Arithmetic Funcs -------
// --------------------------------

// ADD: A <- A + reg
// Flags: Z, S, P, CY, AC
// Op Size: 1
// Cycles: 4
int AddOpCode(Cpu8080 *state, uint8_t *reg) { 
    // Cast to 16-bit to more easily check for a carry
    uint16_t answer = (uint16_t) state->a + (uint16_t) *reg;    

    HandleZeroSignParityFlags(state, answer);
    state->cc.cy = (answer > 0xff);
    state->cc.ac = (((state->a & 0x0F) + (*reg & 0x0F)) > 0x0F);

    state->a = (uint8_t)(answer & 0xff);
    return 4;
}

// ADD: A <- A + Memory Reg
// Flags: Z, S, P, CY, AC
// Op Size: 1
// Cycles: 7
int AddMemOpCode(Cpu8080 *state, uint8_t *reg) { 
    // Cast to 16-bit to more easily check for a carry
    uint16_t answer = (uint16_t) state->a + (uint16_t) *reg;    

    HandleZeroSignParityFlags(state, answer);
    state->cc.cy = (answer > 0xff);
    state->cc.ac = (((state->a & 0x0F) + (*reg & 0x0F)) > 0x0F);

    state->a = (uint8_t)(answer & 0xff);
    return 7;
}

// ADI: A <- A + added_byte
// Flags: Z, S, P, CY, AC
// Op Size: 2
// Cycles: 7
int AdiOpCode(Cpu8080 *state, uint8_t byte_2) {
    // Cast to 16-bit to more easily check for a carry
    uint16_t answer = (uint16_t) state->a + (uint16_t) byte_2;    

    HandleZeroSignParityFlags(state, answer);
    state->cc.cy = (answer > 0xff);
    state->cc.ac = (((state->a & 0x0F) + (byte_2 & 0x0F)) > 0x0F);

    state->a = (uint8_t)(answer & 0xff);
    state->pc += 1;
    return 7;
}

// ADC: A <- A + (reg) + CY
// Flags: Z, S, P, CY, AC
// Op Size: 1
// Cycles: 4
int AdcOpCode(Cpu8080 *state, uint8_t *reg) {
    uint8_t carry_flag = state->cc.cy;

    // Cast to 16-bit to more easily check for a carry
    uint16_t answer = (uint16_t) state->a + (uint16_t) *reg + (uint16_t) carry_flag;    

    HandleZeroSignParityFlags(state, answer);
    state->cc.cy = (answer > 0xff);
    state->cc.ac = (((state->a & 0x0F) + (*reg & 0x0F) + carry_flag) > 0x0F);

    state->a = (uint8_t)(answer & 0xff);
    return 4;
}

// ADC: A <- A + (Memory Addr) + CY
// Flags: Z, S, P, CY, AC
// Op Size: 1
// Cycles: 7
int AdcMemOpCode(Cpu8080 *state, uint8_t *reg) {
    uint8_t carry_flag = state->cc.cy;

    // Cast to 16-bit to more easily check for a carry
    uint16_t answer = (uint16_t) state->a + (uint16_t) *reg + (uint16_t) carry_flag;    

    HandleZeroSignParityFlags(state, answer);
    state->cc.cy = (answer > 0xff);
    state->cc.ac = (((state->a & 0x0F) + (*reg & 0x0F) + carry_flag) > 0x0F);

    state->a = (uint8_t)(answer & 0xff);
    return 7;
}

// ACI: A <- A + data + CY
// Flags: Z, S, P, CY, AC
// Op Size: 2
// Cycles: 7
int AciOpCode(Cpu8080 *state, uint8_t byte_2) {
    uint8_t carry_flag = state->cc.cy;

    // Cast to 16-bit to more easily check for a carry
    uint16_t answer = (uint16_t) state->a + (uint16_t) byte_2 + (uint16_t) carry_flag;    

    HandleZeroSignParityFlags(state, answer);
    state->cc.cy = (answer > 0xff);
    state->cc.ac = (((state->a & 0x0F) + (byte_2 & 0x0F) + carry_flag) > 0x0F);

    state->a = (uint8_t)(answer & 0xff);
    state->pc += 1;
    return 7;
}

// SUB: A <- A - reg
// Flags: Z, S, P, CY, AC
// Op Size: 1
// Cycles: 4
int SubOpCode(Cpu8080 *state, uint8_t *reg) {
    // Cast to 16-bit to more easily check for a carry
    uint16_t answer = (uint16_t) state->a - (uint16_t) *reg;

    HandleZeroSignParityFlags(state, answer);
    state->cc.cy = ((uint16_t) *reg > (uint16_t) state->a);
    state->cc.ac = ((*reg & 0x0F) > (state->a & 0x0F));

    state->a = (uint8_t)(answer & 0xff);
    return 4;
}

// SUB: A <- A - Memory Reg
// Flags: Z, S, P, CY, AC
// Op Size: 1
// Cycles: 7
int SubMemOpCode(Cpu8080 *state, uint8_t *reg) {
    // Cast to 16-bit to more easily check for a carry
    uint16_t answer = (uint16_t) state->a - (uint16_t) *reg;

    HandleZeroSignParityFlags(state, answer);
    state->cc.cy = ((uint16_t) *reg > (uint16_t) state->a);
    state->cc.ac = ((*reg & 0x0F) > (state->a & 0x0F));

    state->a = (uint8_t)(answer & 0xff);
    return 7;
}

// SUI: A <- A - data
// Flags: Z, S, P, CY, AC
// Op Size: 2
// Cycles: 7
int SuiOpCode(Cpu8080 *state, uint8_t byte_2) {
    // Cast to 16-bit to more easily check for a carry
    uint16_t answer = (uint16_t) state->a - (uint16_t) byte_2;

    HandleZeroSignParityFlags(state, answer);
    state->cc.cy = ((uint16_t) byte_2 > (uint16_t) state->a);
    state->cc.ac = ((byte_2 & 0x0F) > (state->a & 0x0F));

    state->a = (uint8_t)(answer & 0xff);
    state->pc += 1;
    return 7;
}

// SBB: A <- A - (reg) - CY
// Flags: Z, S, P, CY, AC
// Op Size: 1
// Cycles: 4
int SbbOpCode(Cpu8080 *state, uint8_t *reg) {
    uint8_t carry_flag = state->cc.cy;

    // Cast to 16-bit to more easily check for a carry
    uint16_t answer = (uint16_t) state->a - (uint16_t) *reg - (uint16_t) carry_flag;

    HandleZeroSignParityFlags(state, answer);
    state->cc.cy = ((uint16_t) *reg + (uint16_t) carry_flag) > (uint16_t) state->a;
    state->cc.ac = (((*reg & 0x0F) + carry_flag) > (state->a & 0x0F));

    state->a = (uint8_t)(answer & 0xff);
    return 4;
}

// SBB: A <- A - (Memory Reg) - CY
// Flags: Z, S, P, CY, AC
// Op Size: 1
// Cycles: 7
int SbbMemOpCode(Cpu8080 *state, uint8_t *reg) {
    uint8_t carry_flag = state->cc.cy;

    // Cast to 16-bit to more easily check for a carry
    uint16_t answer = (uint16_t) state->a - (uint16_t) *reg - (uint16_t) carry_flag;

    HandleZeroSignParityFlags(state, answer);
    state->cc.cy = ((uint16_t) *reg + (uint16_t) carry_flag) > (uint16_t) state->a;
    state->cc.ac = (((*reg & 0x0F) + carry_flag) > (state->a & 0x0F));

    state->a = (uint8_t)(answer & 0xff);
    return 7;
}

// SBI: A <- A - data - CY
// Flags: Z, S, P, CY, AC
// Op Size: 2
// Cycles: 7
int SbiOpCode(Cpu8080 *state, uint8_t byte_2) {
    uint8_t carry_flag = state->cc.cy;

    // Cast to 16-bit to more easily check for a carry
    uint16_t answer = (uint16_t) state->a - (uint16_t) byte_2 - (uint16_t) carry_flag;

    HandleZeroSignParityFlags(state, answer);
    state->cc.cy = ((uint16_t) byte_2 + (uint16_t) carry_flag) > (uint16_t) state->a;
    state->cc.ac = (((byte_2 & 0x0F) + carry_flag) > (state->a & 0x0F));

    state->a = (uint8_t)(answer & 0xff);
    state->pc += 1;
    return 7;
}

// INR: reg <- reg + 1
// Flags: Z, S, P, AC
// Op Size: 1
// Cycles: 5
int InrOpCode(Cpu8080 *state, uint8_t *reg) {
    uint16_t answer = (uint16_t) *reg + 1;

    HandleZeroSignParityFlags(state, answer);
    state->cc.ac = (((*reg & 0x0F) + 0x01) > 0x0F);

    *reg = answer;
    return 5;
}

// INR: Mem Reg <- Mem Reg + 1
// Flags: Z, S, P, AC
// Op Size: 1
// Cycles: 10
int InrMemOpCode(Cpu8080 *state, uint8_t *reg) {
    uint16_t answer = (uint16_t) *reg + 1;

    HandleZeroSignParityFlags(state, answer);
    state->cc.ac = (((*reg & 0x0F) + 0x01) > 0x0F);

    *reg = answer;
    return 10;
}

// DCR: reg <- reg - 1
// Flags: Z, S, P, AC
// Op Size: 1
// Cycles: 5
int DcrOpCode(Cpu8080 *state, uint8_t *reg) {
    uint16_t answer = (uint16_t) *reg - 1;

    HandleZeroSignParityFlags(state, answer);
    state->cc.ac = ((*reg & 0x0F) == 0x0);

    *reg = answer;
    return 5;
}

// DCR: Mem Reg <- Mem Reg - 1
// Flags: Z, S, P, AC
// Op Size: 1
// Cycles: 10
int DcrMemOpCode(Cpu8080 *state, uint8_t *reg) {
    uint16_t answer = (uint16_t) *reg - 1;

    HandleZeroSignParityFlags(state, answer);
    state->cc.ac = ((*reg & 0x0F) == 0x0);

    *reg = answer;
    return 10;
}

// INX: [Register Pair] <- [Register Pair] + 1
// Flags: None
// Op Size: 1
// Cycles: 5
int InxOpCode(uint8_t *high_reg, uint8_t *low_reg) {
    uint16_t adr = GetMemoryAdr(*high_reg, *low_reg);
    adr += 1;

    *high_reg = (adr >> 8) & 0xFF;
    *low_reg = (adr & 0xFF);
    return 5;
}

// INX: Stack Pointer = Stack Pointer + 1
// Flags: None
// Op Size: 1
// Cycles: 5
int InxStackPointerOpCode(Cpu8080 *state) {
    state->sp += 1;
    return 5;
}

// DCX: [Register Pair] = [Register Pair] - 1
// Flags: None
// Op Size: 1
// Cycles: 5
int DcxOpCode(uint8_t *high_reg, uint8_t *low_reg) {
    uint16_t adr = GetMemoryAdr(*high_reg, *low_reg);
    adr -= 1;

    *high_reg = (adr >> 8) & 0xFF;
    *low_reg = (adr & 0xFF);
    return 5;
}

// DCX: Stack Pointer = Stack Pointer - 1
// Flags: None
// Op Size: 1
// Cycles: 5
int DcxStackPointerOpCode(Cpu8080 *state) {
    state->sp -= 1;
    return 5;
}

// DAD: HL = HL + (reg_pair)
// Flags: CY
// Op Size: 1
// Cycles: 10
int DadOpCode(Cpu8080 *state, uint16_t reg_pair) {
    uint32_t hl_adr = (uint32_t) GetMemoryAdr(state->h, state->l);

    hl_adr = hl_adr + (uint32_t) reg_pair;
    state->cc.cy = (hl_adr > 0xFFFF);

    state->h = (uint8_t) ((hl_adr & 0xFFFF) >> 8);
    state->l = (uint8_t) (hl_adr & 0xFF);
    
    return 10;
}

// DAA: special
// Flags: Z, S, P, CY, AC
// Op Size: 1
// Cycles: 4
int DaaOpCode(Cpu8080 *state) {
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
    return 4;
}

// --------------------------------
// --------- Logical Funcs --------
// --------------------------------

// ANA: A <- A & reg
// Flags: Z, S, P, CY, AC
// Op Size: 1
// Cycles: 4
int AnaOpCode(Cpu8080 *state, uint8_t *reg) {
    uint8_t answer = state->a & *reg;    

    HandleZeroSignParityFlags(state, (uint16_t) answer);
    state->cc.cy = 0; // Clears out CY Flag
    state->cc.ac = ((state->a & 0x8) >> 3) | ((*reg & 0x8) >> 3); // AC is the OR of 3rd-bit from both initial operands

    state->a = (uint8_t) answer;
    return 4;
}

// ANA: A <- A & Mem Reg
// Flags: Z, S, P, CY, AC
// Op Size: 1
// Cycles: 7
int AnaMemOpCode(Cpu8080 *state, uint8_t *reg) {
    uint8_t answer = state->a & *reg;    

    HandleZeroSignParityFlags(state, (uint16_t) answer);
    state->cc.cy = 0; // Clears out CY Flag
    state->cc.ac = ((state->a & 0x8) >> 3) | ((*reg & 0x8) >> 3); // AC is the OR of 3rd-bit from both initial operands

    state->a = (uint8_t) answer;
    return 7;
}

// ANI: A <- A & byte_2
// Flags: Z, S, P, CY, AC
// Op Size: 2
// Cycles: 7
int AniOpCode(Cpu8080 *state, uint8_t byte_2) {
    uint8_t answer = state->a & byte_2;    

    HandleZeroSignParityFlags(state, (uint16_t) answer);
    state->cc.cy = 0; // Clears out CY flag
    state->cc.ac = ((state->a & 0x8) >> 3) | ((byte_2 & 0x8) >> 3); // AC is the OR of 3rd-bit from both initial operands

    state->a = (uint8_t) answer;
    state->pc += 1;
    return 7;
}

// XRA: A <- A ^ reg
// Flags: Z, S, P, CY, AC
// Op Size: 1
// Cycles: 4
int XraOpCode(Cpu8080 *state, uint8_t *reg) {
    uint8_t answer = state->a ^ *reg;    

    HandleZeroSignParityFlags(state, (uint16_t) answer);
    state->cc.cy = 0; // Clears out CY Flag
    state->cc.ac = 0; // Clears out AC Flag

    state->a = (uint8_t) answer;
    return 4;
}

// XRA: A <- A ^ Mem Reg
// Flags: Z, S, P, CY, AC
// Op Size: 1
// Cycles: 7
int XraMemOpCode(Cpu8080 *state, uint8_t *reg) {
    uint8_t answer = state->a ^ *reg;    

    HandleZeroSignParityFlags(state, (uint16_t) answer);
    state->cc.cy = 0; // Clears out CY Flag
    state->cc.ac = 0; // Clears out AC Flag

    state->a = (uint8_t) answer;
    return 7;
}

// 	XRI: A <- A ^ data
// Flags: Z, S, P, CY, AC
// Op Size: 2
// Cycles: 7
int XriOpCode(Cpu8080 *state, uint8_t byte_2) {
    uint8_t answer = state->a ^ byte_2;    

    HandleZeroSignParityFlags(state, (uint16_t) answer);
    state->cc.cy = 0; // Clears out CY Flag
    state->cc.ac = 0; // Clears out AC Flag

    state->a = (uint8_t) answer;
    state->pc += 1;
    return 7;
}

// ORA: A <- A | reg
// Flags: Z, S, P, CY, AC
// Op Size: 1
// Cycles: 4
int OraOpCode(Cpu8080 *state, uint8_t *reg) {
    uint8_t answer = state->a | *reg;

    HandleZeroSignParityFlags(state, (uint16_t) answer);
    state->cc.cy = 0; // Clears out CY Flag
    state->cc.ac = 0; // Clears out AC Flag

    state->a = (uint8_t) answer;
    return 4;
}

// ORA: A <- A | Mem Reg
// Flags: Z, S, P, CY, AC
// Op Size: 1
// Cycles: 7
int OraMemOpCode(Cpu8080 *state, uint8_t *reg) {
    uint8_t answer = state->a | *reg;

    HandleZeroSignParityFlags(state, (uint16_t) answer);
    state->cc.cy = 0; // Clears out CY Flag
    state->cc.ac = 0; // Clears out AC Flag

    state->a = (uint8_t) answer;
    return 7;
}

// ORI: A <- A | byte_2
// Flags: Z, S, P, CY, AC
// Op Size: 2
// Cycles: 7
int OriOpCode(Cpu8080 *state, uint8_t byte_2) {
    uint8_t answer = state->a | byte_2;

    HandleZeroSignParityFlags(state, (uint16_t) answer);
    state->cc.cy = 0; // Clears out CY Flag
    state->cc.ac = 0; // Clears out AC Flag

    state->a = (uint8_t) answer;
    state->pc += 1;
    return 7;
}

// CMP: A - reg
// Flags: Z, S, P, CY, AC
// Op Size: 1
// Cycles: 4
int CmpOpCode(Cpu8080 *state, uint8_t reg) {
    uint8_t answer = state->a - reg;

    HandleZeroSignParityFlags(state, (uint16_t) answer);
    state->cc.cy = (reg > state->a);
    state->cc.ac = ((reg & 0x0F) > (state->a & 0x0F));

    return 4;
}

// CMP: A - Mem Reg
// Flags: Z, S, P, CY, AC
// Op Size: 1
// Cycles: 7
int CmpMemOpCode(Cpu8080 *state, uint8_t reg) {
    uint8_t answer = state->a - reg;

    HandleZeroSignParityFlags(state, (uint16_t) answer);
    state->cc.cy = (reg > state->a);
    state->cc.ac = ((reg & 0x0F) > (state->a & 0x0F));

    return 7;
}

// CPI: A - byte_2
// Flags: Z, S, P, CY, AC
// Op Size: 2
// Cycles: 7
int CpiOpCode(Cpu8080 *state, uint8_t byte_2) {
    uint8_t answer = state->a - byte_2;

    HandleZeroSignParityFlags(state, (uint16_t) answer);
    state->cc.cy = (byte_2 > state->a);
    state->cc.ac = ((byte_2 & 0x0F) > (state->a & 0x0F));

    state->pc +=1;
    return 7;
}

// RLC: A = A << 1; bit 0 = prev bit 7; CY = prev bit 7
// Flags: CY
// Op Size: 1
// Cycles: 4
int RlcOpCode(Cpu8080 *state) {
    uint8_t value = state->a;
    uint8_t bit_seven_value = value >> 7;

    value = value << 1;
    state->cc.cy = bit_seven_value;
    value = value | bit_seven_value;

    state->a = value;
    return 4;
}

// RRC: A = A >> 1; bit 7 = prev bit 0; CY = prev bit 0
// Flags: CY
// Op Size: 1
// Cycles: 4
int RrcOpCode(Cpu8080 *state) {
    uint8_t value = state->a;
    uint8_t bit_zero_value = value & 0x1;

    value = value >> 1;
    state->cc.cy = bit_zero_value;
    value = value | (uint8_t) (bit_zero_value << 7);

    state->a = value;
    return 4;
}

// RAL: A = A << 1; bit 0 = prev CY; CY = prev bit 7
// Flags: CY
// Op Size: 1
// Cycles: 4
int RalOpcode(Cpu8080 *state) {
    uint8_t value = state->a;
    uint8_t bit_seven_value = value >> 7;

    value = value << 1;
    value = value | state->cc.cy;
    state->cc.cy = bit_seven_value;

    state->a = value;
    return 4;
}

// RAR: A = A >> 1; bit 7 = prev CY; CY = prev bit 0
// Flags: CY
// Op Size: 1
// Cycles: 4
int RarOpCode(Cpu8080 *state) {
    uint8_t value = state->a;
    uint8_t bit_zero_value = value & 0x1;

    value = value >> 1;
    value = value | (uint8_t) (state->cc.cy << 7);
    state->cc.cy = bit_zero_value;

    state->a = value;
    return 4;
}

// CMA: A <- !A
// Flags: None
// Op Size: 1
// Cycles: 4
int CmaOpCode(Cpu8080 *state) {
    state->a = ~state->a;
    return 4;
}

// CMC: CY=!CY
// Flags: CY
// Op Size: 1
// Cycles: 4
int CmcOpCode(Cpu8080 *state) {
    state->cc.cy = !state->cc.cy;
    return 4;
}

// STC: CY = 1
// Flags: CY
// Op Size: 1
// Cycles: 4
int StcOpCode(Cpu8080 *state) {
    state->cc.cy = 1;
    return 4;
}

// --------------------------------
// --------- Branch Funcs ---------
// --------------------------------

// JMP: PC <= adr
// Flags: None
// Op Size: 3
// Cycles: 10
int JmpOpCode(Cpu8080 *state, uint16_t adr) {
    state->pc = adr - 1;
    return 10;
}

// JNZ: if Zero Flag == 0, PC <- adr
// Flags: None
// Op Size: 3
// Cycles: 10
int JnzOpCode(Cpu8080 *state, uint16_t adr) {
    if (!state->cc.z) {
        state->pc = adr - 1;
    } else {
        state->pc += 2;
    }

    return 10;
}

// JZ: if Z, PC <- adr
// Flags: None
// Op Size: 3
// Cycles: 10
int JzOpCode(Cpu8080 *state, uint16_t adr) {
    if (state->cc.z) {
        state->pc = adr - 1;
    } else {
        state->pc += 2;
    }

    return 10;
}

// JNC: if NCY, PC<-adr
// Flags: None
// Op Size: 3
// Cycles: 10
int JncOpCode(Cpu8080 *state, uint16_t adr) {
    if (!state->cc.cy) {
        state->pc = adr - 1;
    } else {
        state->pc += 2;
    }

    return 10;
}

// JC: if CY, PC<-adr
// Flags: None
// Op Size: 3
// Cycles: 10
int JcOpCode(Cpu8080 *state, uint16_t adr) {
    if (state->cc.cy) {
        state->pc = adr - 1;
    } else {
        state->pc += 2;
    }

    return 10;
}

// JPO: If Parity Odd (cc.p == 0) PC <- adr
// Flags: None
// Op Size: 3
// Cycles: 10
int JpoOpCode(Cpu8080 *state, uint16_t adr) {
    if (!state->cc.p) {
        state->pc = adr - 1;
    } else {
        state->pc += 2;
    }

    return 10;
}

// JPE: If Parity Even (cc.p == 1) PC <- adr
// Flags: None
// Op Size: 3
// Cycles: 10
int JpeOpCode(Cpu8080 *state, uint16_t adr) {
    if (state->cc.p) {
        state->pc = adr - 1;
    } else {
        state->pc += 2;
    }

    return 10;
}

// JP: If Positive (cc.s == 0), PC <- adr
// Flags: None
// Op Size: 3
// Cycles: 10
int JpOpCode(Cpu8080 *state, uint16_t adr) {
    if (!state->cc.s) {
        state->pc = adr - 1;
    } else {
        state->pc += 2;
    }

    return 10;
}

// JM: If Negative (cc.s == 1), PC <- adr
// Flags: None
// Op Size: 3
// Cycles: 10
int JmOpCode(Cpu8080 *state, uint16_t adr) {
    if (state->cc.s) {
        state->pc = adr - 1;
    } else {
        state->pc += 2;
    }

    return 10;
}

// CALL: (SP-1)<-PC.hi;(SP-2)<-PC.lo;SP<-SP-2;PC=adr
// Flags: None
// Op Size: 3
// Cycles: 17
// NOTE: Includes Debug Test check is the TEST_CPU ifdef is set, used with the diagnostic cpudiag.bin file
int CallOpCode(Cpu8080 *state, uint16_t adr) {
    if (is_test_cpu) {
        if (0x0005 ==  adr) {
            if (state->c == 9) {
                uint16_t offset = (state->d<<8) | (state->e);
                uint8_t *str = &state->memory[offset+3];  //skip the prefix bytes
                while (*str != '$') {
                    printf("%c", *str++);
                }
                printf("\n");
                exit(0);
            } else if (state->c == 2) {
                //saw this in the inspected code, never saw it called
                printf ("print char routine called\n");
                exit(0);
            }
        } else if (0x0000 ==  adr) {
            printf("Ran into 0x000 Address");
            exit(0);
        }
    }

    uint16_t ret_adr = state->pc + 3;

    uint8_t pc_high = (uint8_t) ((ret_adr >> 8) & 0xFF);
    uint8_t pc_low = (uint8_t) (ret_adr & 0xFF);

    state->memory[state->sp - 1] = pc_high;
    state->memory[state->sp - 2] = pc_low;

    state->sp -= 2;
    state->pc = adr - 1;
    

    return 17;
}

// CNZ: if state->cc.z == 0, CALL adr
// Flags: None
// Op Size: 3
// Cycles: 17 on Call, 11 on Not Call
int CnzOpCode(Cpu8080 *state, uint16_t adr) {
    if (!state->cc.z) {
        return CallOpCode(state, adr);
    } else {
        state->pc += 2;
        return 11;
    }
}

// CZ: if state->cc.z == 1, CALL adr
// Flags: None
// Op Size: 3
// Cycles: 17 on Call, 11 on Not Call
int CzOpCode(Cpu8080 *state, uint16_t adr) {
    if (state->cc.z) {
        return CallOpCode(state, adr);
    } else {
        state->pc += 2;
        return 11;
    }
}

// CNC: if NCY, CALL adr
// Flags: None
// Op Size: 3
// Cycles: 17 on Call, 11 on Not Call
int CncOpCode(Cpu8080 *state, uint16_t adr) {
    if (!state->cc.cy) {
        return CallOpCode(state, adr);
    } else {
        state->pc += 2;
        return 11;
    }
}

// CC: if CY, CALL adr
// Flags: None
// Op Size: 3
// Cycles: 17 on Call, 11 on Not Call
int CcOpCode(Cpu8080 *state, uint16_t adr) {
    if (state->cc.cy) {
        return CallOpCode(state, adr);
    } else {
        state->pc += 2;
        return 11;
    }
}

// CPO: if Parity Odd, CALL adr
// Flags: None
// Op Size: 3
// Cycles: 17 on Call, 11 on Not Call
int CpoOpCode(Cpu8080 *state, uint16_t adr) {
    if (!state->cc.p) {
        return CallOpCode(state, adr);
    } else {
        state->pc += 2;
        return 11;
    }
}

// CPE: If Parity Even, CALL adr
// Flags: None
// Op Size: 3
// Cycles: 17 on Call, 11 on Not Call
int CpeOpCode(Cpu8080 *state, uint16_t adr) {
    if (state->cc.p) {
        return CallOpCode(state, adr);
    } else {
        state->pc += 2;
        return 11;
    }
}

// CP: if Positive (cc.s == 0), PC <- adr
// Flags: None
// Op Size: 3
// Cycles: 17 on Call, 11 on Not Call
int CpOpCode(Cpu8080 *state, uint16_t adr) {
    if (!state->cc.s) {
        return CallOpCode(state, adr);
    } else {
        state->pc += 2;
        return 11;
    }
}

// CM: If Negative (cc.s == 1), PC <- adr
// Flags: None
// Op Size: 3
// Cycles: 17 on Call, 11 on Not Call
int CmOpCode(Cpu8080 *state, uint16_t adr) {
    if (state->cc.s) {
        return CallOpCode(state, adr);
    } else {
        state->pc += 2;
        return 11;
    }
}

// RET: PC.lo <- (sp); PC.hi<-(sp+1); SP <- SP+2
// Flags: None
// Op Size: 1
// Cycles: 10
int RetOpCode(Cpu8080 *state) {
    uint8_t pc_low = state->memory[state->sp];
    uint8_t pc_high = state->memory[state->sp + 1];
    uint16_t new_pc_adr = GetMemoryAdr(pc_high, pc_low);

    state->pc = new_pc_adr - 1;
    state->sp += 2;
    return 10;
}

// RNZ: if NZ, RET
// Flags: None
// Op Size: 1
// Cycles: 11 on Return, 5 on Not Return
int RnzOpCode(Cpu8080 *state) {
    if (!state->cc.z) {
        return RetOpCode(state) + 1;
    } else {
        return 5;
    }
}

// RZ: if Z, RET
// Flags: None
// Op Size: 1
// Cycles: 11 on Return, 5 on Not Return
int RzOpCode(Cpu8080 *state) {
    if (state->cc.z) {
        return RetOpCode(state) + 1;
    } else {
        return 5;
    }
}

// RNC:	if NCY, RET
// Flags: None
// Op Size: 1
// Cycles: 11 on Return, 5 on Not Return
int RncOpCode(Cpu8080 *state) {
    if (!state->cc.cy) {
        return RetOpCode(state) + 1;
    } else {
        return 5;
    }
}

// RC: if CY, RET
// Flags: None
// Op Size: 1
// Cycles: 11 on Return, 5 on Not Return
int RcOpCode(Cpu8080 *state) {
    if (state->cc.cy) {
        return RetOpCode(state) + 1;
    } else {
        return 5;
    }
}

// RPO: if Parity Odd (cc.p == 0), RET
// Flags: None
// Op Size: 1
// Cycles: 11 on Return, 5 on Not Return
int RpoOpCode(Cpu8080 *state) {
    if (!state->cc.p) {
        return RetOpCode(state) + 1;
    } else {
        return 5;
    }
}

// RPE: if Parity Even (cc.p == 1), RET
// Flags: None
// Op Size: 1
// Cycles: 11 on Return, 5 on Not Return
int RpeOpCode(Cpu8080 *state) {
    if (state->cc.p) {
        return RetOpCode(state) + 1;
    } else {
        return 5;
    }
}

// RP: If Sign flag Positive (cc.s == 0), RET
// Flags: None
// Op Size: 1
// Cycles: 11 on Return, 5 on Not Return
int RpOpCode(Cpu8080 *state) {
    if (!state->cc.s) {
        return RetOpCode(state) + 1;
    } else {
        return 5;
    }
}

// RM: if negative (cc.s == 1), RET
// Flags: None
// Op Size: 1
// Cycles: 11 on Return, 5 on Not Return
int RmOpCode(Cpu8080 *state) {
    if (state->cc.s) {
        return RetOpCode(state) + 1;
    } else {
        return 5;
    }
}

// RST: CALL hard-coded adr
// Flags: None
// Op Size: 1
// Cycles: 11
int RstOpCode(Cpu8080 *state, uint16_t adr) {
    uint16_t ret_adr = state->pc + 1;

    uint8_t pc_high = (uint8_t) ((ret_adr >> 8) & 0xFF);
    uint8_t pc_low = (uint8_t) (ret_adr & 0xFF);
    
    state->memory[state->sp - 1] = pc_high;
    state->memory[state->sp - 2] = pc_low;

    state->sp -= 2;
    state->pc = adr - 1;

    return 11;
}

// PCHL: PC.hi <- H; PC.lo <- L
// Flags: None
// Op Size: 1
// Cycles: 5
int PchlOpCode(Cpu8080 *state) {
    uint16_t new_pc = GetMemoryAdr(state->h, state->l);
    state->pc = new_pc - 1;

    return 5;
}

// ----------------------------------------
// --- Stack, IO, Machine Control Funcs ---
// ----------------------------------------

// PUSH: (sp-2)<-low_reg; (sp-1)<-high_reg; sp <- sp - 2
// Flags: None
// Op Size: 1
// Cycles: 11
int PushOpCode(Cpu8080 *state, uint8_t high_reg, uint8_t low_reg) {
    uint8_t *high_reg_sink = &state->memory[state->sp-1];
    uint8_t *low_reg_sink = &state->memory[state->sp-2];

    *high_reg_sink = high_reg;
    *low_reg_sink = low_reg;

    state->sp -= 2;
    return 11;
}

// PUSH PSW: (sp-2)<-flags; (sp-1)<-A; sp <- sp - 2
// Flags: None
// Op Size: 1
// Cycles: 11
int PushPswOpCode(Cpu8080 *state) {
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
    return 11;
}

// POP: low_reg <- (sp); high_reg <- (sp+1); sp <- sp+2
// Flags: None
// Op Size: 1
// Cycles: 10
int PopOpCode(Cpu8080 *state, uint8_t *high_reg, uint8_t *low_reg) {
    *high_reg = state->memory[state->sp + 1];
    *low_reg = state->memory[state->sp];

    state->sp += 2;
    return 10;
}

// POP PSW: flags <- (sp); A <- (sp+1); sp <- sp+2
// Flags: None
// Op Size: 1
// Cycles: 10
int PopPswOpCode(Cpu8080 *state) {
    uint8_t flag_byte = state->memory[state->sp];

    state->cc.cy = (flag_byte & 0x01) != 0;  // SP0 -> CY, 0th-bit bitmask = 0x01
    state->cc.p  = (flag_byte & 0x04) != 0;  // SP2 -> P,  2nd-bit bitmask = 0x04
    state->cc.ac = (flag_byte & 0x10) != 0;  // SP4 -> AC, 4th-bit bitmask = 0x10
    state->cc.z  = (flag_byte & 0x40) != 0;  // SP6 -> Z,  6th-bit bitmask = 0x40
    state->cc.s  = (flag_byte & 0x80) != 0;  // SP7 -> S,  7th-bit bitmask = 0x80
    
    state->a = state->memory[state->sp + 1];
    state->sp += 2;
    return 10;
}

// XTHL: L <-> (SP); H <-> (SP+1)
// Flags: None
// Op Size: 1
// Cycles: 18
int XthlOpCode(Cpu8080 *state) {
    uint8_t temp_l_reg = state->l;
    uint8_t temp_h_reg = state->h;

    uint8_t sp_low = state->memory[state->sp];
    uint8_t sp_high = state->memory[state->sp + 1];

    state->l = sp_low;
    state->h = sp_high;
    state->memory[state->sp] = temp_l_reg;
    state->memory[state->sp + 1] = temp_h_reg;
    return 18;
}

// SPHL: SP=HL
// Flags: None
// Op Size: 1
// Cycles: 5
int SphlOpCode(Cpu8080 *state) {
    uint16_t hl_adr = GetMemoryAdr(state->h, state->l);
    state->sp = hl_adr;

    return 5;
}

// IN: special
// Flags: None
// Op Size: 2
// Cycles: 10
int InOpCode(Cpu8080 *state, uint8_t byte_2) {
    if (state->io_bus == NULL) {
        return -1;
    }
    state->a = state->io_bus->read(state->io_bus->hardware_context, byte_2);    
    state->pc += 1;

    return 10;
}

// OUT: special
// Flags: None
// Op Size: 2
// Cycles: 10
int OutOpCode(Cpu8080 *state, uint8_t byte_2) {
    if (state->io_bus == NULL) {
        return -1;
    }
    state->io_bus->write(state->io_bus->hardware_context, byte_2, state->a);
    state->pc += 1;

    return 10;
}

// EI: Enable Interrupt Flag
// Flags: None
// Op Size: 1
// Cycles: 4
int EiOpCode(Cpu8080 *state) {
    state->int_enable = 1;
    return 4;
}

// DI: Clear Interrupt Flag
// Flags: None
// Op Size: 1
// Cycles: 4
int DiOpCode(Cpu8080 *state) {
    state->int_enable = 0;
    return 4;
}

// HLT: special
// Flags: None
// Op Size: 1
// Cycles: 7
int HltOpCode(void) {
    // TODO: Figure out HLT mechanic
    return 7;
}

// NOP: Do Nothing
// Flags: None
// Op Size: 1
// Cycles: 4
int NopOpCode(void) {
    return 4;
}

int Emulate8080Op(Cpu8080* state) {
    int cycles = 0;
    unsigned char *opcode = &state->memory[state->pc];

    switch(*opcode) {
        case 0x00: // NOP
            cycles = NopOpCode();
            break;
        case 0x01: // LXI B D16
            cycles = LxiOpCode(state, &state->b, &state->c, opcode[1], opcode[2]);
            break;
        case 0x02: // STAX B
            cycles = StaxOpCode(state, state->b, state->c);
            break;
        case 0x03: // INX B
            cycles = InxOpCode(&state->b, &state->c);
            break;
        case 0x04: // INR B
            cycles = InrOpCode(state, &state->b);
            break;
        case 0x05: // DCR B
            cycles = DcrOpCode(state, &state->b);
            break;
        case 0x06: // MVI B, D8
            cycles = MviOpCode(state, &state->b, opcode[1]);
            break;
        case 0x07: // RLC
            cycles = RlcOpCode(state);
            break;
        case 0x08: // NOP
            cycles = NopOpCode();
            break;
        case 0x09: // DAD B
            cycles = DadOpCode(state, GetMemoryAdr(state->b, state->c));
            break;
        case 0x0A: // LDAX B
            cycles = LdaxOpCode(state, GetMemoryAdr(state->b, state->c));
            break;
        case 0x0B: // DCX B
            cycles = DcxOpCode(&state->b, &state->c);
            break;
        case 0x0C: // INR C
            cycles = InrOpCode(state, &state->c);
            break;
        case 0x0D: // DCR C
            cycles = DcrOpCode(state, &state->c);
            break;
        case 0x0E: // MVI C, D8
            cycles = MviOpCode(state, &state->c, opcode[1]);
            break;
        case 0x0F: // RRC
            cycles = RrcOpCode(state);
            break;
        case 0x10: // NOP
            cycles = NopOpCode();
            break;
        case 0x11: // LXI D, D16
            cycles = LxiOpCode(state, &state->d, &state->e, opcode[1], opcode[2]);
            break;
        case 0x12: // STAX D
            cycles = StaxOpCode(state, state->d, state->e);
            break;
        case 0x13: // INX D
            cycles = InxOpCode(&state->d, &state->e);
            break;
        case 0x14: // INR D
            cycles = InrOpCode(state, &state->d);
            break;
        case 0x15: // DCR D
            cycles = DcrOpCode(state, &state->d);
            break;
        case 0x16: // MVI D, D8
            cycles = MviOpCode(state, &state->d, opcode[1]);
            break;
        case 0x17: // RAL
            cycles = RalOpcode(state);
            break;
        case 0x18: // NOP
            cycles = NopOpCode();
            break;
        case 0x19: // DAD D
            cycles = DadOpCode(state, GetMemoryAdr(state->d, state->e));
            break;
        case 0x1A: // LDAX D
            cycles = LdaxOpCode(state, GetMemoryAdr(state->d, state->e));
            break;
        case 0x1B: // DCX D
            cycles = DcxOpCode(&state->d, &state->e);
            break;
        case 0x1C: // INR E
            cycles = InrOpCode(state, &state->e);
            break;
        case 0x1D: // DCR E
            cycles = DcrOpCode(state, &state->e);
            break;
        case 0x1E: // MVI E, D8
            cycles = MviOpCode(state, &state->e, opcode[1]);
            break;
        case 0x1F: // RAR
            cycles = RarOpCode(state);
            break;
        case 0x20: // NOP
            cycles = NopOpCode();
            break;
        case 0x21: // LXI H, D16
            cycles = LxiOpCode(state, &state->h, &state->l, opcode[1], opcode[2]);
            break;
        case 0x22: // SHLD adr
            cycles = ShldOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0x23: // INX H
            cycles = InxOpCode(&state->h, &state->l);
            break;
        case 0x24: // INR H
            cycles = InrOpCode(state, &state->h);
            break;
        case 0x25: // DCR H
            cycles = DcrOpCode(state, &state->h);
            break;
        case 0x26: // MVI H, D8
            cycles = MviOpCode(state, &state->h, opcode[1]);
            break;
        case 0x27: // DAA
            cycles = DaaOpCode(state);
            break;
        case 0x28: // NOP
            cycles = NopOpCode();
            break;
        case 0x29: // DAD H
            cycles = DadOpCode(state, GetMemoryAdr(state->h, state->l));
            break;
        case 0x2A: // LHLD adr
            cycles = LhldOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0x2B: // DCX H
            cycles = DcxOpCode(&state->h, &state->l);
            break;
        case 0x2C: // INR L
            cycles = InrOpCode(state, &state->l);
            break;
        case 0x2D: // DCR L
            cycles = DcrOpCode(state, &state->l);
            break;
        case 0x2E: // MVI L, D8
            cycles = MviOpCode(state, &state->l, opcode[1]);
            break;
        case 0x2F: // CMA
            cycles = CmaOpCode(state);
            break;
        case 0x30: // NOP
            cycles = NopOpCode();
            break;
        case 0x31: // LXI SP, D16
            cycles = LxiStackPointerOpCode(state, opcode[1], opcode[2]);
            break;
        case 0x32: // STA adr
            cycles = StaOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0x33: // INX SP
            cycles = InxStackPointerOpCode(state);
            break;
        case 0x34: // INR M
            cycles = InrMemOpCode(state, &state->memory[GetMemoryAdr(state->h, state->l)]);
            break;
        case 0x35: // DCR M
            cycles = DcrMemOpCode(state, &state->memory[GetMemoryAdr(state->h, state->l)]);
            break;
        case 0x36: // MVI M, D8
            cycles = MviMemOpCode(state, &state->memory[GetMemoryAdr(state->h, state->l)], opcode[1]);
            break;
        case 0x37: // STC
            cycles = StcOpCode(state);
            break;
        case 0x38: // NOP
            cycles = NopOpCode();
            break;
        case 0x39: // DAD SP
            cycles = DadOpCode(state, state->sp);
            break;
        case 0x3A: // LDA adr
            cycles = LdaOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0x3B: // DCX SP
            cycles = DcxStackPointerOpCode(state);
            break;
        case 0x3C: // INR A
            cycles = InrOpCode(state, &state->a);
            break;
        case 0x3D: // DCR A
            cycles = DcrOpCode(state, &state->a);
            break;
        case 0x3E: // MVI A, D8
            cycles = MviOpCode(state, &state->a, opcode[1]);
            break;
        case 0x3F: // CMC
            cycles = CmcOpCode(state);
            break;
        case 0x40: // MOV B, B
            cycles = MovOpCode(&state->b, &state->b);
            break;
        case 0x41: // MOV B, C
            cycles = MovOpCode(&state->b, &state->c);
            break;
        case 0x42: // MOV B, D
            cycles = MovOpCode(&state->b, &state->d);
            break;
        case 0x43: // MOV B, E
            cycles = MovOpCode(&state->b, &state->e);
            break;
        case 0x44: // MOV B, H
            cycles = MovOpCode(&state->b, &state->h);
            break;
        case 0x45: // MOV B, L
            cycles = MovOpCode(&state->b, &state->l);
            break;
        case 0x46: // MOV B, M
            cycles = MovMemOpCode(&state->b, &state->memory[GetMemoryAdr(state->h, state->l)]);
            break;
        case 0x47: // MOV B, A
            cycles = MovOpCode(&state->b, &state->a);
            break;
        case 0x48: // MOV C, B
            cycles = MovOpCode(&state->c, &state->b);
            break;
        case 0x49: // MOV C, C
            cycles = MovOpCode(&state->c, &state->c);
            break;
        case 0x4A: // MOV C, D
            cycles = MovOpCode(&state->c, &state->d);
            break;
        case 0x4B: // MOV C, E
            cycles = MovOpCode(&state->c, &state->e);
            break;
        case 0x4C: // MOV C, H
            cycles = MovOpCode(&state->c, &state->h);
            break;
        case 0x4D: // MOV C, L
            cycles = MovOpCode(&state->c, &state->l);
            break;
        case 0x4E: // MOV C, M
            cycles = MovMemOpCode(&state->c, &state->memory[GetMemoryAdr(state->h, state->l)]);
            break;
        case 0x4F: // MOV C, A
            cycles = MovOpCode(&state->c, &state->a);
            break;
        case 0x50: // MOV D, B
            cycles = MovOpCode(&state->d, &state->b);
            break;
        case 0x51: // MOV D, C
            cycles = MovOpCode(&state->d, &state->c);
            break;
        case 0x52: // MOV D, D
            cycles = MovOpCode(&state->d, &state->d);
            break;
        case 0x53: // MOV D, E
            cycles = MovOpCode(&state->d, &state->e);
            break;
        case 0x54: // MOV D, H
            cycles = MovOpCode(&state->d, &state->h);
            break;
        case 0x55: // MOV D, L
            cycles = MovOpCode(&state->d, &state->l);
            break;
        case 0x56: // MOV D, M
            cycles = MovMemOpCode(&state->d, &state->memory[GetMemoryAdr(state->h, state->l)]);
            break;
        case 0x57: // MOV D, A
            cycles = MovOpCode(&state->d, &state->a);
            break;
        case 0x58: // MOV E, B
            cycles = MovOpCode(&state->e, &state->b);
            break;
        case 0x59: // MOV E, C
            cycles = MovOpCode(&state->e, &state->c);
            break;
        case 0x5A: // MOV E, D
            cycles = MovOpCode(&state->e, &state->d);
            break;
        case 0x5B: // MOV E, E
            cycles = MovOpCode(&state->e, &state->e);
            break;
        case 0x5C: // MOV E, H
            cycles = MovOpCode(&state->e, &state->h);
            break;
        case 0x5D: // MOV E, L
            cycles = MovOpCode(&state->e, &state->l);
            break;
        case 0x5E: // MOV E, M
            cycles = MovMemOpCode(&state->e, &state->memory[GetMemoryAdr(state->h, state->l)]);
            break;
        case 0x5F: // MOV E, A
            cycles = MovOpCode(&state->e, &state->a);
            break;
        case 0x60: // MOV H, B
            cycles = MovOpCode(&state->h, &state->b);
            break;
        case 0x61: // MOV H, C
            cycles = MovOpCode(&state->h, &state->c);
            break;
        case 0x62: // MOV H, D
            cycles = MovOpCode(&state->h, &state->d);
            break;
        case 0x63: // MOV H, E
            cycles = MovOpCode(&state->h, &state->e);
            break;
        case 0x64: // MOV H, H
            cycles = MovOpCode(&state->h, &state->h);
            break;
        case 0x65: // MOV H, L
            cycles = MovOpCode(&state->h, &state->l);
            break;
        case 0x66: // MOV H, M
            cycles = MovMemOpCode(&state->h, &state->memory[GetMemoryAdr(state->h, state->l)]);
            break;
        case 0x67: // MOV H, A
            cycles = MovOpCode(&state->h, &state->a);
            break;
        case 0x68: // MOV L, B
            cycles = MovOpCode(&state->l, &state->b);
            break;
        case 0x69: // MOV L, C
            cycles = MovOpCode(&state->l, &state->c);
            break;
        case 0x6A: // MOV L, D
            cycles = MovOpCode(&state->l, &state->d);
            break;
        case 0x6B: // MOV L, E
            cycles = MovOpCode(&state->l, &state->e);
            break;
        case 0x6C: // MOV L, H
            cycles = MovOpCode(&state->l, &state->h);
            break;
        case 0x6D: // MOV L, L
            cycles = MovOpCode(&state->l, &state->l);
            break;
        case 0x6E: // MOV L, M
            cycles = MovMemOpCode(&state->l, &state->memory[GetMemoryAdr(state->h, state->l)]);
            break;
        case 0x6F: // MOV L, A
            cycles = MovOpCode(&state->l, &state->a);
            break;
        case 0x70: // MOV M, B
            cycles = MovMemOpCode(&state->memory[GetMemoryAdr(state->h, state->l)], &state->b);
            break;
        case 0x71: // MOV M, C
            cycles = MovMemOpCode(&state->memory[GetMemoryAdr(state->h, state->l)], &state->c);
            break;
        case 0x72: // MOV M, D
            cycles = MovMemOpCode(&state->memory[GetMemoryAdr(state->h, state->l)], &state->d);
            break;
        case 0x73: // MOV M, E
            cycles = MovMemOpCode(&state->memory[GetMemoryAdr(state->h, state->l)], &state->e);
            break;
        case 0x74: // MOV M, H
            cycles = MovMemOpCode(&state->memory[GetMemoryAdr(state->h, state->l)], &state->h);
            break;
        case 0x75: // MOV M, L
            cycles = MovMemOpCode(&state->memory[GetMemoryAdr(state->h, state->l)], &state->l);
            break;
        case 0x76: // HLT
            cycles = HltOpCode();
            break;
        case 0x77: // MOV M, A
            cycles = MovMemOpCode(&state->memory[GetMemoryAdr(state->h, state->l)], &state->a);
            break;
        case 0x78: // MOV A, B
            cycles = MovOpCode(&state->a, &state->b);
            break;
        case 0x79: // MOV A, C
            cycles = MovOpCode(&state->a, &state->c);
            break;
        case 0x7A: // MOV A, D
            cycles = MovOpCode(&state->a, &state->d);
            break;
        case 0x7B: // MOV A, E
            cycles = MovOpCode(&state->a, &state->e);
            break;
        case 0x7C: // MOV A, H
            cycles = MovOpCode(&state->a, &state->h);
            break;
        case 0x7D: // MOV A, L
            cycles = MovOpCode(&state->a, &state->l);
            break;
        case 0x7E: // MOV A, M
            cycles = MovMemOpCode(&state->a, &state->memory[GetMemoryAdr(state->h, state->l)]);
            break;
        case 0x7F: // MOV A, A
            cycles = MovOpCode(&state->a, &state->a);
            break;
        case 0x80: // ADD B
            cycles = AddOpCode(state, &state->b);
            break;
        case 0x81: // ADD C
            cycles = AddOpCode(state, &state->c);
            break;
        case 0x82: // ADD D
            cycles = AddOpCode(state, &state->d);
            break;
        case 0x83: // ADD E
            cycles = AddOpCode(state, &state->e);
            break;
        case 0x84: // ADD H
            cycles = AddOpCode(state, &state->h);
            break;
        case 0x85: // ADD L
            cycles = AddOpCode(state, &state->l);
            break;
        case 0x86: // ADD M
            cycles = AddMemOpCode(state, &state->memory[GetMemoryAdr(state->h, state->l)]);
            break;
        case 0x87: // ADD A
            cycles = AddOpCode(state, &state->a);
            break;
        case 0x88: // ADC B
            cycles = AdcOpCode(state, &state->b);
            break;
        case 0x89: // ADC C
            cycles = AdcOpCode(state, &state->c);
            break;
        case 0x8A: // ADC D
            cycles = AdcOpCode(state, &state->d);
            break;
        case 0x8B: // ADC E
            cycles = AdcOpCode(state, &state->e);
            break;
        case 0x8C: // ADC H
            cycles = AdcOpCode(state, &state->h);
            break;
        case 0x8D: // ADC L
            cycles = AdcOpCode(state, &state->l);
            break;
        case 0x8E: // ADC M
            cycles = AdcMemOpCode(state, &state->memory[GetMemoryAdr(state->h, state->l)]);
            break;
        case 0x8F: // ADC A
            cycles = AdcOpCode(state, &state->a);
            break;
        case 0x90: // SUB B
            cycles = SubOpCode(state, &state->b);
            break;
        case 0x91: // SUB C
            cycles = SubOpCode(state, &state->c);
            break;
        case 0x92: // SUB D
            cycles = SubOpCode(state, &state->d);
            break;
        case 0x93: // SUB E
            cycles = SubOpCode(state, &state->e);
            break;
        case 0x94: // SUB H
            cycles = SubOpCode(state, &state->h);
            break;
        case 0x95: // SUB L
            cycles = SubOpCode(state, &state->l);
            break;
        case 0x96: // SUB M
            cycles = SubMemOpCode(state, &state->memory[GetMemoryAdr(state->h, state->l)]);
            break;
        case 0x97: // SUB A
            cycles = SubOpCode(state, &state->a);
            break;
        case 0x98: // SBB B
            cycles = SbbOpCode(state, &state->b);
            break;
        case 0x99: // SBB C
            cycles = SbbOpCode(state, &state->c);
            break;
        case 0x9A: // SBB D
            cycles = SbbOpCode(state, &state->d);
            break;
        case 0x9B: // SBB E
            cycles = SbbOpCode(state, &state->e);
            break;
        case 0x9C: // SBB H
            cycles = SbbOpCode(state, &state->h);
            break;
        case 0x9D: // SBB L
            cycles = SbbOpCode(state, &state->l);
            break;
        case 0x9E: // SBB M
            cycles = SbbMemOpCode(state, &state->memory[GetMemoryAdr(state->h, state->l)]);
            break;
        case 0x9F: // SBB A
            cycles = SbbOpCode(state, &state->a);
            break;
        case 0xA0: // ANA B
            cycles = AnaOpCode(state, &state->b);
            break;
        case 0xA1: // ANA C
            cycles = AnaOpCode(state, &state->c);
            break;
        case 0xA2: // ANA D
            cycles = AnaOpCode(state, &state->d);
            break;
        case 0xA3: // ANA E
            cycles = AnaOpCode(state, &state->e);
            break;
        case 0xA4: // ANA H
            cycles = AnaOpCode(state, &state->h);
            break;
        case 0xA5: // ANA L
            cycles = AnaOpCode(state, &state->l);
            break;
        case 0xA6: // ANA M
            cycles = AnaMemOpCode(state, &state->memory[GetMemoryAdr(state->h, state->l)]);
            break;
        case 0xA7: // ANA A
            cycles = AnaOpCode(state, &state->a);
            break;
        case 0xA8: // XRA B
            cycles = XraOpCode(state, &state->b);
            break;
        case 0xA9: // XRA C
            cycles = XraOpCode(state, &state->c);
            break;
        case 0xAA: // XRA D
            cycles = XraOpCode(state, &state->d);
            break;
        case 0xAB: // XRA E
            cycles = XraOpCode(state, &state->e);
            break;
        case 0xAC: // XRA H
            cycles = XraOpCode(state, &state->h);
            break;
        case 0xAD: // XRA L
            cycles = XraOpCode(state, &state->l);
            break;
        case 0xAE: // XRA M
            cycles = XraMemOpCode(state, &state->memory[GetMemoryAdr(state->h, state->l)]);
            break;
        case 0xAF: // XRA A
            cycles = XraOpCode(state, &state->a);
            break;
        case 0xB0: // ORA B
            cycles = OraOpCode(state, &state->b);
            break;
        case 0xB1: // ORA C
            cycles = OraOpCode(state, &state->c);
            break;
        case 0xB2: // ORA D
            cycles = OraOpCode(state, &state->d);
            break;
        case 0xB3: // ORA E
            cycles = OraOpCode(state, &state->e);
            break;
        case 0xB4: // ORA H
            cycles = OraOpCode(state, &state->h);
            break;
        case 0xB5: // ORA L
            cycles = OraOpCode(state, &state->l);
            break;
        case 0xB6: // ORA M
            cycles = OraMemOpCode(state, &state->memory[GetMemoryAdr(state->h, state->l)]);
            break;
        case 0xB7: // ORA A
            cycles = OraOpCode(state, &state->a);
            break;
        case 0xB8: // CMP B
            cycles = CmpOpCode(state, state->b);
            break;
        case 0xB9: // CMP C
            cycles = CmpOpCode(state, state->c);
            break;
        case 0xBA: // CMP D
            cycles = CmpOpCode(state, state->d);
            break;
        case 0xBB: // CMP E
            cycles = CmpOpCode(state, state->e);
            break;
        case 0xBC: // CMP H
            cycles = CmpOpCode(state, state->h);
            break;
        case 0xBD: // CMP L
            cycles = CmpOpCode(state, state->l);
            break;
        case 0xBE: // CMP M
            cycles = CmpMemOpCode(state, state->memory[GetMemoryAdr(state->h, state->l)]);
            break;
        case 0xBF: // CMP A
            cycles = CmpOpCode(state, state->a);
            break;
        case 0xC0: // RNZ
            cycles = RnzOpCode(state);
            break;
        case 0xC1: // POP B
            cycles = PopOpCode(state, &state->b, &state->c);
            break;
        case 0xC2: // JNZ adr
            cycles = JnzOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xC3: // JMP adr
            cycles = JmpOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xC4: // CNZ adr
            cycles = CnzOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xC5: // PUSH B
            cycles = PushOpCode(state, state->b, state->c);
            break;
        case 0xC6: // ADI D8
            cycles = AdiOpCode(state, opcode[1]);
            break;
        case 0xC7: // RST 0
            cycles = RstOpCode(state, 0x00);
            break;
        case 0xC8: // RZ
            cycles = RzOpCode(state);
            break;
        case 0xC9: // RET
            cycles = RetOpCode(state);
            break;
        case 0xCA: // JZ adr
            cycles = JzOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xCB: // JMP adr
            cycles = JmpOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xCC: // CZ adr
            cycles = CzOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xCD: // CALL adr
            cycles = CallOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xCE: // ACI D8
            cycles = AciOpCode(state, opcode[1]);
            break;
        case 0xCF: // RST 1
            cycles = RstOpCode(state, 0x08);
            break;
        case 0xD0: // RNC
            cycles = RncOpCode(state);
            break;
        case 0xD1: // POP D
            cycles = PopOpCode(state, &state->d, &state->e);
            break;
        case 0xD2: // JNC adr
            cycles = JncOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xD3: // OUT D8
            cycles = OutOpCode(state, opcode[1]);
            break;
        case 0xD4: // CNC adr
            cycles = CncOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xD5: // PUSH D
            cycles = PushOpCode(state, state->d, state->e);
            break;
        case 0xD6: // SUI D8
            cycles = SuiOpCode(state, opcode[1]);
            break;
        case 0xD7: // RST 2
            cycles = RstOpCode(state, 0x10);
            break;
        case 0xD8: // RC
            cycles = RcOpCode(state);
            break;
        case 0xD9: // RET
            cycles = RetOpCode(state);
            break;
        case 0xDA: // JC adr
            cycles = JcOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xDB: // IN D8
            cycles = InOpCode(state, opcode[1]);
            break;
        case 0xDC: // CC adr
            cycles = CcOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xDD: // CALL adr
            cycles = CallOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xDE: // SBI D8
            cycles = SbiOpCode(state, opcode[1]);
            break;
        case 0xDF: // RST 3
            cycles = RstOpCode(state, 0x18);
            break;
        case 0xE0: // RPO
            cycles = RpoOpCode(state);
            break;
        case 0xE1: // POP H
            cycles = PopOpCode(state, &state->h, &state->l);
            break;
        case 0xE2: // JPO adr
            cycles = JpoOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xE3: // XTHL
            cycles = XthlOpCode(state);
            break;
        case 0xE4: // CPO adr
            cycles = CpoOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xE5: // PUSH H
            cycles = PushOpCode(state, state->h, state->l);
            break;
        case 0xE6: // ANI D8
            cycles = AniOpCode(state, opcode[1]);
            break;
        case 0xE7: // RST 4
            cycles = RstOpCode(state, 0x20);
            break;
        case 0xE8: // RPE
            cycles = RpeOpCode(state);
            break;
        case 0xE9: // PCHL
            cycles = PchlOpCode(state);
            break;
        case 0xEA: // JPE adr
            cycles = JpeOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xEB: // XCHG
            cycles = XchgOpCode(state);
            break;
        case 0xEC: // CPE adr
            cycles = CpeOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xED: // CALL adr
            cycles = CallOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xEE: // XRI D8
            cycles = XriOpCode(state, opcode[1]);
            break;
        case 0xEF: // RST 5
            cycles = RstOpCode(state, 0x28);
            break;
        case 0xF0: // RP
            cycles = RpOpCode(state);
            break;
        case 0xF1: // POP PSW
            cycles = PopPswOpCode(state);
            break;
        case 0xF2: // JP adr
            cycles = JpOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xF3: // DI
            cycles = DiOpCode(state);
            break;
        case 0xF4: // CP adr
            cycles = CpOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xF5: // PUSH PSW
            cycles = PushPswOpCode(state);
            break;
        case 0xF6: // ORI D8
            cycles = OriOpCode(state, opcode[1]);
            break;
        case 0xF7: // RST 6
            cycles = RstOpCode(state, 0x30);
            break;
        case 0xF8: // RM
            cycles = RmOpCode(state);
            break;
        case 0xF9: // SPHL
            cycles = SphlOpCode(state);
            break;
        case 0xFA: // JM adr
            cycles = JmOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xFB: // EI
            cycles = EiOpCode(state);
            break;
        case 0xFC: // CM adr
            cycles = CmOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xFD: // CALL adr
            cycles = CallOpCode(state, GetMemoryAdr(opcode[2], opcode[1]));
            break;
        case 0xFE: // CPI D8
            cycles = CpiOpCode(state, opcode[1]);
            break;
        case 0xFF: // RST 7
            cycles = RstOpCode(state, 0x38);
            break;
    }
    if (cycles == -1) {
        return cycles;
    }

    state->pc+=1;  //for the opcode

    return cycles;
}

Cpu8080 *CreateTestState(uint16_t pc_init) {
    const uint8_t NULL_8_BIT = 0x00;

    Cpu8080 *state = malloc(sizeof(Cpu8080));
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

    // 7. Set up a NULL IoBus Reference (IoBus being the object that liases between CPU and Space Invaders Hardware)
    state->io_bus = NULL;

    return state;
}

void LoadRomIntoState(Cpu8080 *state, unsigned char *buffer, long buffer_len) {
    int b = 0;
    for (int i = state->pc; i < (state->pc + buffer_len); i++) {
        state->memory[i] = buffer[b];
        b++;
    }

    return;
}

void FreeState(Cpu8080 *state) {
    if (state != NULL) {
        free(state->memory);
        free(state);
    }
}
