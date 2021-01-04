#include <stdint.h>
#include <iostream>
#include "cpu.h"
using namespace std;

Cpu::Cpu() {
    regX = regY = regA = 0x00;
    regPC = mem.read_word(0xFFFC);
    regSP = 0x01ff;
    regP.C = regP.Z = regP.I = regP.D = regP.B = regP.V = regP.N = 0;
}

void Cpu::execute() {
    uint16_t opcode = mem.read_byte(regPC);
    addr_mode_t addr_mode;
    uint32_t address;
    uint16_t operand;
    read_operand(&addr_mode, &address, &operand, opcode);

    switch (opcode) {
	case ADC1: case ADC2: case ADC3: case ADC4: case ADC5:
	case ADC6: case ADC7: case ADC8: case ADC9: case ADCa:
    case ADCb: case ADCc: case ADCd: case ADCe: case ADCf:
	    ADC_execute(address, operand);
	    break;
	case AND1: case AND2: case AND3: case AND4: case AND5:
	case AND6: case AND7: case AND8: case AND9: case ANDa:
    case ANDb: case ANDc: case ANDd: case ANDe: case ANDf:
	    AND_execute(address, operand);
	    break;
	case ASL2:
	    ASL_A_execute();
	    break;
	case ASL1: case ASL3: case ASL4: case ASL5:
	    ASL_mem_execute(address, operand);
	    break;
	case BCC:
	    BCC_execute(address);
	    break;
	case BCS:
	    BCS_execute(address);
	    break;
	case BEQ:
	    BEQ_execute(address);
	    break;
	case BIT1: case BIT2: case BIT3: case BIT4: case BIT5:
	    BIT_execute(operand);
	    break;
	case BMI:
	    BMI_execute(address);
	    break;
	case BNE:
	    BNE_execute(address);
	    break;
	case BPL:
	    BPL_execute(address);
	    break;
    case BRA:
        BRA_execute(address);
        break;
	case BRK:
	    BRK_execute();
	    break;
    case BRL:
        BRL_execute(address);
        break;
	case BVC:
	    BVC_execute(address);
	    break;
	case BVS:
	    BVS_execute(address);
	    break;
	case CLC:
	    CLC_execute();
	    break;
	case CLD:
	    CLD_execute();
	    break;
	case CLI:
	    CLI_execute();
	    break;
	case CLV:
	    CLV_execute();
	    break;
	case CMP1: case CMP2: case CMP3: case CMP4: case CMP5:
    case CMP6: case CMP7: case CMP8: case CMP9: case CMPa:
    case CMPb: case CMPc: case CMPd: case CMPe: case CMPf:
	    CMP_execute(operand);
	    break;
    case COP:
        COP_execute();
	case CPX1: case CPX2: case CPX3:
	    CPX_execute(operand);
	    break;
	case CPY1: case CPY2: case CPY3:
	    CPY_execute(operand);
	    break;
	case DEC1: case DEC2: case DEC3: case DEC4: case DEC5:
	    DEC_execute(address, operand);
	    break;
	case DEX:
	    DEX_execute();
	    break;
	case DEY:
	    DEY_execute();
	    break;
	case EOR1: case EOR2: case EOR3: case EOR4: case EOR5:
    case EOR6: case EOR7: case EOR8: case EOR9: case EORa:
    case EORb: case EORc: case EORd: case EORe: case EORf:
	    EOR_execute(operand);
	    break;
	case INC1: case INC2: case INC3: case INC4: case INC5:
	    INC_execute(address, operand);
	    break;
	case INX:
	    INX_execute();
	    break;
	case INY:
	    INY_execute();
	    break;
	case JMP1: case JMP2: case JMP3: case JMP4: case JMP5:
	    JMP_execute(address);
	    break;
	case JSR1: case JSR2: case JSR3:
	    JSR_execute(address);
	    break;
	case LDA1: case LDA2: case LDA3: case LDA4: case LDA5:
    case LDA6: case LDA7: case LDA8: case LDA9: case LDAa:
    case LDAb: case LDAc: case LDAd: case LDAe: case LDAf:
	    LDA_execute(operand);
	    break;
	case LDX1: case LDX2: case LDX3: case LDX4: case LDX5:
	    LDX_execute(operand);
	    break;
	case LDY1: case LDY2: case LDY3: case LDY4: case LDY5:
	    LDY_execute(operand);
	    break;
	case LSR1: case LSR3: case LSR4: case LSR5:
	    LSR_mem_execute(address, operand);
	    break;
	case LSR2:
	    LSR_A_execute();
	    break;
	case ORA1: case ORA2: case ORA3: case ORA4: case ORA5:
    case ORA6: case ORA7: case ORA8: case ORA9: case ORAa:
    case ORAb: case ORAc: case ORAd: case ORAe: case ORAf:
	    ORA_execute(operand);
	    break;
    case PEA:
        PEA_execute(address);
        break;
    case PEI:
        PEI_execute(address);
        break;
    case PER:
        PER_execute(address);
        break;
	case PHA:
	    PHA_execute();
	    break;
    case PHB:
        PHB_execute();
        break;
    case PHD:
        PHD_execute();
        break;
    case PHK:
        PHK_execute();
        break;
	case PHP:
	    PHP_execute();
	    break;
    case PHX:
        PHX_execute();
        break;
    case PHY:
        PHY_execute();
        break;
    case PLA:
	    PLA_execute();
	    break;
    case PLB:
        PLB_execute();
        break;
    case PLD:
        PLD_execute();
        break;
	case PLP:
	    PLP_execute();
        break;
    case PLX:
        PLX_execute();
        break;
    case PLY:
        PLY_execute();
        break;
    case REP:
        REP_execute(operand);
        break;
    //FINS AQUI
	case ROL1: case ROL3: case ROL4: case ROL5:
	    ROL_mem_execute(address, operand);
	    break;
	case ROL2:
	    ROL_A_execute();
	    break;
	case ROR1: case ROR3: case ROR4: case ROR5:
	    ROR_mem_execute(address, operand);
	    break;
	case ROR2:
	    ROR_A_execute();
	    break;
	case RTI:
	    RTI_execute();
	    break;
	case RTS:
	    RTS_execute();
	    break;
	case SBC1: case SBC2: case SBC3: case SBC4:
	case SBC5: case SBC6: case SBC7: case SBC8:
	    SBC_execute(operand);
	    break;
	case SEC:
	    SEC_execute();
	    break;
	case SED:
	    SED_execute();
	    break;
	case SEI:
	    SEI_execute();
	    break;
	case STA1: case STA2: case STA3:
	case STA4: case STA5: case STA6: case STA7:
	    STA_execute(address);
	    break;
	case STX1: case STX2: case STX3:
	    STX_execute(address);
	    break;
	case STY1: case STY2: case STY3:
	    STY_execute(address);
	    break;
	case TAX:
	    TAX_execute();
	    break;
	case TAY:
	    TAY_execute();
	    break;
	case TSX:
	    TSX_execute();
	    break;
	case TXA:
	    TXA_execute();
	    break;
	case TXS:
	    TXS_execute();
	    break;
	case TYA:
	    TYA_execute();
	    break;
	default : 
	    NOP_execute();
    }
    add_clock_cycles(opcode, address, addr_mode);
    //debug_dump(opcode);
}

