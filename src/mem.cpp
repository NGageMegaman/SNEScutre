#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "mem.h"
using namespace std;

Mem::Mem() {
    ram = (unsigned char *)malloc(RAM_SIZE);
    //Mem init with 0s
    for (unsigned int i = 0; i<=RAM_SIZE; ++i) {
	    ram[i] = 0xff;
    }
    
    load_rom("smw.sfc");

    //SPC700 IO PORT 0 ENABLE
    ram[0x2140] = 0xaa;
    ram[0x2141] = 0xbb;
    spc_transfer = false;

    transfer_mode = (uint8_t *) malloc(sizeof(uint8_t) * 8);
    dma_fixed_transfer = (bool *) malloc(sizeof(bool) * 8);
    dma_addr_increment = (bool *) malloc(sizeof(bool) * 8);
    hdma_addr_mode = (bool *) malloc(sizeof(bool) * 8);
    dma_direction = (bool *) malloc(sizeof(bool) * 8);
    bus_b_address = (uint8_t *) malloc(sizeof(uint8_t) * 8);
    bus_a_address_l = (uint8_t *) malloc(sizeof(uint8_t) * 8);
    bus_a_address_h = (uint8_t *) malloc(sizeof(uint8_t) * 8);
    bus_a_address_b = (uint8_t *) malloc(sizeof(uint8_t) * 8);
    dma_size_l = (uint8_t *) malloc(sizeof(uint8_t) * 8);
    dma_size_h = (uint8_t *) malloc(sizeof(uint8_t) * 8);

    hdma_en = (bool *) malloc(sizeof(bool) * 8);
    hdma_addr_mode = (bool *) malloc(sizeof(bool) * 8);
    hdma_dir = (bool *) malloc(sizeof(bool) * 8);
    hdma_finished = (bool *) malloc(sizeof(bool) * 8);
    hdma_repeat = (bool *) malloc(sizeof(bool) * 8);
    hdma_mode = (uint8_t *) malloc(sizeof(uint8_t) * 8);
    hdma_lcount = (uint8_t *) malloc(sizeof(uint8_t) * 8);
    hdma_ind_b = (uint8_t *) malloc(sizeof(uint8_t) * 8);
    hdma_table = (uint32_t *) malloc(sizeof(uint32_t) * 8);
    hdma_B = (uint32_t *) malloc(sizeof(uint32_t) * 8);
    last_i = (uint32_t *) malloc(sizeof(uint32_t) * 8);
    ind_incr = (uint32_t *) malloc(sizeof(uint32_t) * 8);

    for (int i = 0; i<8; ++i) {
        transfer_mode[i] = 0;
        dma_fixed_transfer[i] = 0;
        dma_addr_increment[i] = 0;
        hdma_addr_mode[i] = 0;
        dma_direction[i] = 0;
        bus_b_address[i] = 0;
        bus_a_address_l[i] = 0;
        bus_a_address_h[i] = 0;
        bus_a_address_b[i] = 0;
        dma_size_l[i] = 0;
        dma_size_h[i] = 0;

        hdma_en[i] = 0;
        hdma_addr_mode[i] = 0;
        hdma_dir[i] = 0;
        hdma_finished[i] = 0;
        hdma_repeat[i] = 0;
        hdma_mode[i] = 0;
        hdma_lcount[i] = 0;
        hdma_ind_b[i] = 0;
        hdma_table[i] = 0;
        hdma_B[i] = 0;
        last_i[i] = 0;
        ind_incr[i] = 0;
    }

    controller_latch = 0xffff;
    ram[0x4017] = 0;

    debug = false;
}

uint8_t Mem::read_byte(uint32_t address) {
    address = mirror(address);
    uint8_t byte0;
    if (address == 0x4016) {
        byte0 = controller_latch & 1;
        controller_latch = controller_latch >> 1;
        controller_latch |= 0x8000;
    }
    else if (!access_memory_mapped(address, 0, &byte0, false))
        byte0 = ram[address];
    return byte0;
}

uint16_t Mem::read_word(uint32_t address) {
    address = mirror(address);
    uint8_t byte0, byte1;
    if (address == 0x4016) {
        byte0 = controller_latch & 1;
        controller_latch = controller_latch >> 1;
        controller_latch |= 0x8000;
    }
    else if (!access_memory_mapped(address, 0, &byte0, false))
        byte0 = ram[address];

    if (address+1 == 0x4016) {
        byte1 = controller_latch & 1;
        controller_latch = controller_latch >> 1;
        controller_latch |= 0x8000;
    }
    else if (!access_memory_mapped(address+1, 0, &byte1, false))
        byte1 = ram[address+1];
    return ((byte1 << 8) | byte0);
}

