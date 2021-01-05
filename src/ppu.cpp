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
    XAutoRepeatOff(di);
    gc = XCreateGC(di, ro, 0, NULL);
    XMapWindow(di, wi); //Make window visible
    XStoreName(di, wi, "SNEScutre");

    oam = (uint8_t *) malloc(sizeof(uint8_t) * OAM_SIZE);
    vram = (uint8_t *) malloc(sizeof(uint8_t) * VRAM_SIZE);
    cg = (uint16_t *) malloc(sizeof(uint16_t) * CG_SIZE);
}

void Ppu::drawSprites(int n_scanline) {
    /*
    //Select what events the window will listen to
    uint8_t index, attribute, palette;
    int pos_x, pos_y;
    unsigned int patterntable, line_addr, h_offset, large_sprite;
    bool flip_h, flip_v, priority;

    //Show sprites ppumask bit
    if ((mem_ppu->PPUMASK >> 4)&1) {
	for (int i = 0; i<64; ++i) {
	    pos_y     = mem_ppu->oam[(i*4) + 0];
	    index     = mem_ppu->oam[(i*4) + 1];
	    attribute = mem_ppu->oam[(i*4) + 2];
	    pos_x     = mem_ppu->oam[(i*4) + 3];

	    if (n_scanline == pos_y) {

	    	palette = attribute & 3;
	    	priority = (attribute >> 5) & 1;
	    	flip_h = (attribute >> 6) & 1;
	    	flip_v = (attribute >> 7) & 1;

	    	large_sprite = (mem_ppu->PPUCTRL >> 5) & 1;
	    	if (large_sprite) {
	    	    patterntable = index & 1;
	    	    index = index & 0xfe;
	    	}
	    	else {
	    	    patterntable = (mem_ppu->PPUCTRL >> 3) & 1;
	    	}

	    	if (patterntable)
	    	    index += 256;

	    	if (pos_x > 8 || ((mem_ppu->PPUMASK >> 2)&1)) {
	    	    for (int i2 = 0; i2<8; ++i2) {
	    	        for (int j2 = 0; j2<8; ++j2) {
			    line_addr = (index * 16) + i2;
	    	            h_offset = (7-j2);
	    	            unsigned char bitplane =
	    	                (((mem_ppu->ram[line_addr])>>h_offset)&1) +
	    	               ((((mem_ppu->ram[line_addr+8])>>h_offset)&1)<<1);
            	            
	    	            unsigned char color = mem_ppu->ram[0x3f10 + (palette << 2) + bitplane];
	    	    	    uint8_t tmp_pos_x;
	    	    	    uint8_t tmp_pos_y;
	    	    	    if (flip_h)
	    	    	        tmp_pos_x = pos_x + 7 - j2;
	    	    	    else
	    	    	        tmp_pos_x = pos_x + j2;
	    	    	    if (flip_v)
	    	    	        tmp_pos_y = pos_y + 7 - i2;
	    	    	    else
	    	    	        tmp_pos_y = pos_y + i2;

			    int position = (tmp_pos_y*256) + tmp_pos_x;
			    if (bitplane != 0 && position < 61440) {
				if (priority == 0) {
				    mem_ppu->vram[position] = colors[color];
				}
				else if (mem_ppu->vram[position] == colors[mem_ppu->ram[0x3f00]]) {
				    mem_ppu->vram[position] = colors[color];
				}
			    }
	    	        }
	    	    }
	    	}

	    	if (large_sprite) {
	    	    index += 1;
	    	    pos_y += 16;
	    	    if (pos_x > 8 || ((mem_ppu->PPUMASK >> 2)&1)) {
	    	        for (int i2 = 0; i2<8; ++i2) {
	    		    for (int j2 = 0; j2<8; ++j2) {
				line_addr = (index * 16) + i2;
	    		        h_offset = (7-j2);
	    		        unsigned char bitplane =
	    		            (((mem_ppu->ram[line_addr])>>h_offset)&1) +
	    		           ((((mem_ppu->ram[line_addr+8])>>h_offset)&1)<<1);
            		            
	    		        unsigned char color = mem_ppu->ram[0x3f00 + (palette << 2) + 0x04 + bitplane];
			        uint8_t tmp_pos_x;
			        uint8_t tmp_pos_y;
	    	    	        if (flip_h)
				    tmp_pos_x = pos_x + 7 - j2;
	    	    	        else
				    tmp_pos_x = pos_x + j2;
	    	    	        if (flip_v)
				    tmp_pos_y = pos_y + 7 - i2;
	    	    	        else
				    tmp_pos_y = pos_y + i2;

				int position = (tmp_pos_y*256) + tmp_pos_x;

			    	if (bitplane != 0 && position < 61440) {
			    	    if (priority == 0) {
					mem_ppu->vram[position] = colors[color];
			    	    }
			    	    else if (mem_ppu->vram[position] == colors[mem_ppu->ram[0x3f00]]) {
					mem_ppu->vram[position] = colors[color];
			    	    }
			    	}
	    		    }
	    		}
	    	    }
	    	}
	    }
	}
    }
    */
}

