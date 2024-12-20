#include "Memory.h"
#include <cstring>

namespace MOS6502 {
	MOS6502_Memory::MOS6502_Memory(Display* display) : display(display) { 
		std::memset(this->data, 0, sizeof(this->data));

		// Setting up reset vector to point to our program
		std::cout << "MEM: Setting up reset vector to 0x0800" << std::endl;
		data[0xFFFC] = 0x00;  // Low byte
		data[0xFFFD] = 0x08;  // High byte
			// Our program starts at 0x0800
		uint16_t addr = 0x0800;
		
		// Simple test pattern - write white pixels to screen
		data[addr++] = 0xA2;  // LDX #$00    ; X = 0
		data[addr++] = 0x00;
		
		// Loop:
		data[addr++] = 0xA9;  // LDA #$FF    ; A = 0xFF (white)
		data[addr++] = 0xFF;
		
		data[addr++] = 0x9D;  // STA $2000,X ; Store to display memory + X
		data[addr++] = 0x00;  
		data[addr++] = 0x20;
		
		data[addr++] = 0xE8;  // INX
		data[addr++] = 0xD0;  // BNE Loop    ; Branch if X != 0
		data[addr++] = 0xF8;  // -8 (relative branch)
		std::cout << "MEM: Program loaded at 0x0800" << std::endl;
	}

	MOS6502_Memory::~MOS6502_Memory() { }

	uint8_t MOS6502_Memory::Read(uint16_t addr) {
		std::cout << "MEM: MemRead at: 0x" << std::hex << addr << std::endl;
		return this->data[addr];
	}

	void MOS6502_Memory::Write(uint16_t addr, uint8_t value) {
		std::cout << "MEM: MemWrite at 0x" << std::hex << addr 
				  << " with data 0x" << std::hex << value << std::endl;

		if (addr >= Display::DISPLAY_START && addr < Display::DISPLAY_END) {
			this->display->WritePixel(addr, value);
		}

		this->data[addr] = value;
	}
}