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
    Clock nes_clock;
    Ppu nes_ppu;
    Cpu nes_cpu;
    nes_cpu.clock = &nes_clock;
    nes_cpu.mem.ppu = &nes_ppu;
    while(1) {
	    nes_cpu.execute();
    }
}
