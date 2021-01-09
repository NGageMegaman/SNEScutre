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
    int x = 0, y = 0, width = 512, height = 512, border_width = 1;
    sc = DefaultScreen(di);
    ro = DefaultRootWindow(di);
    wi = XCreateSimpleWindow(di, ro, x, y, width, height, border_width, BlackPixel(di, sc), WhitePixel(di, sc));
    XSelectInput(di, wi, KeyPressMask | KeyReleaseMask | ExposureMask);
    XAutoRepeatOn(di);
    gc = XCreateGC(di, ro, 0, NULL);
    XMapWindow(di, wi); //Make window visible
    XStoreName(di, wi, "SNEScutre");

    oam = (uint8_t *) malloc(sizeof(uint8_t) * OAM_SIZE);
    vram = (uint16_t *) malloc(sizeof(uint16_t) * VRAM_SIZE);
    cg = (uint16_t *) malloc(sizeof(uint16_t) * CG_SIZE);
    BG1_frame_buffer = (uint32_t *) malloc(sizeof(uint32_t) * 512 * 512);
    BG2_frame_buffer = (uint32_t *) malloc(sizeof(uint32_t) * 512 * 512);
    BG3_frame_buffer = (uint32_t *) malloc(sizeof(uint32_t) * 512 * 512);
    BG4_frame_buffer = (uint32_t *) malloc(sizeof(uint32_t) * 512 * 512);
    obj_frame_buffer = (uint32_t *) malloc(sizeof(uint32_t) * 512 * 512);
    BG1_priority_buffer = (bool *) malloc(sizeof(bool) * 512 * 512);
    BG2_priority_buffer = (bool *) malloc(sizeof(bool) * 512 * 512);
    BG3_priority_buffer = (bool *) malloc(sizeof(bool) * 512 * 512);
    BG4_priority_buffer = (bool *) malloc(sizeof(bool) * 512 * 512);
    obj_priority_buffer = (uint8_t *) malloc(sizeof(uint8_t) * 512 * 512);
    frame_buffer = (uint32_t *) malloc(sizeof(uint32_t) * 512 * 512);
    bpp_matrix = (uint16_t *) malloc(sizeof(uint16_t) * 4 * 8);
    
    fixed_color = 0;
    initBppMatrix();

    BG1HOFS_h = 0; BG1VOFS_h = 0;
    BG2HOFS_h = 0; BG2VOFS_h = 0;
    BG3HOFS_h = 0; BG3VOFS_h = 0;
    BG4HOFS_h = 0; BG4VOFS_h = 0;
    oam_h_addr = 0;
}

void Ppu::drawBGs() {
    for (int i = 0; i<4; ++i) {
        drawBG(i+1);
    }
    drawSprites();
    renderFrame();
}