void Cpu::add_clock_cycles(uint8_t opcode, uint16_t address, addr_mode_t addr_mode) {
    //Add cycles depending on address mode and instruction
    switch (addr_mode) {
	case IMMEDIATE:
	    clock->cycles += 2;
	    break;
	case ZERO_PAGE:
	    clock->cycles += 3;
	    if (opcode == ASL1 ||
		opcode == ASL2 ||
		opcode == ASL3 ||
		opcode == ASL4 ||
		opcode == ASL5 ||
		opcode == DEC1 || 
		opcode == DEC2 || 
		opcode == DEC3 || 
		opcode == DEC4 || 
		opcode == INC1 || 
		opcode == INC2 || 
		opcode == INC3 || 
		opcode == INC4 || 
		opcode == LSR1 || 
		opcode == LSR2 || 
		opcode == LSR3 || 
		opcode == LSR4 || 
		opcode == LSR5 || 
		opcode == ROL1 ||
		opcode == ROL2 ||
		opcode == ROL3 ||
		opcode == ROL4 ||
		opcode == ROL5 ||
		opcode == ROR1 ||
		opcode == ROR2 ||
		opcode == ROR3 ||
		opcode == ROR4 ||
		opcode == ROR5) {
		    clock->cycles += 2;
	    }
	    break;
	case ZERO_PAGE_X:
	    clock->cycles += 4;
	    if (opcode == ASL1 ||
		opcode == ASL2 ||
		opcode == ASL3 ||
		opcode == ASL4 ||
		opcode == ASL5 ||
		opcode == DEC1 || 
		opcode == DEC2 || 
		opcode == DEC3 || 
		opcode == DEC4 || 
		opcode == INC1 || 
		opcode == INC2 || 
		opcode == INC3 || 
		opcode == INC4 || 
		opcode == LSR1 || 
		opcode == LSR2 || 
		opcode == LSR3 || 
		opcode == LSR4 || 
		opcode == LSR5 || 
		opcode == ROL1 ||
		opcode == ROL2 ||
		opcode == ROL3 ||
		opcode == ROL4 ||
		opcode == ROL5 ||
		opcode == ROR1 ||
		opcode == ROR2 ||
		opcode == ROR3 ||
		opcode == ROR4 ||
		opcode == ROR5) {
		    clock->cycles += 2;
	    }
	    break;
	case ABSOLUTE:
	    clock->cycles += 4;
	    if (opcode == ASL1 ||
		opcode == ASL2 ||
		opcode == ASL3 ||
		opcode == ASL4 ||
		opcode == ASL5 ||
		opcode == DEC1 || 
		opcode == DEC2 || 
		opcode == DEC3 || 
		opcode == DEC4 || 
		opcode == INC1 || 
		opcode == INC2 || 
		opcode == INC3 || 
		opcode == INC4 || 
		opcode == LSR1 || 
		opcode == LSR2 || 
		opcode == LSR3 || 
		opcode == LSR4 || 
		opcode == LSR5 || 
		opcode == JSR  ||
		opcode == ROL1 ||
		opcode == ROL2 ||
		opcode == ROL3 ||
		opcode == ROL4 ||
		opcode == ROL5 ||
		opcode == ROR1 ||
		opcode == ROR2 ||
		opcode == ROR3 ||
		opcode == ROR4 ||
		opcode == ROR5) {
		    clock->cycles += 2;
	    }
	    if (opcode == JMP1 || opcode == JMP2) {
		    clock->cycles -= 1;
	    }
	case ABSOLUTE_X:
	    clock->cycles += 4;
	    if (opcode == STA1 || opcode == STA2 ||
		opcode == STA3 || opcode == STA4 ||
		opcode == STA5 || opcode == STA6 ||
		opcode == STA7) {
		    clock->cycles += 1;
	    }
	    else if (opcode == ASL1 ||
		opcode == ASL2 ||
		opcode == ASL3 ||
		opcode == ASL4 ||
		opcode == ASL5 ||
		opcode == DEC1 || 
		opcode == DEC2 || 
		opcode == DEC3 || 
		opcode == DEC4 || 
		opcode == INC1 || 
		opcode == INC2 || 
		opcode == INC3 || 
		opcode == INC4 || 
		opcode == LSR1 || 
		opcode == LSR2 || 
		opcode == LSR3 || 
		opcode == LSR4 || 
		opcode == LSR5 || 
		opcode == JSR  ||
		opcode == ROL1 ||
		opcode == ROL2 ||
		opcode == ROL3 ||
		opcode == ROL4 ||
		opcode == ROL5 ||
		opcode == ROR1 ||
		opcode == ROR2 ||
		opcode == ROR3 ||
		opcode == ROR4 ||
		opcode == ROR5) {
		    clock->cycles += 3;
	    }
	    else if ((address & 0x00FF) == 0xFF) {
		    //Page crossing
		    clock->cycles += 1;
	    }
	    break;
	case ABSOLUTE_Y:
	    clock->cycles += 4;
	    if (opcode == STA1 || opcode == STA2 ||
		opcode == STA3 || opcode == STA4 ||
		opcode == STA5 || opcode == STA6 ||
		opcode == STA7) {
		    clock->cycles += 1;
	    }
	    else if ((address & 0x00FF) == 0xFF) {
		    //Page crossing
		    clock->cycles += 1;
	    }
	    break;
	case INDIRECT_X:
	    clock->cycles += 6;
	    break;
	case INDIRECT_Y:
	    clock->cycles += 5;
	    if (opcode == STA1 || opcode == STA2 ||
		opcode == STA3 || opcode == STA4 ||
		opcode == STA5 || opcode == STA6 ||
		opcode == STA7) {
		    clock->cycles += 1;
	    }
	    else if ((address & 0x00FF) == 0xFF) {
		    //Page crossing
		    clock->cycles += 1;
	    }
	    break;
	case RELATIVE:
	    clock->cycles += 2;
	    break;
	case INDIRECT:
	    clock->cycles += 5;
	    break;
	case IMPLIED:
	    clock->cycles += 2;
	    if (opcode == PHA || opcode == PHP) {
		    clock->cycles += 1;
	    }
	    else if (opcode == PLA || opcode == PLP) {
		    clock->cycles += 2;
	    }
	    else if (opcode == RTI || opcode == RTS) {
		    clock->cycles += 4;
	    }
	    else if (opcode == BRK) {
		    clock->cycles += 5;
	    }
	    break;
	default:
	    //SOMETHING WENT REALLY WRONG
	    clock->cycles += 1;
	    cout << "Timing unrecognized" << endl;
    }
}