uint32_t Mem::read_long(uint32_t address) {
    address = mirror(address);
    uint8_t byte0, byte1, byte2;
    if (!access_memory_mapped(address, 0, &byte0, false))
        byte0 = ram[address];
    if (!access_memory_mapped(address+1, 0, &byte1, false))
        byte1 = ram[address+1];
    if (!access_memory_mapped(address+2, 0, &byte2, false))
        byte2 = ram[address+2];
    return (byte2 << 16) | (byte1 << 8) | byte0;
}

void Mem::write_byte(uint32_t address, uint8_t data) {
    address = mirror(address);
    //if (address == 0x7e0100) {
    //    cout << "GAMEMODE " << std::hex << (unsigned) data << endl;
    //}
    //cout << "write " << address << " " << (unsigned) data << endl;
    if (address == 0x4016 || address == 0x4200) {
	    controller_latch = 0;
	    XEvent event;
	    while (XPending(di)) {
	        XNextEvent(di, &event);
	        int a;
	        if (event.type == KeyRelease) {
	    	    a = event.xkey.keycode;
                cout << std::hex << (unsigned) a << endl;
	    	    if (a == 52)       //z_key, B
	        	    controller_inputs &= 0x7fff;
	        	else if (a == 53)  //x_key, A
	        	    controller_inputs &= 0xff7f;
                else if (a == 38)  //a_key, Y
                    controller_inputs &= 0xbfff;
                else if (a == 39)  //s_key, X
                    controller_inputs &= 0xffbf;
	        	else if (a == 22)  //backspace_key, SELECT
	        	    controller_inputs &= 0xdfff;
	        	else if (a == 36)  //enter_key, START
	        	    controller_inputs &= 0xefff;
	        	else if (a == 98)  //up_key, UP
	        	    controller_inputs &= 0xf7ff;
	        	else if (a == 104)  //down_key, DOWN
	        	    controller_inputs &= 0xfbff;
	        	else if (a == 100)  //left_key, LEFT
	        	    controller_inputs &= 0xfdff;
	        	else if (a == 102)  //right_key, RIGHT
	        	    controller_inputs &= 0xfeff;
	        }
	        if (event.type == KeyPress) {
	    	    a = event.xkey.keycode;
	    	    if (a == 52)       //z_key, B
	        	    controller_inputs |= 0x8000;
	        	else if (a == 53)  //x_key, A
	        	    controller_inputs |= 0x0080;
                else if (a == 38)  //a_key, Y
                    controller_inputs &= 0x4000;
                else if (a == 39)  //s_key, X
                    controller_inputs &= 0x0040;
	        	else if (a == 22)  //backspace_key, SELECT
	        	    controller_inputs |= 0x2000;
	        	else if (a == 36)  //enter_key, START
	        	    controller_inputs |= 0x1000;
	        	else if (a == 98)  //up_key, UP
	        	    controller_inputs |= 0x0800;
	        	else if (a == 104)  //down_key, DOWN
	        	    controller_inputs |= 0x0400;
	        	else if (a == 100)  //left_key, LEFT
	        	    controller_inputs |= 0x0200;
	        	else if (a == 102)  //right_key, RIGHT
	        	    controller_inputs |= 0x0100;
	        }
	    }
	    controller_latch = controller_inputs;
        controller_status = controller_inputs;
        ram[0x4212] = 0;
    }
    else if (!access_memory_mapped(address, data, NULL, true)) { 
        if (address == 0x2140) {
            if (!spc_transfer) {
                if (data == 0xcc) {
                    spc_transfer = true;
                }
                else {
                    return;
                }
            }
        }
        else if (address == 0x2141) {
            if (!spc_transfer) {
                return;
            }
            else {
                if (data == 0) {
                    spc_transfer = false;
                    ram[0x2140] = 0xaa;
                    ram[0x2141] = 0xbb;
                    return;
                }
            }
        }
	ram[address] = data;
    }
}

