#include <stdint.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <X11/Xlib.h>
#include "ppu.h"
using namespace std;

Ppu::Ppu() {
    di = XOpenDisplay(getenv("DISPLAY"));
    //Create window
    int x = 0, y = 0, width = 256, height = 240, border_width = 1;
    sc = DefaultScreen(di);
    ro = DefaultRootWindow(di);
    wi = XCreateSimpleWindow(di, ro, x, y, width, height, border_width, BlackPixel(di, sc), WhitePixel(di, sc));
    XSelectInput(di, wi, KeyPressMask | KeyReleaseMask | ExposureMask);
    XAutoRepeatOn(di);
    gc = XCreateGC(di, ro, 0, NULL);
    XMapWindow(di, wi); //Make window visible
    XStoreName(di, wi, "SNEScutre");

    oam = (uint8_t *) malloc(sizeof(uint8_t) * OAM_SIZE);
    vram = (uint8_t *) malloc(sizeof(uint8_t) * VRAM_SIZE);
    cg = (uint16_t *) malloc(sizeof(uint16_t) * CG_SIZE);
    BG1_frame_buffer = (uint32_t *) malloc(sizeof(uint32_t) * 1024 * 1024);
    BG2_frame_buffer = (uint32_t *) malloc(sizeof(uint32_t) * 1024 * 1024);
    BG3_frame_buffer = (uint32_t *) malloc(sizeof(uint32_t) * 1024 * 1024);
    BG4_frame_buffer = (uint32_t *) malloc(sizeof(uint32_t) * 1024 * 1024);
    bpp_matrix = (uint16_t *) malloc(sizeof(uint16_t) * 4 * 8);
    initBppMatrix();
}

void Ppu::drawBG(uint8_t BG, uint8_t mode) {
    uint32_t *bg_frame_buffer;
    if (BG == 1) bg_frame_buffer = BG1_frame_buffer;
    else if (BG == 2) bg_frame_buffer = BG2_frame_buffer;
    else if (BG == 3) bg_frame_buffer = BG3_frame_buffer;
    else if (BG == 4) bg_frame_buffer = BG4_frame_buffer;
    //Determine tilemap addresses
    uint32_t tilemap_address, char_address;
    bool tilemap_x_mirror, tilemap_y_mirror;
    if (BG == 1) {
	tilemap_address  = BG1_tilemap_address;
	tilemap_x_mirror = BG1_tilemap_x_mirror;
	tilemap_y_mirror = BG1_tilemap_y_mirror;
	char_address     = BG1_char_address;
    }
    else if (BG == 2) {
	tilemap_address  = BG2_tilemap_address;
	tilemap_x_mirror = BG2_tilemap_x_mirror;
	tilemap_y_mirror = BG2_tilemap_y_mirror;
	char_address     = BG2_char_address;
    }
    else if (BG == 3) {
	tilemap_address  = BG3_tilemap_address;
	tilemap_x_mirror = BG3_tilemap_x_mirror;
	tilemap_y_mirror = BG3_tilemap_y_mirror;
	char_address     = BG3_char_address;
    }
    else if (BG == 4) {
	tilemap_address  = BG4_tilemap_address;
	tilemap_x_mirror = BG4_tilemap_x_mirror;
	tilemap_y_mirror = BG4_tilemap_y_mirror;
	char_address     = BG4_char_address;
    }
    uint32_t tilemap_size = 0x800;
    if (tilemap_x_mirror && !tilemap_y_mirror)
	tilemap_size += 0x800;
    if (tilemap_y_mirror && !tilemap_x_mirror)
	tilemap_size += 0x800;
    if (tilemap_x_mirror && tilemap_y_mirror)
	tilemap_size += 3*0x800;
    
    //Determine Mode specific parameters
    uint8_t bpp = bpp_matrix[4*mode + (BG-1)];
    
    //Tile loop
    for (uint32_t i = 0; i < tilemap_size; i+=2) {
	uint8_t high, low;
	uint16_t tile_number;
	uint8_t palette;
	bool v_flip, h_flip, priority;
	low = vram[tilemap_address + (i*2)];
	high = vram[tilemap_address + (i*2) + 1];

	tile_number = ((high & 0x03) << 8) | low;
	palette = (high >> 2) & 0x07;
	priority = (high >> 5) & 1;
	h_flip = (high >> 6) & 1;
	v_flip = (high >> 7) & 1;

	//Determine tile position on the screen
	//TODO: consider 16x16 tiles, which is to say divide by 32*2*2
	uint8_t tile_x, tile_y;
	tile_x = (((i % 0x800) % (32*2))/2);
	tile_y = (((i % 0x800) / (32*2)));

	uint32_t character_address = (char_address << 13) + (tile_number * 8*bpp);
	uint32_t palette_address = determinePaletteAddress(BG, mode);

	//TODO:If direct color
	//Separate in a function?
	for (int i2; i2 < 8; ++i2) {
	    for (int j2; j2 < 8; ++j2) {
		uint8_t cg_index = 0;
		for (uint8_t plane = 0; plane < bpp; plane += 2) {
		    uint8_t lowplane = vram[character_address + (8*plane) + (i2*2)];
		    uint8_t highplane = vram[character_address + (8*plane) + (i2*2) + 1];
		    lowplane  = (lowplane >> (7-j2)) & 1;
		    highplane = ((highplane >> (7-j2)) << 1) & 1;
		    cg_index = (highplane | lowplane) << plane;
		}
		//To find the corresponding color, we sum:
		//  - where the palette starts for this BG
		//  - The palette index * 8 colors for each bpp
		//  - The index computed with the bitplanes
		uint32_t color = cg[palette_address + (palette * (8*bpp)) + cg_index];
		color = convert_BGR_RGB(color);
		
		uint8_t pos_x = tile_x * 8, pos_y = tile_y * 8;
		if (i >= 0x800 && i < 2*0x800) {
		    if (tilemap_x_mirror) pos_x += 256;
		    else if (tilemap_y_mirror) pos_y += 256;
		}
		else if (i >= 2*0x800 && i < 3*0x800) {
		    pos_y += 256;
		}
		else {
		    pos_x += 256;
		    pos_y += 256;
		}
		pos_x += j2;
		pos_y += i2;
		bg_frame_buffer[pos_x + (1024*pos_y)] = color;	
	    }
	}
    }
}