void Cpu::read_operand(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand, uint8_t opcode) {
    *addr_mode = IMPLIED;
    *address = 0;
    *operand = 0;
    uint8_t column = opcode & 0x1f;
    uint8_t row    = opcode & 0xe0;

    switch (column) {
	case 0x00:
	    if (opcode == JSR1)
		    read_operand_absolute(addr_mode, address, operand);
        else if (opcode == BRA)
            read_operand_relative(addr_mode, address, operand);
        else if (opcode == BRK)
            read_operand_immediate(addr_mode, address, operand);
	    else if (opcode != RTI && opcode != RTS) {
            if (regP.X)
		        read_operand_immediate(addr_mode, address, operand);
            else
                read_operand_immediate_long(addr_mode, address, operand);
        }
	    break;
	case 0x01:
	    read_operand_direct_indexed_indirect(addr_mode, address, operand);
	    break;
	case 0x02:
	    if (opcode == COP || opcode == REP || opcode == SEP)
		    read_operand_immediate(addr_mode, address, operand);
        else if (opcode == LDX) {
            if (regP.X)
                read_operand_immediate(addr_mode, address, operand);
            else
                read_operand_immediate_long(addr_mode, address, operand);
        }
	    else if (opcode == PER || opcode == BRL)
            read_operand_relative_long(addr_mode, address, operand);
        else
            read_operand_absolute_long(addr_mode, address, operand);	    
	    break;
    case 0x03:
        read_operand_stack_relative(addr_mode, address, operand);
        break;
	case 0x04:
        if (opcode == MVP)
            read_operand_block_move(addr_mode, address, operand);
        else
	        read_operand_direct(addr_mode, address, operand);
	    break;
	case 0x05:
	    read_operand_direct(addr_mode, address, operand);
	    break;
	case 0x06:
	    read_operand_direct(addr_mode, address, operand);
	    break;
    case 0x07:
        read_operand_direct_indirect_long(addr_mode, address, operand);    
	case 0x09:
        if (regP.M)
            read_operand_immediate(addr_mode, address, operand);
        else
            read_operand_immediate_long(addr_mode, address, operand);
        break;
	case 0x0c:
	    if (opcode == JMP3) 
		    read_operand_absolute_indirect(addr_mode, address, operand);
	    else 
            read_operand_absolute(addr_mode, address, operand);
	    break;
	case 0x0d:
	    read_operand_absolute(addr_mode, address, operand);
	    break;
	case 0x0e:
	    read_operand_absolute(addr_mode, address, operand);
	    break;
    case 0x0f:
	    read_operand_absolute_long(addr_mode, address, operand);
        break;
	case 0x10:
        read_operand_relative(addr_mode, address, operand);
	    break;
	case 0x11:
	    read_operand_direct_indirect_indexed(addr_mode, address, operand);
	    break;
    case 0x12:
        read_operand_direct_indirect(addr_mode, address, operand);
        break;
    cases 0x13:
        read_operand_stack_relative_indirect_indexed(addr_mode, address, operand);
        break;
	case 0x14:
        if (opcode == TRB1)
            read_operand_direct(addr_mode, address, operand);
        else if (opcode == MVN)
            read_operand_block_move(addr_mode, address, operand);
        else if (opcode == PEI)
            read_operand_direct_indirect(addr_mode, address, operand);
        else if (opcode == PEA)
            read_operand_absolute(addr_mode, address, operand);
        else
            read_operand_direct_indexed_x(addr_mode, address, operand);
        break;
	case 0x15:
        read_operand_direct_indexed_x(addr_mode, address, operand);
	    break;
	case 0x16:
        if (opcode == STX3 || opcode == LDX4)
            read_operand_direct_indexed_y(addr_mode, address, operand);
        else
            read_operand_direct_indexed_x(addr_mode, address, operand);	    
	    break;
    case 0x17:
        read_operand_direct_indirect_long_indexed(addr_mode, address, operand);
        break;
	case 0x19:
	    read_operand_absolute_indexed_y(addr_mode, address, operand);
	    break;
	case 0x1c:
        if (opcode == TRB2)
	        read_operand_absolute(addr_mode, address, operand);
        else if (opcode == JMP2)
            read_operand_absolute_long(addr_mode, address, operand);
        else if (opcode == JMP4)
            read_operand_absolute_indexed_indirect(addr_mode, address, operand);
        else if (opcode == STZ3)
            read_operand_absolute(addr_mode, address, operand);
        else if (opcode == JMP5)
            read_operand_absolute_indirect_long(addr_mode, address, operand);
        else if (opcode == JSR3)
            read_operand_absolute_indexed_indirect(addr_mode, address, operand);
        else
            read_operand_absolute_indexed_x(addr_mode, address, operand);
	    break;
	case 0x1d:
        read_operand_absolute_indexed_x(addr_mode, address, operand);
	    break;
	case 0x1e:
	    if (opcode == LDX5) 
            read_operand_absolute_indexed_y(addr_mode, address, operand);
	    else 
            read_operand_absolute_indexed_x(addr_mode, address, operand);
	    break;
	default:
	    regPC = regPC + 1; //No operand
    }
}

//////////////////////
// ADDRESSING MODES //
//////////////////////

void Cpu::read_operand_immediate(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand) {
    // INS #byte
    // address = 0
    // operand = byte
    uint8_t op = mem.read_byte(regPC + 1);
    
    *address = 0;
    *operand = op;
    *addr_mode = IMMEDIATE;
    regPC += 2;
}

void Cpu::read_operand_immediate_long(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand) {
    // INS #word
    // address = 0
    // operand = word
    uint8_t op_l = mem.read_byte(regPC + 1);
    uint8_t op_h = mem.read_byte(regPC + 2);
    
    *address = 0;
    *operand = ((op_h << 8) & 0xff00) | (op_l & 0x00ff);
    *addr_mode = IMMEDIATE_LONG;
    regPC += 3;
}

void Cpu::read_operand_relative(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand) {
    // INS #byte
    // address = PC + increment (signed)
    // operand = increment

    int8_t increment = mem.read_byte(regPC + 1);

    regPC += 2;
    *address = (int16_t) regPC + (int16_t) increment;
    *operand = increment;
    *addr_mode = RELATIVE;
}

void Cpu::read_operand_relative_long(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand) {
    // INS #word
    // address = PC + increment (signed)(2-bytes)
    // operand = increment

    int16_t increment = mem.read_word(regPC + 1);

    regPC += 3;
    *address = (int16_t) regPC + increment;
    *operand = increment;
    *addr_mode = RELATIVE_LONG;
}

void Cpu::read_operand_direct(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand) {
    // INS $byte
    // address = 00:DP+byte
    // operand = ram[address]

    *address = (mem.read_byte(regPC + 1) + regDP) & 0x00ffff;
    *operand = mem.read_word(byte);
    *addr_mode = DIRECT;
    regPC += 2;
}