void Mem::write_word(uint32_t address, uint16_t data) {
    address = mirror(address);
    if (address == 0x4016 || address == 0x4200) {
	    controller_latch = 0;
	    XEvent event;
	    while (XPending(di)) {
	        XNextEvent(di, &event);
	        int a;
	        if (event.type == KeyPress) {
	    	    a = event.xkey.keycode;
	    	    if (a == 52)       //z_key, B
	        	    controller_inputs &= 0x7fff;
	        	else if (a == 53)  //x_key, A
	        	    controller_inputs &= 0xff7f;
	        	else if (a == 22)  //backspace_key, SELECT
	        	    controller_inputs &= 0xdfff;
	        	else if (a == 36)  //enter_key, START
	        	    controller_inputs &= 0xefff;
	        	else if (a == 98)  //up_key, UP
	        	    controller_inputs &= 0xf7ff;
	        	else if (a == 104)  //down_key, DOWN
	        	    controller_inputs &= 0xfbff;
	        	else if (a == 100)  //left_key, LEFT
	        	    controller_inputs &= 0xfdff;
	        	else if (a == 102)  //right_key, RIGHT
	        	    controller_inputs &= 0xfeff;
	        }
	        if (event.type == KeyRelease) {
	    	    a = event.xkey.keycode;
	    	    if (a == 52)       //z_key, B
	        	    controller_inputs |= 0x8000;
	        	else if (a == 53)  //x_key, A
	        	    controller_inputs |= 0x0080;
	        	else if (a == 22)  //backspace_key, SELECT
	        	    controller_inputs |= 0x2000;
	        	else if (a == 36)  //enter_key, START
	        	    controller_inputs |= 0x1000;
	        	else if (a == 98)  //up_key, UP
	        	    controller_inputs |= 0x0800;
	        	else if (a == 104)  //down_key, DOWN
	        	    controller_inputs |= 0x0400;
	        	else if (a == 100)  //left_key, LEFT
	        	    controller_inputs |= 0x0200;
	        	else if (a == 102)  //right_key
	        	    controller_inputs |= 0x0100;
	        }
	    }
	    controller_latch = controller_inputs;
        controller_status = controller_inputs;
        ram[0x4212] = 0;
    }
    //cout << "write " << address << " " << (unsigned) data << endl;
    else if (!access_memory_mapped(address, (data & 0x00ff), NULL, true)) 
        ram[address] = data & 0x00ff;
    if (!access_memory_mapped(address+1, ((data >> 8) & 0x00ff), NULL, true)) 
        ram[address+1] = (data >> 8) & 0x00ff;
}

void Mem::write_long(uint32_t address, uint32_t data) {
    address = mirror(address);
    //cout << "write " << address << " " << (unsigned) data << endl;
    if (!access_memory_mapped(address, (data & 0x0000ff), NULL, true)) 
        ram[address] = data & 0x0000ff;
    if (!access_memory_mapped(address+1,((data>>8) & 0x0000ff), NULL, true)) 
        ram[address+1] = (data >> 8) & 0x0000ff;
    if (!access_memory_mapped(address+2,((data>>16) & 0x0000ff), NULL, true)) 
        ram[address+2] = (data >> 16) & 0x0000ff;
}

uint32_t Mem::mirror(uint32_t address) {
    uint8_t bank = (address >> 16) & 0x0000ff;
    uint16_t offset = address & 0x00ffff;

    //Upper banks (minus last 2) are mirrored into lower banks.
    //The two leftover banks correspond to $7e and $7f
    if (bank >= 0x80 && bank <= 0xdf) {
        bank -= 0x80;
    }

    if (bank <= 0x3f) {
        if (offset <= 0x1fff) {
            bank = 0x7e;
        }
        else if (offset <= 0x7fff) {
            bank = 0x00;
        }
    }

            
    uint32_t return_address = (bank << 16) | offset;
    return return_address;
}

