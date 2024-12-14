#include "CPU.h"

namespace MOS6502 {
	MOS6502_CPU::MOS6502_CPU() {
		// Initialize CPU register (Important)
		std::cout << "CPU: Trying initialize CPU" << std::endl;

		if (this->cpu == nullptr) {
			std::cout << "CPU: CPU is not initialized before. Initializing CPU..." << std::endl;
			this->cpu = new CPU();
			std::cout << "CPU: CPU is initialized addr=0x" << std::hex << (uint32_t)this->cpu << " var_addr=0x" << std::hex << (uint32_t)&this->cpu << std::endl;
		}

		std::cout << "MEM: CPU is requesting initialize the memory" << std::endl;
		std::cout << "MEM: Trying initialize memory" << std::endl;

		if (this->memory == nullptr) {
			std::cout << "MEM: Memory is not initialized before. Initializing memory..." << std::endl;
			this->memory = new MOS6502_Memory();
			std::cout << "MEM: Memory is initialized addr=0x" << std::hex << (uint32_t)this->memory << " var_addr=0x" << std::hex << (uint32_t)&this->memory << std::endl;
		}

		std::cout << "CPU: CPU Reset requested" << std::endl;
		this->MOS6502_CPU_Reset();
	}

	MOS6502_CPU::~MOS6502_CPU() {
		std::cout << "CPU: Shutting down the CPU" << std::endl;
		if (this->cpu) {
			std::cout << "CPU: CPU was initialized. Continue to shutdown the CPU" << std::endl;
			try {
				std::cout << "CPU: Shutting down the CPU addr=0x" << std::hex << (uint32_t)this->cpu << " var_addr=0x" << (uint32_t)&this->cpu << std::endl;
				delete this->cpu;
				std::cout << "CPU: CPU shutdown completed!" << std::endl;
			}
			catch (const std::exception& e) {
				std::cout << "CPU: EXCEPTION: " << e.what() << std::endl;
			}
		}
	}

	void MOS6502_CPU::MOS6502_CPU_Reset() {
		std::cout << "CPU: CPU is resetting" << std::endl;
		if (this->cpu) {
			std::cout << "CPU: CPU was initialized. Continuing to reset the CPU" << std::endl;
			try {
				std::cout << "CPU: Setting up the CPU..." << std::endl;
				this->cpu->A = 0x00;
				this->cpu->P = UNUSED;
				this->cpu->S = 0xFD;
				this->cpu->X = 0x00;
				this->cpu->Y = 0x00;

				this->cpu->PC = this->MOS6502_CPU_MemReadWord(0xFFFC);

				this->cycles = 0;

				std::cout << "CPU: Setting up the CPU was successfully!" << std::endl;

			}
			catch (const std::exception& e) {
				std::cout << "CPU: EXCEPTION: " << e.what() << std::endl;
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
		std::cout << "CPU: MemRead at address 0x" << std::hex << addr << std::endl;
		return this->memory->Read(addr);
	}

	void MOS6502_CPU::MOS6502_CPU_MemWrite(uint16_t addr, uint8_t value) {
		std::cout << "CPU: MemWrite at address 0x" << std::hex << addr << " with data 0x" << std::hex << value << std::endl;
		this->memory->Write(addr, value);
	}

	uint16_t MOS6502_CPU::MOS6502_CPU_MemReadWord(uint16_t addr) {
		std::cout << "CPU: MemReadWord at addr 0x" << std::hex << addr << std::endl;

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
		std::cout << "CPU: Fetch instruction" << std::endl;
		uint8_t data = this->MOS6502_CPU_MemRead(this->cpu->PC);
		std::cout << "CPU: Fetch(): PC = 0x" << std::hex << (int)this->cpu->PC << std::endl;
		this->cpu->PC++;
		std::cout << "CPU: Fetch(): Increasing PC register with 0x1" << std::endl;
		std::cout << "CPU: Fetch(): Current PC = 0x" << std::hex << (int)this->cpu->PC << std::endl;
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
		std::cout << "CPU: GetInstructionCycles: opcode = 0x" << std::hex << (int)opcode << std::endl;
		return 2;
	}

	void MOS6502_CPU::MOS6502_CPU_ExecuteInstruction(uint8_t opcode) {
		std::cout << "CPU: ExecuteInstruction: opcode = 0x" << std::hex << (int)opcode << std::endl;
		switch (opcode) {
		default:
			std::cout << "CPU: ExecutionInstruction: Unknown opcode: 0x" << std::hex << (int)opcode << std::endl;
			break;
		}
	}

	bool MOS6502_CPU::MOS6502_CPU_Complete() {
		//std::cout << "CPU: Completed" << std::endl;
		return this->cycles_remaining == 0;
	}
}													 