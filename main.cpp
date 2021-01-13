#include <stdint.h>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#ifndef CLOCK_H
#include "clock.h"
#define CLOCK_H
#endif
#ifndef CPU_H
#include "cpu.h"
#define CPU_H
#endif
using namespace std;

int main() {
    Clock snes_clock;
    Ppu snes_ppu;
    Cpu snes_cpu;
    snes_cpu.clock = &snes_clock;
    snes_cpu.mem.ppu = &snes_ppu;
    snes_cpu.mem.di = snes_ppu.di;
    int count = 0;
    int count2 = 0;
    while(1) {
        if ((count % 18 == 0) && (count/18 < 242)) {
            snes_ppu.drawBGs(count/18);
            snes_cpu.mem.hblank();
            snes_ppu.in_hblank = true;
        }
        if (count % 18 == 4) snes_ppu.in_hblank = false;
	    if (count > 10000) {
		    snes_ppu.drawScreen();
		    ++count2;
            snes_ppu.in_vblank = true;
		    snes_cpu.NMI_execute();
            for (int i = 0; i<50000; ++i) snes_cpu.execute();
		    count = 0;
	    }
        snes_ppu.in_vblank = false;
	    count++;
	    snes_cpu.execute();
    }
}
