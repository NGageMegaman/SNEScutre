#include <stdint.h>
#include <X11/Xlib.h>
#ifndef MEM_PPU_H
#include "mem_ppu.h"
#define MEM_PPU_H
#endif
#ifndef CPU_H
#include "cpu.h"
#define CPU_H
#endif
using namespace std;

class Ppu {
    public:
	Ppu();
	void draw(int n_scanline);
	void drawSprites(int n_scanline);
	void drawScreen();
	void checkSprite0Hit(int n_scanline);
	void vblank();
	void initColors();
	Cpu *cpu;
	Mem_ppu *mem_ppu;
	Display *di;
	int sc;
	Window ro, wi;
	GC gc;
	uint32_t colors[64];

    bool Fblank, Hblank, Vblank;
    uint8_t brightness;
    uint8_t object_size;
    uint8_t name_select;
    uint8_t name_base_select;
    uint8_t OAMADDL, OAMADDH;
    uint16_t oam_address;
    uint8_t oam_low_buffer;
    bool oam_h_addr;
    bool obj_priority_bit;
    bool BG1_char_size, BG2_char_size, BG3_char_size, BG4_char_size;
    uint8_t BG_mode;
    bool mode1_BG3_priority;
    bool BG1_mosaic, BG2_mosaic, BG3_mosaic, BG4_mosaic;
    uint8_t mosaic_pixel_size;
    uint8_t BG1_tilemap_address, BG2_tilemap_address, BG3_tilemap_address, BG4_tilemap_address,
    bool BG1_tilemap_x_mirror, BG2_tilemap_x_mirror, BG3_tilemap_x_mirror, BG4_tilemap_x_mirror;
    bool BG1_tilemap_y_mirror, BG2_tilemap_y_mirror, BG3_tilemap_y_mirror, BG4_tilemap_y_mirror;
    uint8_t BG1_char_address, BG2_char_address, BG3_char_address, BG4_char_address;
    uint16_t BG1_hscroll, BG1_vscroll;
    uint16_t BG2_hscroll, BG2_vscroll;
    uint16_t BG3_hscroll, BG3_vscroll;
    uint16_t BG4_hscroll, BG4_vscroll;
    uint16_t BG5_hscroll, BG5_vscroll;
    uint16_t BG6_hscroll, BG6_vscroll;
    uint16_t BG7_hscroll, BG7_vscroll;
    bool BG1HOFS_h, BG1VOFS_h; 
    bool BG2HOFS_h, BG2VOFS_h;
    bool BG3HOFS_h, BG3VOFS_h; 
    bool BG4HOFS_h, BG4VOFS_h;
    bool BG5HOFS_h, BG5VOFS_h;
    bool BG6HOFS_h, BG6VOFS_h; 
    uint8_t address_increment_mode;
    uint8_t address_increment_amount;
    uint8_t address_remapping;
    uint8_t VMADDL, VMADDH;
    uint16_t vram_address;
    uint16_t vram_read_buffer;
    //MODE 7 UNIMPLEMENTED, ALL M7* REGISTERS UNIMPLEMENTED
    uint8_t CGADD;
    uint8_t cg_address;
    uint8_t cg_low_buffer;
    bool cg_h_addr;
    //WINDOWS UNIMPLEMENTED, W* REGISTERS UNIMPLEMENTED
    uint8_t BG1_main_en, BG2_main_en, BG3_main_en, BG4_main_en, OBJ_main_en;
    uint8_t BG1_subs_en, BG2_subs_en, BG3_subs_en, BG4_subs_en, OBJ_subs_en;
    uint8_t clip_colors_to_black;
    uint8_t prevent_color_math;
    bool add_subscreen;
    bool direct_color_256_BGs;
    bool add_sub_color;
    bool half_color_math;
    bool BG1_color_math_en, BG2_color_math_en, BG3_color_math_en, BG4_color_math_en, OBJ_color_math_en, BD_color_math_en;
    uint8_t color_intensity;
    bool r_intensity, g_intensity, b_intensity;
    uint8_t h_scanline, v_scanline;
    bool time_over, range_over, interlace, external_latch;
};
