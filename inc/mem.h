#include <stdint.h>
#include <X11/Xlib.h>
#include <string>
#include <vector>
#ifndef DEFINES_H
#include "defines.h"
#define DEFINES_H
#endif
#ifndef PPU_H
#include "ppu.h"
#define PPU_H
#endif
using namespace std;

class Mem {
    public:
	    Mem();
        uint32_t mirror(uint32_t address);
        bool access_memory_mapped(uint32_t address, uint8_t data, uint8_t *result, bool wr);
	    uint8_t read_byte(uint32_t address);
	    uint16_t read_word(uint32_t address);
        uint32_t read_long(uint32_t address);
	    void write_byte(uint32_t address, uint8_t data);
	    void write_word(uint32_t address, uint16_t data);
	    void write_long(uint32_t address, uint32_t data);
	    void load_rom(char *name);
	    unsigned char *ram;
        bool spc_transfer;

        Ppu *ppu;
};