uint32_t Ppu::determinePaletteAddress(uint8_t BG, uint8_t mode) {
    //Mode zero separates BG palettes, the other don't
    if (mode == 0)
	return cg_address + (8*4*(BG-1));
    else
	return cg_address;
 }
	    
uint32_t Ppu::convert_BGR_RGB(uint32_t bgr) {
    uint8_t R = ((bgr       ) % 32) * 8;
    uint8_t G = ((bgr /   32) % 32) * 8;
    uint8_t B = ((bgr / 1024) % 32) * 8;
    return ((R << 16) | (G << 8) | B);
}

void Ppu::drawScreen() {
    for (uint32_t i = 0; i<256; ++i) {
	for (uint32_t j = 0; j<256; ++j) {
	    XSetForeground(di, gc, BG1_frame_buffer[j + (i*1024)]);
	    XDrawPoint(di, wi, gc, j, i);
	}
    }
}

void Ppu::vblank() {
}

///////////////////
// PPU REGISTERS //
///////////////////

void Ppu::write_INIDISP(uint8_t data) {
    //x---bbbb
    brightness = data & 0x0f;
    Fblank = (data >> 7) & 1;
}

void Ppu::write_OBSEL(uint8_t data) {
    //sssnnbbb
    name_base_select = data & 0x07;
    name_select = (data >> 3) & 0x03;
    object_size = (data >> 5) & 0x07;
}

void Ppu::write_OAMADDL(uint8_t data) {
    //aaaaaaaa
    OAMADDL = data;
    oam_address = ((OAMADDH & 1) << 8) | OAMADDL;
}

void Ppu::write_OAMADDH(uint8_t data) {
    //p------b
    OAMADDH = data & 0x01;
    obj_priority_bit = (data >> 7) & 0x01;
    oam_address = ((OAMADDH & 1) << 8) | OAMADDL;
}