void Ppu::drawBG(uint8_t BG) {
    uint32_t *bg_frame_buffer;
    bool *bg_priority_buffer;
    uint16_t hscroll, vscroll;
    //Determine tilemap addresses
    uint32_t tilemap_address, char_address;
    bool tilemap_x_mirror, tilemap_y_mirror;
    if (BG == 1) {
        bg_frame_buffer    = BG1_frame_buffer;
        bg_priority_buffer = BG1_priority_buffer;
	    tilemap_address    = BG1_tilemap_address;
	    tilemap_x_mirror   = BG1_tilemap_x_mirror;
	    tilemap_y_mirror   = BG1_tilemap_y_mirror;
	    char_address       = BG1_char_address;
        hscroll            = BG1_hscroll;
        vscroll            = BG1_vscroll;
    }
    else if (BG == 2) {
        bg_frame_buffer    = BG2_frame_buffer;
        bg_priority_buffer = BG2_priority_buffer;
	    tilemap_address    = BG2_tilemap_address;
	    tilemap_x_mirror   = BG2_tilemap_x_mirror;
	    tilemap_y_mirror   = BG2_tilemap_y_mirror;
	    char_address       = BG2_char_address;
        hscroll            = BG2_hscroll;
        vscroll            = BG2_vscroll;
    }
    else if (BG == 3) {
        bg_frame_buffer    = BG3_frame_buffer;
        bg_priority_buffer = BG3_priority_buffer;
	    tilemap_address    = BG3_tilemap_address;
	    tilemap_x_mirror   = BG3_tilemap_x_mirror;
	    tilemap_y_mirror   = BG3_tilemap_y_mirror;
	    char_address       = BG3_char_address;
        hscroll            = BG3_hscroll;
        vscroll            = BG3_vscroll;
    }
    else if (BG == 4) {
        bg_frame_buffer    = BG4_frame_buffer;
        bg_priority_buffer = BG4_priority_buffer;
	    tilemap_address    = BG4_tilemap_address;
	    tilemap_x_mirror   = BG4_tilemap_x_mirror;
	    tilemap_y_mirror   = BG4_tilemap_y_mirror;
	    char_address       = BG4_char_address;
        hscroll            = BG4_hscroll;
        vscroll            = BG4_vscroll;
    }
    uint32_t tilemap_size = 0x400;
    if (tilemap_x_mirror && !tilemap_y_mirror)
	    tilemap_size += 0x400;
    if (tilemap_y_mirror && !tilemap_x_mirror)
	    tilemap_size += 0x400;
    if (tilemap_x_mirror && tilemap_y_mirror)
	    tilemap_size += 3*0x400;
    
    //Determine Mode specific parameters
    uint8_t bpp = bpp_matrix[4*BG_mode + (BG-1)];
    
    //Tile loop
    for (uint32_t i = 0; i < tilemap_size; i++) {
	    uint16_t tile;
	    uint16_t tile_number;
	    uint8_t palette;
	    bool v_flip, h_flip, priority;
	    tile = vram[tilemap_address + i];

	    tile_number = tile & 0x3ff;
	    palette = (tile >> 10) & 0x07;
	    priority = (tile >> 13) & 1;
	    h_flip = (tile >> 14) & 1;
	    v_flip = (tile >> 15) & 1;

	    //Determine tile position on the screen
	    //TODO: consider 16x16 tiles, which is to say divide by 16
	    uint32_t tile_x, tile_y;
	    tile_x = (i % 0x400) % (32);
	    tile_y = (i % 0x400) / (32);

	    uint32_t character_address = char_address + (tile_number*4*bpp);
	    uint32_t palette_address = determinePaletteAddress(BG, BG_mode);

	    //TODO:If direct color
	    //Separate in a function?
	    for (uint8_t i2 = 0; i2 < 8; ++i2) {
	        for (uint8_t j2 = 0; j2 < 8; ++j2) {
	    	    uint8_t cg_index = 0;
	    	    for (uint8_t plane = 0; plane < bpp; plane += 2) {
	    	        uint16_t hilo_plane = vram[character_address + (4*plane) + i2];
	    	        uint8_t lowplane = hilo_plane & 0x00ff;
	    	        uint8_t highplane = (hilo_plane >> 8) & 0x00ff;
	    	        lowplane  = (lowplane >> (7-j2)) & 1;
	    	        highplane = ((highplane >> (7-j2)) & 1) << 1;
	    	        cg_index |= ((highplane | lowplane) << plane);
	    	    }

	    	    //To find the corresponding color, we sum:
	    	    //  - where the palette starts for this BG
	    	    //  - The palette index * 8 colors for each bpp
	    	    //  - The index computed with the bitplanes
                uint32_t color;
                if (cg_index == 0) color = -1; 
	    	    else color = cg[palette_address + (palette*bpp*bpp) + cg_index];
	    	    
	    	    uint32_t pos_x = tile_x*8; 
                uint32_t pos_y = tile_y*8;
	    	    if (i >= 0x400 && i < 2*0x400) {
	    	        if (tilemap_x_mirror) pos_x += 256;
	    	        else if (tilemap_y_mirror) pos_y += 256;
	    	    }
	    	    else if (i >= 2*0x400 && i < 3*0x400) {
	    	        pos_y += 256;
	    	    }
	    	    else if (i >= 3*0x400) {
	    	        pos_x += 256;
	    	        pos_y += 256;
	    	    }
                if (h_flip) pos_x += (7-j2);
	    	    else pos_x += j2;
                if (v_flip) pos_y += (7-i2);
	    	    else pos_y += i2;
                pos_x = (pos_x + hscroll) % 512;
                pos_y = (pos_y - vscroll) % 512;
	    	    bg_frame_buffer[pos_x + (512*pos_y)] = color;	
                bg_priority_buffer[pos_x + (512*pos_y)] = priority;
	        }
	    }
    }
}

