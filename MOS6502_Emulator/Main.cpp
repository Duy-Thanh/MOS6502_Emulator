#include "CPU.h"

#include <iostream>
#include <chrono>
#include <thread>

int main() {
	std::cout << std::endl << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(2));

	std::cout << "--------------------------------------------------------------------------------" << std::endl;
	std::cout << "\n\n";
	std::cout << "                            MOS6502 EMULATOR                                    " << std::endl;
	std::cout << "\n\n";
	std::cout << "                     MOS6502 COMPLETE SYSTEM EMULATOR                           " << std::endl;
	std::cout << "\n\n";
	std::cout << "              COPYRIGHT (C) 2024 NEKKOCHAN. ALL RIGHT RESERVED.                 " << std::endl;
	std::cout << "\n\n";
	std::cout << "--------------------------------------------------------------------------------" << std::endl;
	std::cout << "\n\n";

	MOS6502::MOS6502_CPU cpu;

	std::cout << "MAIN: cpu_var_addr=0x" << std::hex << (uint32_t) & cpu << std::endl;

	bool running = true;

	std::cout << "MAIN: State is set to RUNNING" << std::endl;

	while (running) {
		cpu.MOS6502_CPU_Clock();

		if (cpu.MOS6502_CPU_Complete()) {
			// TODO
		}
	}

	return 0;
}