void Ppu::write_OAMDATA(uint8_t data) {
    //dddddddd
    
    if (oam_h_addr) {
        //when we write to the high table, we write
        //the buffered value to the low table also.
        //On the high table we only use the first
        //4 bits of the address
        oam[0x200 | (oam_address & 0x0f)] = data;
        oam[oam_address] = oam_low_buffer;
        oam_h_addr = false;
        oam_address++;
    }
    else {
        //We set the low buffer
        oam_low_buffer = data;
        oam_h_addr = true;
    }
}

void Ppu::write_BGMODE(uint8_t data) {
    //DCBAemmm
    BG_mode = (data & 0x07);
    mode1_BG3_priority = (data >> 3) & 0x01;
    BG1_char_size = (data >> 4) & 0x01;
    BG2_char_size = (data >> 5) & 0x01;
    BG3_char_size = (data >> 6) & 0x01;
    BG4_char_size = (data >> 7) & 0x01;
}

void Ppu::write_MOSAIC(uint8_t data) {
    //xxxxDCBA
    BG1_mosaic = data & 0x01;
    BG2_mosaic = (data >> 1) & 0x01;
    BG3_mosaic = (data >> 2) & 0x01;
    BG4_mosaic = (data >> 3) & 0x01;
    mosaic_pixel_size = (data >> 4) & 0x0f;
}

void Ppu::write_BG1SC(uint8_t data) {
    //aaaaaayx
    BG1_tilemap_x_mirror = data & 0x01;
    BG1_tilemap_y_mirror = (data >> 1) & 0x01;
    BG1_tilemap_address = (data >> 2) & 0x3f;
}

void Ppu::write_BG2SC(uint8_t data) {
    //aaaaaayx
    BG2_tilemap_x_mirror = data & 0x01;
    BG2_tilemap_y_mirror = (data >> 1) & 0x01;
    BG2_tilemap_address = (data >> 2) & 0x3f;
}

void Ppu::write_BG3SC(uint8_t data) {
    //aaaaaayx
    BG3_tilemap_x_mirror = data & 0x01;
    BG3_tilemap_y_mirror = (data >> 1) & 0x01;
    BG3_tilemap_address = (data >> 2) & 0x3f;
}

void Ppu::write_BG4SC(uint8_t data) {
    //aaaaaayx
    BG4_tilemap_x_mirror = data & 0x01;
    BG4_tilemap_y_mirror = (data >> 1) & 0x01;
    BG4_tilemap_address = (data >> 2) & 0x3f;
}

void Ppu::write_BG12NBA(uint8_t data) {
    //bbbbaaaa
    BG1_char_address = data & 0x0f;
    BG2_char_address = (data >> 4) & 0x0f;
}

void Ppu::write_BG34NBA(uint8_t data) {
    //bbbbaaaa
    BG3_char_address = data & 0x0f;
    BG4_char_address = (data >> 4) & 0x0f;
}

void Ppu::write_BG1HOFS(uint8_t data) {
    //------xx xxxxxxxx write twice
    if (BG1HOFS_h) {
        BG1_hscroll |= ((data << 8) & 0x03);
        BG1HOFS_h = false;
    }
    else {
        BG1_hscroll = data;
        BG1HOFS_h = true;
    }
}

void Ppu::write_BG1VOFS(uint8_t data) {
    //------xx xxxxxxxx write twice
    if (BG1VOFS_h) {
        BG1_vscroll |= ((data << 8) & 0x03);
        BG1VOFS_h = false;
    }
    else {
        BG1_vscroll = data;
        BG1VOFS_h = true;
    }
}

void Ppu::write_BG2HOFS(uint8_t data) {
    //------xx xxxxxxxx write twice
    if (BG2HOFS_h) {
        BG2_hscroll |= ((data << 8) & 0x03);
        BG2HOFS_h = false;
    }
    else {
        BG2_hscroll = data;
        BG2HOFS_h = true;
    }
}

void Ppu::write_BG2VOFS(uint8_t data) {
    //------xx xxxxxxxx write twice
    if (BG2VOFS_h) {
        BG2_vscroll |= ((data << 8) & 0x03);
        BG2VOFS_h = false;
    }
    else {
        BG2_vscroll = data;
        BG2VOFS_h = true;
    }
}

