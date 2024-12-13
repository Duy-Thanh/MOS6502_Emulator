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

		this->cpu->A = 0x00;
		this->cpu->P = 0x00;
		this->cpu->PC = 0x0200;
		this->cpu->S = 0xFF;
		this->cpu->X = 0x00;
		this->cpu->Y = 0x00;
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
				delete this->cpu;

				this->cpu = new CPU();

				this->cpu->A = 0x00;
				this->cpu->P = 0x00;
				this->cpu->PC = 0x0200;
				this->cpu->S = 0xFF;
				this->cpu->X = 0x00;
				this->cpu->Y = 0x00;
			}
			catch (const std::exception& e) {
				std::cout << "EXCEPTION: " << e.what() << std::endl;
			}
		}
	}

	void MOS6502_CPU::MOS6502_CPU_SetFlags(uint8_t& status, uint8_t flag, bool condition) {
		if (condition) status |= flag;
		else status &= ~flag;

		this->cpu->P = MOS6502_CPU_STATUS_FLAGS::UNUSED;
	}

	bool MOS6502_CPU::MOS6502_CPU_GetFlags(uint8_t status, uint8_t flag) {
		return (status & flag) != 0;
	}

	uint8_t MOS6502_CPU::MOS6502_CPU_get7BitStatus(uint8_t status) {
		return status & 0x7F;				// Mask out the 8th bit (bit 7)
	}

	uint8_t MOS6502_CPU::MOS6502_CPU_MemRead(uint16_t addr) {
		return this->memory->Read(addr);
	}

	void MOS6502_CPU::MOS6502_CPU_MemWrite(uint16_t addr, uint8_t value) {
		this->memory->Write(addr, value);
	}
}