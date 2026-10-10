#ifndef STATE_H
#define STATE_H

#include <stdint.h>

#include "iobus.h"

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
    IoBus       *io_bus;
} State8080;

extern int is_test_cpu;

// Helper Funcs
uint8_t Parity(uint8_t input);
uint16_t GetMemoryAdr(uint8_t high, uint8_t low);
void HandleZeroSignParityFlags(State8080 *state, uint16_t value);

// Data Transfer Funcs
int MovOpCode(uint8_t *dest_reg, uint8_t *send_reg);
int MovMemOpCode(uint8_t *dest_reg, uint8_t *send_reg);
int MviOpCode(State8080 *state, uint8_t *dest_reg, uint8_t byte_2);
int MviMemOpCode(State8080 *state, uint8_t *dest_reg, uint8_t byte_2);
int LxiOpCode(State8080 *state, uint8_t *lower_reg, uint8_t *upper_reg, uint8_t byte_2, uint8_t byte_3);
int LxiStackPointerOpCode(State8080 *state, uint8_t byte_2, uint8_t byte_3);
int LdaOpCode(State8080 *state, uint16_t adr);
int StaOpCode(State8080 *state, uint16_t adr);
int LhldOpCode(State8080 *state, uint16_t adr);
int ShldOpCode(State8080 *state, uint16_t adr);
int LdaxOpCode(State8080 *state, uint16_t reg_pair);
int StaxOpCode(State8080 *state, uint8_t high_reg, uint8_t low_reg);
int XchgOpCode(State8080 *state);

// Arithmetic Funcs
int AddOpCode(State8080 *state, uint8_t *reg);
int AddMemOpCode(State8080 *state, uint8_t *reg);
int AdiOpCode(State8080 *state, uint8_t added_byte);
int AdcOpCode(State8080 *state, uint8_t *reg);
int AdcMemOpCode(State8080 *state, uint8_t *reg);
int AciOpCode(State8080 *state, uint8_t byte_2);
int SubOpCode(State8080 *state, uint8_t *reg);
int SubMemOpCode(State8080 *state, uint8_t *reg);
int SuiOpCode(State8080 *state, uint8_t byte_2);
int SbbOpCode(State8080 *state, uint8_t *reg);
int SbbMemOpCode(State8080 *state, uint8_t *reg);
int SbiOpCode(State8080 *state, uint8_t byte_2);
int InrOpCode(State8080 *state, uint8_t *reg);
int InrMemOpCode(State8080 *state, uint8_t *reg);
int DcrOpCode(State8080 *state, uint8_t *reg);
int DcrMemOpCode(State8080 *state, uint8_t *reg);
int InxOpCode(uint8_t *high_reg, uint8_t *low_reg);
int InxStackPointerOpCode(State8080 *state);
int DcxOpCode(uint8_t *high_reg, uint8_t *low_reg);
int DcxStackPointerOpCode(State8080 *state);
int DadOpCode(State8080 *state, uint16_t reg_pair);
int DaaOpCode(State8080 *state);

// Logical Funcs
int AnaOpCode(State8080 *state, uint8_t *reg);
int AnaMemOpCode(State8080 *state, uint8_t *reg);
int AniOpCode(State8080 *state, uint8_t byte_2);
int XraOpCode(State8080 *state, uint8_t *reg);
int XraMemOpCode(State8080 *state, uint8_t *reg);
int XriOpCode(State8080 *state, uint8_t byte_2);
int OraOpCode(State8080 *state, uint8_t *reg);
int OraMemOpCode(State8080 *state, uint8_t *reg);
int OriOpCode(State8080 *state, uint8_t byte_2);
int CmpOpCode(State8080 *state, uint8_t reg);
int CmpMemOpCode(State8080 *state, uint8_t reg);
int CpiOpCode(State8080 *state, uint8_t byte_2);
int RlcOpCode(State8080 *state);
int RrcOpCode(State8080 *state);
int RalOpcode(State8080 *state);
int RarOpCode(State8080 *state);
int CmaOpCode(State8080 *state);
int CmcOpCode(State8080 *state);
int StcOpCode(State8080 *state);

// Branch Funcs
int JmpOpCode(State8080 *state, uint16_t adr);
int JnzOpCode(State8080 *state, uint16_t adr);
int JzOpCode(State8080 *state, uint16_t adr);
int JncOpCode(State8080 *state, uint16_t adr);
int JcOpCode(State8080 *state, uint16_t adr);
int JpoOpCode(State8080 *state, uint16_t adr);
int JpeOpCode(State8080 *state, uint16_t adr);
int JpOpCode(State8080 *state, uint16_t adr);
int JmOpCode(State8080 *state, uint16_t adr);
int CallOpCode(State8080 *state, uint16_t adr);
int CnzOpCode(State8080 *state, uint16_t adr);
int CzOpCode(State8080 *state, uint16_t adr);
int CncOpCode(State8080 *state, uint16_t adr);
int CcOpCode(State8080 *state, uint16_t adr);
int CpoOpCode(State8080 *state, uint16_t adr);
int CpeOpCode(State8080 *state, uint16_t adr);
int CpOpCode(State8080 *state, uint16_t adr);
int CmOpCode(State8080 *state, uint16_t adr);
int RetOpCode(State8080 *state);
int RnzOpCode(State8080 *state);
int RzOpCode(State8080 *state);
int RncOpCode(State8080 *state);
int RcOpCode(State8080 *state);
int RpoOpCode(State8080 *state);
int RpeOpCode(State8080 *state);
int RpOpCode(State8080 *state);
int RmOpCode(State8080 *state);
int RstOpCode(State8080 *state, uint16_t adr);
int PchlOpCode(State8080 *state);

// Stack, IO, Machine Control Funcs
int PushOpCode(State8080 *state, uint8_t high_reg, uint8_t low_reg);
int PushPswOpCode(State8080 *state);
int PopOpCode(State8080 *state, uint8_t *high_reg, uint8_t *low_reg);
int PopPswOpCode(State8080 *state);
int XthlOpCode(State8080 *state);
int SphlOpCode(State8080 *state);
int InOpCode(State8080 *state, uint8_t byte_2);
int OutOpCode(State8080 *state, uint8_t byte_2);
int EiOpCode(State8080 *state);
int DiOpCode(State8080 *state);
int HltOpCode();
int NopOpCode();

int Emulate8080Op(State8080* state);
State8080 *CreateTestState(uint16_t pc_init);
void LoadRomIntoState(State8080 *state, unsigned char *buffer, long buffer_len);
void FreeState(State8080 *state);

#endif
