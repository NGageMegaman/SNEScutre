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
	    ram[i] = 0x01;
    }
    
    load_rom("smw.sfc");

    //SPC700 IO PORT 0 ENABLE
    ram[0x2140] = 0xaa;
    ram[0x2141] = 0xbb;
    spc_transfer = false;
}

uint8_t Mem::read_byte(uint32_t address) {
    address = mirror(address);
    uint8_t byte0;
    if (!access_memory_mapped(address, 0, &byte0, false))
        byte0 = ram[address];
    return byte0;
}

uint16_t Mem::read_word(uint32_t address) {
    address = mirror(address);
    uint8_t byte0, byte1;
    if (!access_memory_mapped(address, 0, &byte0, false))
        byte0 = ram[address];
    if (!access_memory_mapped(address+1, 0, &byte1, false))
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
    if (!access_memory_mapped(address, data, NULL, true)) { 
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
    if (!access_memory_mapped(address, (data & 0x00ff), NULL, true)) 
        ram[address] = data & 0x00ff;
    if (!access_memory_mapped(address+1, ((data >> 8) & 0x00ff), NULL, true)) 
        ram[address+1] = (data >> 8) & 0x00ff;
}

void Mem::write_long(uint32_t address, uint32_t data) {
    address = mirror(address);
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
    if (bank >= 0x80 && bank <= 0xfd) {
        bank -= 0x80;
    }

    //First two pages of banks $00~3f are mirrored into
    //first two pages of bank $7e
    if (bank <= 0x3f) {
        if (offset <= 0x1fff) {
            bank = 0x7e;
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
            //W12SEL
            //W34SEL
            //WOBJSEL
            //WH0
            //WH1
            //WH2
            //WH3
            //WBGLOG
            //WOBJLOG
            case (0x212c):
                ppu->write_TM(data);
                break;
            case (0x212d):
                ppu->write_TS(data);
                break;
            //TMW
            //TSW
            case (0x2130):
                ppu->write_CGWSEL(data);
                break;
            case (0x2131):
                ppu->write_CGADSUB(data);
                break;
            case (0x2132):
                ppu->write_COLDATA(data);
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
        else
            return false;
    }
    //If a function was executed, we get here
    return true;
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