void Ppu::write_BG3HOFS(uint8_t data) {
    //------xx xxxxxxxx write twice
    if (BG3HOFS_h) {
        BG3_hscroll |= ((data << 8) & 0x03);
        BG3HOFS_h = false;
    }
    else {
        BG3_hscroll = data;
        BG3HOFS_h = true;
    }
}

void Ppu::write_BG3VOFS(uint8_t data) {
    //------xx xxxxxxxx write twice
    if (BG3VOFS_h) {
        BG3_vscroll |= ((data << 8) & 0x03);
        BG3VOFS_h = false;
    }
    else {
        BG3_vscroll = data;
        BG3VOFS_h = true;
    }
}

void Ppu::write_BG4HOFS(uint8_t data) {
    //------xx xxxxxxxx write twice
    if (BG4HOFS_h) {
        BG4_hscroll |= ((data << 8) & 0x03);
        BG4HOFS_h = false;
    }
    else {
        BG4_hscroll = data;
        BG4HOFS_h = true;
    }
}

void Ppu::write_BG4VOFS(uint8_t data) {
    //------xx xxxxxxxx write twice
    if (BG4VOFS_h) {
        BG4_vscroll |= ((data << 8) & 0x03);
        BG4VOFS_h = false;
    }
    else {
        BG4_vscroll = data;
        BG4VOFS_h = true;
    }
}

void Ppu::write_BG5HOFS(uint8_t data) {
    //------xx xxxxxxxx write twice
    if (BG5HOFS_h) {
        BG5_hscroll |= ((data << 8) & 0x03);
        BG5HOFS_h = false;
    }
    else {
        BG5_hscroll = data;
        BG5HOFS_h = true;
    }
}

void Ppu::write_BG5VOFS(uint8_t data) {
    //------xx xxxxxxxx write twice
    if (BG5VOFS_h) {
        BG5_vscroll |= ((data << 8) & 0x03);
        BG5VOFS_h = false;
    }
    else {
        BG5_vscroll = data;
        BG5VOFS_h = true;
    }
}

void Ppu::write_BG6HOFS(uint8_t data) {
    //------xx xxxxxxxx write twice
    if (BG6HOFS_h) {
        BG6_hscroll |= ((data << 8) & 0x03);
        BG6HOFS_h = false;
    }
    else {
        BG6_hscroll = data;
        BG6HOFS_h = true;
    }
}

void Ppu::write_BG6VOFS(uint8_t data) {
    //------xx xxxxxxxx write twice
    if (BG6VOFS_h) {
        BG6_vscroll |= ((data << 8) & 0x03);
        BG6VOFS_h = false;
    }
    else {
        BG6_vscroll = data;
        BG6VOFS_h = true;
    }
}

void Ppu::write_VMAIN(uint8_t data) {
    //n---mmii
    address_increment_amount = data & 0x03;
    address_remapping = (data >> 2) & 0x03;
    address_increment_mode = (data >> 7) & 0x01;
}

void Ppu::write_VMADDL(uint8_t data) {
    //aaaaaaaa
    VMADDL = data;
    vram_address = (vram_address & 0xff00) | data;
}

void Ppu::write_VMADDH(uint8_t data) {
    //aaaaaaaa
    VMADDH = data;
    vram_address = (data << 8) | (vram_address & 0x00ff);
}

