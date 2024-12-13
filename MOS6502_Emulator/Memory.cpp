#include "Memory.h"

namespace MOS6502 {
	MOS6502_Memory::MOS6502_Memory() { }

	MOS6502_Memory::~MOS6502_Memory() { }

	uint8_t MOS6502_Memory::Read(uint16_t addr) {
		return this->data[addr];
	}

	void MOS6502_Memory::Write(uint16_t addr, uint8_t value) {
		this->data[addr] = value;
	}
}