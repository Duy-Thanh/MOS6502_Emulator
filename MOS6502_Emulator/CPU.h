#pragma once

#include "Memory.h"
#include <cstdint>
#include <iostream>
#include <unordered_map>

#define SET_FLAG(P, F)			((P) |= (F))
#define CLEAR_FLAG(P, F)		((P) &= ~(F))
#define GET_FLAG(P, F)			((P) & (F))

typedef struct _CPU {
	uint8_t A;					// Accumulator (8-bit register)
	uint8_t P;					// Status register (7-bit register)
	uint16_t PC;				// Program counter register (16-bit register)
	uint8_t S;					// Stack pointer (8-bit)
	uint8_t X;					// Index register (8-bit register)
	uint8_t Y;					// Another index register (8-bit register)
} CPU, * PCPU;

//enum MOS6502_CPU_STATUS_FLAGS {
//	DEFAULT		= 0x00,			// DEFAULT_CPU_STATUS_FLAGS
//	CARRY		= 0x01,			// Bit 0
//	ZERO		= 0x02,			// Bit 1
//	INTERRUPT	= 0x04,			// Bit 2
//	DECIMAL		= 0x08,			// Bit 3
//	BREAK		= 0x10,			// Bit 4
//	UNUSED		= 0x20,			// Bit 5 (always 1 or ignored)
//	FOVERFLOW	= 0x40,			// Bit 6
//	NEGATIVE	= 0x80			// Bit 7
//};

enum MOS6502_CPU_STATUS_FLAGS {
	DEFAULT		= (0 << 0),			// DEFAULT_CPU_STATUS_FLAGS
	CARRY		= (1 << 0),			// Bit 0
	ZERO		= (1 << 1),			// Bit 1
	INTERRUPT	= (1 << 2),			// Bit 2
	DECIMAL		= (1 << 3),			// Bit 3
	BREAK		= (1 << 4),			// Bit 4
	UNUSED		= (1 << 5),			// Bit 5 (always 1 or ignored)
	FOVERFLOW	= (1 << 6),			// Bit 6
	NEGATIVE	= (1 << 7)			// Bit 7
};

namespace MOS6502 {
	class MOS6502_CPU {
	private:
		PCPU cpu;
		MOS6502_Memory* memory;
		unsigned long cycles;

		uint8_t opcode = 0x00;										// Current instruction opcodes
		uint8_t cycles_remaining = 0;
		bool completed = true;

		uint8_t MOS6502_CPU_Fetch();
		uint16_t MOS6502_CPU_FetchWord();
		void MOS6502_CPU_Push(uint8_t value);
		uint8_t MOS6502_CPU_Pop();

		// Addressing modes
		uint16_t MOS6502_CPU_AddrImmediate();    // #$00
		uint16_t MOS6502_CPU_AddrZeroPage();     // $00
		uint16_t MOS6502_CPU_AddrZeroPageX();    // $00,X
		uint16_t MOS6502_CPU_AddrZeroPageY();    // $00,Y
		uint16_t MOS6502_CPU_AddrAbsolute();     // $0000
		uint16_t MOS6502_CPU_AddrAbsoluteX();    // $0000,X
		uint16_t MOS6502_CPU_AddrAbsoluteY();    // $0000,Y
		uint16_t MOS6502_CPU_AddrIndirect();     // ($0000)
		uint16_t MOS6502_CPU_AddrIndirectX();    // ($00,X)
		uint16_t MOS6502_CPU_AddrIndirectY();    // ($00),Y

		// Instructions
		void MOS6502_CPU_LDA(uint16_t addr);  // Load Accumulator
		void MOS6502_CPU_LDX(uint16_t addr);  // Load X Register
		void MOS6502_CPU_LDY(uint16_t addr);  // Load Y Register
		void MOS6502_CPU_STA(uint16_t addr);  // Store Accumulator
		void MOS6502_CPU_STX(uint16_t addr);  // Store X Register
		void MOS6502_CPU_STY(uint16_t addr);  // Store Y Register
		void MOS6502_CPU_NOP();				  // NOP Instruction
		void MOS6502_CPU_TAX();				  // TAX
		void MOS6502_CPU_TAY();				  // TAY
		void MOS6502_CPU_TXA();               // TXA
		void MOS6502_CPU_TYA();               // TYA
		void MOS6502_CPU_INX();               // INX
		void MOS6502_CPU_INY();	              // INY
		void MOS6502_CPU_DEX();               // DEX
		void MOS6502_CPU_DEY();               // DEY
		void MOS6502_CPU_BNE(int8_t offset);  // BNE $<int8_t>
		void MOS6502_CPU_JMP(uint16_t addr);  // JMP $<addr>

		// Add cycle counting for page boundary crosses
		bool MOS6502_CPU_PageBoundaryCrossed(uint16_t addr1, uint16_t addr2);

		// Helper for setting Zero and Negative flags
		void MOS6502_CPU_UpdateZeroAndNegativeFlags(uint8_t value);
	public:
		MOS6502_CPU(MOS6502_Memory* memory);
		~MOS6502_CPU();

		// Helper functions
		void MOS6502_CPU_DEV_PrintDiagnostics();

		// Memory operations
		uint8_t MOS6502_CPU_MemRead(uint16_t addr);
		void MOS6502_CPU_MemWrite(uint16_t addr, uint8_t value);
		uint16_t MOS6502_CPU_MemReadWord(uint16_t addr);
		void MOS6502_CPU_MemWriteWord(uint16_t addr, uint16_t value);

		// Core functions
		void MOS6502_CPU_Reset();
		void MOS6502_CPU_Clock();
		bool MOS6502_CPU_Complete();

		uint8_t MOS6502_CPU_GetInstructionCycles(uint8_t opcode);
		void MOS6502_CPU_ExecuteInstruction(uint8_t opcode);

		// Stack operations
		void MOS6502_CPU_PushWord(uint16_t value);
		uint16_t MOS6502_CPU_PopWord();

		// Status flag operations
		void MOS6502_CPU_SetFlags(MOS6502_CPU_STATUS_FLAGS flag, bool value);
		bool MOS6502_CPU_GetFlags(MOS6502_CPU_STATUS_FLAGS flag);
		uint8_t MOS6502_CPU_get7BitStatus(uint8_t status);
	};
}