bool Mem::access_memory_mapped(uint32_t address, uint8_t data, uint8_t *result, bool wr) {
    //We check for memory mapped addresses
    if (wr) {
        switch (address) {
            case(0x2100):
                ppu->write_INIDISP(data);
                break;
            case(0x2101):
                ppu->write_OBSEL(data);
                break;
            case(0x2102):
                ppu->write_OAMADDL(data);
                break;
            case(0x2103):
                ppu->write_OAMADDH(data);
                break;
            case(0x2104):
                ppu->write_OAMDATA(data);
                break;
            case(0x2105):
                ppu->write_BGMODE(data);
                break;
            case(0x2106):
                ppu->write_MOSAIC(data);
                break;
            case(0x2107):
                ppu->write_BG1SC(data);
                break;
            case(0x2108):
                ppu->write_BG2SC(data);
                break;
            case(0x2109):
                ppu->write_BG3SC(data);
                break;
            case(0x210a):
                ppu->write_BG4SC(data);
                break;
            case(0x210b):
                ppu->write_BG12NBA(data);
                break;
            case(0x210c):
                ppu->write_BG34NBA(data);
                break;
            case(0x210d):
                ppu->write_BG1HOFS(data);
                break;
            case(0x210e):
                ppu->write_BG1VOFS(data);
                break;
            case(0x210f):
                ppu->write_BG2HOFS(data);
                break;
            case(0x2110):
                ppu->write_BG2VOFS(data);
                break;
            case(0x2111):
                ppu->write_BG3HOFS(data);
                break;
            case(0x2112):
                ppu->write_BG3VOFS(data);
                break;
            case(0x2113):
                ppu->write_BG4HOFS(data);
                break;
            case(0x2114):
                ppu->write_BG4VOFS(data);
                break;
            case(0x2115):
                ppu->write_VMAIN(data);
                break;
            case(0x2116):
                ppu->write_VMADDL(data);
                break;
            case(0x2117):
                ppu->write_VMADDH(data);
                break;
            case(0x2118):
                ppu->write_VMDATAL(data);
                break;
            case(0x2119):
                ppu->write_VMDATAH(data);
                break;
            //M7SEL
            //M7A
            //M7B
            //M7C
            //M7D
            //M7X
            //M7Y
            case (0x2121):
                ppu->write_CGADD(data);
                break;
            case (0x2122):
                ppu->write_CGDATA(data);
                break;
            case (0x2123):
                ppu->write_W12SEL(data);
                break;
            case (0x2124):
                ppu->write_W34SEL(data);
                break;
            case (0x2125):
                ppu->write_WOBJSEL(data);
                break;
            case (0x2126):
                ppu->write_WH0(data);
                break;
            case (0x2127):
                ppu->write_WH1(data);
                break;
            case (0x2128):
                ppu->write_WH2(data);
                break;
            case (0x2129):
                ppu->write_WH3(data);
                break;
            case (0x212a):
                ppu->write_WBGLOG(data);
                break;
            case (0x212b):
                ppu->write_WOBJLOG(data);
                break;
            case (0x212c):
                ppu->write_TM(data);
                break;
            case (0x212d):
                ppu->write_TS(data);
                break;
            case (0x212e):
                ppu->write_TMW(data);
                break;
            case (0x212f):
                ppu->write_TSW(data);
                break;
            case (0x2130):
                ppu->write_CGWSEL(data);
                break;
            case (0x2131):
                ppu->write_CGADSUB(data);
                break;
            case (0x2132):
                ppu->write_COLDATA(data);
                break;
	        case (0x2180):
		    write_WMDATA(data);
		    break;
	        case (0x2181):
		    write_WMADDL(data);
		    break;
	        case (0x2182):
		    write_WMADDM(data);
		    break;
	        case (0x2183):
		    write_WMADDH(data);
		    break;
            case (0x4202):
                write_WRMPYA(data);
                break;
            case (0x4203):
                write_WRMPYB(data);
                break;
            case (0x4204):
                write_WRDIVL(data);
                break;
            case (0x4205):
                write_WRDIVH(data);
                break;
            case (0x4206):
                write_WRDIVB(data);
                break;
	        /////////////////
	        //DMA REGISTERS//
	        /////////////////
	        case (0x420b):
		    write_MDMAEN(data);
		    break;
	        case (0x420c):
		    write_HDMAEN(data);
		    break;
	        case (0x4300):case (0x4310):case (0x4320):case (0x4330):
	        case (0x4340):case (0x4350):case (0x4360):case (0x4370):
		    write_DMAPx(data, ((address >> 4) & 7));
		    break;
	        case (0x4301):case (0x4311):case (0x4321):case (0x4331):
	        case (0x4341):case (0x4351):case (0x4361):case (0x4371):
		    write_BBADx(data, ((address >> 4) & 7));
		    break;
	        case (0x4302):case (0x4312):case (0x4322):case (0x4332):
	        case (0x4342):case (0x4352):case (0x4362):case (0x4372):
		    write_A1TxL(data, ((address >> 4) & 7));
		    break;
	        case (0x4303):case (0x4313):case (0x4323):case (0x4333):
	        case (0x4343):case (0x4353):case (0x4363):case (0x4373):
		    write_A1TxH(data, ((address >> 4) & 7));
		    break;
	        case (0x4304):case (0x4314):case (0x4324):case (0x4334):
	        case (0x4344):case (0x4354):case (0x4364):case (0x4374):
		    write_A1Bx(data, ((address >> 4) & 7));
		    break;
	        case (0x4305):case (0x4315):case (0x4325):case (0x4335):
	        case (0x4345):case (0x4355):case (0x4365):case (0x4375):
		    write_DASxL(data, ((address >> 4) & 7));
		    break;
	        case (0x4306):case (0x4316):case (0x4326):case (0x4336):
	        case (0x4346):case (0x4356):case (0x4366):case (0x4376):
		    write_DASxH(data, ((address >> 4) & 7));
		    break;
	        case (0x4307):case (0x4317):case (0x4327):case (0x4337):
	        case (0x4347):case (0x4357):case (0x4367):case (0x4377):
		    write_DASBx(data, ((address >> 4) & 7));
		    break;
	        case (0x4308):case (0x4318):case (0x4328):case (0x4338):
	        case (0x4348):case (0x4358):case (0x4368):case (0x4378):
		    write_A2AxL(data, ((address >> 4) & 7));
		    break;
	        case (0x4309):case (0x4319):case (0x4329):case (0x4339):
	        case (0x4349):case (0x4359):case (0x4369):case (0x4379):
		    write_A2AxH(data, ((address >> 4) & 7));
		    break;
	        case (0x430a):case (0x431a):case (0x432a):case (0x433a):
	        case (0x434a):case (0x435a):case (0x436a):case (0x437a):
		    write_NLTRx(data, ((address >> 4) & 7));
		    break;
            default:
                return false;
        }
    }
    else {
        if (address == 0x2138)
            *result = ppu->read_OAMDATAREAD();
        else if (address == 0x2139)
            *result = ppu->read_VMDATALREAD();
        else if (address == 0x213a)
            *result = ppu->read_VMDATAHREAD();
        else if (address == 0x213b)
            *result = ppu->read_CGDATAREAD();
    	else if (address == 0x2180)
    	    *result = read_WMDATA();
        else if (address == 0x4212)
            *result = ppu->read_HVBJOY();
    	else if (address == 0x4214)
    	    *result = read_RDDIVL();
    	else if (address == 0x4215)
    	    *result = read_RDDIVH();
    	else if (address == 0x4216)
    	    *result = read_RDMPYL();
    	else if (address == 0x4217)
    	    *result = read_RDMPYH();
    	else if (address == 0x4218)
    	    *result = read_JOY1L();
    	else if (address == 0x4219)
    	    *result = read_JOY1H();
        else
            return false;
    }
    //If a function was executed, we get here
    return true;
}

