#pragma once

#include "Memory.h"
#include <cstdint>
#include <iostream>
#include <unordered_map>

typedef struct _CPU {
	uint8_t A;					// Accumulator (8-bit register)
	uint8_t P;					// Status register (7-bit register)
	uint16_t PC;				// Program counter register (16-bit register)
	uint8_t S;					// Stack pointer (8-bit)
	uint8_t X;					// Index register (8-bit register)
	uint8_t Y;					// Another index register (8-bit register)
} CPU, * PCPU;

enum MOS6502_CPU_STATUS_FLAGS {
	DEFAULT		= 0x00,			// DEFAULT_CPU_STATUS_FLAGS
	CARRY		= 0x01,			// Bit 0
	ZERO		= 0x02,			// Bit 1
	INTERRUPT	= 0x04,			// Bit 2
	DECIMAL		= 0x08,			// Bit 3
	BREAK		= 0x10,			// Bit 4
	UNUSED		= 0x20,			// Bit 5 (always 1 or ignored)
	FOVERFLOW	= 0x40,			// Bit 6
	NEGATIVE	= 0x80			// Bit 7
};

namespace MOS6502 {
	class MOS6502_CPU {
	private:
		PCPU cpu;
		MOS6502_Memory* memory;
		unsigned long cycles;
	public:
		MOS6502_CPU();
		~MOS6502_CPU();

		uint8_t MOS6502_CPU_MemRead(uint16_t addr);
		void MOS6502_CPU_MemWrite(uint16_t addr, uint8_t value);

		void MOS6502_CPU_Reset();
		void MOS6502_CPU_SetFlags(uint8_t& status, uint8_t flag, bool condition);
		bool MOS6502_CPU_GetFlags(uint8_t status, uint8_t flag);
		uint8_t MOS6502_CPU_get7BitStatus(uint8_t status);
	};
}