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
    int count = 0;
    int count2 = 0;
    while(1) {
	    if (count > 100000) {
		snes_ppu.drawBG(3);
		snes_ppu.drawScreen();
		cout << "NMI " << count2 << endl;
		++count2;
		snes_cpu.NMI_execute();
		count = 0;
	    }
	    count++;
	    snes_cpu.execute();
    }
}
