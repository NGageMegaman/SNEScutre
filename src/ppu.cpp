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
    int x = 0, y = 0, width = 256, height = 224, border_width = 1;
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
    BG1_sub_frame_buffer = (uint32_t *) malloc(sizeof(uint32_t) * 512 * 512);
    BG2_sub_frame_buffer = (uint32_t *) malloc(sizeof(uint32_t) * 512 * 512);
    BG3_sub_frame_buffer = (uint32_t *) malloc(sizeof(uint32_t) * 512 * 512);
    BG4_sub_frame_buffer = (uint32_t *) malloc(sizeof(uint32_t) * 512 * 512);
    obj_sub_frame_buffer = (uint32_t *) malloc(sizeof(uint32_t) * 512 * 512);
    main_frame_buffer = (uint32_t *) malloc(sizeof(uint32_t) * 512 * 512);
    sub_frame_buffer = (uint32_t *) malloc(sizeof(uint32_t) * 512 * 512);
    BG1_priority_buffer = (bool *) malloc(sizeof(bool) * 512 * 512);
    BG2_priority_buffer = (bool *) malloc(sizeof(bool) * 512 * 512);
    BG3_priority_buffer = (bool *) malloc(sizeof(bool) * 512 * 512);
    BG4_priority_buffer = (bool *) malloc(sizeof(bool) * 512 * 512);
    obj_priority_buffer = (uint8_t *) malloc(sizeof(uint8_t) * 512 * 512);
    BG1w = (bool *) malloc(sizeof(bool) * 512 * 512);
    BG2w = (bool *) malloc(sizeof(bool) * 512 * 512);
    BG3w = (bool *) malloc(sizeof(bool) * 512 * 512);
    BG4w = (bool *) malloc(sizeof(bool) * 512 * 512);
    OBJw = (bool *) malloc(sizeof(bool) * 512 * 512);
    BDmain = (bool *) malloc(sizeof(bool) * 512 * 512);
    BDsub = (bool *) malloc(sizeof(bool) * 512 * 512);
    frame_buffer = (uint32_t *) malloc(sizeof(uint32_t) * 512 * 512);
    bpp_matrix = (uint16_t *) malloc(sizeof(uint16_t) * 4 * 8);
    
    fixed_color = 0;
    initBppMatrix();

    BG1HOFS_h = 0; BG1VOFS_h = 0;
    BG2HOFS_h = 0; BG2VOFS_h = 0;
    BG3HOFS_h = 0; BG3VOFS_h = 0;
    BG4HOFS_h = 0; BG4VOFS_h = 0;
    oam_h_addr = 0;

    brightness = 0xf;
}

void Ppu::drawBGs(uint32_t scanline) {
    for (int i = 0; i<4; ++i) {
        determineWindows(scanline);
        drawBG(i+1, scanline);
    }
    if (scanline == 241) {
        drawSprites();
        renderFrame();
    }
}

