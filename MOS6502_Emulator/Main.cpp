#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include "CPU.h"

#include <iostream>
#include <chrono>
#include <thread>

int main(int argc, char* argv[]) {
	std::cout << "MAIN: Machine starting..." << std::endl;

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

	MOS6502::Display display;
	if (!display.Initialize()) {
		std::cout << "MAIN: FATAL: Failed to initialize display!" << std::endl;
		return -1;
	}

	MOS6502::MOS6502_Memory memory(&display);

	MOS6502::MOS6502_CPU cpu(&memory);

	std::cout << "MAIN: cpu_var_addr=0x" << std::hex << (uint32_t) & cpu << std::endl;

	bool running = true;

	std::cout << "MAIN: Machine state is set to RUNNING" << std::endl;

	while (running && cpu.IsRunning()) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
				cpu.Stop();
				break;
			}
		}

		if (running) {
			cpu.ExecuteNext();
			display.Update();
			SDL_Delay(1);
		}
	}

	// Cleanup
	SDL_Quit();
	return 0;
}