void Ppu::drawSprites() {
    for (uint32_t i = 0; i < 512; ++i) {
        for (uint32_t j = 0; j < 512; ++j) {
            obj_frame_buffer[j + (i*512)] = -1;
        }
    }
    //Sprite loop
    for (uint32_t i = 0; i < 128; i++) {
        uint8_t low_x = oam[(i*4)];
        uint16_t tile_y = (oam[(i*4)+1]) & 0x00ff;
        uint8_t first_tile = oam[(i*4)+2];
        uint8_t byte4 = oam[(i*4)+3];
        bool nametable = byte4 & 1;
        uint8_t palette = (byte4 >> 1) & 0x07;
        uint8_t priority = (byte4 >> 4) & 0x03;
        bool h_flip = (byte4 >> 6) & 1;
        bool v_flip = (byte4 >> 7) & 1;

        uint8_t highByte = oam[512 + (i % 4)];
        bool high_x = (highByte >> ((i % 4)*2)) & 1;
        bool size = (highByte >> (((i % 4)*2)+1)) & 1;
        //cout << size << endl;
        uint16_t tile_x = (high_x << 8) | low_x;

        uint8_t bpp = 4;

        uint8_t size_x, size_y;
        determineObjectSize(&size_x, &size_y, size);

        if (i == (9 * 16 * 4)) cout << "puff pos y " << std::hex << (unsigned) tile_y << endl;

        uint32_t character_address, base_pos_x, base_pos_y;

        for (uint8_t horiz = 0; horiz < size_x; horiz++) {
            for (uint8_t vert = 0; vert < size_y; vert++) {
                uint32_t off_x = first_tile & 0x00f;
                uint32_t off_y = first_tile & 0x0f0;
                off_x = (off_x + (horiz)) & 0x00f;
                off_y = (off_y + ((vert) << 4)) & 0x0f0;
                uint8_t tile_number = off_y | off_x;
	            character_address = (name_base_select + (tile_number<<4) + (nametable ? ((name_select+1)<<12) : 0)) & 0x7fff;
                base_pos_x = tile_x;
                base_pos_y = tile_y;
	            //TODO:If direct color
	            //Separate in a function?
	            for (uint8_t i2 = 0; i2 < 8; ++i2) {
	                for (uint8_t j2 = 0; j2 < 8; ++j2) {
	            	    uint8_t cg_index = 0;
	            	    for (uint8_t plane = 0; plane < bpp; plane += 2) {
	            	        uint16_t hilo_plane = vram[character_address + (4*plane) + i2];
	            	        uint8_t lowplane = hilo_plane & 0x00ff;
	            	        uint8_t highplane = (hilo_plane >> 8) & 0x00ff;
	            	        lowplane  = (lowplane >> (7-j2)) & 1;
	            	        highplane = ((highplane >> (7-j2)) & 1) << 1;
	            	        cg_index |= ((highplane | lowplane) << plane);
	            	    }
                        uint32_t color;
                        if (cg_index == 0) color = -1; 
	            	    else color = cg[128 + (palette*bpp*bpp) + cg_index];

                        uint32_t pos_x = base_pos_x;
                        uint32_t pos_y = base_pos_y;
                        if (h_flip) {
                            pos_x += (7-j2);
                            pos_x += (((size_x-1) * 8) - (horiz * 8));
                        }
	            	    else pos_x += j2 + (horiz * 8);
                        if (v_flip) pos_y += (7-i2) + (vert * 8);
	            	    else pos_y += i2 + (vert * 8);
                        pos_x %= 512;
                        pos_y %= 512;
	            	    obj_frame_buffer[pos_x + (512*pos_y)] = color;	
                        obj_priority_buffer[pos_x + (512*pos_y)] = priority;
	                }
	            }
            }
        }
    }
}

