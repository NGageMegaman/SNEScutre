#include <stdint.h>
#ifndef MEM_H
#include "mem.h"
#define MEM_H
#endif
#ifndef CLOCK_H
#include "clock.h"
#define CLOCK_H
#endif
using namespace std;

struct ProcStatus {
    bool C; // Carry Flag
    bool Z; // Zero Flag
    bool M; // Accumulator Size Flag
    bool I; // Interrupt Disable
    bool D; // Decimal Mode Flag
    bool X; // Index Size Flag
    bool V; // Overflow Flag
    bool N; // Negative Flag
    bool E; // Emulation Flag
};

class Cpu {
    public:
	    Cpu();
	    void execute();
	    void add_clock_cycles(uint8_t opcode, uint32_t address, addr_mode_t addr_mode);
	    void read_operand(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand, uint8_t opcode);
        void read_operand_immediate(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand);
        void read_operand_immediate_word(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand);
        void read_operand_immediate_long(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand);
        void read_operand_relative(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand);
        void read_operand_relative_long(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand);
        void read_operand_direct(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand);
        void read_operand_direct_indexed_x(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand);
        void read_operand_direct_indexed_y(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand);
        void read_operand_direct_indirect(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand);
        void read_operand_indexed_indirect(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand);
        void read_operand_direct_indirect_long(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand);
        void read_operand_absolute(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand);
        void read_operand_absolute_indexed_x(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand);
        void read_operand_absolute_indexed_y(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand);
        void read_operand_absolute_long(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand);
        void read_operand_absolute_indexed_long(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand);
        void read_operand_stack_relative(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand);
        void read_operand_stack_relative_indirect_indexed(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand);
        void read_operand_absolute_indirect(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand);
        void read_operand_absolute_indirect_long(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand);
        void read_operand_absolute_indexed_indirect(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand);
        void read_operand_absolute_long_indexed(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand);
        void read_operand_block_move(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand);
        void read_operand_direct_indexed_indirect(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand);
        void read_operand_direct_indirect_indexed(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand);
        void read_operand_direct_indirect_long_indexed(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand);
	    void ADC_execute(uint16_t operand);
	    void AND_execute(uint16_t operand);
	    void ASL_mem_execute(uint32_t address, uint16_t operand);
	    void ASL_A_execute();
	    void BCC_execute(uint32_t address);
	    void BCS_execute(uint32_t address);
	    void BEQ_execute(uint32_t address);
	    void BIT_execute(uint16_t operand);
        void BIT_imm_execute(uint16_t operand);
	    void BMI_execute(uint32_t address);
	    void BNE_execute(uint32_t address);
	    void BPL_execute(uint32_t address);
        void BRA_execute(uint32_t address);
	    void BRK_execute();
        void BRL_execute(uint32_t address);
	    void BVC_execute(uint32_t address);
	    void BVS_execute(uint32_t address);
	    void CLC_execute();
	    void CLD_execute();
	    void CLI_execute();
	    void CLV_execute();
	    void CMP_execute(uint16_t operand);
        void COP_execute();
	    void CPX_execute(uint16_t operand);
	    void CPY_execute(uint16_t operand);
	    void DEC_mem_execute(uint32_t address, uint16_t operand);
	    void DEC_A_execute();
	    void DEX_execute();
	    void DEY_execute();
	    void EOR_execute(uint16_t operand);
	    void INC_mem_execute(uint32_t address, uint16_t operand);
	    void INC_A_execute();
	    void INX_execute();
	    void INY_execute();
	    void JMP_execute(uint32_t address);
        void JML_execute(uint32_t address);
	    void JSR_execute(uint32_t address);
        void JSL_execute(uint32_t address);
	    void LDA_execute(uint16_t operand);
	    void LDX_execute(uint16_t operand);
	    void LDY_execute(uint16_t operand);
	    void LSR_mem_execute(uint32_t address, uint16_t operand);
	    void LSR_A_execute();
	    void NOP_execute();
        void MVN_execute(uint8_t srcBank, uint8_t dstBank);
        void MVP_execute(uint8_t srcBank, uint8_t dstBank);
	    void ORA_execute(uint16_t operand);
        void PEA_execute(uint32_t address);
        void PEI_execute(uint32_t address);
        void PER_execute(uint32_t address);
	    void PHA_execute();
        void PHB_execute();
        void PHD_execute();
        void PHK_execute();
	    void PHP_execute();
        void PHX_execute();
        void PHY_execute();
	    void PLA_execute();
        void PLB_execute();
        void PLD_execute();
	    void PLP_execute();
        void PLX_execute();
        void PLY_execute();
        void REP_execute(uint16_t operand);
	    void ROL_mem_execute(uint32_t address, uint16_t operand);
	    void ROL_A_execute();
	    void ROR_mem_execute(uint32_t address, uint16_t operand);
	    void ROR_A_execute();
	    void RTI_execute();
        void RTL_execute();
	    void RTS_execute();
	    void SBC_execute(uint16_t operand);
	    void SEC_execute();
	    void SED_execute();
	    void SEI_execute();
	    void SEP_execute(uint16_t operand);
	    void STA_execute(uint32_t address);
        void STP_execute();
	    void STX_execute(uint32_t address);
	    void STY_execute(uint32_t address);
	    void STZ_execute(uint32_t address);
	    void TAX_execute();
	    void TAY_execute();
	    void TCD_execute();
	    void TCS_execute();
	    void TDC_execute();
	    void TRB_execute(uint32_t address, uint16_t operand);
	    void TSB_execute(uint32_t address, uint16_t operand);
        void TSC_execute();
	    void TSX_execute();
	    void TXA_execute();
	    void TXS_execute();
	    void TXY_execute();
	    void TYA_execute();
	    void TYX_execute();
	    void WAI_execute();
	    void WDM_execute();
	    void XBA_execute();
	    void XCE_execute();

	    void NMI_execute();
	    void pushStack(uint8_t data);
	    uint8_t pullStack();
	    void debug_dump(uint8_t inst);
	    void endExecution();
	    Clock *clock;
	    Mem mem;
    private:
	    uint16_t   regA, regX, regY;
	    uint16_t   regPC, regSP;
	    ProcStatus regP;
        uint8_t    regPB, regDB;
        uint16_t   regDP;
};
