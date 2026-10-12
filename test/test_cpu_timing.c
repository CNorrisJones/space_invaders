#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "iobus.h"
#include "cpu.h"

typedef struct TestHardware {
    uint8_t return_value;
} TestHardware;

static uint8_t ReadTest(void *context, uint8_t port) {
    TestHardware *test_hardware = context;

    return test_hardware->return_value + port;
}

static void WriteTest(void *context, uint8_t port, uint8_t value) {
    TestHardware *test_hardware = context;
    test_hardware->return_value = value + port;
}

int main(void) {
    Cpu8080 *demo_cpu = CreateTestCpu(0x00);
    if (demo_cpu == NULL) {
        printf("Demo CPU Creation Failure\n");
        return 1;
    }

    TestHardware *test_hardware = malloc(sizeof(*test_hardware));
    if (test_hardware == NULL) {
        printf("Failure allocating test_hardware\n");
        FreeCpu(demo_cpu);
        return 1;
    }
    test_hardware->return_value = 0xA5;

    IoBus *test_bus = IoBusInit(test_hardware, ReadTest, WriteTest);
    if (test_bus == NULL) {
        printf("Failure setting up test_bus\n");
        FreeCpu(demo_cpu);
        free(test_hardware);
        return 1;
    }

    int cycle_test;
    unsigned char mov_op[] = {0x40}; // MOV
    LoadRomIntoCpu(demo_cpu, mov_op, sizeof(mov_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 5) {
        printf("MOV OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char mov_mem_op[] = {0x46}; // MOV MEM
    LoadRomIntoCpu(demo_cpu, mov_mem_op, sizeof(mov_mem_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 7) {
        printf("MOV MEM OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char mvi_op[] = {0x06, 0x01}; // MVI
    LoadRomIntoCpu(demo_cpu, mvi_op, sizeof(mvi_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 7) {
        printf("MVI OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char mvi_mem_op[] = {0x36, 0x01}; // MVI MEM
    LoadRomIntoCpu(demo_cpu, mvi_mem_op, sizeof(mvi_mem_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 10) {
        printf("MVI MEM OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char lxi_op[] = {0x01, 0xDE, 0xAD}; // LXI
    LoadRomIntoCpu(demo_cpu, lxi_op, sizeof(lxi_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 10) {
        printf("LXI OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char lxi_stack_op[] = {0x31, 0xDE, 0xAD}; // LXI STACK POINTER
    LoadRomIntoCpu(demo_cpu, lxi_stack_op, sizeof(lxi_stack_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 10) {
        printf("LXI STACK POINTER OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char lda_op[] = {0x3A, 0xDE, 0xAD}; // LDA
    LoadRomIntoCpu(demo_cpu, lda_op, sizeof(lda_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 13) {
        printf("LDA OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char sta_op[] = {0x32, 0xDE, 0xAD}; // STA
    LoadRomIntoCpu(demo_cpu, sta_op, sizeof(sta_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 13) {
        printf("STA OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char lhld_op[] = {0x2A, 0xDE, 0xAD}; // LHLD
    LoadRomIntoCpu(demo_cpu, lhld_op, sizeof(lhld_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 16) {
        printf("LHLD OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char shld_op[] = {0x22, 0xDE, 0xAD}; // SHLD
    LoadRomIntoCpu(demo_cpu, shld_op, sizeof(shld_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 16) {
        printf("SHLD OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char ldax_op[] = {0x0A}; // LDAX
    LoadRomIntoCpu(demo_cpu, ldax_op, sizeof(ldax_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 7) {
        printf("LDAX OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char stax_op[] = {0x02}; // STAX
    LoadRomIntoCpu(demo_cpu, stax_op, sizeof(stax_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 7) {
        printf("STAX OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char xchg_op[] = {0xEB}; // XCHG
    LoadRomIntoCpu(demo_cpu, xchg_op, sizeof(xchg_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 4) {
        printf("XCHG OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char add_op[] = {0x80}; // ADD
    LoadRomIntoCpu(demo_cpu, add_op, sizeof(add_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 4) {
        printf("ADD OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char add_mem_op[] = {0x86}; // ADD MEM
    LoadRomIntoCpu(demo_cpu, add_mem_op, sizeof(add_mem_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 7) {
        printf("ADD MEM OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char adi_op[] = {0xC6, 0xBE}; // ADI
    LoadRomIntoCpu(demo_cpu, adi_op, sizeof(adi_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 7) {
        printf("ADI OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char adc_op[] = {0x88}; // ADC
    LoadRomIntoCpu(demo_cpu, adc_op, sizeof(adc_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 4) {
        printf("ADC OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char adc_mem_op[] = {0x8E}; // ADC MEM
    LoadRomIntoCpu(demo_cpu, adc_mem_op, sizeof(adc_mem_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 7) {
        printf("ADC OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char aci_op[] = {0xCE, 0xBE}; // ACI
    LoadRomIntoCpu(demo_cpu, aci_op, sizeof(aci_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 7) {
        printf("ACI OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char sub_op[] = {0x90}; // SUB
    LoadRomIntoCpu(demo_cpu, sub_op, sizeof(sub_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 4) {
        printf("SUB OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char sub_mem_op[] = {0x96}; // SUB MEM
    LoadRomIntoCpu(demo_cpu, sub_mem_op, sizeof(sub_mem_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 7) {
        printf("SUB MEM OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char sui_op[] = {0xD6, 0xBE}; // SUI
    LoadRomIntoCpu(demo_cpu, sui_op, sizeof(sui_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 7) {
        printf("SUI OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char sbb_op[] = {0x98}; // SBB
    LoadRomIntoCpu(demo_cpu, sbb_op, sizeof(sbb_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 4) {
        printf("SBB OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char sbb_mem_op[] = {0x9E}; // SBB MEM
    LoadRomIntoCpu(demo_cpu, sbb_mem_op, sizeof(sbb_mem_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 7) {
        printf("SBB MEM OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char sbi_op[] = {0xDE, 0xBE}; // SBI
    LoadRomIntoCpu(demo_cpu, sbi_op, sizeof(sbi_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 7) {
        printf("SBI OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char inr_op[] = {0x04}; // INR
    LoadRomIntoCpu(demo_cpu, inr_op, sizeof(inr_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 5) {
        printf("INR OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char inr_mem_op[] = {0x34}; // INR MEM
    LoadRomIntoCpu(demo_cpu, inr_mem_op, sizeof(inr_mem_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 10) {
        printf("INR MEM OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char dcr_op[] = {0x05}; // DCR
    LoadRomIntoCpu(demo_cpu, dcr_op, sizeof(dcr_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 5) {
        printf("DCR OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char dcr_mem_op[] = {0x35}; // DCR MEM
    LoadRomIntoCpu(demo_cpu, dcr_mem_op, sizeof(dcr_mem_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 10) {
        printf("DCR MEM OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char inx_op[] = {0x03}; // INX
    LoadRomIntoCpu(demo_cpu, inx_op, sizeof(inx_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 5) {
        printf("INX OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char inx_stack_op[] = {0x33}; // INX STACK POINTER
    LoadRomIntoCpu(demo_cpu, inx_stack_op, sizeof(inx_stack_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 5) {
        printf("INX STACK OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char dcx_op[] = {0x0B}; // DCX
    LoadRomIntoCpu(demo_cpu, dcx_op, sizeof(dcx_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 5) {
        printf("DCX OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char dcx_stack_op[] = {0x3B}; // DCX STACK POINTER
    LoadRomIntoCpu(demo_cpu, dcx_stack_op, sizeof(dcx_stack_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 5) {
        printf("DCX STACK OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char dad_op[] = {0x09}; // DAD
    LoadRomIntoCpu(demo_cpu, dad_op, sizeof(dad_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 10) {
        printf("DAD OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char daa_op[] = {0x27}; // DAA
    LoadRomIntoCpu(demo_cpu, daa_op, sizeof(daa_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 4) {
        printf("DAA OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char ana_op[] = {0xA0}; // ANA
    LoadRomIntoCpu(demo_cpu, ana_op, sizeof(ana_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 4) {
        printf("ANA OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char ana_mem_op[] = {0xA6}; // ANA MEM
    LoadRomIntoCpu(demo_cpu, ana_mem_op, sizeof(ana_mem_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 7) {
        printf("ANA MEM OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char ani_op[] = {0xE6, 0xBE}; // ANI
    LoadRomIntoCpu(demo_cpu, ani_op, sizeof(ani_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 7) {
        printf("ANI OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char xra_op[] = {0xA8}; // XRA
    LoadRomIntoCpu(demo_cpu, xra_op, sizeof(xra_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 4) {
        printf("XRA OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char xra_mem_op[] = {0xAE}; // XRA MEM
    LoadRomIntoCpu(demo_cpu, xra_mem_op, sizeof(xra_mem_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 7) {
        printf("XRA MEM OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char xri_op[] = {0xEE, 0xBE}; // XRI
    LoadRomIntoCpu(demo_cpu, xri_op, sizeof(xri_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 7) {
        printf("XRI OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char ora_op[] = {0xB0}; // ORA
    LoadRomIntoCpu(demo_cpu, ora_op, sizeof(ora_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 4) {
        printf("ORA OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char ora_mem_op[] = {0xB6}; // ORA MEM
    LoadRomIntoCpu(demo_cpu, ora_mem_op, sizeof(ora_mem_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 7) {
        printf("ORA MEM OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char ori_op[] = {0xF6, 0xBE}; // ORI
    LoadRomIntoCpu(demo_cpu, ori_op, sizeof(ori_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 7) {
        printf("ORI OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char cmp_op[] = {0xB8}; // CMP
    LoadRomIntoCpu(demo_cpu, cmp_op, sizeof(cmp_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 4) {
        printf("CMP OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char cmp_mem_op[] = {0xBE}; // CMP MEM
    LoadRomIntoCpu(demo_cpu, cmp_mem_op, sizeof(cmp_mem_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 7) {
        printf("CMP MEM OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char cpi_op[] = {0xFE, 0xBE}; // CPI
    LoadRomIntoCpu(demo_cpu, cpi_op, sizeof(cpi_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 7) {
        printf("CPI OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char rlc_op[] = {0x07}; // RLC
    LoadRomIntoCpu(demo_cpu, rlc_op, sizeof(rlc_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 4) {
        printf("RLC OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char rrc_op[] = {0x0F}; // RRC
    LoadRomIntoCpu(demo_cpu, rrc_op, sizeof(rrc_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 4) {
        printf("RRC OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char ral_op[] = {0x17}; // RAL
    LoadRomIntoCpu(demo_cpu, ral_op, sizeof(ral_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 4) {
        printf("RAL OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char rar_op[] = {0x1F}; // RAR
    LoadRomIntoCpu(demo_cpu, rar_op, sizeof(rar_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 4) {
        printf("RAR OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char cma_op[] = {0x2F}; // CMA
    LoadRomIntoCpu(demo_cpu, cma_op, sizeof(cma_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 4) {
        printf("CMA OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char cmc_op[] = {0x3F}; // CMC
    LoadRomIntoCpu(demo_cpu, cmc_op, sizeof(cmc_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 4) {
        printf("CMC OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char stc_op[] = {0x37}; // STC
    LoadRomIntoCpu(demo_cpu, stc_op, sizeof(stc_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 4) {
        printf("STC OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char jmp_op[] = {0xC3, 0xDE, 0xAD}; // JMP
    LoadRomIntoCpu(demo_cpu, jmp_op, sizeof(jmp_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 10) {
        printf("JMP OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char jnz_op[] = {0xC2, 0xDE, 0xAD}; // JNZ
    LoadRomIntoCpu(demo_cpu, jnz_op, sizeof(jnz_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 10) {
        printf("JNZ OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char jz_op[] = {0xCA, 0xDE, 0xAD}; // JZ
    LoadRomIntoCpu(demo_cpu, jz_op, sizeof(jz_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 10) {
        printf("JZ OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char jnc_op[] = {0xD2, 0xDE, 0xAD}; // JNC
    LoadRomIntoCpu(demo_cpu, jnc_op, sizeof(jnc_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 10) {
        printf("JNC OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char jc_op[] = {0xDA, 0xDE, 0xAD}; // JC
    LoadRomIntoCpu(demo_cpu, jc_op, sizeof(jc_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 10) {
        printf("JC OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char jpo_op[] = {0xE2, 0xDE, 0xAD}; // JPO
    LoadRomIntoCpu(demo_cpu, jpo_op, sizeof(jpo_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 10) {
        printf("JPO OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char jpe_op[] = {0xEA, 0xDE, 0xAD}; // JPE
    LoadRomIntoCpu(demo_cpu, jpe_op, sizeof(jpe_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 10) {
        printf("JPE OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char jp_op[] = {0xF2, 0xDE, 0xAD}; // JP
    LoadRomIntoCpu(demo_cpu, jp_op, sizeof(jp_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 10) {
        printf("JP OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char jm_op[] = {0xFA, 0xDE, 0xAD}; // JM
    LoadRomIntoCpu(demo_cpu, jm_op, sizeof(jm_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 10) {
        printf("JM OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char call_op[] = {0xFD, 0xDE, 0xAD}; // CALL
    LoadRomIntoCpu(demo_cpu, call_op, sizeof(call_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 17) {
        printf("CALL OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char cnz_op[] = {0xC4, 0xDE, 0xAD}; // CNZ CALL
    LoadRomIntoCpu(demo_cpu, cnz_op, sizeof(cnz_op));
    demo_cpu->cc.z = 0;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 17) {
        printf("CNZ OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char cnz_no_op[] = {0xC4, 0xDE, 0xAD}; // CNZ NO CALL
    LoadRomIntoCpu(demo_cpu, cnz_no_op, sizeof(cnz_no_op));
    demo_cpu->cc.z = 1;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 11) {
        printf("CNZ NO OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char cz_op[] = {0xCC, 0xDE, 0xAD}; // CZ CALL
    LoadRomIntoCpu(demo_cpu, cz_op, sizeof(cz_op));
    demo_cpu->cc.z = 1;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 17) {
        printf("CZ OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char cz_no_op[] = {0xCC, 0xDE, 0xAD}; // CZ NO CALL
    LoadRomIntoCpu(demo_cpu, cz_no_op, sizeof(cz_no_op));
    demo_cpu->cc.z = 0;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 11) {
        printf("CZ NO OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char cnc_op[] = {0xD4, 0xDE, 0xAD}; // CNC CALL
    LoadRomIntoCpu(demo_cpu, cnc_op, sizeof(cnc_op));
    demo_cpu->cc.cy = 0;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 17) {
        printf("CNC OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char cnc_no_op[] = {0xD4, 0xDE, 0xAD}; // CNC NO CALL
    LoadRomIntoCpu(demo_cpu, cnc_no_op, sizeof(cnc_no_op));
    demo_cpu->cc.cy = 1;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 11) {
        printf("CNC NO OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char cc_op[] = {0xDC, 0xDE, 0xAD}; // CC CALL
    LoadRomIntoCpu(demo_cpu, cc_op, sizeof(cc_op));
    demo_cpu->cc.cy = 1;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 17) {
        printf("CC OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char cc_no_op[] = {0xDC, 0xDE, 0xAD}; // CC NO CALL
    LoadRomIntoCpu(demo_cpu, cc_no_op, sizeof(cc_no_op));
    demo_cpu->cc.cy = 0;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 11) {
        printf("CC NO OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char cpo_op[] = {0xE4, 0xDE, 0xAD}; // CPO CALL
    LoadRomIntoCpu(demo_cpu, cpo_op, sizeof(cpo_op));
    demo_cpu->cc.p = 0;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 17) {
        printf("CPO OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char cpo_no_op[] = {0xE4, 0xDE, 0xAD}; // CPO NO CALL
    LoadRomIntoCpu(demo_cpu, cpo_no_op, sizeof(cpo_no_op));
    demo_cpu->cc.p = 1;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 11) {
        printf("CPO NO OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char cpe_op[] = {0xEC, 0xDE, 0xAD}; // CPE CALL
    LoadRomIntoCpu(demo_cpu, cpe_op, sizeof(cpe_op));
    demo_cpu->cc.p = 1;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 17) {
        printf("CPE OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char cpe_no_op[] = {0xEC, 0xDE, 0xAD}; // CPE NO CALL
    LoadRomIntoCpu(demo_cpu, cpe_no_op, sizeof(cpe_no_op));
    demo_cpu->cc.p = 0;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 11) {
        printf("CPE NO OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char cp_op[] = {0xF4, 0xDE, 0xAD}; // CP CALL
    LoadRomIntoCpu(demo_cpu, cp_op, sizeof(cp_op));
    demo_cpu->cc.s = 0;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 17) {
        printf("CP OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char cp_no_op[] = {0xF4, 0xDE, 0xAD}; // CP NO CALL
    LoadRomIntoCpu(demo_cpu, cp_no_op, sizeof(cp_no_op));
    demo_cpu->cc.s = 1;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 11) {
        printf("CP NO OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char cm_op[] = {0xFC, 0xDE, 0xAD}; // CM CALL
    LoadRomIntoCpu(demo_cpu, cm_op, sizeof(cm_op));
    demo_cpu->cc.s = 1;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 17) {
        printf("CM OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char cm_no_op[] = {0xFC, 0xDE, 0xAD}; // CM NO CALL
    LoadRomIntoCpu(demo_cpu, cm_no_op, sizeof(cm_no_op));
    demo_cpu->cc.s = 0;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 11) {
        printf("CM NO OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char ret_op[] = {0xD9}; // RET
    LoadRomIntoCpu(demo_cpu, ret_op, sizeof(ret_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 10) {
        printf("RET OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char rnz_op[] = {0xC0}; // RNZ RETURN
    LoadRomIntoCpu(demo_cpu, rnz_op, sizeof(rnz_op));
    demo_cpu->cc.z = 0;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 11) {
        printf("RNZ OP CODE RETURN CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char rnz_no_op[] = {0xC0}; // RNZ NO RETURN
    LoadRomIntoCpu(demo_cpu, rnz_no_op, sizeof(rnz_no_op));
    demo_cpu->cc.z = 1;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 5) {
        printf("RNZ OP CODE NO RETURN CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char rz_op[] = {0xC8}; // RZ RETURN
    LoadRomIntoCpu(demo_cpu, rz_op, sizeof(rz_op));
    demo_cpu->cc.z = 1;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 11) {
        printf("RZ OP CODE RETURN CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char rz_no_op[] = {0xC8}; // RZ NO RETURN
    LoadRomIntoCpu(demo_cpu, rz_no_op, sizeof(rz_no_op));
    demo_cpu->cc.z = 0;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 5) {
        printf("RZ OP CODE NO RETURN CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char rnc_op[] = {0xD0}; // RNC RETURN
    LoadRomIntoCpu(demo_cpu, rnc_op, sizeof(rnc_op));
    demo_cpu->cc.cy = 0;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 11) {
        printf("RNC OP CODE RETURN CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char rnc_no_op[] = {0xD0}; // RNC NO RETURN
    LoadRomIntoCpu(demo_cpu, rnc_no_op, sizeof(rnc_no_op));
    demo_cpu->cc.cy = 1;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 5) {
        printf("RNC OP CODE NO RETURN CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char rc_op[] = {0xD8}; // RC RETURN
    LoadRomIntoCpu(demo_cpu, rc_op, sizeof(rc_op));
    demo_cpu->cc.cy = 1;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 11) {
        printf("RC OP CODE RETURN CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char rc_no_op[] = {0xD8}; // RC NO RETURN
    LoadRomIntoCpu(demo_cpu, rc_no_op, sizeof(rc_no_op));
    demo_cpu->cc.cy = 0;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 5) {
        printf("RC OP CODE NO RETURN CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char rpo_op[] = {0xE0}; // RPO RETURN
    LoadRomIntoCpu(demo_cpu, rpo_op, sizeof(rpo_op));
    demo_cpu->cc.p = 0;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 11) {
        printf("RPO OP CODE RETURN CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char rpo_no_op[] = {0xE0}; // RPO NO RETURN
    LoadRomIntoCpu(demo_cpu, rpo_no_op, sizeof(rpo_no_op));
    demo_cpu->cc.p = 1;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 5) {
        printf("RPO OP CODE NO RETURN CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char rpe_op[] = {0xE8}; // RPE RETURN
    LoadRomIntoCpu(demo_cpu, rpe_op, sizeof(rpe_op));
    demo_cpu->cc.p = 1;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 11) {
        printf("RPE OP CODE RETURN CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char rpe_no_op[] = {0xE8}; // RPE NO RETURN
    LoadRomIntoCpu(demo_cpu, rpe_no_op, sizeof(rpe_no_op));
    demo_cpu->cc.p = 0;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 5) {
        printf("RPE OP CODE NO RETURN CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char rp_op[] = {0xF0}; // RP RETURN
    LoadRomIntoCpu(demo_cpu, rp_op, sizeof(rp_op));
    demo_cpu->cc.s = 0;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 11) {
        printf("RP OP CODE RETURN CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char rp_no_op[] = {0xF0}; // RP NO RETURN
    LoadRomIntoCpu(demo_cpu, rp_no_op, sizeof(rp_no_op));
    demo_cpu->cc.s = 1;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 5) {
        printf("RP OP CODE NO RETURN CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char rm_op[] = {0xF8}; // RM RETURN
    LoadRomIntoCpu(demo_cpu, rm_op, sizeof(rm_op));
    demo_cpu->cc.s = 1;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 11) {
        printf("RM OP CODE RETURN CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char rm_no_op[] = {0xF8}; // RM NO RETURN
    LoadRomIntoCpu(demo_cpu, rm_no_op, sizeof(rm_no_op));
    demo_cpu->cc.s = 0;
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 5) {
        printf("RM OP CODE NO RETURN CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char rst_op[] = {0xC7}; // RST
    LoadRomIntoCpu(demo_cpu, rst_op, sizeof(rst_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 11) {
        printf("RST OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char pchl_op[] = {0xE9}; // PCHL
    LoadRomIntoCpu(demo_cpu, pchl_op, sizeof(pchl_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 5) {
        printf("PCHL OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char push_op[] = {0xC5}; // PUSH
    LoadRomIntoCpu(demo_cpu, push_op, sizeof(push_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 11) {
        printf("PUSH OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char push_psw_op[] = {0xF5}; // PUSH PSW
    LoadRomIntoCpu(demo_cpu, push_psw_op, sizeof(push_psw_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 11) {
        printf("PUSH PSW OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char pop_op[] = {0xC1}; // POP
    LoadRomIntoCpu(demo_cpu, pop_op, sizeof(pop_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 10) {
        printf("POP OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char pop_psw_op[] = {0xF1}; // POP PSW
    LoadRomIntoCpu(demo_cpu, pop_psw_op, sizeof(pop_psw_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 10) {
        printf("POP PSW OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char xthl_op[] = {0xE3}; // XTHL
    LoadRomIntoCpu(demo_cpu, xthl_op, sizeof(xthl_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 18) {
        printf("XTHL OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char sphl_op[] = {0xF9}; // SPHL
    LoadRomIntoCpu(demo_cpu, sphl_op, sizeof(sphl_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 5) {
        printf("SPHL OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char ei_op[] = {0xFB}; // EI
    LoadRomIntoCpu(demo_cpu, ei_op, sizeof(ei_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 4) {
        printf("EI OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char di_op[] = {0xF3}; // DI
    LoadRomIntoCpu(demo_cpu, di_op, sizeof(di_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 4) {
        printf("DI OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char hlt_op[] = {0x76}; // HLT
    LoadRomIntoCpu(demo_cpu, hlt_op, sizeof(hlt_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 7) {
        printf("HLT OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char nop_op[] = {0x00}; // NOP
    LoadRomIntoCpu(demo_cpu, nop_op, sizeof(nop_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 4) {
        printf("NOP OP CODE CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char in_fail_op[] = {0xDB, 0xBE}; // IN FAIL
    LoadRomIntoCpu(demo_cpu, in_fail_op, sizeof(in_fail_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != -1) {
        printf("IN OP CODE FAIL CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    unsigned char out_fail_op[] = {0xD3, 0xBE}; // OUT FAIL
    LoadRomIntoCpu(demo_cpu, out_fail_op, sizeof(out_fail_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != -1) {
        printf("OUT OP CODE FAIL CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    demo_cpu->io_bus = test_bus;

    unsigned char in_succ_op[] = {0xDB, 0xBE}; // IN SUCC
    LoadRomIntoCpu(demo_cpu, in_succ_op, sizeof(in_succ_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 10) {
        printf("IN OP CODE SUCCESS CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }
    
    unsigned char out_succ_op[] = {0xD3, 0xBE}; // OUT SUCC
    LoadRomIntoCpu(demo_cpu, out_succ_op, sizeof(out_succ_op));
    cycle_test = Emulate8080Op(demo_cpu);

    if (cycle_test != 10) {
        printf("OUT OP CODE SUCCESS CYCLE TIMING FAILURE\n");
        FreeCpu(demo_cpu);
        IoBusShutdown(test_bus);
        free(test_hardware);
        return 1;
    }

    printf("ALL TESTS SUCCESS FOR CPU TIMING\n");
    FreeCpu(demo_cpu);
    free(test_hardware);
    IoBusShutdown(test_bus);
    return 0;
}
