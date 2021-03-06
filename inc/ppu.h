#include <stdint.h>
#include <X11/Xlib.h>
#ifndef DEFINES_H
#include "defines.h"
#define DEFINES_H
#endif
using namespace std;

class Ppu {
    public:
	Ppu();
    void drawBGs(uint32_t scanline);
	void drawBG(uint8_t BG, uint32_t scanline);
	void drawSprites();
    void renderFrame();
    void determineWindows(uint32_t scanline);
    void colorMath(uint8_t top_layer, uint32_t *color, uint32_t sub_color);
	void drawScreen();
	void vblank();
    uint32_t applyBrightness(uint32_t color);
	uint32_t determinePaletteAddress(uint8_t BG, uint8_t mode);
    void determineObjectSize(uint8_t *x, uint8_t *y, bool s);
	uint32_t convert_BGR_RGB(uint32_t bgr);
	void initBppMatrix();
    void write_INIDISP(uint8_t data);
    void write_OBSEL(uint8_t data);
    void write_OAMADDL(uint8_t data);
    void write_OAMADDH(uint8_t data);
    void write_OAMDATA(uint8_t data);
    void write_BGMODE(uint8_t data);
    void write_BG1SC(uint8_t data);
    void write_BG2SC(uint8_t data);
    void write_BG3SC(uint8_t data);
    void write_BG4SC(uint8_t data);
    void write_BG12NBA(uint8_t data);
    void write_BG34NBA(uint8_t data);
    void write_BG1HOFS(uint8_t data);
    void write_BG1VOFS(uint8_t data);
    void write_BG2HOFS(uint8_t data);
    void write_BG2VOFS(uint8_t data);
    void write_BG3HOFS(uint8_t data);
    void write_BG3VOFS(uint8_t data);
    void write_BG4HOFS(uint8_t data);
    void write_BG4VOFS(uint8_t data);
    void write_BG5HOFS(uint8_t data);
    void write_BG5VOFS(uint8_t data);
    void write_BG6HOFS(uint8_t data);
    void write_BG6VOFS(uint8_t data);
    void write_VMAIN(uint8_t data);
    void write_VMADDL(uint8_t data);
    void write_VMADDH(uint8_t data);
    void write_VMDATAL(uint8_t data);
    void write_VMDATAH(uint8_t data);
    void write_CGADD(uint8_t data);
    void write_CGDATA(uint8_t data);
    void write_CGWSEL(uint8_t data);
    void write_CGADSUB(uint8_t data);
    void write_COLDATA(uint8_t data);
    void write_W12SEL(uint8_t data);
    void write_W34SEL(uint8_t data);
    void write_WOBJSEL(uint8_t data);
    void write_WH0(uint8_t data);
    void write_WH1(uint8_t data);
    void write_WH2(uint8_t data);
    void write_WH3(uint8_t data);
    void write_WBGLOG(uint8_t data);
    void write_WOBJLOG(uint8_t data);
    void write_TM(uint8_t data);
    void write_TS(uint8_t data);
    void write_TMW(uint8_t data);
    void write_TSW(uint8_t data);
    void write_MOSAIC(uint8_t data);

    uint8_t read_OAMDATAREAD();
    uint8_t read_VMDATALREAD();
    uint8_t read_VMDATAHREAD();
    uint16_t read_CGDATAREAD();

    uint8_t read_HVBJOY();

	Display *di;
	int sc;
	Window ro, wi;
	GC gc;

    uint8_t *oam;
    uint16_t *vram;
    uint16_t *cg;