void Ppu::draw(int n_scanline) {
    /*
    //Select what events the window will listen to
    //XEvent ev;
    unsigned int nametable;
    unsigned int idx = 0;
    unsigned int attribute_addr = 0;
    unsigned char attribute;
    unsigned int line_addr, h_offset;
    unsigned int nt_off;
    unsigned int patterntable;
    int pos_x, pos_y, sc_off_x, sc_off_y;
    //int a = XNextEvent(di, &ev);
    //if (ev.type == Expose) {
    //BG enable
    if ((mem_ppu->PPUMASK >> 3)&1) {
	if ((n_scanline & 0x7) == 0) {
	    int i = n_scanline/8;
	    //Leftmost tiles
	    if (i > 0 || ((mem_ppu->PPUMASK >> 1)&1)) {
		for (int j = 0; j<32; ++j) {
	    	    //TILE
	    	    for (int i2 = 0; i2<8; ++i2) {
			for (int j2 = 0; j2<8; ++j2) {

			    //Compute the scrolling tile offsets
			    sc_off_y = mem_ppu->PPUSCROLL & 0x00ff;
			    sc_off_x = ((mem_ppu->PPUSCROLL & 0xff00)>>8) & 0x00ff;

			    //Compute which nametable we use
			    nt_off = (mem_ppu->PPUCTRL & 3);

			    //Compute the position
			    pos_x = (j*8)+j2 - sc_off_x;
			    if (pos_x < 0) {
				pos_x += 32*8;
				nt_off = (nt_off & 2) | ((nt_off+1) & 1);
			    }
			    pos_y = (i*8)+i2 - sc_off_y;
			    if (pos_y < 0) {
				pos_y += 30*8;
				nt_off = ((nt_off+2) & 2) | (nt_off & 1);
			    }

			    nametable = 0x2000 + (nt_off * 0x0400);

			    //Compute which pattern table we use
			    patterntable = (mem_ppu->PPUCTRL >> 4) & 1;
			    //Fetch the index of the bg tile
			    idx = (mem_ppu->ram[nametable+(i*32)+j]);
			    //If we use the second pt, we add 256 to the index
			    if (patterntable) 
				idx += 256;
			    //Each pattern is 16-byte wide. i2 is the vertical offset
			    line_addr = (idx * 16) + i2;
			    //j2 is the horizontal offset. We start from the MSB
			    h_offset = (7-j2);

            	    	    //Compute the bitplane

            	    	    
            	    	    //Each tile is formed by two bitplanes. The first one
            	    	    //is followed by the second one. We fetch the line_addr byte,
            	    	    //which contains the first bitplane of a given line, and the
            	    	    //line_addr+8 byte, which contains the second one. Using
            	    	    //h_offset we get the particular bit of the line that we want
            	    	    

	    	    	    unsigned char bitplane = 
	    	    	     (((mem_ppu->ram[line_addr]  )>>h_offset)&1) |
	    	    	    ((((mem_ppu->ram[line_addr+8])>>h_offset)&1)<<1);

            	    	    //Compute the attribute address of the tile
            	    	    //TODO: this can be done outside the tile render loop
	    	    	    attribute_addr = nametable + 0x03c0;
            	    	    
            	    	    
            	    	    //Each attribute contains 4 pallette indexes corresponding to
            	    	    //a 2x2 tile portion of the screen. They are arranged, from
            	    	    //MSB to LSB as such:

            	    	    //bottom right, bottom left, top right, top left
            	    	    //     7,6           5,4        3,2       1,0

            	    	    //That means that each pattern contains the pallette of
            	    	    //two different rows and two different columns. If the
            	    	    //row is odd, we shift the attribute 4 bits right, and if the
            	    	    //column is odd, we shift the attribute 2 bits right. This
            	    	    //way we get the correct pallette index on the lower 2 bits.
            	    	    

	    	    	    //Determine the attribute address
	    	            attribute_addr = attribute_addr + (i/4)*8;
	    	            attribute_addr = attribute_addr + (j/4);
	    	    	    //Fetch the attribute byte
	    	            attribute = mem_ppu->ram[attribute_addr];
	    	    	    //Arrange the lower 2 bits with the correct pallette index
	    	            if ((i & 3) > 1) {
	    	                attribute = attribute >> 4;
	    	            }
	    	            if ((j & 3) > 1) {
	    	                attribute = attribute >> 2;
	    	            }
	    	    	    //We compute the final pallette index with the bitplane value
	    	            attribute = ((attribute << 2) | bitplane) & 0x000f;
			    if (bitplane == 0) attribute = 0;
	    	    	    //We fetch the color value
	    	            unsigned char color = mem_ppu->ram[0x3f00 + attribute];
			    int position = (pos_y*256)+pos_x;
			    if (position < 61440)
				mem_ppu->vram[position] = colors[color];
			}
		    }
	    	}
	    }
	}
    }
    */
}

void Ppu::drawScreen() {
    for (uint32_t i = 0; i<VRAM_SIZE; ++i) {
	    XSetForeground(di, gc, vram[i]);
	    XDrawPoint(di, wi, gc, (i%256), (i/256));
    }
}

void Ppu::vblank() {
    /*
    //Set the vblank bit
    mem_ppu->PPUSTATUS = mem_ppu->PPUSTATUS | 0x80;
    mem_ppu->in_vblank = true;
    if ((mem_ppu->PPUCTRL >> 7) & 1)
	cpu->NMI_execute();
    */
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
