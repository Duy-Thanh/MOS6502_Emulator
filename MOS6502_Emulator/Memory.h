#pragma once

#include <iostream>
#include <cstdint>

namespace MOS6502 {
	class MOS6502_Memory {
	private:
		uint8_t data[0xFFFF + 1];				// 64 KB Memory

	public:
		MOS6502_Memory();
		~MOS6502_Memory();

		uint8_t Read(uint16_t addr);
		void Write(uint16_t addr, uint8_t value);
	};
}