void Cpu::read_operand_direct_indexed_x(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand) {
    // INS $byte, x
    // address = 00:DP+byte+x
    // operand = ram[address]

    *address = ((mem.read_byte(regPC + 1) + regX + regDP) & 0x00ffff;
    *operand = mem.read_word(addr);
    *addr_mode = DIRECT_INDEXED;
    regPC += 2;
}

void Cpu::read_operand_direct_indexed_y(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand) {
    // INS $byte, y
    // address = 00:DP+byte + y
    // operand = ram[address]

    *address = (mem.read_byte(regPC + 1) + regY + regDP) & 0x00ffff;
    *operand = mem.read_word(addr);
    *addr_mode = DIRECT_INDEXED;
    regPC += 2;
}

void Cpu::read_operand_direct_indirect(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand) {
    // INS ($byte)
    // address = DB:ram[byte+DP]
    // operand = ram[address]

    uint32_t ptr = (mem.read_byte(regPC + 1) + regDP) & 0x00ffff;
    
    *address = (DB << 16) + mem.read_word(ptr);
    *operand = mem.read_word(*address);
    *addr_mode = DIRECT_INDIRECT;
    regPC += 2;
}

void Cpu::read_operand_indexed_indirect(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand) {
    // INS ($byte, x)
    // address = ram[00:00:byte + x]
    // operand = ram[address]

    uint32_t ptr = mem.read_byte(regPC + 1) + regX;

    *address = mem.read_word(ptr);
    *operand = mem.read_word(*address);
    *addr_mode = DIRECT_INDEXED_INDIRECT;
    regPC += 2;
}

void Cpu::read_operand_direct_indirect_long(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand) {
    // INS [$byte]
    // address_long = ram[byte+DP]
    // operand = ram[address_long]

    uint32_t ptr = (mem.read_byte(regPC + 1) + regDP) & 0x00ffff;
    
    *address = mem.read_long(ptr);
    *operand = mem.read_word(*address);
    *addr_mode = DIRECT_INDIRECT_LONG;
    regPC += 2;
}

void Cpu::read_operand_absolute(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand) {
    // INS $word
    // address = DB:word
    // operand = ram[address]

    uint32_t addr = (regDB << 16) | mem.read_word(regPC + 1);
    *address = addr;
    *operand = mem.read_word(*address);
    *addr_mode = ABSOLUTE;
    regPC += 3;
}

void Cpu::read_operand_absolute_indexed_x(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand) {
    // INS $word, x
    // address = DB:word + x
    // operand = ram[address]

    uint32_t addr = 
        ((regDB << 16) | ((mem.read_word(regPC + 1) + regX) & 0x00ffff);
    *address = addr;
    *operand = mem.read_word(*address);
    *addr_mode = ABSOLUTE_INDEXED;
    regPC += 3;
}

void Cpu::read_operand_absolute_indexed_y(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand) {
    // INS $word, y
    // address = DB:word + y
    // operand = ram[address]

    uint32_t addr = 
        ((regDB << 16) | ((mem.read_word(regPC + 1) + regY) & 0x00ffff);
    *address = addr;
    *operand = mem.read_word(*address);
    *addr_mode = ABSOLUTE_INDEXED;
    regPC += 3;
}

void Cpu::read_operand_absolute_long(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand) {
    // INS $long
    // address = long
    // operand = ram[address]

    *address = mem.read_long(regPC + 1);
    *operand = mem.read_word(*address);
    *addr_mode = ABSOLUTE_LONG;
    regPC += 4;
}

void Cpu::read_operand_absolute_indexed_long(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand) {
    // INS $long, x
    // address = long + x
    // operand = ram[address]

    *address = mem.read_long(regPC + 1) + regX;
    *operand = mem.read_word(*address);
    *addr_mode = ABSOLUTE_INDEXED_LONG;
    regPC += 4;
}

void Cpu::read_operand_stack_relative(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand) {
    // INS $byte, s
    // address = 00:regSP + byte
    // operand = ram[address]

    *address = (mem.read_byte(regPC + 1) + regSP) & 0x00ffff;
    *operand = mem.read_word(*address);
    *addr_mode = STACK_RELATIVE;
    regPC += 2;
}

void Cpu::read_operand_stack_relative_indirect_indexed(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand) {
    // INS ($byte, s), y
    // address = ram[00:regSP + byte]
    // operand = ram[address]

    uint32_t ptr = (mem.read_byte(regPC + 1) + regSP) & 0x00ffff;

    *address = (regDB << 16) + mem.read_word(ptr) + regY;
    *operand = mem.read_word(address);
    *addr_mode = STACK_RELATIVE_INDIRECT_INDEXED;
    regPC += 2;
}

void Cpu::read_operand_absolute_indirect(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand) {
    // INS ($word)
    // address = ram[word]
    // operand = ram[address]

    uint32_t ptr = mem.read_word(regPC + 1);

    *address = mem.read_word(ptr);
    *operand = mem.read_word(address);
    *addr_mode = ABSOLUTE_INDIRECT;
    regPC += 2;
}

void Cpu::read_operand_absolute_indirect_long(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand) {
    // INS [$word]
    // address_long = ram[word]
    // operand = ram[address_long]

    uint32_t ptr = mem.read_word(regPC + 1);

    *address = mem.read_long(ptr);
    *operand = mem.read_word(address);
    *addr_mode = ABSOLUTE_INDIRECT_LONG;
    regPC += 2;
}

void Cpu::read_operand_absolute_indexed_indirect(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand) {
    // INS ($word, x)
    // address = ram[word + x]
    // operand = ram[address]

    uint32_t ptr = (mem.read_word(regPC + 1) + regX) & 0x00ffff;

    *address = mem.read_word(ptr);
    *operand = mem.read_word(address);
    *addr_mode = ABSOLUTE_INDEXED_INDIRECT;
    regPC += 2;
}

void Cpu::read_operand_absolute_long_indexed(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand) {
    // INS $long, x
    // address = long + x
    // operand = ram[address]

    *address = mem.read_long(regPC + 1) + regX;
    *operand = mem.read_word(address);
    *addr_mode = ABSOLUTE_LONG_INDEXED;
    regPC += 3;
}

void Cpu::read_operand_block_move(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand) {
    // MV[N,P] dst, src
    // address = 0
    // operand = (dst << 8) | src

    *address = 0;
    *operand = ((mem.read_byte(regPC+1)<<8)&0x00ff) | mem.read_byte(regPC+2);
    *addr_mode = BLOCK_MOVE;
    regPC += 3;
}

void Cpu::read_operand_direct_indexed_indirect(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand) {
    // INS ($byte, x)
    // address = ram[DP+byte+x]
    // operand = ram[address]

    uint32_t ptr = (regDP + regX + mem.read_byte(regPC+1)) & 0x00ffff;
    *address = mem.read_word(ptr);
    *operand = mem.read_word((regDB << 16) + *address);
    *addr_mode = DIRECT_INDEXED_INDIRECT;
    regPC += 2;
}

void Cpu::read_operand_direct_indirect_indexed(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand) {
    // INS ($byte), y
    // address = (DB:mem[DP+byte])+y
    // operand = ram[address]

    uint32_t ptr = (mem.read_byte(regPC+1) + regDP) & 0x00ffff;

    *address = mem.read_word(ptr) + (regDB << 16) + regY;
    *operand = mem.read_word(*address);
    *addr_mode = DIRECT_INDIRECT_INDEXED;
    regPC += 2;
}

void Cpu::read_operand_direct_indirect_long_indexed(addr_mode_t *addr_mode, uint32_t *address, uint16_t *operand) {
    // INS [byte], y
    // address_long = mem[DP+byte] + y
    // operand = ram[address_long]

    uint32_t ptr = (mem.read_byte(regPC+1) + regDP) & 0x00ffff;

    *address = mem.read_long(ptr) + regY;
    *operand = mem.read_word(*address);
    *addr_mode = DIRECT_INDIRECT_LONG_INDEXED;
    regPC += 2;
}

///////////////////////////
// INSTRUCTION EXECUTION //
///////////////////////////

void Cpu::ADC_execute(uint16_t operand) {
    // Add with carry
    // A,Z,C,N = A+M+C

    bool signResult, signRegA, signOperand;
    bool isCarry = false;

    uint16_t result_part = regA + operand;
    if (regP.M)
        if ((uint8_t) result_part < (uint8_t) regA) isCarry = true;
    else
        if (result_part < regA) isCarry = true;
    
    uint16_t result = result_part + regP.C;
    if (regP.M)
        if ((uint8_t) result < (uint8_t) result_part) isCarry = true;
    else
        if (result < regA) isCarry = true;
    
    if (regP.M) {
        signResult = (result >> 15) & 1;
        signRegA = (regA >> 15) & 1;
        signOperand = (operand >> 15) & 1;
        regP.Z = (uint8_t) result == 0;
        regA = (uint8_t) result;
    }
    else {
        signResult = (result >> 7) & 1;
        signRegA = (regA >> 7) & 1;
        signOperand = (operand >> 7) & 1;
        regP.Z = result == 0;
    }

    regP.C = isCarry;
    regP.V = ((signRegA == signOperand) && (signResult != signRegA));
    regP.N = signResult;

}

void Cpu::AND_execute(uint16_t operand) {
    // Logical AND
    // A,Z,N = A&M
    uint16_t result = regA & operand;

    if (regP.M) {
        regA = (result & 0x00ff);

        regP.Z = (uint8_t) result == 0;
        regP.N = (result >> 7) & 1;
    }
    else {
        regA = result;

        regP.Z = result == 0;
        regP.N = (result >> 7) & 1;
    }
}

void Cpu::ASL_mem_execute(uint32_t address, uint16_t operand) {
    // Arithmetic shift left (Memory content)
    // M,Z,C,N = M*2

    uint16_t result = operand << 1;

    mem.write_word(address, result);

    regP.Z = result == 0;
    regP.C = (operand >> 15) & 1;
    regP.N = (result >> 15) & 1;
}

void Cpu::ASL_A_execute() {
    // Arithmetic shift left (Accumulator content)
    // A,Z,C,N = A*2

    uint16_t result = regA << 1;

    if (regP.M) {
        regP.Z = (uint8_t) result == 0;
        regP.C = (regA >> 7) & 1;
        regP.N = (result >> 7) & 1;

        regA = result & 0x00ff;
    }
    else {
        regP.Z = result == 0;
        regP.C = (regA >> 15) & 1;
        regP.N = (result >> 15) & 1;

        regA = result;
    }

}

void Cpu::BCC_execute(uint32_t address) {
    // Branch if carry clear
    
    if (!regP.C) {
	    regPC = address;
    }
}

void Cpu::BCS_execute(uint32_t address) {
    // Branch if carry set
    
    if (regP.C) {
	    regPC = address;
    }
}

void Cpu::BEQ_execute(uint32_t address) {
    // Branch if equal

    if (regP.Z) {
	    regPC = address;
    }
}

void Cpu::BIT_execute(uint16_t operand) {
    // Bit test
    // tmp = A & M, N = tmp[7], V = tmp[6]

    uint16_t tmp = regA & operand;

    if (regP.M) {
        regP.Z = (uint8_t) tmp == 0;
        regP.V = (tmp >> 6) & 1;
        regP.N = (tmp >> 7) & 1;
    }
    else {
        regP.Z = tmp == 0;
        regP.V = (tmp >> 14) & 1;
        regP.N = (tmp >> 15) & 1;
    }
}

void Cpu::BIT_imm_execute(uint16_t operand) {
    // Bit test
    // tmp = A & M, N = tmp[7], V = tmp[6]

    uint16_t tmp = regA & operand;

    if (regP.M) {
        regP.Z = (uint8_t) tmp == 0;
    }
    else {
        regP.Z = tmp == 0;
    }
}

void Cpu::BMI_execute(uint32_t address) {
    // Branch if minus
    
    if (regP.N) {
	    regPC = address;
    }
}

void Cpu::BNE_execute(uint32_t address) {
    // Branch if not equal

    if (!regP.Z) {
	    regPC = address;
    }
}

void Cpu::BPL_execute(uint32_t address) {
    // Branch if positive

    if (!regP.N) {
	    regPC = address;
    }
}

void Cpu::BRK_execute() {
    // Force interrupt

    uint8_t lowPC, highPC;
    lowPC = regPC;
    highPC = (regPC >> 8);
    
    uint8_t procStat;
    procStat =  (regP.N) << 7 |
		(regP.V) << 6 |
        (regP.M) << 5 |
		(regP.B) << 4 |
		(regP.D) << 3 |
		(regP.I) << 2 |
		(regP.Z) << 1 |
		(regP.C);
    
    //We push the PC and Processor Status into the stack
    pushStack(highPC);
    pushStack(lowPC);
    pushStack(procStat);
    if (!regP.E)
        pushStack(regPB);

    //We set the PC to the interrupt vector and the Break command
    regPC = mem.read_word(BRK_INT_VECTOR_ADDR);
    regPB = 0;
    regP.D = 0;
    regP.I = 1;
}

void Cpu::COP_execute() {
    // Co-Processor enable (Same as BRK)

    uint8_t lowPC, highPC;
    lowPC = regPC;
    highPC = (regPC >> 8);
    
    uint8_t procStat;
    procStat =  (regP.N) << 7 |
		(regP.V) << 6 |
		(regP.M) << 5 |
		(regP.X) << 4 |
		(regP.D) << 3 |
		(regP.I) << 2 |
		(regP.Z) << 1 |
		(regP.C);
    
    //We push the PC and Processor Status into the stack
    pushStack(highPC);
    pushStack(lowPC);
    pushStack(procStat);
    if (!regP.E)
        pushStack(regPB);

    //We set the PC to the interrupt vector and the Break command
    regPC = mem.read_word(COP_INT_VECTOR_ADDR);
    regPB = 0;
    regP.D = 0;
    regP.I = 1;
}

void Cpu::BVC_execute(uint32_t address) {
    // Branch if overflow clear

    if (!regP.V) {
	    regPC = address;
    }
}

void Cpu::BVS_execute(uint32_t address) {
    //Branch if overflow set

    if (regP.V) {
	    regPC = address;
    }
}

void Cpu::BRA_execute(uint32_t address) {
    //Branch always

    regPC = address;
}

void Cpu::BRL_execute(uint32_t address) {
    //Branch always long
    //The address will be already computed

    regPC = address;
}

void Cpu::CLC_execute() {
    //Clear carry flag

    regP.C = 0;
}

void Cpu::CLD_execute() {
    //Clear decimal mode

    regP.D = 0;
}

void Cpu::CLI_execute() {
    //Clear interrupt disable

    regP.I = 0;
}

void Cpu::CLV_execute() {
    //Clear overflow flag

    regP.V = 0;
}

void Cpu::CMP_execute(uint16_t operand) {
    //Compare
    //Z,C,N = A-M

    uint16_t comp = regA - operand;

    if (regP.M) {
        regP.C = (uint8_t) regA >= (uint8_t) operand;
        regP.Z = (uint8_t) regA == (uint8_t) operand;
        regP.N = (comp >> 7) & 1;
    }
    else {
        regP.C = regA >= operand;
        regP.Z = regA == operand;
        regP.N = (comp >> 15) & 1;
    }
}

void Cpu::CPX_execute(uint16_t operand) {
    //Compare X register
    //Z,C,N = X-M

    uint16_t comp = regX - operand;

    if (regP.X) {
        regP.C = (uint8_t) regX >= (uint8_t) operand;
        regP.Z = (uint8_t) regX == (uint8_t) operand;
        regP.N = (comp >> 7) & 1;
    }
    else {
        regP.C = regX >= operand;
        regP.Z = regX == operand;
        regP.N = (comp >> 15) & 1;
    }
}   

void Cpu::CPY_execute(uint16_t operand) {
    //Compare Y register
    //Z,C,N = Y-M

    uint16_t comp = regY - operand;

    if (regP.X) {
        regP.C = (uint8_t) regY >= (uint8_t) operand;
        regP.Z = (uint8_t) regY == (uint8_t) operand;
        regP.N = (comp >> 7) & 1;
    }
    else {
        regP.C = regY >= operand;
        regP.Z = regY == operand;
        regP.N = (comp >> 15) & 1;
    }
}

void Cpu::DEC_execute(uint32_t address, uint16_t operand) {
    //Decrement memory
    //M,Z,N = M-1

    uint16_t result = operand-1;

    mem.write_word(address, result);

    regP.Z = result == 0;
    regP.N = (result >> 15) & 1;
}

void Cpu::DEX_execute() {
    //Decrement X register
    //X,Z,N = X-1

    uint16_t result = regX-1;

    if (regP.X) {
        regX = (uint8_t) result;

        regP.Z = (uint8_t) result == 0;
        regP.N = (result >> 7) & 1;
    }
    else {
        regX = result;

        regP.Z = result == 0;
        regP.N = (result >> 15) & 1;
    }
}

void Cpu::DEY_execute() {
    //Decrement Y register
    //Y,Z,N = Y-1

    uint16_t result = regY-1;

    if (regP.X) {
        regY = (uint8_t) result;

        regP.Z = (uint8_t) result == 0;
        regP.N = (result >> 7) & 1;
    }
    else {
        regY = result;

        regP.Z = result == 0;
        regP.N = (result >> 15) & 1;
    }
}

void Cpu::EOR_execute(uint16_t operand) {
    //Exclusive OR
    //A,Z,N = A^M

    uint16_t result = regA ^ operand;

    if (regP.M) {
        regA = (uint8_t) result;

        regP.Z = (uint8_t) result == 0;
        regP.N = (result >> 7) & 1;
    }
    else {
        regA = result;

        regP.Z = result == 0;
        regP.N = (result >> 15) & 1;
    }
}

void Cpu::INC_execute(uint32_t address, uint16_t operand) {
    //Increment memory
    //M,Z,N = M+1

    uint16_t result = operand + 1;

    if (regP.M) {
        mem.write_byte(address, (uint8_t) result);

        regP.Z = (uint8_t) result == 0;
        regP.N = (result >> 7) & 1;
    }
    else {
        mem.write_word(address, result);

        regP.Z = result == 0;
        regP.N = (result >> 15) & 1;
    }
}

void Cpu::INX_execute() {
    //Increment X register
    //X,Z,N = X+1

    uint16_t result = regX + 1;

    if (regP.X) {
        regX = (uint8_t) result;

        regP.Z = (uint8_t) result == 0;
        regP.N = (result >> 7) & 1;
    }
    else {
        regX = result;

        regP.Z = result == 0;
        regP.N = (result >> 15) & 1;
    }
}

void Cpu::INY_execute() {
    //Increment Y register
    //Y,Z,N = Y+1

    uint16_t result = regY + 1;

    if (regP.X) {
        regY = (uint8_t) result;

        regP.Z = (uint8_t) result == 0;
        regP.N = (result >> 7) & 1;
    }
    else {
        regY = result;

        regP.Z = result == 0;
        regP.N = (result >> 15) & 1;
    }
}

void Cpu::JMP_execute(uint32_t address) {
    //Jump
    
    regPC = address;
}

void Cpu::JSR_execute(uint32_t address) {
    //Jump to subroutine

    uint16_t returnAddr = regPC - 1; //Last byte of the instruction
    pushStack((returnAddr >> 8) & 0x00ff); //high
    pushStack(returnAddr & 0x00ff); //low
    regPC = address;
}

void Cpu::JSL_execute(uint32_t address) {
    //Jump to subroutine long

    uint16_t returnAddr = regPC - 1; //Last byte of the instruction
    pushStack(regPB);
    pushStack((returnAddr >> 8) & 0x00ff); //high
    pushStack(returnAddr & 0x00ff); //low
    //PB:PC = address
    regPC = address & 0x0000ffff;
    regPB = (address >> 16) & 0x000000ff;
}

void Cpu::LDA_execute(uint16_t operand) {
    //Load accumulator
    //A,Z,N = M

    if (regP.M) {
        regA = (uint8_t) operand;

        regP.Z = (uint8_t) regA == 0;
        regP.N = (regA >> 7) & 1;
    }
    else {
        regA = operand;

        regP.Z = regA == 0;
        regP.N = (regA >> 15) & 1;
}

void Cpu::LDX_execute(uint16_t operand) {
    //Load X register
    //X,Z,N = M

    if (regP.X) {
        regX = (uint8_t) operand;

        regP.Z = (uint8_t) regX == 0;
        regP.N = (regX >> 7) & 1;
    }
    else {
        regX = operand;

        regP.Z = regX == 0;
        regP.N = (regX >> 15) & 1;
    }
}

void Cpu::LDY_execute(uint16_t operand) {
    //Load Y register
    //Y,Z,N = M

    if (regP.X) {
        regY = (uint8_t) operand;

        regP.Z = (uint8_t) regY == 0;
        regP.N = (regY >> 7) & 1;
    }
    else {
        regY = operand;

        regP.Z = regY == 0;
        regP.N = (regY >> 15) & 1;
    }
}

void Cpu::LSR_mem_execute(uint32_t address, uint16_t operand) {
    // Logical shift right (Memory content)
    // M,Z,C,N = M/2

    uint16_t result = (operand >> 1) & 0x7fff;

    mem.write_word(address, result);

    regP.Z = result == 0;
    regP.C = operand & 1;
    regP.N = (result >> 15) & 1;
}

void Cpu::LSR_A_execute() {
    // Logical shift right (Accumulator content)
    // A,Z,C,N = A/2

    uint16_t result = (regA >> 1) & 0x7fff;

    if (regP.M) {
        regP.Z = (uint8_t) result == 0;
        regP.C = regA & 1;
        regP.N = (result >> 7) & 1;

        regA = (uint8_t) result;
    }
    else {
        regP.Z = result == 0;
        regP.C = regA & 1;
        regP.N = (result >> 15) & 1;

        regA = result;
    }
}

void Cpu::MVN_execute(uint8_t srcBank, uint8_t destBank) {
    //Move block negative

    uint32_t len, srcAddr, destAddr;
    if (regP.M)
        len = regA & 0x00ff;
    else
        len = regA;

    len++;

    if (regP.X) {
        srcAddr = (srcBank << 16) & 0xff0000 | (regX & 0x00ff);
        srcAddr = (dstBank << 16) & 0xff0000 | (regY & 0x00ff);
    }
    else {
        srcAddr = (srcBank << 16) & 0xff0000 | regX;
        dstAddr = (srcBank << 16) & 0xff0000 | regY;
    }

    do {
        uint8_t byte = mem.read_byte(srcAddr);
        mem.write_byte(dstAddr, byte);
        srcAddr++;
        dstAddr++;
        len--;
    } while (len != 0xffffffff);

    regDB = destBank;

    if (regP.M)
        regA = len & 0x00ff;
    else
        regA = len & 0x00ffff;

    if (regP.X) {
        regX = srcAddr & 0x0000ff;
        regY = dstAddr & 0x0000ff;
    }
    else {
        regX = srcAddr & 0x00ffff;
        regY = dstAddr & 0x00ffff;
    }
}

void Cpu::MVP_execute(uint8_t srcBank, uint8_t destBank) {
    //Move block positive

    uint32_t len, srcAddr, destAddr;
    if (regP.M)
        len = regA & 0x00ff;
    else
        len = regA;

    len++;

    if (regP.X) {
        srcAddr = (srcBank << 16) & 0xff0000 | (regX & 0x00ff);
        srcAddr = (dstBank << 16) & 0xff0000 | (regY & 0x00ff);
    }
    else {
        srcAddr = (srcBank << 16) & 0xff0000 | regX;
        dstAddr = (srcBank << 16) & 0xff0000 | regY;
    }

    do {
        uint8_t byte = mem.read_byte(srcAddr);
        mem.write_byte(dstAddr, byte);
        srcAddr--;
        dstAddr--;
        len--;
    } while (len != 0xffffffff);

    regDB = destBank;

    if (regP.M)
        regA = len & 0x00ff;
    else
        regA = len & 0x00ffff;

    if (regP.X) {
        regX = srcAddr & 0x0000ff;
        regY = dstAddr & 0x0000ff;
    }
    else {
        regX = srcAddr & 0x00ffff;
        regY = dstAddr & 0x00ffff;
    }
}

void Cpu::NOP_execute() {
    // No operation
}

void Cpu::ORA_execute(uint16_t operand) {
    // Logical inclusive OR
    // A,Z,N = A|M

    uint16_t result = regA | operand;

    if (regP.M) {
	regA = (uint8_t) result;

    	regP.Z = (uint8_t) result == 0;
    	regP.N = (result >> 7) & 1;
    }
    else {
	regA = result;

	regP.Z = result == 0;
	regP.N = (result >> 15) & 1;
    }
}

void Cpu::PEA_execute(uint32_t address) {
    // Push effective absolute address

    pushStack(address & 0x00ff);
    pushStack((address >> 8) & 0x00ff);
}

void Cpu::PEI_execute(uint32_t address) {
    // Push effective indirect address

    pushStack(address & 0x00ff);
    pushStack((address >> 8) & 0x00ff);
}

void Cpu::PER_execute(uint32_t address) {
    // Push effective PC relative indirect address

    pushStack(address & 0x00ff);
    pushStack((address >> 8) & 0x00ff);
}

void Cpu::PHA_execute() {
    // Push accumulator

    if (!regP.M)
	pushStack((regA >> 8) & 0x00ff);
    pushStack(regA & 0x00ff);
}

void Cpu::PHB_execute() {
    // Push databank

    pushStack(regDB);
}

void Cpu::PHD() {
    // Push direct page register

    pushStack(regDP);
}

void Cpu::PHK() {
    // Push program bank register

    pushStack(regPB);
}

void Cpu::PHP_execute() {
    // Push processor status
    
    uint8_t procStat;
    procStat =  (regP.N) << 7 |
		(regP.V) << 6 |
		(regP.M) << 5 |
		(regP.X) << 4 |
		(regP.D) << 3 |
		(regP.I) << 2 |
		(regP.Z) << 1 |
		(regP.C);
    pushStack(procStat);
}

void Cpu::PHX_execute() {
    // Push index register X

    if (!regP.X)
	pushStack((regX >> 8) & 0x00ff);
    pushStack(regX & 0x00ff);
}

void Cpu::PHY_execute() {
    // Push index register Y

    if (!regP.X)
	pushStack((regY >> 8) & 0x00ff);
    pushStack(regY & 0x00ff);
}

void Cpu::PLA_execute() {
    // Pull accumulator

    regA = pullStack();
    if (!regP.M) {
	regA |= (pullStack() << 8);

	regP.Z = regA == 0;
	regP.N = (regA >> 15) & 1;
    }
    else {
        regP.Z = (uint8_t) regA == 0;
	regP.N = (regA >> 7) & 1;
    }
}

void Cpu::PLB_execute() {
    // Pull data bank

    regDB = pullStack();

    regP.Z = regDB == 0;
    regP.N = (regDB >> 7) & 1;
}

void Cpu::PLD_execute() {
    // Pull direct page register

    regDP = pullStack();

    regP.Z = regDP == 0;
    regP.N = (regDP >> 7) & 1;
}

void Cpu::PLP_execute() {
    // Pull processor status

    uint8_t procStat = pullStack();
    regP.N = (procStat >> 7) & 1;
    regP.V = (procStat >> 6) & 1;
    regP.M = (procStat >> 5) & 1;
    regP.X = (procStat >> 4) & 1;
    regP.D = (procStat >> 3) & 1;
    regP.I = (procStat >> 2) & 1;
    regP.Z = (procStat >> 1) & 1;
    regP.C = procStat & 1;

    if (regP.E) {
	regP.X = 1;
	regP.M = 1;
    }

    if (regP.X) {
	regX &= 0x00ff;
	regY &= 0x00ff;
    }
}

void Cpu::PLX_execute() {
    // Pull index register X

    regX = pullStack();
    if (!regP.X) {
	regX |= (pullStack() << 8);

	regP.Z = regX == 0;
	regP.N = (regX >> 15) & 1;
    }
    else {
        regP.Z = (uint8_t) regX == 0;
	regP.N = (regX >> 7) & 1;
    }
}

void Cpu::PLY_execute() {
    // Pull index register Y

    regY = pullStack();
    if (!regP.X) {
	regY |= (pullStack() << 8);

	regP.Z = regY == 0;
	regP.N = (regY >> 15) & 1;
    }
    else {
        regP.Z = (uint8_t) regY == 0;
	regP.N = (regY >> 7) & 1;
    }
}

void Cpu::REP_execute(uint16_t operand) {
    // Reset status bits

    uint8_t noperand = ~((uint8_t) operand);
    regP.N &= ((noperand >> 7) & 1);
    regP.V &= ((noperand >> 6) & 1);
    regP.M &= ((noperand >> 5) & 1);
    regP.X &= ((noperand >> 4) & 1);
    regP.D &= ((noperand >> 3) & 1);
    regP.I &= ((noperand >> 2) & 1);
    regP.Z &= ((noperand >> 1) & 1);
    regP.C &= (noperand & 1);

    if (regP.E) {
	regP.X = 1;
	regP.M = 1;
    }
}

void Cpu::ROL_mem_execute(uint32_t address, uint16_t operand) {
    // Rotate left (memory contents)

    uint16_t result = (operand << 1) | regP.C;

    mem.write_word(address, result);

    regP.C = (operand >> 15) & 1;
    regP.Z = result == 0;
    regP.N = (result >> 15) & 1;
}

void Cpu::ROL_A_execute() {
    // Rotate Left (accumulator)

    uint8_t result = (regA << 1) | regP.C;

    if (regP.M) {
	regP.C = (regA >> 7) & 1;
    	regP.Z = (uint8_t) result == 0;
    	regP.N = (result >> 7) & 1;
    	
    	regA = result & 0x00ff;
    }
    else {
	regP.C = (regA >> 15) & 1;
	regP.Z = result == 0;
	regP.N = (result >> 15) & 1;

	regA = result;
    }
}

void Cpu::ROR_mem_execute(uint32_t address, uint16_t operand) {
    // Rotate right (memory contents)

    uint16_t result = ((operand >> 1) & 0x7fff) | (regP.C << 15);
    mem.write_word(address, result);

    regP.C = operand & 1;
    regP.Z = result == 0;
    regP.N = (result >> 15) & 1;
}

void Cpu::ROR_A_execute() {
    // Rotate right (accumulator)
    
    if (regP.M) {
	uint8_t result = ((regA >> 1) & 0x007f) | (regP.C << 7);

	regP.C = regA & 1;
	regP.Z = result == 0;
	regP.N = (result >> 7) & 1;

	regA = result;
    }
    else {
	uint16_t result = ((regA >> 1) & 0x7fff) | (regP.C << 15);

	regP.C = regA & 1;
    	regP.Z = result == 0;
    	regP.N = (result >> 15) & 1;

    	regA = result;
    }
}

void Cpu::RTI_execute() {
    // Return from interrupt

    uint8_t procStat = pullStack();
    regP.N = (procStat >> 7) & 1;
    regP.V = (procStat >> 6) & 1;
    regP.M = (procStat >> 5) & 1;
    regP.X = (procStat >> 4) & 1;
    regP.D = (procStat >> 3) & 1;
    regP.I = (procStat >> 2) & 1;
    regP.Z = (procStat >> 1) & 1;
    regP.C = procStat & 1;

    uint8_t PC_low = pullStack();
    uint8_t PC_high = pullStack();
    regPC = (PC_high << 8) | PC_low;
    
    if (regP.E) {
	regP.X = 1;
	regP.M = 1;
    }
    else {
	regDB = pullStack();
    }

    if (regP.X) {
	regX &= 0x00ff;
	regY &= 0x00ff;
    }
}

void Cpu::RTS_execute() {
    // Return from subroutine

    uint8_t PC_low = pullStack();
    uint8_t PC_high = pullStack();
    uint16_t PC = (PC_high << 8) | PC_low;
    regPC = PC + 1; //It had the last byte of the last instr
}

void Cpu::RTL_execute() {
    // Return from subroutine long

    uint8_t PC_low = pullStack();
    uint8_t PC_high = pullStack();
    uint8_t regPB = pullStack();
    uint16_t PC = (PC_high << 8) | PC_low;
    regPC = PC + 1; //It had the last byte of the last instr
}

void Cpu::SBC_execute(uint16_t operand) {
    // Subtract with carry
    // A,Z,C,N = A-M-(1-C)

    uint16_t noperand = ~operand;
    ADC_execute(noperand);
}

void Cpu::SEC_execute() {
    // Set carry flag

    regP.C = 1;
}

void Cpu::SED_execute() {
    // Set decimal flag
    
    regP.D = 1;
}

void Cpu::SEI_execute() {
    // Set interrupt disable flag

    regP.I = 1;
}

void Cpu::SEP_execute(uint16_t operand) {
    //Set status bits
    // P = P | M

    regP.N |= ((operand >> 7) & 1);
    regP.V |= ((operand >> 6) & 1);
    regP.M |= ((operand >> 5) & 1);
    regP.X |= ((operand >> 4) & 1);
    regP.D |= ((operand >> 3) & 1);
    regP.I |= ((operand >> 2) & 1);
    regP.Z |= ((operand >> 1) & 1);
    regP.C |= (operand & 1);

    if (regP.X) {
	regP.X &= 0x00ff;
	regP.Y &= 0x00ff;
    }
}
    

void Cpu::STA_execute(uint32_t address) {
    // Store accumulator
    // M=A

    if (regP.M)
	mem.write_byte(address, (uint8_t) regA);
    else 
	mem.write_word(address, regA);
}

void Cpu::STP_execute() {
    // Stop the processor

    while(1);
}

void Cpu::STX_execute(uint32_t address) {
    // Store register X
    // M=X

    if (regP.X)
	mem.write_byte(address, (uint8_t) regX);
    else
	mem.write_word(address, regX);
}

void Cpu::STY_execute(uint32_t address) {
    // Store register Y
    // M=Y

    if (regP.X)
	mem.write_byte(address, (uint8_t) regY);
    else
	mem.write_word(address, regY);
}

void Cpu::STZ_execute(uint32_t address) {
    // Store zero to memory
    // M=0

    if (regP.M)
	mem.write_byte(address, 0);
    else
	mem.write_word(address, 0);
}

void Cpu::TAX_execute() {
    // Transfer accumulator to X
    // X=A

    if (regP.X) {
	regX = (regX & 0xff00) | (regA & 0x00ff);
	regP.Z = (uint8_t) regX == 0;
	regP.N = (regX >> 7) & 1;
    }
    else {
	regX = regA;
	regP.Z = regX == 0;
	regP.N = (regX >> 15) & 1;
    }
}

void Cpu::TAY_execute() {
    // Transfer accumulator to Y
    // Y=A

    if (regP.X) {
	regY = (regY & 0xff00) | (regA & 0x00ff);
	regP.Z = (uint8_t) regY == 0;
	regP.N = (regY >> 7) & 1;
    }
    else {
	regY = regA;
	regP.Z = regY == 0;
	regP.N = (regY >> 15) & 1;
    }
}

void Cpu::TCD_execute() {
    // Transfer accumulator to DP
    // DP = A (always 16 bits)

    regDP = regA;

    regP.Z = regDP == 0;
    regP.N = (regDP >> 15) & 1;
}

void Cpu::TCS_execute() {
    // Transfer accumulator to SP
    // SP = A (always 16 bits)

    regSP = regA;
}

void Cpu::TDC_execute() {
    // Transfer DP to accumulator
    // A = DP (always 16 bits)

    regA = regDP;

    regP.Z = regA == 0;
    regP.N = (regA >> 15) & 1;
}

void Cpu::TSC_execute() {
    // Transfer SP to accumulator
    // A = SP (always 16 bits)

    regA = regSP;

    regP.Z = regA == 0;
    regP.N = (regA >> 15) & 1;
}

void Cpu::TSX_execute() {
    // Transfer stack pointer to X
    // X=SP

    if (regP.X) {
	regX = regSP & 0x00ff;
	
	regP.Z = (uint8_t) regX == 0;
	regP.N = (regX >> 7) & 1;
    }
    else {
	regX = regSP;

	regP.Z = regX == 0;
	regP.N = (regX >> 15) & 1;
    }
}

void Cpu::TXA_execute() {
    // Transfer X to accumulator
    // A=X

    if (regP.M) {
	regA = (regA & 0xff00) | (regX & 0x00ff);

	regP.Z = (uint8_t) regA == 0;
	regP.N = (regA >> 7) & 1;
    }
    else {
	regA = regX;
	if (!regP.X)
	    regA &= 0x00ff;
	
	regP.Z = regA == 0;
	regP.N = (regA >> 15) & 1;
    }
}

void Cpu::TXS_execute() {
    // Transfer X to stack pointer
    // S=X

    regSP = regX;

    if (regP.E) {
	regSP &= 0x00ff;
	regSP |= 0x0100;
    }
}

void Cpu::TXY_execute() {
    // Transfer X to Y
    // Y=X

    regY = regX;

    if (regP.X) {
	regP.Z = (uint8_t) regY == 0;
	regP.N = (regY >> 7) & 1;
    }
    else {
	regP.Z = regY == 0;
	regP.N = (regY >> 15) & 1;
    }
}

void Cpu::TYA_execute() {
    // Transfer Y to accumulator
    // A = Y

    if (regP.M) {
	regA = (regA & 0xff00) | (regY & 0x00ff);

	regP.Z = (uint8_t) regA == 0;
	regP.N = (regA >> 7) & 1;
    }
    else {
	regA = regY;
	if (!regP.X)
	    regA &= 0x00ff;
	
	regP.Z = regA == 0;
	regP.N = (regA >> 15) & 1;
    }
}

void Cpu::TYX_execute() {
    // Transfer Y to X
    // X = Y

    regX = regY;

    if (regP.X) {
	regP.Z = (uint8_t) regX == 0;
	regP.N = (regX >> 7) & 1;
    }
    else {
	regP.Z = regX == 0;
	regP.N = (regX >> 15) & 1;
    }
}

void Cpu::TRB_execute(uint32_t address, uint16_t operand) {
    // Test and reset memory bits against accumulator
    
    uint16_t nregA = ~regA;
    uint16_t result = operand & nregA;

    if (regP.M) {
	mem.write_byte(address, (uint8_t) result);

	regP.Z = ((uint8_t) regA & (uint8_t) operand) == 0;
    }
    else {
	mem.write_word(address, result);
	
	regP.Z = regA & operand;
    }
}

void Cpu::TSB_execute(uint32_t address, uint16_t operand) {
    // Test and set memory bits against accumulator
    
    uint16_t result = operand | regA;

    if (regP.M) {
	mem.write_byte(address, (uint8_t) result);

	regP.Z = ((uint8_t) regA & (uint8_t) operand) == 0;
    }
    else {
	mem.write_word(address, result);
	
	regP.Z = regA & operand;
    }
}

void Cpu::WAI_execute() {
    // Wait for interrupt
    // TBI

}

void Cpu::WDM_execute() {
    // Reserved, does nothing
}

void Cpu::XBA_execute() {
    // Exchange the B and A accumulators

    uint8_t tmp = (regA >> 8) & 0x00ff;
    regA = (regA << 8) & 0xff00;
    regA |= (tmp & 0x00ff);

    regP.Z = (uint8_t) regA == 0;
    regP.N = (regA >> 7) & 1;
}

void Cpu::XCE_execute() {
    // Exchange carry and emulation bits

    bool tmpC = regP.C;
    regP.C = regP.E;
    regP.E = tmpC;

    if (!regP.E) {
	regP.M = 1;
	regP.X = 1;
    }

    if (regP.X) {
	regX &= 0x00ff;
	regY &= 0x00ff;
    }
}

void Cpu::NMI_execute() {
    // Non-Maskable interrupt

    uint8_t lowPC, highPC;
    lowPC = regPC;
    highPC = (regPC >> 8);
    
    uint8_t procStat;
    procStat =  (regP.N) << 6 |
		(regP.V) << 5 |
		(regP.B) << 4 |
		(regP.D) << 3 |
		(regP.I) << 2 |
		(regP.Z) << 1 |
		(regP.C);
    
    //We push the PC and Processor Status into the stack
    pushStack(highPC);
    pushStack(lowPC);
    pushStack(procStat);

    //We set the PC to the interrupt vector
    regPC = mem.read_word(NMI_INT_VECTOR_ADDR);
}

void Cpu::pushStack(uint8_t data) {
    mem.write_byte(regSP, data);
    regSP--;
}

uint8_t Cpu::pullStack() {
    regSP++;
    uint8_t result = mem.read_byte(regSP);
    return result;
}

void Cpu::debug_dump(uint8_t inst) {
    cout << "INSTRUCTION" << std::hex << (int) inst << endl;
    cout << "regA =  " << std::hex << (unsigned) regA << endl;
    cout << "regX =  " << std::hex << (unsigned) regX << endl;
    cout << "regY =  " << std::hex << (unsigned) regY << endl;
    cout << "regPC = " << std::hex << (unsigned) regPC << endl;
    cout << "regSP = " << std::hex << (unsigned) regSP << endl;
    cout << "regP =" << endl;
    cout << "    C = " << regP.C;
    cout << "    Z = " << regP.Z;
    cout << "    I = " << regP.I;
    cout << "    D = " << regP.D;
    cout << "    B = " << regP.B;
    cout << "    V = " << regP.V;
    cout << "    N = " << regP.N << endl;
    cout << "cycles = " << clock->cycles << endl;
}
