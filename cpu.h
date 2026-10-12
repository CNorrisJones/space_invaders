#ifndef CPU_H
#define CPU_H

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

typedef struct Cpu8080 {
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
} Cpu8080;

extern int is_test_cpu;

// Helper Funcs
uint8_t Parity(uint8_t input);
uint16_t GetMemoryAdr(uint8_t high, uint8_t low);
void HandleZeroSignParityFlags(Cpu8080 *state, uint16_t value);

// Data Transfer Funcs
int MovOpCode(uint8_t *dest_reg, uint8_t *send_reg);
int MovMemOpCode(uint8_t *dest_reg, uint8_t *send_reg);
int MviOpCode(Cpu8080 *state, uint8_t *dest_reg, uint8_t byte_2);
int MviMemOpCode(Cpu8080 *state, uint8_t *dest_reg, uint8_t byte_2);
int LxiOpCode(Cpu8080 *state, uint8_t *lower_reg, uint8_t *upper_reg, uint8_t byte_2, uint8_t byte_3);
int LxiStackPointerOpCode(Cpu8080 *state, uint8_t byte_2, uint8_t byte_3);
int LdaOpCode(Cpu8080 *state, uint16_t adr);
int StaOpCode(Cpu8080 *state, uint16_t adr);
int LhldOpCode(Cpu8080 *state, uint16_t adr);
int ShldOpCode(Cpu8080 *state, uint16_t adr);
int LdaxOpCode(Cpu8080 *state, uint16_t reg_pair);
int StaxOpCode(Cpu8080 *state, uint8_t high_reg, uint8_t low_reg);
int XchgOpCode(Cpu8080 *state);

// Arithmetic Funcs
int AddOpCode(Cpu8080 *state, uint8_t *reg);
int AddMemOpCode(Cpu8080 *state, uint8_t *reg);
int AdiOpCode(Cpu8080 *state, uint8_t added_byte);
int AdcOpCode(Cpu8080 *state, uint8_t *reg);
int AdcMemOpCode(Cpu8080 *state, uint8_t *reg);
int AciOpCode(Cpu8080 *state, uint8_t byte_2);
int SubOpCode(Cpu8080 *state, uint8_t *reg);
int SubMemOpCode(Cpu8080 *state, uint8_t *reg);
int SuiOpCode(Cpu8080 *state, uint8_t byte_2);
int SbbOpCode(Cpu8080 *state, uint8_t *reg);
int SbbMemOpCode(Cpu8080 *state, uint8_t *reg);
int SbiOpCode(Cpu8080 *state, uint8_t byte_2);
int InrOpCode(Cpu8080 *state, uint8_t *reg);
int InrMemOpCode(Cpu8080 *state, uint8_t *reg);
int DcrOpCode(Cpu8080 *state, uint8_t *reg);
int DcrMemOpCode(Cpu8080 *state, uint8_t *reg);
int InxOpCode(uint8_t *high_reg, uint8_t *low_reg);
int InxStackPointerOpCode(Cpu8080 *state);
int DcxOpCode(uint8_t *high_reg, uint8_t *low_reg);
int DcxStackPointerOpCode(Cpu8080 *state);
int DadOpCode(Cpu8080 *state, uint16_t reg_pair);
int DaaOpCode(Cpu8080 *state);

// Logical Funcs
int AnaOpCode(Cpu8080 *state, uint8_t *reg);
int AnaMemOpCode(Cpu8080 *state, uint8_t *reg);
int AniOpCode(Cpu8080 *state, uint8_t byte_2);
int XraOpCode(Cpu8080 *state, uint8_t *reg);
int XraMemOpCode(Cpu8080 *state, uint8_t *reg);
int XriOpCode(Cpu8080 *state, uint8_t byte_2);
int OraOpCode(Cpu8080 *state, uint8_t *reg);
int OraMemOpCode(Cpu8080 *state, uint8_t *reg);
int OriOpCode(Cpu8080 *state, uint8_t byte_2);
int CmpOpCode(Cpu8080 *state, uint8_t reg);
int CmpMemOpCode(Cpu8080 *state, uint8_t reg);
int CpiOpCode(Cpu8080 *state, uint8_t byte_2);
int RlcOpCode(Cpu8080 *state);
int RrcOpCode(Cpu8080 *state);
int RalOpcode(Cpu8080 *state);
int RarOpCode(Cpu8080 *state);
int CmaOpCode(Cpu8080 *state);
int CmcOpCode(Cpu8080 *state);
int StcOpCode(Cpu8080 *state);

// Branch Funcs
int JmpOpCode(Cpu8080 *state, uint16_t adr);
int JnzOpCode(Cpu8080 *state, uint16_t adr);
int JzOpCode(Cpu8080 *state, uint16_t adr);
int JncOpCode(Cpu8080 *state, uint16_t adr);
int JcOpCode(Cpu8080 *state, uint16_t adr);
int JpoOpCode(Cpu8080 *state, uint16_t adr);
int JpeOpCode(Cpu8080 *state, uint16_t adr);
int JpOpCode(Cpu8080 *state, uint16_t adr);
int JmOpCode(Cpu8080 *state, uint16_t adr);
int CallOpCode(Cpu8080 *state, uint16_t adr);
int CnzOpCode(Cpu8080 *state, uint16_t adr);
int CzOpCode(Cpu8080 *state, uint16_t adr);
int CncOpCode(Cpu8080 *state, uint16_t adr);
int CcOpCode(Cpu8080 *state, uint16_t adr);
int CpoOpCode(Cpu8080 *state, uint16_t adr);
int CpeOpCode(Cpu8080 *state, uint16_t adr);
int CpOpCode(Cpu8080 *state, uint16_t adr);
int CmOpCode(Cpu8080 *state, uint16_t adr);
int RetOpCode(Cpu8080 *state);
int RnzOpCode(Cpu8080 *state);
int RzOpCode(Cpu8080 *state);
int RncOpCode(Cpu8080 *state);
int RcOpCode(Cpu8080 *state);
int RpoOpCode(Cpu8080 *state);
int RpeOpCode(Cpu8080 *state);
int RpOpCode(Cpu8080 *state);
int RmOpCode(Cpu8080 *state);
int RstOpCode(Cpu8080 *state, uint16_t adr);
int PchlOpCode(Cpu8080 *state);

// Stack, IO, Machine Control Funcs
int PushOpCode(Cpu8080 *state, uint8_t high_reg, uint8_t low_reg);
int PushPswOpCode(Cpu8080 *state);
int PopOpCode(Cpu8080 *state, uint8_t *high_reg, uint8_t *low_reg);
int PopPswOpCode(Cpu8080 *state);
int XthlOpCode(Cpu8080 *state);
int SphlOpCode(Cpu8080 *state);
int InOpCode(Cpu8080 *state, uint8_t byte_2);
int OutOpCode(Cpu8080 *state, uint8_t byte_2);
int EiOpCode(Cpu8080 *state);
int DiOpCode(Cpu8080 *state);
int HltOpCode(void);
int NopOpCode(void);

int Emulate8080Op(Cpu8080* state);
Cpu8080 *CreateTestState(uint16_t pc_init);
void LoadRomIntoState(Cpu8080 *state, unsigned char *buffer, long buffer_len);
void FreeState(Cpu8080 *state);

#endif
