#include "CPU.h"

namespace MOS6502 {
	MOS6502_CPU::MOS6502_CPU() {
		// Initialize CPU register (Important)
		if (this->cpu == nullptr) {
			this->cpu = new CPU();
		}

		if (this->memory == nullptr) {
			this->memory = new MOS6502_Memory();
		}

		this->MOS6502_CPU_Reset();
	}

	MOS6502_CPU::~MOS6502_CPU() {
		if (this->cpu) {
			try {
				delete this->cpu;
			}
			catch (const std::exception& e) {
				std::cout << "EXCEPTION: " << e.what() << std::endl;
			}
		}
	}

	void MOS6502_CPU::MOS6502_CPU_Reset() {
		if (this->cpu) {
			try {
				this->cpu->A = 0x00;
				this->cpu->P = UNUSED;
				this->cpu->S = 0xFD;
				this->cpu->X = 0x00;
				this->cpu->Y = 0x00;

				this->cpu->PC = this->MOS6502_CPU_MemReadWord(0xFFFC);

				this->cycles = 0;
			}
			catch (const std::exception& e) {
				std::cout << "EXCEPTION: " << e.what() << std::endl;
			}
		}
	}

	// Status flag operations
	void MOS6502_CPU::MOS6502_CPU_SetFlags(MOS6502_CPU_STATUS_FLAGS flag, bool value) {
		if (value)
			SET_FLAG(this->cpu->P, flag);
		else
			CLEAR_FLAG(this->cpu->P, flag);
	}

	bool MOS6502_CPU::MOS6502_CPU_GetFlags(MOS6502_CPU_STATUS_FLAGS flag) {
		return GET_FLAG(this->cpu->P, flag);
	}

	uint8_t MOS6502_CPU::MOS6502_CPU_get7BitStatus(uint8_t status) {
		return status & 0x7F;				// Mask out the 8th bit (bit 7)
	}

	// Memory operations
	uint8_t MOS6502_CPU::MOS6502_CPU_MemRead(uint16_t addr) {
		return this->memory->Read(addr);
	}

	void MOS6502_CPU::MOS6502_CPU_MemWrite(uint16_t addr, uint8_t value) {
		this->memory->Write(addr, value);
	}

	uint16_t MOS6502_CPU::MOS6502_CPU_MemReadWord(uint16_t addr) {
		uint16_t low = this->MOS6502_CPU_MemRead(addr);
		uint16_t high = this->MOS6502_CPU_MemRead(addr + 1);

		return (high << 8) | low;
	}

	void MOS6502_CPU::MOS6502_CPU_MemWriteWord(uint16_t addr, uint16_t value) {
		this->MOS6502_CPU_MemWrite(addr, value & 0xFF);
		this->MOS6502_CPU_MemWrite(addr + 1, (value >> 8) & 0xFF);
	}

	// Private functions
	void MOS6502_CPU::MOS6502_CPU_Push(uint8_t value) {
		this->MOS6502_CPU_MemWrite(0x0100 + this->cpu->S, value);
		this->cpu->S--;
	}

	uint8_t MOS6502_CPU::MOS6502_CPU_Pop() {
		this->cpu->S++;
		return this->MOS6502_CPU_MemRead(0x0100 + this->cpu->S);
	}

	void MOS6502_CPU::MOS6502_CPU_PushWord(uint16_t value) {
		this->MOS6502_CPU_Push((value >> 8) & 0xFF);
		this->MOS6502_CPU_Push(value & 0xFF);
	}

	uint16_t MOS6502_CPU::MOS6502_CPU_PopWord() {
		uint16_t low = this->MOS6502_CPU_Pop();
		uint16_t high = this->MOS6502_CPU_Pop();

		return (high << 8) | low;
	}

	uint8_t MOS6502_CPU::MOS6502_CPU_Fetch() {
		uint8_t data = this->MOS6502_CPU_MemRead(this->cpu->PC);
		this->cpu->PC++;
		return data;
	}

	uint16_t MOS6502_CPU::MOS6502_CPU_FetchWord() {
		uint16_t word = this->MOS6502_CPU_MemReadWord(this->cpu->PC);
		this->cpu->PC += 2;
		return word;
	}

	// Core functions
	void MOS6502_CPU::MOS6502_CPU_Clock() {
		if (this->cycles_remaining == 0) {
			this->opcode = this->MOS6502_CPU_Fetch();

			this->cycles_remaining = this->MOS6502_CPU_GetInstructionCycles(this->opcode);

			this->MOS6502_CPU_ExecuteInstruction(this->opcode);
		}
	}

	uint8_t MOS6502_CPU::MOS6502_CPU_GetInstructionCycles(uint8_t opcode) {
		return 2;
	}

	void MOS6502_CPU::MOS6502_CPU_ExecuteInstruction(uint8_t opcode) {
		switch (opcode) {
		default:
			std::cout << "Unknown opcode: 0x" << std::hex << (int)opcode << std::endl;
			break;
		}
	}

	bool MOS6502_CPU::MOS6502_CPU_Complete() {
		return this->cycles_remaining == 0;
	}
}													 