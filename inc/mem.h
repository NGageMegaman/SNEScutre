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
	void write_WMADDL(uint8_t data);
	void write_WMADDM(uint8_t data);
	void write_WMADDH(uint8_t data);
	void write_WMDATA(uint8_t data);
	uint8_t read_WMDATA();
	void write_MDMAEN(uint8_t data);
	void write_DMAPx(uint8_t data, uint8_t channel);
	void write_BBADx(uint8_t data, uint8_t channel);
	void write_A1TxL(uint8_t data, uint8_t channel);
	void write_A1TxH(uint8_t data, uint8_t channel);
	void write_A1Bx(uint8_t data, uint8_t channel);
	void write_DASxL(uint8_t data, uint8_t channel);
	void write_DASxH(uint8_t data, uint8_t channel);
	void write_DASBx(uint8_t data, uint8_t channel);
	void DMA_enable(uint8_t channel);
	void DMA_transfer_byte(uint32_t A, uint32_t B, bool dir);
	void load_rom(char *name);

	unsigned char *ram;
        bool spc_transfer;

	uint8_t wmaddl, wmaddm, wmaddh;

	//DMA
	uint8_t *transfer_mode;
	bool *dma_fixed_transfer;
	bool *dma_addr_increment;
	bool *hdma_addr_mode;
	bool *dma_direction;
	uint8_t *bus_b_address;
	uint8_t *bus_a_address_l;
	uint8_t *bus_a_address_h;
	uint8_t *bus_a_address_b;
	uint8_t *dma_size_l;
	uint8_t *dma_size_h;
    
    bool debug;

    Ppu *ppu;
};