uint32_t Ppu::determinePaletteAddress(uint8_t BG, uint8_t mode) {
    //Mode zero separates BG palettes, the other don't
    if (mode == 0)
	    return (4*4*(BG-1));
    else
	    return 0;
 }

void Ppu::determineObjectSize(uint8_t *x, uint8_t *y, bool s) {
    switch (object_size) {
        case 0:
            if (!s) {
                *x = 2;
                *y = 2;
            }
            else {
                *x = 1;
                *y = 1;
            }
            break;
        case 1:
            if (!s) {
                *x = 4;
                *y = 4;
            }
            else {
                *x = 1;
                *y = 1;
            }
            break;
        case 2:
            if (!s) {
                *x = 8;
                *y = 8;
            }
            else {
                *x = 1;
                *y = 1;
            }
            break;
        case 3:
            if (!s) {
                *x = 4;
                *y = 4;
            }
            else {
                *x = 2;
                *y = 2;
            }
            break;
        case 4:
            if (!s) {
                *x = 8;
                *y = 8;
            }
            else {
                *x = 2;
                *y = 2;
            }
            break;
        case 5:
            if (!s) {
                *x = 8;
                *y = 8;
            }
            else {
                *x = 4;
                *y = 4;
            }
            break;
        case 6:
            if (!s) {
                *x = 4;
                *y = 8;
            }
            else {
                *x = 2;
                *y = 4;
            }
            break;
        default:
            if (!s) {
                *x = 4;
                *y = 4;
            }
            else {
                *x = 2;
                *y = 4;
            }
    }
}

	    
uint32_t Ppu::convert_BGR_RGB(uint32_t bgr) {
    uint8_t R = (bgr & 0x001f) << 3;
    uint8_t G = ((bgr & 0x03e0) >> 5) << 3;
    uint8_t B = ((bgr & 0x7c00) >> 10) << 3;
    return ((R << 16) | (G << 8) | B);
}

