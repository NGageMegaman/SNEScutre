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
}

uint8_t Mem::read_byte(uint32_t address) {
    return ram[address];
}

uint16_t Mem::read_word(uint32_t address) {
    uint8_t byte0, byte1;
    byte0 = ram[address];
    byte1 = ram[address+1];
    return (byte1 << 8) | byte0;
}

uint32_t Mem::read_long(uint32_t address) {
    uint8_t byte0, byte1, byte2;
    byte0 = ram[address];
    byte1 = ram[address+1];
    byte2 = ram[address+2];
    return (byte2 << 16) | (byte1 << 8) | byte0;
}

void Mem::write_byte(uint32_t address, uint8_t data) {
	ram[address] = data;
}

void Mem::write_word(uint32_t address, uint16_t data) {
    ram[address] = data & 0x00ff;
    ram[address+1] = (data >> 8) & 0x00ff;
}

void Mem::write_long(uint32_t address, uint32_t data) {
    ram[address] = data & 0x0000ff;
    ram[address+1] = (data >> 8) & 0x0000ff;
    ram[address+2] = (data >> 16) & 0x0000ff;
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