/////////////////////////////
// WRITE TO WRAM REGISTERS //
/////////////////////////////

void Mem::write_WMADDL(uint8_t data) {
    //llllllll
    wmaddl = data;
}

void Mem::write_WMADDM(uint8_t data) {
    //mmmmmmmm
    wmaddm = data;
}

void Mem::write_WMADDH(uint8_t data) {
    //-------h
    wmaddh = data & 1;
}

void Mem::write_WMDATA(uint8_t data) {
    uint32_t address = 0x7e0000 |
		       (wmaddh << 16) |
		       (wmaddm << 8) |
		       wmaddl;
    ram[address] = data;
    address++;
    wmaddl = address & 0x0000ff;
    wmaddm = (address >> 8) & 0x0000ff;
    wmaddh = (address >> 16) & 0x0000ff;
}

uint8_t Mem::read_WMDATA() {
    uint8_t result;
    uint32_t address = 0x7e0000 |
		       (wmaddh << 16) |
		       (wmaddm << 8) |
		       wmaddl;
    result = ram[address];
    address++;
    wmaddl = address & 0x0000ff;
    wmaddm = (address >> 8) & 0x0000ff;
    wmaddh = (address >> 16) & 0x0000ff;
    return result;
}

////////////////////////////
// WRITE TO DMA REGISTERS //
////////////////////////////

void Mem::write_MDMAEN(uint8_t data) {
    //76543210
    //We check all bits for DMA transfers
    for (int i = 0; i<8; ++i) {
	    if ((data & 1) == 1) {
	        DMA_enable(i);
	    }
	    data = data >> 1;
    }
}

void Mem::write_HDMAEN(uint8_t data) {
    //7654321
    //We check all bits for HDMA transfers
    for (int i = 0; i<8; ++i) {
        if ((data & 1) == 1) {
            HDMA_enable(i);
        }
        data = data >> 1;
    }
}