    bool Fblank, Hblank, Vblank;
    uint8_t brightness;
    uint8_t object_size;
    uint8_t name_select;
    uint32_t name_base_select;
    uint8_t OAMADDL, OAMADDH;
    uint16_t oam_address;
    uint8_t oam_low_buffer;
    bool oam_h_addr;
    bool obj_priority_bit;
    bool BG1_char_size, BG2_char_size, BG3_char_size, BG4_char_size;
    uint8_t BG_mode;
    bool mode1_BG3_priority;
    uint8_t mosaic_pixel_size;
    uint16_t BG1_tilemap_address, BG2_tilemap_address, BG3_tilemap_address, BG4_tilemap_address;
    bool BG1_tilemap_x_mirror, BG2_tilemap_x_mirror, BG3_tilemap_x_mirror, BG4_tilemap_x_mirror;
    bool BG1_tilemap_y_mirror, BG2_tilemap_y_mirror, BG3_tilemap_y_mirror, BG4_tilemap_y_mirror;
    uint16_t BG1_char_address, BG2_char_address, BG3_char_address, BG4_char_address;
    uint16_t BG1_hscroll, BG1_vscroll;
    uint16_t BG2_hscroll, BG2_vscroll;
    uint16_t BG3_hscroll, BG3_vscroll;
    uint16_t BG4_hscroll, BG4_vscroll;
    bool BG1HOFS_h, BG1VOFS_h; 
    bool BG2HOFS_h, BG2VOFS_h;
    bool BG3HOFS_h, BG3VOFS_h; 
    bool BG4HOFS_h, BG4VOFS_h;
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
    bool cg_h_write;
    //WINDOWS UNIMPLEMENTED, W* REGISTERS UNIMPLEMENTED
    uint8_t clip_colors_black;
    uint8_t prevent_color_math;
    bool add_subscreen;
    bool direct_color;
    bool add_sub_color;
    bool half_color_math;
    bool sw_fixed_color;
    bool BG1_color_math_en, BG2_color_math_en, BG3_color_math_en, BG4_color_math_en, OBJ_color_math_en, BD_color_math_en;
    uint8_t h_scanline, v_scanline;
    bool time_over, range_over, interlace, external_latch;
    uint32_t fixed_color;
    uint32_t *BG1_frame_buffer;
    uint32_t *BG2_frame_buffer;
    uint32_t *BG3_frame_buffer;
    uint32_t *BG4_frame_buffer;
    uint32_t *obj_frame_buffer;
    uint32_t *BG1_sub_frame_buffer;
    uint32_t *BG2_sub_frame_buffer;
    uint32_t *BG3_sub_frame_buffer;
    uint32_t *BG4_sub_frame_buffer;
    uint32_t *obj_sub_frame_buffer;
    uint32_t *main_frame_buffer;
    uint32_t *sub_frame_buffer;
    bool *BG1_priority_buffer;
    bool *BG2_priority_buffer;
    bool *BG3_priority_buffer;
    bool *BG4_priority_buffer;
    bool *priority_buffer;
    uint8_t *obj_priority_buffer;
    bool *BG1w;
    bool *BG2w;
    bool *BG3w;
    bool *BG4w;
    bool *OBJw;
    bool *BDmain;
    bool *BDsub;
    bool w1invBG1, w1invBG2, w1invBG3, w1invBG4, w1invOBJ, w1invBD; 
    bool w1BG1en, w1BG2en, w1BG3en, w1BG4en, w1OBJen, w1BDen; 
    bool w2invBG1, w2invBG2, w2invBG3, w2invBG4, w2invOBJ, w2invBD; 
    bool w2BG1en, w2BG2en, w2BG3en, w2BG4en, w2OBJen, w2BDen; 
    uint8_t w1_left, w1_right, w2_left, w2_right;
    uint8_t BG1_mask_logic, BG2_mask_logic, BG3_mask_logic, BG4_mask_logic; 
    uint8_t OBJ_mask_logic, BD_mask_logic;
    bool BG1_main_en, BG2_main_en, BG3_main_en, BG4_main_en, OBJ_main_en; 
    bool BG1_sub_en, BG2_sub_en, BG3_sub_en, BG4_sub_en, OBJ_sub_en; 
    bool BG1_mainw_en, BG2_mainw_en, BG3_mainw_en, BG4_mainw_en, OBJ_mainw_en; 
    bool BG1_subw_en, BG2_subw_en, BG3_subw_en, BG4_subw_en, OBJ_subw_en; 
    bool BG1_mosaic, BG2_mosaic, BG3_mosaic, BG4_mosaic;
    uint8_t mosaic_amount;

    bool in_vblank, in_hblank;

    uint32_t *frame_buffer;
    uint16_t *bpp_matrix;
};
