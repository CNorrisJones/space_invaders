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

// Helper Funcs
uint8_t Parity(uint8_t input);
uint16_t GetMemoryAdr(uint8_t high, uint8_t low);
void HandleZeroSignParityFlags(State8080 *state, uint16_t value);

// Data Transfer Funcs
void MovOpCode(uint8_t *dest_reg, uint8_t *send_reg);
void MviOpCode(State8080 *state, uint8_t *dest_reg, uint8_t byte_2);
void LxiOpCode(State8080 *state, uint8_t *lower_reg, uint8_t *upper_reg, uint8_t byte_2, uint8_t byte_3);
void LxiStackPointerOpCode(State8080 *state, uint8_t byte_2, uint8_t byte_3);
void LdaOpCode(State8080 *state, uint16_t adr);
void StaOpCode(State8080 *state, uint16_t adr);
void LhldOpCode(State8080 *state, uint16_t adr);
void ShldOpCode(State8080 *state, uint16_t adr);
void LdaxOpCode(State8080 *state, uint16_t reg_pair);
void StaxOpCode(State8080 *state, uint8_t high_reg, uint8_t low_reg);
void XchgOpCode(State8080 *state);

// Arithmetic Funcs
void AddOpCode(State8080 *state, uint8_t *reg);
void AdiOpCode(State8080 *state, uint8_t added_byte);
void AdcOpCode(State8080 *state, uint8_t *reg);
void AciOpCode(State8080 *state, uint8_t byte_2);
void SubOpCode(State8080 *state, uint8_t *reg);
void SuiOpCode(State8080 *state, uint8_t byte_2);
void SbbOpCode(State8080 *state, uint8_t *reg);
void SbiOpCode(State8080 *state, uint8_t byte_2);
void InrOpCode(State8080 *state, uint8_t *reg);
void DcrOpCode(State8080 *state, uint8_t *reg);
void InxOpCode(uint8_t *high_reg, uint8_t *low_reg);
void InxStackPointerOpCode(State8080 *state);
void DcxOpCode(uint8_t *high_reg, uint8_t *low_reg);
void DcxStackPointerOpCode(State8080 *state);
void DadOpCode(State8080 *state, uint16_t reg_pair);
void DaaOpCode(State8080 *state);

// Logical Funcs
void AnaOpCode(State8080 *state, uint8_t *reg);
void AniOpCode(State8080 *state, uint8_t byte_2);
void XraOpCode(State8080 *state, uint8_t *reg);
void XriOpCode(State8080 *state, uint8_t byte_2);
void OraOpCode(State8080 *state, uint8_t *reg);
void OriOpCode(State8080 *state, uint8_t byte_2);
void CmpOpCode(State8080 *state, uint8_t reg);
void CpiOpCode(State8080 *state, uint8_t byte_2);
void RlcOpCode(State8080 *state);
void RrcOpCode(State8080 *state);
void RalOpcode(State8080 *state);
void RarOpCode(State8080 *state);
void CmaOpCode(State8080 *state);
void CmcOpCode(State8080 *state);
void StcOpCode(State8080 *state);

// Branch Funcs
void JmpOpCode(State8080 *state, uint16_t adr);
void JnzOpCode(State8080 *state, uint16_t adr);
void JzOpCode(State8080 *state, uint16_t adr);
void JncOpCode(State8080 *state, uint16_t adr);
void JcOpCode(State8080 *state, uint16_t adr);
void JpoOpCode(State8080 *state, uint16_t adr);
void JpeOpCode(State8080 *state, uint16_t adr);
void JpOpCode(State8080 *state, uint16_t adr);
void JmOpCode(State8080 *state, uint16_t adr);
void CallOpCode(State8080 *state, uint16_t adr);
void CnzOpCode(State8080 *state, uint16_t adr);
void CzOpCode(State8080 *state, uint16_t adr);
void CncOpCode(State8080 *state, uint16_t adr);
void CcOpCode(State8080 *state, uint16_t adr);
void CpoOpCode(State8080 *state, uint16_t adr);
void CpeOpCode(State8080 *state, uint16_t adr);
void CpOpCode(State8080 *state, uint16_t adr);
void CmOpCode(State8080 *state, uint16_t adr);
void RetOpCode(State8080 *state);
void RnzOpCode(State8080 *state);
void RzOpCode(State8080 *state);
void RncOpCode(State8080 *state);
void RcOpCode(State8080 *state);
void RpoOpCode(State8080 *state);
void RpeOpCode(State8080 *state);
void RpOpCode(State8080 *state);
void RmOpCode(State8080 *state);
void RstOpCode(State8080 *state, uint16_t adr);
void PchlOpCode(State8080 *state);

// Stack, IO, Machine Control Funcs
void PushOpCode(State8080 *state, uint8_t high_reg, uint8_t low_reg);
void PushPswOpCode(State8080 *state);
void PopOpCode(State8080 *state, uint8_t *high_reg, uint8_t *low_reg);
void PopPswOpCode(State8080 *state);
void XthlOpCode(State8080 *state);
void SphlOpCode(State8080 *state);
void InOpCode(State8080 *state, uint8_t byte_2);
void OutOpCode(State8080 *state, uint8_t byte_2);
void EiOpCode(State8080 *state);
void DiOpCode(State8080 *state);
void HltOpCode();
void NopOpCode();

int Emulate8080Op(State8080* state);
State8080 *CreateTestState();
void FreeState(State8080 *state);
void TestState(State8080* state, uint8_t *command, size_t command_len);

#endif
