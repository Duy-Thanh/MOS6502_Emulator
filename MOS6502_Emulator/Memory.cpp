#include "Memory.h"

namespace MOS6502 {
	MOS6502_Memory::MOS6502_Memory() { }

	MOS6502_Memory::~MOS6502_Memory() { }

	uint8_t MOS6502_Memory::Read(uint16_t addr) {
		std::cout << "MEM: MemRead at: 0x" << std::hex << addr << std::endl;
		return this->data[addr];
	}

	void MOS6502_Memory::Write(uint16_t addr, uint8_t value) {
		std::cout << "MEM: MemWrite at 0x" << std::hex << addr << " with data 0x" << std::hex << value << std::endl;
		this->data[addr] = value;
	}
}