void Ppu::renderFrame() {
    uint32_t backdrop = fixed_color;
    uint8_t top_layer = 0xf; //backdrop
    switch (BG_mode) {
        case 0:
            for (uint32_t i = 0; i<512; ++i) {
                for (uint32_t j = 0; j<512; ++j) {
                    frame_buffer[j + (i * 512)] = backdrop;
                    uint32_t color;
                    if (!BG4_priority_buffer[j + (i*512)]) {
                        color = BG4_frame_buffer[j + (i*512)];
                        if (color != -1) {
                            frame_buffer[j + (i*512)] = color;
                            top_layer = 4;
                        }
                    }
                    if (!BG3_priority_buffer[j + (i*512)]) {
                        color = BG3_frame_buffer[j + (i*512)];
                        if (color != -1) {
                            frame_buffer[j + (i*512)] = color;
                            top_layer = 3;
                        }
                    }
                    if (BG4_priority_buffer[j + (i*512)]) {
                        color = BG4_frame_buffer[j + (i*512)];
                        if (color != -1) {
                            frame_buffer[j + (i*512)] = color;
                            top_layer = 4;
                        }
                    }
                    if (BG3_priority_buffer[j + (i*512)]) {
                        color = BG3_frame_buffer[j + (i*512)];
                        if (color != -1) {
                            frame_buffer[j + (i*512)] = color;
                            top_layer = 3;
                        }
                    }
                    if (!BG2_priority_buffer[j + (i*512)]) {
                        color = BG2_frame_buffer[j + (i*512)];
                        if (color != -1) {
                            frame_buffer[j + (i*512)] = color;
                            top_layer = 2;
                        }
                    }
                    if (!BG1_priority_buffer[j + (i*512)]) {
                        color = BG1_frame_buffer[j + (i*512)];
                        if (color != -1) {
                            frame_buffer[j + (i*512)] = color;
                            top_layer = 1;
                        }
                    }
                    if (BG2_priority_buffer[j + (i*512)]) {
                        color = BG2_frame_buffer[j + (i*512)];
                        if (color != -1) {
                            frame_buffer[j + (i*512)] = color;
                            top_layer = 2;
                        }
                    }
                    if (BG1_priority_buffer[j + (i*512)]) {
                        color = BG1_frame_buffer[j + (i*512)];
                        if (color != -1) {
                            frame_buffer[j + (i*512)] = color;
                            top_layer = 1;
                        }
                    }
                }
            }
            break;
        case 1:
            for (uint32_t i = 0; i<512; ++i) {
                for (uint32_t j = 0; j<512; ++j) {
                    frame_buffer[j + (i * 512)] = backdrop;
                    uint32_t color;
                    if (!BG3_priority_buffer[j + (i*512)]) {
                        color = BG3_frame_buffer[j + (i*512)];
                        if (color != -1) {
                            frame_buffer[j + (i*512)] = color;
                            top_layer = 3;
                        }
                    }
                    if (!mode1_BG3_priority) {
                        if (BG3_priority_buffer[j + (i*512)]) {
                            color = BG3_frame_buffer[j + (i*512)];
                            if (color != -1) {
                                frame_buffer[j + (i*512)] = color;
                                top_layer = 3;
                            }
                        }
                    }
                    if (!BG2_priority_buffer[j + (i*512)]) {
                        color = BG2_frame_buffer[j + (i*512)];
                        if (color != -1) {
                            frame_buffer[j + (i*512)] = color;
                            top_layer = 2;
                        }
                    }
                    if (!BG1_priority_buffer[j + (i*512)]) {
                        color = BG1_frame_buffer[j + (i*512)];
                        if (color != -1) {
                            frame_buffer[j + (i*512)] = color;
                            top_layer = 1;
                        }
                    }
                    if (BG2_priority_buffer[j + (i*512)]) {
                        color = BG2_frame_buffer[j + (i*512)];
                        if (color != -1) {
                            frame_buffer[j + (i*512)] = color;
                            top_layer = 2;
                        }
                    }
                    if (BG1_priority_buffer[j + (i*512)]) {
                        color = BG1_frame_buffer[j + (i*512)];
                        if (color != -1) {
                            frame_buffer[j + (i*512)] = color;
                            top_layer = 1;
                        }
                    }
                    if (mode1_BG3_priority) {
                        if (BG3_priority_buffer[j + (i*512)]) {
                            color = BG3_frame_buffer[j + (i*512)];
                            if (color != -1) {
                                frame_buffer[j + (i*512)] = color;
                                top_layer = 3;
                            }
                        }
                    }
                    //if (BG1_priority_buffer[j + (i*512)]) {
                        color = obj_frame_buffer[j + (i*512)];
                        if (color != -1) {
                            frame_buffer[j + (i*512)] = color;
                            top_layer = 5;
                        }
                    //}
                    if (colorMath(top_layer, &color, backdrop)) {
                        //frame_buffer[j + (i*512)] = color;
                    }
                }
            }
            break;
        default:
            break;
    }

    
        
}