void Mem::write_DMAPx(uint8_t data, uint8_t channel) {
    //da-ifttt
    transfer_mode[channel] = data & 0x07;
    dma_fixed_transfer[channel] = (data >> 3) & 1;
    dma_addr_increment[channel] = (data >> 4) & 1;
    hdma_addr_mode[channel] = (data >> 6) & 1;
    dma_direction[channel] = (data >> 7) & 1;
}

void Mem::write_BBADx(uint8_t data, uint8_t channel) {
    //pppppppp
    bus_b_address[channel] = data;
}

void Mem::write_A1TxL(uint8_t data, uint8_t channel) {
    //llllllll
    bus_a_address_l[channel] = data;
}

void Mem::write_A1TxH(uint8_t data, uint8_t channel) {
    //hhhhhhhh
    bus_a_address_h[channel] = data;
}

void Mem::write_A1Bx(uint8_t data, uint8_t channel) {
    //bbbbbbbb
    bus_a_address_b[channel] = data;
}

void Mem::write_DASxL(uint8_t data, uint8_t channel) {
    //llllllll
    dma_size_l[channel] = data;
    //hdma_ind_l[channel] = data;
}

void Mem::write_DASxH(uint8_t data, uint8_t channel) {
    //hhhhhhhh
    dma_size_h[channel] = data;
    //hdma_ind_h[channel] = data;
}

void Mem::write_DASBx(uint8_t data, uint8_t channel) {
    //bbbbbbbb
    hdma_ind_b[channel] = data;
}

void Mem::write_A2AxL(uint8_t data, uint8_t channel) {
    //aaaaaaaa
    hdma_table[channel] = (hdma_table[channel] & 0xff00) | data;
}

void Mem::write_A2AxH(uint8_t data, uint8_t channel) {
    //aaaaaaaa
    hdma_table[channel] = (hdma_table[channel] & 0x00ff) | (data << 8);
}

void Mem::write_NLTRx(uint8_t data, uint8_t channel) {
    //rccccccc
    hdma_repeat[channel] = (data >> 7) & 1;
    hdma_lcount[channel] = data & 0x7f;
}

uint8_t Mem::read_NLTRx(uint8_t channel) {
    return hdma_lcount[channel] | (hdma_repeat[channel] << 1);
}


//////////////////////
// MULDIV REGISTERS //
//////////////////////

void Mem::write_WRMPYA(uint8_t data) {
    //mmmmmmmm
    mul_a = data;
}

void Mem::write_WRMPYB(uint8_t data) {
    //mmmmmmmm
    mul_b = data;
    uint16_t result = mul_a * mul_b;
    mult_divr_l = result & 0x00ff;
    mult_divr_h = (result >> 8) & 0x00ff;
}

void Mem::write_WRDIVL(uint8_t data) {
    //dddddddd
    dividend_l = data;
}

void Mem::write_WRDIVH(uint8_t data) {
    //dddddddd
    dividend_h = data;
}

void Mem::write_WRDIVB(uint8_t data) {
    //bbbbbbbb
    uint16_t divisor = data;
    if (divisor == 0) {
        quotient_h = 0xff;
        quotient_l = 0xff;
        mult_divr_h = dividend_h;
        mult_divr_l = dividend_l;
    }
    else {
        uint16_t dividend = (dividend_h << 8) | dividend_l;
        uint16_t quotient = dividend/divisor;
        uint16_t remainder = dividend%divisor;
        quotient_h = (quotient >> 8) & 0x00ff;
        quotient_l = quotient & 0x00ff;
        mult_divr_h = (remainder >> 8) & 0x00ff;
        mult_divr_l = remainder & 0x00ff;
    }
}

uint8_t Mem::read_RDDIVL() {
    return quotient_l;
}

uint8_t Mem::read_RDDIVH() {
    return quotient_h;
}

uint8_t Mem::read_RDMPYL() {
    return mult_divr_l;
}

uint8_t Mem::read_RDMPYH() {
    return mult_divr_h;
}

uint8_t Mem::read_JOY1L() {
    return (controller_status & 0x00ff);
}

uint8_t Mem::read_JOY1H() {
    return ((controller_status >> 8) & 0x00ff);
}

/////////
// DMA //
/////////