void Ppu::write_VMDATAL(uint8_t data) {
    //aaaaaaaa
    uint16_t vram_address_remap;

    if (address_remapping == 0) {
        vram_address_remap = vram_address;
    } else if (address_remapping == 1) {
        //aaaaaaaaBBBccccc => aaaaaaaacccccBBB
        vram_address_remap = 
            (vram_address & 0xff00) | ((vram_address & 0x001f)<<3) | ((vram_address & 0x00e0)>>5);
    } else if (address_remapping == 2) {
        //aaaaaaaBBBcccccc => aaaaaaaccccccBBB
        vram_address_remap = 
            (vram_address & 0xfe00) | ((vram_address & 0x003f)<<3) | ((vram_address & 0x01c0)>>6);
    } else {
        //aaaaaaBBBccccccc => aaaaaacccccccBBB
        vram_address_remap = 
            (vram_address & 0xfc00) | ((vram_address & 0x007f)<<3) | ((vram_address & 0x0380)>>7);
    }
    
    vram[vram_address_remap] = (vram[vram_address_remap] & 0xff00) | data;
    if (address_increment_mode == 0) {
        vram_read_buffer = vram[vram_address_remap];
        if (address_increment_amount == 0) {
            vram_address++;
        } else if (address_increment_amount == 1) {
            vram_address += 32;
        } else {
            vram_address += 128;
        }
    }
}

void Ppu::write_VMDATAH(uint8_t data) {
    //aaaaaaaa
    uint16_t vram_address_remap;

    if (address_remapping == 0) {
        vram_address_remap = vram_address;
    } else if (address_remapping == 1) {
        //aaaaaaaaBBBccccc => aaaaaaaacccccBBB
        vram_address_remap = 
            (vram_address & 0xff00) | ((vram_address & 0x001f)<<3) | ((vram_address & 0x00e0)>>5);
    } else if (address_remapping == 2) {
        //aaaaaaaBBBcccccc => aaaaaaaccccccBBB
        vram_address_remap = 
            (vram_address & 0xfe00) | ((vram_address & 0x003f)<<3) | ((vram_address & 0x01c0)>>6);
    } else {
        //aaaaaaBBBccccccc => aaaaaacccccccBBB
        vram_address_remap = 
            (vram_address & 0xfc00) | ((vram_address & 0x007f)<<3) | ((vram_address & 0x0380)>>7);
    }
    
    vram[vram_address_remap] = (vram[vram_address_remap]&0x00ff) | (data<<8);
    if (address_increment_mode == 1) {
        vram_read_buffer = vram[vram_address_remap];
        if (address_increment_amount == 0) {
            vram_address++;
        } else if (address_increment_amount == 1) {
            vram_address += 32;
        } else {
            vram_address += 128;
        }
    }
}

void Ppu::write_CGADD(uint8_t data) {
    //cccccccc
    CGADD = data;
    cg_address = data;
}

void Ppu::write_CGDATA(uint8_t data) {
    //-bbbbbgg gggrrrrr
    if (cg_low_buffer == 1) {
        cg[cg_address] = (data<<8) | cg_low_buffer;
        cg_address++;
    } else {
        cg_low_buffer = data;
    }
}

void Ppu::write_TM(uint8_t data) {
    //---o4321
    BG1_main_en = data & 1;
    BG2_main_en = (data >> 1) & 1;
    BG3_main_en = (data >> 2) & 1;
    BG4_main_en = (data >> 3) & 1;
    OBJ_main_en = (data >> 4) & 1;
}

void Ppu::write_TS(uint8_t data) {
    //---o4321
    BG1_subs_en = data & 1;
    BG2_subs_en = (data >> 1) & 1;
    BG3_subs_en = (data >> 2) & 1;
    BG4_subs_en = (data >> 3) & 1;
    OBJ_subs_en = (data >> 4) & 1;
}

void Ppu::write_CGWSEL(uint8_t data) {
    //ccmm--sd
    direct_color_256_BGs = data & 1;
    add_subscreen = (data >> 1) & 1;
    prevent_color_math = (data >> 2) & 0x03;
    clip_colors_to_black = (data >> 4) & 0x03;
}

void Ppu::write_CGADSUB(uint8_t data) {
    //shbo4321
    BG1_color_math_en = data & 1;
    BG2_color_math_en = (data >> 1) & 1;
    BG3_color_math_en = (data >> 2) & 1;
    BG4_color_math_en = (data >> 3) & 1;
    OBJ_color_math_en = (data >> 4) & 1;
    BD_color_math_en = (data >> 5) & 1;
    half_color_math = (data >> 6) & 1;
    add_sub_color = (data >> 7) & 1;
}

