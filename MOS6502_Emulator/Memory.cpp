#include "Memory.h"
#include <cstring>

namespace MOS6502 {
	MOS6502_Memory::MOS6502_Memory(Display* display) : display(display) { 
		std::memset(this->data, 0, sizeof(this->data));

		// Setting up reset vector
		data[0xFFFC] = 0x00;
		data[0xFFFD] = 0x08;

		uint16_t addr = 0x0800; // Test pattern

		// Initialize X and Y
		data[addr++] = 0xA2;  // LDX #$00
		data[addr++] = 0x00;
		data[addr++] = 0xA0;  // LDY #$00
		data[addr++] = 0x00;

		// Main loop start
		data[addr++] = 0x98;  // TYA (Transfer Y to A)
		data[addr++] = 0x9D;  // STA $2000,X (Store A at $2000+X)
		data[addr++] = 0x00;
		data[addr++] = 0x20;

		// Increment X
		data[addr++] = 0xE8;  // INX

		// Increment Y
		data[addr++] = 0xC8;  // INY

		// Loop if X != 0
		data[addr++] = 0xD0;  // BNE (-10)
		data[addr++] = 0xF6;  // Jump back 10 bytes

		// Infinite loop at the end
		data[addr++] = 0x4C;  // JMP $0800
		data[addr++] = 0x00;
		data[addr++] = 0x08;
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