void Mem::DMA_enable(uint8_t channel) {
    uint32_t size=(dma_size_h[channel] << 8) | dma_size_l[channel];
    if (size == 0) size = 0x10000; 
    uint8_t mode = transfer_mode[channel];
    bool dir = dma_direction[channel];
    uint32_t A, B;
    A = (bus_a_address_b[channel] << 16) | 
        (bus_a_address_h[channel] << 8) |
        (bus_a_address_l[channel]);
    B = 0x2100 | bus_b_address[channel];
    A = mirror(A);

    //cout << "DMA ENABLE " << (unsigned) channel << " " << std::hex << A << " " << B << " " << size << endl;
    if (B == 0x2122) debug = true;

    for (uint32_t i = 0; i < size; ++i) {
	if (mode == 0) {
	    DMA_transfer_byte(A, B, dir);
	    } else if (mode == 1) {
	        DMA_transfer_byte(A, B + (i % 2), dir);
	    } else if (mode == 2) {
	        DMA_transfer_byte(A, B, dir);
	    } else if (mode == 3) {
	        DMA_transfer_byte(A, B + ((i%4)/2), dir);
	    } else if (mode == 4) {
	        DMA_transfer_byte(A, B + (i % 4), dir);
	    } else if (mode == 5) {
	        DMA_transfer_byte(A, B + (i % 2), dir);
	    } else if (mode == 6) {
	        DMA_transfer_byte(A, B, dir);
	    } else if (mode == 7) {
	        DMA_transfer_byte(A, B + ((i%4)/2), dir);
	    }
	    if (!dma_fixed_transfer[channel]) {
	        if (!dma_addr_increment[channel]) A++;
	        else A--;
	    }
        //cout << std::hex << (unsigned) A << " " << (unsigned) ram[A] << endl;
    }
    dma_size_h[channel] = 0;
    dma_size_l[channel] = 0;
    bus_a_address_b[channel] = (A >> 16) & 0x00ff;
    bus_a_address_h[channel] = (A >> 8) & 0x00ff;
    bus_a_address_l[channel] = A & 0x00ff;
}

void Mem::DMA_transfer_byte(uint32_t A, uint32_t B, bool dir) {
    if (!dir) { //A to B
	    access_memory_mapped(B, ram[A], NULL, true);
    }
    else {
	    uint8_t data;
	    access_memory_mapped(B, 0, &data, false);
	    ram[A] = data;
    }
}

void Mem::HDMA_enable(uint8_t channel) {
        
    hdma_en[channel] = true;
    hdma_mode[channel] = transfer_mode[channel];
    hdma_dir[channel] = dma_direction[channel];
    hdma_table[channel] = 0;
    hdma_table[channel] = (bus_a_address_b[channel] << 16) | 
                          (bus_a_address_h[channel] << 8) |
                          (bus_a_address_l[channel]);
    hdma_B[channel] = 0x2100 | bus_b_address[channel];
    hdma_table[channel] = mirror(hdma_table[channel]);
    hdma_finished[channel] = false;
    last_i[channel] = 0;
    write_NLTRx(0, channel);

    //cout << "HDMA ENABLE " << (unsigned) channel << " " << (unsigned) hdma_B[channel] << endl;

}

