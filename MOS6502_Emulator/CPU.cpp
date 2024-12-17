#include "CPU.h"

namespace MOS6502 {
	MOS6502_CPU::MOS6502_CPU() {
		// Initialize CPU register (Important)
		std::cout << "CPU: Trying initialize CPU" << std::endl;

		if (this->cpu == nullptr) {
			std::cout << "CPU: CPU is not initialized before. Initializing CPU..." << std::endl;
			this->cpu = new CPU();
			std::cout << "CPU: CPU is initialized addr=0x" << std::hex << (uint32_t)this->cpu << " va_addr=0x" << std::hex << (uint32_t)&this->cpu << std::endl;
		}

		std::cout << "MEM: CPU is requesting initialize the memory" << std::endl;
		std::cout << "MEM: Trying initialize memory" << std::endl;

		if (this->memory == nullptr) {
			std::cout << "MEM: Memory is not initialized before. Initializing memory..." << std::endl;
			this->memory = new MOS6502_Memory();
			std::cout << "MEM: Memory is initialized addr=0x" << std::hex << (uint32_t)this->memory << " va_addr=0x" << std::hex << (uint32_t)&this->memory << std::endl;
		}

		std::cout << "CPU: CPU Reset requested" << std::endl;
		this->MOS6502_CPU_Reset();
	}