bool Ppu::colorMath(uint8_t top_layer, uint32_t *color, uint32_t sub_color) {
    bool enable = false;
    if (top_layer == 1) enable = BG1_color_math_en;
    else if (top_layer == 2) enable = BG2_color_math_en;
    else if (top_layer == 3) enable = BG3_color_math_en;
    else if (top_layer == 4) enable = BG4_color_math_en;
    
    if (enable) {
        uint8_t colorb = (*color >> 10) & 0x1f;
        uint8_t colorg = (*color >> 5) & 0x1f;
        uint8_t colorr = *color & 0x1f;
        uint8_t subb = (sub_color >> 10) & 0x1f;
        uint8_t subg = (sub_color >> 5) & 0x1f;
        uint8_t subr = sub_color & 0x1f;
        if (add_sub_color) {
            colorb -= subb;
            colorg -= subg;
            colorr -= subr;
        }
        else {
            colorb += subb;
            colorg += subg;
            colorr += subr;
        }
        if (half_color_math) {
            colorb = colorb >> 1;
            colorg = colorg >> 1;
            colorr = colorr >> 1;
        }
        *color = ((colorb & 0x1f) << 10) | ((colorg & 0x1f) << 10) | (colorr & 0x1f);
        return true;
    }
    return false;
}

void Ppu::drawScreen() {
    for (uint32_t i = 0; i<512; ++i) {
	    for (uint32_t j = 0; j<512; ++j) {
	        XSetForeground(di, gc, convert_BGR_RGB(frame_buffer[j + (i*512)]));
	        XDrawPoint(di, wi, gc, j, i);
	    }
    }
    
    
   /* 
    cout << "---- VRAM ----" << endl;
    cout << "BG1 TILEMAP DIR = " << std::hex << (unsigned) BG1_tilemap_address << endl; 
    cout << "BG2 TILEMAP DIR = " << std::hex << (unsigned) BG2_tilemap_address << endl; 
    cout << "BG3 TILEMAP DIR = " << std::hex << (unsigned) BG3_tilemap_address << endl; 
    cout << "BG4 TILEMAP DIR = " << std::hex << (unsigned) BG4_tilemap_address << endl; 
    cout << "---- VRAM ----" << endl;
    cout << "BG1 CHAR DIR = " << std::hex << (unsigned) BG1_char_address << endl; 
    cout << "BG2 CHAR DIR = " << std::hex << (unsigned) BG2_char_address << endl; 
    cout << "BG3 CHAR DIR = " << std::hex << (unsigned) BG3_char_address << endl; 
    cout << "BG4 CHAR DIR = " << std::hex << (unsigned) BG4_char_address << endl; 
     
    for (uint32_t i = 0; i<VRAM_SIZE; i += 16) {
	    cout << std::hex << (unsigned) i << " ";
	    for (int j = 0; j<16; ++j) {
	        cout << std::hex << (unsigned) vram[i+j] << " ";
	    }
	    cout << endl;
    }
    for (uint32_t i = 0; i<CG_SIZE; i += 16) {
	cout << std::hex << (unsigned) i << " ";
	for (int j = 0; j<16; ++j) {
	    cout << std::hex << (unsigned) cg[i+j] << " ";
	}
	cout << endl;
    }
    */
     
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
    name_base_select = (data & 0x07) << 13;
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
        oam[oam_address+1] = data;
        oam[oam_address] = oam_low_buffer;
        oam_h_addr = false;
        oam_address+=2;
    }
    else {
        if (oam_address >= 0x100) {
            oam[oam_address] = data;
        }
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
    BG1_tilemap_address = ((data >> 2) & 0x3f) << 10;
}

void Ppu::write_BG2SC(uint8_t data) {
    //aaaaaayx
    BG2_tilemap_x_mirror = data & 0x01;
    BG2_tilemap_y_mirror = (data >> 1) & 0x01;
    BG2_tilemap_address = ((data >> 2) & 0x3f) << 10;
}