void Mem::HDMA_hblank(uint8_t channel) {
	uint8_t data;
    bool first = false;
    if (hdma_lcount[channel] == 0) {
        if (hdma_dir[channel]) {
	        access_memory_mapped(hdma_B[channel], 0, &data, false);
        } else {
            data = read_byte(hdma_table[channel]);
        }
        write_NLTRx(data, channel);
        first = true;
        ind_incr[channel] = 0;
    }

    if (hdma_lcount[channel] == 0 && hdma_repeat[channel] == 0) {
        hdma_en[channel] = false;
        hdma_finished[channel] = true;
        return;
    }

    if (first || (hdma_lcount[channel] != 0 && hdma_repeat[channel] != 0)) {

        first = false;
        uint32_t base = hdma_table[channel];
        uint32_t A;
        uint32_t B = hdma_B[channel];
        bool dir = hdma_dir[channel];
        uint32_t i = last_i[channel];
        uint32_t increment;
        if (hdma_addr_mode[channel] == 0) {
            A = base + 1;
            increment = 2;
        } else {
            A = read_word(base + 1);
            A = ((hdma_ind_b[channel] << 16) | A) + ind_incr[channel];
            increment = 0;
        }

        A = mirror(A);

	    if (hdma_mode[channel] == 0) {
	        DMA_transfer_byte(A, B, dir);
            ind_incr[channel] += 1;
	    } else if (hdma_mode[channel] == 1) {
	        DMA_transfer_byte(A, B, dir);
	        DMA_transfer_byte(A + 1, B + 1, dir);
            ind_incr[channel] += 2;
	    } else if (hdma_mode[channel] == 2) {
	        DMA_transfer_byte(A, B, dir);
	        DMA_transfer_byte(A + 1, B, dir);
            ind_incr[channel] += 2;
	    } else if (hdma_mode[channel] == 3) {
	        DMA_transfer_byte(A, B, dir);
	        DMA_transfer_byte(A + 1, B, dir);
	        DMA_transfer_byte(A + 2, B + 1, dir);
	        DMA_transfer_byte(A + 3, B + 1, dir);
            ind_incr[channel] += 4;
	    } else if (hdma_mode[channel] == 4) {
	        DMA_transfer_byte(A, B, dir);
	        DMA_transfer_byte(A + 1, B + 1, dir);
	        DMA_transfer_byte(A + 2, B + 2, dir);
	        DMA_transfer_byte(A + 3, B + 3, dir);
            ind_incr[channel] += 4;
	    } else if (hdma_mode[channel] == 5) {
	        DMA_transfer_byte(A, B, dir);
	        DMA_transfer_byte(A + 1, B + 1, dir);
	        DMA_transfer_byte(A + 2, B, dir);
	        DMA_transfer_byte(A + 3, B + 1, dir);
            ind_incr[channel] += 4;
	    } else if (hdma_mode[channel] == 6) {
	        DMA_transfer_byte(A, B, dir);
	        DMA_transfer_byte(A + 1, B, dir);
            ind_incr[channel] += 2;
	    } else if (hdma_mode[channel] == 7) {
	        DMA_transfer_byte(A, B, dir);
	        DMA_transfer_byte(A + 1, B, dir);
	        DMA_transfer_byte(A + 2, B + 1, dir);
	        DMA_transfer_byte(A + 3, B + 1, dir);
            ind_incr[channel] += 4;
	    }
        hdma_table[channel] += increment;
        if (hdma_repeat[channel]) {
            hdma_lcount[channel]--;
            if (hdma_lcount[channel] == 0) hdma_table[channel] += 3;
        }
    }
    else {
        hdma_lcount[channel]--;
    }
}

void Mem::hblank() {
    for (int i = 0; i<8; ++i) {
        if (hdma_en[i]) {
            if (hdma_finished[i]) HDMA_enable(i);
            else HDMA_hblank(i);
        }
    }
}

void Mem::load_rom(char *name) {

    /*This rom loads .sfc roms file format
    The sfc file format header follows the following sections:

    Address   len  
    ---------------------------------
    $00:7fc0 | 21 | Internal ROM Name
    $00:7fd5 |  1 | Map Mode
    $00:7fd6 |  1 | ROM Type
    $00:7fd7 |  1 | ROM Size
    $00:7fd8 |  1 | SRAM Size
    $00:7fd9 |  1 | Destination Code
    $00:7fda |  1 | Fixed Value ($33)
    $00:7fdb |  1 | Version#
    $00:7fdc |  2 | Complement check
    $00:7fde |  2 | Checksum
    ----------------------------------
    */

    ifstream rom;
    rom.open((char *)name, std::ios_base::binary);

    rom.seekg(0x7fc0, ios::beg);

    char header[0x1e];
    rom.read(header, 0x1e);

    unsigned int rom_size = 1024 * (1 << header[23]);

    for (int i = 0; i<21; ++i) {
        cout << std::hex << header[i];
    }
    cout << endl;
    cout << "Map Mode: ";
    if (header[21] == 0x20) cout << "loROM" << endl;
    else cout << "hiROM" << endl;
    cout << "ROM size: " << std::dec << rom_size << endl;

    rom.seekg(0, ios::beg);
    // For each bank, we map 0x8000 bytes from rom to upper half of bank
    // $00:8000 ~ $00:ffff <- rom[$00:0000] ~ rom[$00:7fff]
    // $01:8000 ~ $01:ffff <- rom[$00:8000] ~ rom[$00:ffff]
    // $02:8000 ~ $02:ffff <- rom[$01:0000] ~ rom[$01:7fff]
    // $03:8000 ~ $03:ffff <- rom[$01:8000] ~ rom[$01:ffff]
    // ...

    for (int i = 0; rom_size != 0; ++i) {
        rom.read((char *) ram + 0x8000 + (0x010000 * i), 0x8000);
        rom_size -= 0x8000;
    }
    rom.close();
}

