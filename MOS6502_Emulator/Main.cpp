#include "CPU.h"

#include <iostream>
#include <chrono>
#include <thread>

int main() {
	MOS6502::MOS6502_CPU cpu;

	bool running = true;
	while (running) {
		cpu.MOS6502_CPU_Clock();

		if (cpu.MOS6502_CPU_Complete()) {

		}
	}

	return 0;
}