void Ppu::write_COLDATA(uint8_t data) {
    //bgrccccc
    color_intensity = data & 0x1f;
    r_intensity = (data >> 5) & 1;
    g_intensity = (data >> 6) & 1;
    b_intensity = (data >> 7) & 1;
}

uint8_t Ppu::read_OAMDATAREAD() {
    uint8_t result;
    if (oam_h_addr == 1) {
        result = oam[0x0200 | (oam_address & 0x0f)];
        oam_address++;
        oam_h_addr = 0;
    }
    else {
        result = oam[oam_address];
        oam_address++;
        oam_h_addr = 1;
    }
    return result;
}

uint8_t Ppu::read_VMDATALREAD() {
    uint16_t vram_address_remap;
    uint8_t result;

    if (address_remapping == 0) {
        vram_address_remap = vram_address;
    } else if (address_remapping == 1) {
        //aaaaaaaaBBBccccc => aaaaaaaacccccBBB
        vram_address_remap = 
            (vram_address & 0xff00) | ((vram_address & 0x001f)<<3) | ((vram_address & 0x00e0)>>5);
    } else if (address_remapping == 2) {
        //aaaaaaaBBBcccccc => aaaaaaaccccccBBB
        vram_address_remap = 
            (vram_address & 0xfe00) | ((vram_address & 0x003f)<<3) | ((vram_address & 0x01c0)>>6);
    } else {
        //aaaaaaBBBccccccc => aaaaaacccccccBBB
        vram_address_remap = 
            (vram_address & 0xfc00) | ((vram_address & 0x007f)<<3) | ((vram_address & 0x0380)>>7);
    }
    
    result = vram_read_buffer & 0x00ff;
    if (address_increment_mode == 0) {
        vram_read_buffer = vram[vram_address_remap];
        if (address_increment_amount == 0) {
            vram_address++;
        } else if (address_increment_amount == 1) {
            vram_address += 32;
        } else {
            vram_address += 128;
        }
    }
    return result;
}

uint8_t Ppu::read_VMDATAHREAD() {
    uint16_t vram_address_remap;
    uint8_t result;

    if (address_remapping == 0) {
        vram_address_remap = vram_address;
    } else if (address_remapping == 1) {
        //aaaaaaaaBBBccccc => aaaaaaaacccccBBB
        vram_address_remap = 
            (vram_address & 0xff00) | ((vram_address & 0x001f)<<3) | ((vram_address & 0x00e0)>>5);
    } else if (address_remapping == 2) {
        //aaaaaaaBBBcccccc => aaaaaaaccccccBBB
        vram_address_remap = 
            (vram_address & 0xfe00) | ((vram_address & 0x003f)<<3) | ((vram_address & 0x01c0)>>6);
    } else {
        //aaaaaaBBBccccccc => aaaaaacccccccBBB
        vram_address_remap = 
            (vram_address & 0xfc00) | ((vram_address & 0x007f)<<3) | ((vram_address & 0x0380)>>7);
    }
    
    result = (vram_read_buffer >> 8) & 0x00ff;
    if (address_increment_mode == 0) {
        vram_read_buffer = vram[vram_address_remap];
        if (address_increment_amount == 0) {
            vram_address++;
        } else if (address_increment_amount == 1) {
            vram_address += 32;
        } else {
            vram_address += 128;
        }
    }
    return result;
}

uint16_t Ppu::read_CGDATAREAD() {
    uint16_t result = cg[cg_address];
    cg_address++;
    return result;
}   

void Ppu::initBppMatrix() {
    bpp_matrix[0] = 4;
    bpp_matrix[1] = 4;
    bpp_matrix[2] = 4;
    bpp_matrix[3] = 4;
    bpp_matrix[4] = 16;
    bpp_matrix[5] = 16;
    bpp_matrix[6] = 4;
    bpp_matrix[8] = 16;
    bpp_matrix[9] = 16;
    bpp_matrix[12] = 256;
    bpp_matrix[13] = 16;
    bpp_matrix[16] = 256;
    bpp_matrix[17] = 4;
    bpp_matrix[20] = 16;
    bpp_matrix[21] = 4;
    bpp_matrix[24] = 16;
    bpp_matrix[28] = 256;
}