	MOS6502_CPU::~MOS6502_CPU() {
		std::cout << "CPU: Shutting down the CPU" << std::endl;
		if (this->cpu) {
			std::cout << "CPU: CPU was initialized. Continue to shutdown the CPU" << std::endl;
			try {
				std::cout << "CPU: Shutting down the CPU addr=0x" << std::hex << (uint32_t)this->cpu << " va_addr=0x" << (uint32_t)&this->cpu << std::endl;
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

				std::cout << "CPU: Current registers value:" << std::endl;
				
				// PRINT DIAGNOSTICS
				std::cout << "CPU:       A (va_addr=0x" << std::hex << (uint32_t)&this->cpu->A << ") value=0x" << std::hex << (unsigned int)this->cpu->A << std::endl;
				std::cout << "CPU:       P (va_addr=0x" << std::hex << (uint32_t)&this->cpu->P << ") value=0x" << std::hex << (unsigned int)this->cpu->P << std::endl;
				std::cout << "CPU:       S (va_addr=0x" << std::hex << (uint32_t)&this->cpu->S << ") value=0x" << std::hex << (unsigned int)this->cpu->S << std::endl;
				std::cout << "CPU:       X (va_addr=0x" << std::hex << (uint32_t)&this->cpu->X << ") value=0x" << std::hex << (unsigned int)this->cpu->X << std::endl;


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
		std::cout << "CPU: Fetch(): PC = 0x" << std::hex << (unsigned int)this->cpu->PC << std::endl;
		this->cpu->PC++;
		std::cout << "CPU: Fetch(): Increasing PC register with 0x1" << std::endl;
		std::cout << "CPU: Fetch(): Current PC = 0x" << std::hex << (unsigned int)this->cpu->PC << std::endl;
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
		std::cout << "CPU: GetInstructionCycles: opcode = 0x" << std::hex << (unsigned int)opcode << std::endl;
		return 2;
	}

	void MOS6502_CPU::MOS6502_CPU_ExecuteInstruction(uint8_t opcode) {
		std::cout << "CPU: ExecuteInstruction: opcode = 0x" << std::hex << (unsigned int)opcode << std::endl;
		switch (opcode) {
		default:
			std::cout << "CPU: CPU has received unknown opcode when trying execute instruction. Jumping to FAILBACK_DEFAULT_ACTION!" << std::endl;
			std::cout << "CPU: ExecutionInstruction: FAILBACK_DEFAULT_ACTION: Unknown opcode: 0x" << std::hex << (unsigned int)opcode << std::endl;
			break;
		}
	}

	bool MOS6502_CPU::MOS6502_CPU_Complete() {
		//std::cout << "CPU: Completed" << std::endl;
		return this->cycles_remaining == 0;
	}

	// Helper function to check page boundary crossing
	bool MOS6502_CPU::MOS6502_CPU_PageBoundaryCrossed(uint16_t addr1, uint16_t addr2) {
		return (addr1 & 0xFF00) != (addr2 & 0xFF00);
	}

	// Immediate addressing: #$00
	uint16_t MOS6502_CPU::MOS6502_CPU_AddrImmediate() {
		return this->cpu->PC++;
	}

	// Zero Page addressing: $00
	uint16_t MOS6502_CPU::MOS6502_CPU_AddrZeroPage() {
		return this->MOS6502_CPU_Fetch();
	}

	// Zero Page,X addressing: $00,X
	uint16_t MOS6502_CPU::MOS6502_CPU_AddrZeroPageX() {
		uint8_t addr = this->MOS6502_CPU_Fetch();
		return (addr + this->cpu->X) & 0xFF;  // Zero-page wrap
	}

	// Zero Page,Y addressing: $00,Y
	uint16_t MOS6502_CPU::MOS6502_CPU_AddrZeroPageY() {
		uint8_t addr = this->MOS6502_CPU_Fetch();
		return (addr + this->cpu->Y) & 0xFF;  // Zero-page wrap
	}

	// Absolute addressing: $0000
	uint16_t MOS6502_CPU::MOS6502_CPU_AddrAbsolute() {
		return this->MOS6502_CPU_FetchWord();
	}

	// Absolute,X addressing: $0000,X
	uint16_t MOS6502_CPU::MOS6502_CPU_AddrAbsoluteX() {
		uint16_t base = this->MOS6502_CPU_FetchWord();
		uint16_t addr = base + this->cpu->X;

		// Some instructions add a cycle if page boundary is crossed
		if (this->MOS6502_CPU_PageBoundaryCrossed(base, addr)) {
			this->cycles_remaining++;
		}

		return addr;
	}

	// Absolute,Y addressing: $0000,Y
	uint16_t MOS6502_CPU::MOS6502_CPU_AddrAbsoluteY() {
		uint16_t base = this->MOS6502_CPU_FetchWord();
		uint16_t addr = base + this->cpu->Y;

		// Some instructions add a cycle if page boundary is crossed
		if (this->MOS6502_CPU_PageBoundaryCrossed(base, addr)) {
			this->cycles_remaining++;
		}

		return addr;
	}

	/**
	 *
// Indirect addressing: ($0000)
uint16_t MOS6502_CPU::AddrIndirect() {
    uint16_t ptr = FetchWord();
    
    // Simulate 6502 bug: if pointer is at page boundary
    // high byte is fetched from start of page rather than next page
    if ((ptr & 0xFF) == 0xFF) {
        uint8_t low = Read(ptr);
        uint8_t high = Read(ptr & 0xFF00);
        return (high << 8) | low;
    }
    
    return ReadWord(ptr);
}

// Indirect,X addressing: ($00,X)
uint16_t MOS6502_CPU::AddrIndirectX() {
    uint8_t ptr = Fetch();
    ptr += cpu.X;  // Zero-page wrap
    
    uint8_t low = Read(ptr & 0xFF);
    uint8_t high = Read((ptr + 1) & 0xFF);
    
    return (high << 8) | low;
}

// Indirect,Y addressing: ($00),Y
uint16_t MOS6502_CPU::AddrIndirectY() {
    uint8_t ptr = Fetch();
    
    uint8_t low = Read(ptr);
    uint8_t high = Read((ptr + 1) & 0xFF);
    uint16_t base = (high << 8) | low;
    uint16_t addr = base + cpu.Y;
    
    // Some instructions add a cycle if page boundary is crossed
    if (PageBoundaryCrossed(base, addr)) {
        cycles_remaining++;
    }
    
    return addr;
}

void MOS6502_CPU::UpdateZeroAndNegativeFlags(uint8_t value) {
    SetFlag(ZERO, value == 0);
    SetFlag(NEGATIVE, value & 0x80);
}

void MOS6502_CPU::LDA(uint16_t addr) {
    cpu.A = Read(addr);
    UpdateZeroAndNegativeFlags(cpu.A);
}

void MOS6502_CPU::LDX(uint16_t addr) {
    cpu.X = Read(addr);
    UpdateZeroAndNegativeFlags(cpu.X);
}

void MOS6502_CPU::LDY(uint16_t addr) {
    cpu.Y = Read(addr);
    UpdateZeroAndNegativeFlags(cpu.Y);
}

void MOS6502_CPU::STA(uint16_t addr) {
    Write(addr, cpu.A);
}

void MOS6502_CPU::STX(uint16_t addr) {
    Write(addr, cpu.X);
}

void MOS6502_CPU::STY(uint16_t addr) {
    Write(addr, cpu.Y);
}

void MOS6502_CPU::ExecuteInstruction(uint8_t opcode) {
    switch (opcode) {
        // LDA
        case 0xA9: LDA(AddrImmediate()); break;  // LDA Immediate
        case 0xA5: LDA(AddrZeroPage()); break;   // LDA Zero Page
        case 0xB5: LDA(AddrZeroPageX()); break;  // LDA Zero Page,X
        case 0xAD: LDA(AddrAbsolute()); break;   // LDA Absolute
        case 0xBD: LDA(AddrAbsoluteX()); break;  // LDA Absolute,X
        case 0xB9: LDA(AddrAbsoluteY()); break;  // LDA Absolute,Y
        case 0xA1: LDA(AddrIndirectX()); break;  // LDA (Indirect,X)
        case 0xB1: LDA(AddrIndirectY()); break;  // LDA (Indirect),Y

        // STA
        case 0x85: STA(AddrZeroPage()); break;   // STA Zero Page
        case 0x95: STA(AddrZeroPageX()); break;  // STA Zero Page,X
        case 0x8D: STA(AddrAbsolute()); break;   // STA Absolute
        case 0x9D: STA(AddrAbsoluteX()); break;  // STA Absolute,X
        case 0x99: STA(AddrAbsoluteY()); break;  // STA Absolute,Y
        case 0x81: STA(AddrIndirectX()); break;  // STA (Indirect,X)
        case 0x91: STA(AddrIndirectY()); break;  // STA (Indirect),Y

        // Add more instructions here...

        default:
            std::cout << "Unknown opcode: 0x" << std::hex << (int)opcode << std::endl;
            break;
    }
}
	 */
}													 