void Ppu::write_BG3SC(uint8_t data) {
    //aaaaaayx
    BG3_tilemap_x_mirror = data & 0x01;
    BG3_tilemap_y_mirror = (data >> 1) & 0x01;
    BG3_tilemap_address = ((data >> 2) & 0x3f) << 10;
}

void Ppu::write_BG4SC(uint8_t data) {
    //aaaaaayx
    BG4_tilemap_x_mirror = data & 0x01;
    BG4_tilemap_y_mirror = (data >> 1) & 0x01;
    BG4_tilemap_address = ((data >> 2) & 0x3f) << 10;
}

void Ppu::write_BG12NBA(uint8_t data) {
    //bbbbaaaa
    BG1_char_address = (data & 0x0f) << 12;
    BG2_char_address = ((data >> 4) & 0x0f) << 12;
}

void Ppu::write_BG34NBA(uint8_t data) {
    //bbbbaaaa
    BG3_char_address = (data & 0x0f) << 12;
    BG4_char_address = ((data >> 4) & 0x0f) << 12;
}

void Ppu::write_BG1HOFS(uint8_t data) {
    //------xx xxxxxxxx write twice
    if (BG1HOFS_h) {
        BG1_hscroll = (BG1_hscroll & 0x00ff) | ((data & 0x07) << 8);
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
        BG1_vscroll = (BG1_vscroll & 0x00ff) | ((data & 0x07) << 8);
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
        BG2_hscroll = (BG2_hscroll & 0x00ff) | ((data & 0x07) << 8);
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
        BG2_vscroll = (BG2_vscroll & 0x00ff) | ((data & 0xf7) << 8);
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
        BG3_hscroll = (BG3_hscroll & 0x00ff) | ((data & 0x07) << 8);
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
        BG3_vscroll |= (BG3_vscroll & 0x00ff) | ((data & 0x07) << 8);
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
        BG4_hscroll = (BG4_hscroll & 0x00ff) | ((data & 0x07) << 8);
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
        BG4_vscroll |= (BG4_vscroll & 0x00ff) | ((data & 0x07) << 8);
        BG4VOFS_h = false;
    }
    else {
        BG4_vscroll = data;
        BG4VOFS_h = true;
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
    cg_h_write = 0;
}

void Ppu::write_CGDATA(uint8_t data) {
    //-bbbbbgg gggrrrrr
    if (cg_h_write == 1) {
        cg[cg_address] = (data<<8) | cg_low_buffer;
        cg_address++;
	    cg_h_write = 0;
    } else {
        cg_low_buffer = data;
	    cg_h_write = 1;
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
    uint8_t intensity = data & 0x1f;
    if ((data >> 7) & 1) {
        fixed_color = (fixed_color & 0x03ff) | (intensity << 10);
    }
    if ((data >> 6) & 1) {
        fixed_color = (fixed_color & 0x7c1f) | (intensity << 5);
    }
    if ((data >> 5) & 1) {
        fixed_color = (fixed_color & 0x7fe0) | intensity;
    }
}

uint8_t Ppu::read_OAMDATAREAD() {
    uint8_t result;
    if (oam_h_addr == 1) {
        result = oam[oam_address+1];
        oam_address+=2;
        oam_h_addr = 0;
    }
    else {
        result = oam[oam_address];
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
    return result;
}   

void Ppu::initBppMatrix() {
    bpp_matrix[0] = 2;
    bpp_matrix[1] = 2;
    bpp_matrix[2] = 2;
    bpp_matrix[3] = 2;
    bpp_matrix[4] = 4;
    bpp_matrix[5] = 4;
    bpp_matrix[6] = 2;
    bpp_matrix[8] = 4;
    bpp_matrix[9] = 4;
    bpp_matrix[12] = 8;
    bpp_matrix[13] = 4;
    bpp_matrix[16] = 8;
    bpp_matrix[17] = 2;
    bpp_matrix[20] = 4;
    bpp_matrix[21] = 2;
    bpp_matrix[24] = 4;
    bpp_matrix[28] = 8;
}