void Ppu::drawBG(uint8_t BG, uint32_t scanline) {
    uint32_t *bg_frame_buffer;
    uint32_t *bg_sub_frame_buffer;
    bool *bg_priority_buffer;
    bool *bg_window;
    bool bg_main_en, bg_sub_en, bg_mainw_en, bg_subw_en;
    uint16_t hscroll, vscroll;
    bool mosaic;
    //Determine tilemap addresses
    uint32_t tilemap_address, char_address;
    bool tilemap_x_mirror, tilemap_y_mirror;
    if (BG == 1) {
        bg_frame_buffer     = BG1_frame_buffer;
        bg_sub_frame_buffer = BG1_sub_frame_buffer;
        bg_main_en          = BG1_main_en;
        bg_sub_en           = BG1_sub_en;
        bg_mainw_en         = BG1_mainw_en;
        bg_subw_en          = BG1_subw_en;
        bg_window           = BG1w;
        bg_priority_buffer  = BG1_priority_buffer;
	    tilemap_address     = BG1_tilemap_address;
	    tilemap_x_mirror    = BG1_tilemap_x_mirror;
	    tilemap_y_mirror    = BG1_tilemap_y_mirror;
	    char_address        = BG1_char_address;
        hscroll             = BG1_hscroll;
        vscroll             = BG1_vscroll;
        mosaic              = BG1_mosaic;
    }
    else if (BG == 2) {
        bg_frame_buffer     = BG2_frame_buffer;
        bg_sub_frame_buffer = BG2_sub_frame_buffer;
        bg_main_en          = BG2_main_en;
        bg_sub_en           = BG2_sub_en;
        bg_mainw_en         = BG2_mainw_en;
        bg_subw_en          = BG2_subw_en;
        bg_window           = BG2w;
        bg_priority_buffer  = BG2_priority_buffer;
	    tilemap_address     = BG2_tilemap_address;
	    tilemap_x_mirror    = BG2_tilemap_x_mirror;
	    tilemap_y_mirror    = BG2_tilemap_y_mirror;
	    char_address        = BG2_char_address;
        hscroll             = BG2_hscroll;
        vscroll             = BG2_vscroll;
        mosaic              = BG2_mosaic;
    }
    else if (BG == 3) {
        bg_frame_buffer     = BG3_frame_buffer;
        bg_sub_frame_buffer = BG3_sub_frame_buffer;
        bg_main_en          = BG3_main_en;
        bg_sub_en           = BG3_sub_en;
        bg_mainw_en         = BG3_mainw_en;
        bg_subw_en          = BG3_subw_en;
        bg_window           = BG3w;
        bg_priority_buffer  = BG3_priority_buffer;
	    tilemap_address     = BG3_tilemap_address;
	    tilemap_x_mirror    = BG3_tilemap_x_mirror;
	    tilemap_y_mirror    = BG3_tilemap_y_mirror;
	    char_address        = BG3_char_address;
        hscroll             = BG3_hscroll;
        vscroll             = BG3_vscroll;
        mosaic              = BG3_mosaic;
    }
    else if (BG == 4) {
        bg_frame_buffer     = BG4_frame_buffer;
        bg_sub_frame_buffer = BG4_sub_frame_buffer;
        bg_main_en          = BG4_main_en;
        bg_sub_en           = BG4_sub_en;
        bg_mainw_en         = BG4_mainw_en;
        bg_subw_en          = BG4_subw_en;
        bg_window           = BG4w;
        bg_priority_buffer  = BG4_priority_buffer;
	    tilemap_address     = BG4_tilemap_address;
	    tilemap_x_mirror    = BG4_tilemap_x_mirror;
	    tilemap_y_mirror    = BG4_tilemap_y_mirror;
	    char_address        = BG4_char_address;
        hscroll             = BG4_hscroll;
        vscroll             = BG4_vscroll;
        mosaic              = BG4_mosaic;
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
    uint32_t line = (scanline + vscroll)%512;
    for (uint32_t i = 0; i<33; i++) {
	    uint16_t tile;
	    uint16_t tile_number;
	    uint8_t palette;
	    bool v_flip, h_flip, priority;
        uint32_t addr_x, addr_y, addr_base;
        addr_base = 0;
        addr_x = ((i*8) + hscroll)%512;
        if (addr_x >= 256) {
            addr_x -= 256;
            if (tilemap_x_mirror) {
                addr_base += 0x400;
            }
        }
        addr_y = line;
        if (line >= 256) {
            addr_y -= 256;
            if (tilemap_y_mirror && tilemap_x_mirror) {
                addr_base += 0x800;
            }
            else if (tilemap_y_mirror) {
                addr_base += 0x400;
            }
        }
	    tile = vram[tilemap_address + addr_base + ((addr_y/8)*32) + (addr_x/8)];
	    tile_number = tile & 0x3ff;
	    palette = (tile >> 10) & 0x07;
	    priority = (tile >> 13) & 1;
	    h_flip = (tile >> 14) & 1;
	    v_flip = (tile >> 15) & 1;

	    //Determine tile position on the screen
	    //TODO: consider 16x16 tiles, which is to say divide by 16
	    uint32_t tile_x, tile_y;
	    tile_x = addr_x;
	    tile_y = addr_y;

	    uint32_t character_address = char_address + (tile_number*4*bpp);
	    uint32_t palette_address = determinePaletteAddress(BG, BG_mode);

	    //TODO:If direct color
	    //Separate in a function?
        uint8_t i2 = (line % 8);
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
            uint32_t color, sub_color;
            if (cg_index == 0) color = -1; 
	        else color = cg[palette_address + (palette*bpp*bpp) + cg_index];
	        
	        uint32_t pos_x = ((i*8) - (hscroll%8)); 
            uint32_t pos_y = (scanline - (vscroll%8));

            if (h_flip) pos_x += (7-j2);
	        else pos_x += j2;
            if (v_flip) pos_y += (7-2*((scanline%8)));
            pos_x = pos_x % 512;
            pos_y = pos_y % 512;
            
            if (color != -1)
                color = applyBrightness(color); 

            sub_color = color;

            if (mosaic) {
                //0 amount = 1x1, f amount = 16x16
                uint32_t pos_x_mosaic = pos_x - (pos_x % (mosaic_amount+1));
                uint32_t pos_y_mosaic = pos_y - (pos_y % (mosaic_amount+1));
                //Avoid for first pixel of the square
                if (pos_x % (mosaic_amount+1) != 0 || pos_y % (mosaic_amount+1) != 0) {
                    color = bg_frame_buffer[pos_x_mosaic + (512*pos_y_mosaic)];
                    sub_color = bg_sub_frame_buffer[pos_x_mosaic + (512*pos_y_mosaic)];
                }
            }

            if (bg_main_en) {
                if (bg_mainw_en && bg_window[pos_x + (512*pos_y)]) {
                    bg_frame_buffer[pos_x + (512*pos_y)] = -1;
                }
                else {
                    bg_frame_buffer[pos_x + (512*pos_y)] = color;
                }
            }
            else bg_frame_buffer[pos_x + (512*pos_y)] = -1;
            if (bg_sub_en) {
                if (bg_subw_en && bg_window[pos_x + (512*pos_y)]) {
                    bg_sub_frame_buffer[pos_x + (512*pos_y)] = -1;
                }
                else {
                    bg_sub_frame_buffer[pos_x + (512*pos_y)] = sub_color;
                }
            }
            else bg_sub_frame_buffer[pos_x + (512*pos_y)] = -1;
            bg_priority_buffer[pos_x + (512*pos_y)] = priority;
	    }
    }
}

void Ppu::determineWindows(uint32_t scanline) {
    bool b11, b12, b13, b14, bobj1, bbd1;
    bool b21, b22, b23, b24, bobj2, bbd2;
    bool b1, b2, b3, b4, bobj, bbd;
    for (uint32_t j = 0; j<256; ++j) {
        //Window 1
        b11 = b12 = b13 = b14 = bobj1 = bbd1 = ((j > w1_left) && (j < w1_right));
        //We do this in reverse, because if we disable
        //the window we don't want it to re-enable because
        //of the inverter
        if (w1invBG1) b11 = !b11; if (w1invBG2) b12 = !b12; if (w1invBG3) b13 = !b13;
        if (w1invBG4) b14 = !b14; if (w1invOBJ) bobj1 = !bobj1; if (w1invBD) bbd1 = !bbd1;
        b11 &= w1BG1en; b12 &= w1BG2en; b13 &= w1BG3en;
        b14 &= w1BG4en; bobj1 &= w1OBJen; bbd1 &= w1BDen;

        //Window 2
        b21 = b22 = b23 = b24 = bobj2 = bbd2 = (j > w2_left) && (j < w2_right);
        if (w2invBG1) b21 = !b21; if (w2invBG2) b22 = !b22; if (w2invBG3) b23 = !b23;
        if (w2invBG4) b24 = !b24; if (w2invOBJ) bobj2 = !bobj2; if (w2invBD) bbd2 = !bbd2;
        b21 &= w2BG1en; b22 &= w2BG2en; b23 &= w2BG3en;
        b24 &= w2BG4en; bobj2 &= w2OBJen; bbd2 &= w2BDen;

        b1 = b2 = b3 = b4 = bobj = bbd = 0;
        if (w1BG1en && w2BG1en) {
            if      (BG1_mask_logic == 0) b1 = b11 | b21;       else if (BG1_mask_logic == 1) b1 = b11 & b21;
            else if (BG1_mask_logic == 2) b1 = b11 ^ b21;       else if (BG1_mask_logic == 3) b1 = b11 ^ !b21;
        }
        else if (w1BG1en) b1 = b11;
        else if (w2BG1en) b1 = b21;
        if (w1BG2en && w2BG2en) {
            if      (BG2_mask_logic == 0) b2 = b12 | b22;       else if (BG2_mask_logic == 1) b2 = b12 & b22;
            else if (BG2_mask_logic == 2) b2 = b12 ^ b22;       else if (BG2_mask_logic == 3) b2 = b12 ^ !b22;
        }
        else if (w1BG2en) b2 = b12;
        else if (w2BG2en) b2 = b22;
        if (w1BG3en && w2BG3en) {
            if      (BG3_mask_logic == 0) b3 = b13 | b23;       else if (BG3_mask_logic == 1) b3 = b13 & b23;
            else if (BG3_mask_logic == 2) b3 = b13 ^ b23;       else if (BG3_mask_logic == 3) b3 = b13 ^ !b23;
        }
        else if (w1BG3en) b3 = b13;
        else if (w2BG3en) b3 = b23;
        if (w1BG4en && w2BG4en) {
            if      (BG4_mask_logic == 0) b4 = b14 | b24;       else if (BG4_mask_logic == 1) b4 = b14 & b24;
            else if (BG4_mask_logic == 2) b4 = b14 ^ b24;       else if (BG4_mask_logic == 3) b4 = b14 ^ !b24;
        }
        else if (w1BG4en) b4 = b14;
        else if (w2BG4en) b4 = b24;
        if (w1OBJen && w2OBJen) {
            if      (OBJ_mask_logic == 0) bobj = bobj1 | bobj2; else if (OBJ_mask_logic == 1) bobj = bobj1 & bobj2;
            else if (OBJ_mask_logic == 2) bobj = bobj1 ^ bobj2; else if (OBJ_mask_logic == 3) bobj = bobj1 ^ !bobj2;
        }
        else if (w1OBJen) bobj = bobj1;
        else if (w2OBJen) bobj = bobj2;
        if (w1BDen && w2BDen) {
            if      ( BD_mask_logic == 0) bbd = bbd1 | bbd2;    else if ( BD_mask_logic == 1) bbd = bbd1 & bbd2;
            else if ( BD_mask_logic == 2) bbd = bbd1 ^ bbd2;    else if ( BD_mask_logic == 3) bbd = bbd1 ^ !bbd2;
        }
        else if (w1BDen) bbd = bbd1;
        else if (w2BDen) bbd = bbd2;

        BG1w[j + (scanline*512)] = b1;
        BG2w[j + (scanline*512)] = b2;
        BG3w[j + (scanline*512)] = b3;
        BG4w[j + (scanline*512)] = b4;
        OBJw[j + (scanline*512)] = bobj;

        bool mainbbd, subbbd;

        if      (clip_colors_black == 0) mainbbd = 1;    else if (clip_colors_black == 1) mainbbd = bbd;
        else if (clip_colors_black == 3) mainbbd = !bbd; else if (clip_colors_black == 4) mainbbd = 0;
        if      (prevent_color_math == 0) subbbd = 1;    else if (prevent_color_math == 1) subbbd = bbd;
        else if (prevent_color_math == 3) subbbd = !bbd; else if (prevent_color_math == 4) subbbd = 0;
        BDmain[j + (scanline*512)] = mainbbd;
        BDsub [j + (scanline*512)] = subbbd;
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

        uint8_t highByte = oam[512 + (i / 4)];
        bool high_x = (highByte >> ((i % 4)*2)) & 1;
        bool size = (highByte >> (((i % 4)*2)+1)) & 1;
        uint16_t tile_x = (high_x << 8) | low_x;

        uint8_t bpp = 4;

        uint8_t size_x, size_y;
        determineObjectSize(&size_x, &size_y, size);

        uint32_t character_address, base_pos_x, base_pos_y;

        for (uint8_t horiz = 0; horiz < size_x; horiz++) {
            for (uint8_t vert = 0; vert < size_y; vert++) {
                uint8_t off_x = first_tile & 0x00f;
                uint8_t off_y = first_tile & 0x0f0;
                off_x = (off_x + horiz) & 0x00f;
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
                            pos_x += ((size_x-1) * 8) - (horiz * 8);
                        }
	            	    else pos_x += j2 + (horiz * 8);
                        if (v_flip) {
                            pos_y += (7-i2);
                            pos_y += ((size_y-1) * 8) - (vert * 8);
                        }
	            	    else pos_y += i2 + (vert * 8);
                        pos_x %= 512;
                        pos_y %= 512;
                        if (color != -1) {
	            	        obj_frame_buffer[pos_x + (512*pos_y)] = color;	
                        }
                        obj_priority_buffer[pos_x + (512*pos_y)] = priority;
	                }
	            }
            }
        }
    }
}

uint32_t Ppu::applyBrightness(uint32_t color) {
    uint32_t r = color & 0x1f;
    uint32_t g = (color >> 5) & 0x1f;
    uint32_t b = (color >> 10) & 0x1f;
    // 0 brightness = black
    // I should consider a new approach
    r = r & ((brightness << 1) | (brightness & 1));
    g = g & ((brightness << 1) | (brightness & 1));
    b = b & ((brightness << 1) | (brightness & 1));
    uint32_t new_color = (b << 10) | (g << 5) | r;
    return new_color;
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
            if (s) {
                *x = 2;
                *y = 2;
            }
            else {
                *x = 1;
                *y = 1;
            }
            break;
        case 1:
            if (s) {
                *x = 4;
                *y = 4;
            }
            else {
                *x = 1;
                *y = 1;
            }
            break;
        case 2:
            if (s) {
                *x = 8;
                *y = 8;
            }
            else {
                *x = 1;
                *y = 1;
            }
            break;
        case 3:
            if (s) {
                *x = 4;
                *y = 4;
            }
            else {
                *x = 2;
                *y = 2;
            }
            break;
        case 4:
            if (s) {
                *x = 8;
                *y = 8;
            }
            else {
                *x = 2;
                *y = 2;
            }
            break;
        case 5:
            if (s) {
                *x = 8;
                *y = 8;
            }
            else {
                *x = 4;
                *y = 4;
            }
            break;
        case 6:
            if (s) {
                *x = 4;
                *y = 8;
            }
            else {
                *x = 2;
                *y = 4;
            }
            break;
        default:
            if (s) {
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
    uint8_t top_layer;
    uint32_t color1, color2, color3, color4;
    uint32_t color1_s, color2_s, color3_s, color4_s;
    bool p4, p3, p2, p1;
    switch (BG_mode) {
        case 0:
            for (uint32_t i = 0; i<242; ++i) {
                for (uint32_t j = 0; j<256; ++j) {
                    main_frame_buffer[j + (i * 512)] = cg[0];
                    sub_frame_buffer[j + (i * 512)] = backdrop;
                    color1 = BG1_frame_buffer[j + (i*512)];
                    color2 = BG2_frame_buffer[j + (i*512)];
                    color3 = BG3_frame_buffer[j + (i*512)];
                    color4 = BG4_frame_buffer[j + (i*512)];
                    color1_s = BG1_sub_frame_buffer[j + (i*512)];
                    color2_s = BG2_sub_frame_buffer[j + (i*512)];
                    color3_s = BG3_sub_frame_buffer[j + (i*512)];
                    color4_s = BG4_sub_frame_buffer[j + (i*512)];
                    p1 = BG1_priority_buffer[j + (i*512)];
                    p2 = BG2_priority_buffer[j + (i*512)];
                    p3 = BG3_priority_buffer[j + (i*512)];
                    p4 = BG4_priority_buffer[j + (i*512)];
                    top_layer = 0xf;
                    if (!p4) {
                        if (color4 != -1) {
                            main_frame_buffer[j + (i*512)] = color4;
                            top_layer = 4;
                        }
                        if (color4_s != -1) sub_frame_buffer[j + (i*512)] = color4_s;
                    }
                    if (!p3) {
                        if (color3 != -1) {
                            main_frame_buffer[j + (i*512)] = color3;
                            top_layer = 3;
                        }
                        if (color3_s != -1) sub_frame_buffer[j + (i*512)] = color3_s;
                    }
                    if (p4) {
                        if (color4 != -1) {
                            main_frame_buffer[j + (i*512)] = color4;
                            top_layer = 4;
                        }
                        if (color4_s != -1) sub_frame_buffer[j + (i*512)] = color4_s;
                    }
                    if (p3) {
                        if (color3 != -1) {
                            main_frame_buffer[j + (i*512)] = color3;
                            top_layer = 3;
                        }
                        if (color3_s != -1) sub_frame_buffer[j + (i*512)] = color3_s;
                    }
                    if (!p2) {
                        if (color2 != -1) {
                            main_frame_buffer[j + (i*512)] = color2;
                            top_layer = 2;
                        }
                        if (color2_s != -1) sub_frame_buffer[j + (i*512)] = color2_s;
                    }
                    if (!p1) {
                        if (color1 != -1) {
                            main_frame_buffer[j + (i*512)] = color1;
                            top_layer = 1;
                        }
                        if (color1_s != -1) sub_frame_buffer[j + (i*512)] = color1_s;
                    }
                    if (p2) {
                        if (color2 != -1) {
                            main_frame_buffer[j + (i*512)] = color2;
                            top_layer = 2;
                        }
                        if (color2_s != -1) sub_frame_buffer[j + (i*512)] = color2_s;
                    }
                    if (p1) {
                        if (color1 != -1) {
                            main_frame_buffer[j + (i*512)] = color1;
                            top_layer = 1;
                        }
                        if (color1_s != -1) sub_frame_buffer[j + (i*512)] = color1_s;
                    }
                    if (!sw_fixed_color) sub_frame_buffer[j + (i*512)] = backdrop;
                    if (!BDsub[j + (i * 512)]) sub_frame_buffer[j + (i*512)] = backdrop;
                    if (!BDmain[j + (i * 512)]) main_frame_buffer[j + (i*512)] = cg[0];
                    colorMath(top_layer, &main_frame_buffer[j + (i*512)], sub_frame_buffer[j + (i*512)]);
                    frame_buffer[j + (i*512)] = main_frame_buffer[j + (i*512)];
                }
            }
            break;
        case 1:
            for (uint32_t i = 0; i<242; ++i) {
                for (uint32_t j = 0; j<256; ++j) {
                    color1 = BG1_frame_buffer[j + (i*512)];
                    color2 = BG2_frame_buffer[j + (i*512)];
                    color3 = BG3_frame_buffer[j + (i*512)];
                    color4 = BG4_frame_buffer[j + (i*512)];
                    color1_s = BG1_sub_frame_buffer[j + (i*512)];
                    color2_s = BG2_sub_frame_buffer[j + (i*512)];
                    color3_s = BG3_sub_frame_buffer[j + (i*512)];
                    color4_s = BG4_sub_frame_buffer[j + (i*512)];
                    p1 = BG1_priority_buffer[j + (i*512)];
                    p2 = BG2_priority_buffer[j + (i*512)];
                    p3 = BG3_priority_buffer[j + (i*512)];
                    p4 = BG4_priority_buffer[j + (i*512)];
                    main_frame_buffer[j + (i * 512)] = cg[0];
                    sub_frame_buffer[j + (i * 512)] = backdrop;
                    top_layer = 0xf;
                    if (!p3) {
                        if (color3 != -1) {
                            main_frame_buffer[j + (i*512)] = color3;
                            top_layer = 3;
                        }
                        if (color3_s != -1) sub_frame_buffer[j + (i*512)] = color3_s;
                    }
                    if (!mode1_BG3_priority) {
                        if (p3) {
                            if (color3 != -1) {
                                main_frame_buffer[j + (i*512)] = color3;
                                top_layer = 3;
                            }
                            if (color3_s != -1) sub_frame_buffer[j + (i*512)] = color3_s;
                        }
                    }
                    if (!p2) {
                        if (color2 != -1) {
                            main_frame_buffer[j + (i*512)] = color2;
                            top_layer = 2;
                        }
                        if (color2_s != -1) sub_frame_buffer[j + (i*512)] = color2_s;
                    }
                    if (!p1) {
                        if (color1 != -1) {
                            main_frame_buffer[j + (i*512)] = color1;
                            top_layer = 1;
                        }
                        if (color1_s != -1) sub_frame_buffer[j + (i*512)] = color1_s;
                    }
                    if (p2) {
                        if (color2 != -1) {
                            main_frame_buffer[j + (i*512)] = color2;
                            top_layer = 2;
                        }
                        if (color2_s != -1) sub_frame_buffer[j + (i*512)] = color2_s;
                    }
                    if (p1) {
                        if (color1 != -1) {
                            main_frame_buffer[j + (i*512)] = color1;
                            top_layer = 1;
                        }
                        if (color1_s != -1) sub_frame_buffer[j + (i*512)] = color1_s;
                    }
                    if (mode1_BG3_priority) {
                        if (p3) {
                            if (color3 != -1) {
                                main_frame_buffer[j + (i*512)] = color3;
                                top_layer = 3;
                            }
                            if (color3_s != -1) sub_frame_buffer[j + (i*512)] = color3_s;
                        }
                    }
                    //if (BG1_priority_buffer[j + (i*512)]) {
                        uint32_t color = obj_frame_buffer[j + (i*512)];
                        if (color != -1) {
                            main_frame_buffer[j + (i*512)] = color;
                            top_layer = 5;
                        }
                    //}
                    if (!sw_fixed_color) sub_frame_buffer[j + (i*512)] = backdrop;
                    if (!BDsub[j + (i * 512)]) sub_frame_buffer[j + (i*512)] = cg[0]; // WHY DOES THIS WORK???????
                    if (!BDmain[j + (i * 512)]) main_frame_buffer[j + (i*512)] = cg[0];
                    colorMath(top_layer, &main_frame_buffer[j + (i*512)], sub_frame_buffer[j + (i*512)]);
                    frame_buffer[j + (i*512)] = main_frame_buffer[j + (i*512)];
                }
            }
            break;
        default:
            break;
    }
}

void Ppu::colorMath(uint8_t top_layer, uint32_t *color, uint32_t sub_color) {
    bool enable = false;
    if (top_layer == 1) enable = BG1_color_math_en;
    else if (top_layer == 2) enable = BG2_color_math_en;
    else if (top_layer == 3) enable = BG3_color_math_en;
    else if (top_layer == 4) enable = BG4_color_math_en;
    else if (top_layer == 0xf) enable = BD_color_math_en; 

    uint8_t colorb = (*color >> 10) & 0x1f;
    uint8_t colorg = (*color >> 5) & 0x1f;
    uint8_t colorr = *color & 0x1f;
    uint8_t subb = (sub_color >> 10) & 0x1f;
    uint8_t subg = (sub_color >> 5) & 0x1f;
    uint8_t subr = sub_color & 0x1f;
    if (enable && !add_sub_color) {
        colorb += subb;
        colorg += subg;
        colorr += subr;
    }
    if (enable && add_sub_color) {
        colorb -= subb;
        colorg -= subg;
        colorr -= subr;
    }
    if (enable && half_color_math) {
        colorb = colorb >> 1;
        colorg = colorg >> 1;
        colorr = colorr >> 1;
    }
    *color = ((colorb & 0x1f) << 10) | ((colorg & 0x1f) << 5) | (colorr & 0x1f);
}

void Ppu::drawScreen() {
    for (uint32_t i = 0; i<224; ++i) {
	    for (uint32_t j = 0; j<256; ++j) {
            uint32_t color = frame_buffer[j + (i*512)];
	        XSetForeground(di, gc, convert_BGR_RGB(color));
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

//////////////////////
// WINDOW REGISTERS //
//////////////////////

void Ppu::write_W12SEL(uint8_t data) {
    //ABCDabcd
    w1invBG1 = data & 1;
    w1BG1en  = (data >> 1) & 1;
    w2invBG1 = (data >> 2) & 1;
    w2BG1en  = (data >> 3) & 1;
    w1invBG2 = (data >> 4) & 1;
    w1BG2en  = (data >> 5) & 1;
    w2invBG2 = (data >> 6) & 1;
    w2BG2en  = (data >> 7) & 1;
}

void Ppu::write_W34SEL(uint8_t data) {
    //ABCDabcd
    w1invBG3 = data & 1;
    w1BG3en  = (data >> 1) & 1;
    w2invBG3 = (data >> 2) & 1;
    w2BG3en  = (data >> 3) & 1;
    w1invBG4 = (data >> 4) & 1;
    w1BG4en  = (data >> 5) & 1;
    w2invBG4 = (data >> 6) & 1;
    w2BG4en  = (data >> 7) & 1;
}

void Ppu::write_WOBJSEL(uint8_t data) {
    //ABCDabcd
    w1invOBJ = data & 1;
    w1OBJen  = (data >> 1) & 1;
    w2invOBJ = (data >> 2) & 1;
    w2OBJen  = (data >> 3) & 1;
    w1invBD  = (data >> 4) & 1;
    w1BDen   = (data >> 5) & 1;
    w2invBD  = (data >> 6) & 1;
    w2BDen   = (data >> 7) & 1;
}

void Ppu::write_WH0(uint8_t data) {
    //xxxxxxxx
    w1_left = data;
    //cout << "WINDOW 1 LEFT " << std::hex << (unsigned) data << endl;
}

void Ppu::write_WH1(uint8_t data) {
    //xxxxxxxx
    w1_right = data;
    //cout << "WINDOW 1 RIGHT " << std::hex << (unsigned) data << endl;
}

void Ppu::write_WH2(uint8_t data) {
    //xxxxxxxx
    w2_left = data;
    //cout << "WINDOW 2 LEFT " << std::hex << (unsigned) data << endl;
}

void Ppu::write_WH3(uint8_t data) {
    //xxxxxxxx
    w2_right = data;
    //cout << "WINDOW 2 RIGHT " << std::hex << (unsigned) data << endl;
}

void Ppu::write_WBGLOG(uint8_t data) {
    //44332211
    BG1_mask_logic = data & 0x03;
    BG2_mask_logic = (data >> 2) & 0x03;
    BG3_mask_logic = (data >> 4) & 0x03;
    BG4_mask_logic = (data >> 6) & 0x03;
}

void Ppu::write_WOBJLOG(uint8_t data) {
    //----ccoo
    OBJ_mask_logic = data & 0x03;
    BD_mask_logic = (data >> 2) & 0x03;
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
    BG1_sub_en = data & 1;
    BG2_sub_en = (data >> 1) & 1;
    BG3_sub_en = (data >> 2) & 1;
    BG4_sub_en = (data >> 3) & 1;
    OBJ_sub_en = (data >> 4) & 1;
}

void Ppu::write_TMW(uint8_t data) {
    //---o4321
    BG1_mainw_en = data & 1;
    BG2_mainw_en = (data >> 1) & 1;
    BG3_mainw_en = (data >> 2) & 1;
    BG4_mainw_en = (data >> 3) & 1;
    OBJ_mainw_en = (data >> 4) & 1;
}

void Ppu::write_TSW(uint8_t data) {
    //---o4321
    BG1_subw_en = data & 1;
    BG2_subw_en = (data >> 1) & 1;
    BG3_subw_en = (data >> 2) & 1;
    BG4_subw_en = (data >> 3) & 1;
    OBJ_subw_en = (data >> 4) & 1;
}

void Ppu::write_CGWSEL(uint8_t data) {
    //ccmm--sd
    direct_color = data & 1;
    sw_fixed_color = (data >> 1) & 1;
    prevent_color_math = (data >> 4) & 0x03;
    clip_colors_black = (data >> 6) & 0x03;
}

uint8_t Ppu::read_HVBJOY() {
    //vh-----a
    return ((in_vblank << 7) | (in_hblank << 6));
}

void Ppu::write_MOSAIC(uint8_t data) {
    //xxxxDCBA
    
    BG1_mosaic = data & 1;
    BG2_mosaic = (data >> 1) & 1;
    BG3_mosaic = (data >> 2) & 1;
    BG4_mosaic = (data >> 3) & 1;
    mosaic_amount = (data >> 4) & 0xf;
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
