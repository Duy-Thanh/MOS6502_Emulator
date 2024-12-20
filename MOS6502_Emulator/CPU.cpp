#include "CPU.h"

namespace MOS6502 {
	MOS6502_CPU::MOS6502_CPU(MOS6502_Memory* memory) : memory(memory) {
		// Initialize CPU register (Important)
		std::cout << "CPU: Trying initialize CPU" << std::endl;

		if (this->cpu == nullptr) {
			std::cout << "CPU: CPU is not initialized before. Initializing CPU..." << std::endl;
			this->cpu = new CPU();
			std::cout << "CPU: CPU is initialized addr=0x" 
				      << std::hex << (uint32_t)this->cpu 
					  << " va_addr=0x" << std::hex 
				      << (uint32_t)&this->cpu << std::endl;
		}

		std::cout << "MEM: CPU is requesting initialize the memory" << std::endl;
		std::cout << "MEM: Trying initialize memory" << std::endl;

		std::cout << "CPU: CPU Reset requested" << std::endl;
		this->MOS6502_CPU_Reset();
		std::cout << "CPU: CPU initialized" << std::endl;
	}

	MOS6502_CPU::~MOS6502_CPU() {
		std::cout << "CPU: Shutting down the CPU" << std::endl;
		if (this->cpu) {
			std::cout << "CPU: CPU was initialized. Continue to shutdown the CPU" << std::endl;
			try {
				std::cout << "CPU: Shutting down the CPU addr=0x" 
						  << std::hex << (uint32_t)this->cpu 
						  << " va_addr=0x" << (uint32_t)&this->cpu << std::endl;
				try {
					delete this->cpu;
				}
				catch (const std::exception& e) {
					std::cout << "CPU: EXCEPTION: " << e.what() << std::endl;
					return;
				}
				std::cout << "CPU: CPU shutdown completed!" << std::endl;
			}
			catch (const std::exception& e) {
				std::cout << "CPU: EXCEPTION: " << e.what() << std::endl;
			}
		}

		std::cout << "CPU: CPU shutdown completed" << std::endl;
	}

	// Implementation for developer functions
	void MOS6502_CPU::MOS6502_CPU_DEV_PrintDiagnostics() {
		std::cout << "CPU: " << std::endl;
		std::cout << "CPU: ===================================================================" << std::endl;
		std::cout << "CPU: " << std::endl;
		std::cout << "CPU: CPU model: MOS Technology MOS6500-Family (MOS6502)" << std::endl;
		std::cout << "CPU: CPU is emulating through host's CPU" << std::endl;

		// PRINT
		std::cout << "CPU: Current registers value:" << std::endl;

		// PRINT DIAGNOSTICS
		std::cout << "CPU:       A  (va_addr=0x" << std::hex << (uint32_t)&this->cpu->A << ") value=0x" << std::hex << (unsigned int)this->cpu->A << std::endl;
		std::cout << "CPU:       P  (va_addr=0x" << std::hex << (uint32_t)&this->cpu->P << ") value=0x" << std::hex << (unsigned int)this->cpu->P << std::endl;
		std::cout << "CPU:       S  (va_addr=0x" << std::hex << (uint32_t)&this->cpu->S << ") value=0x" << std::hex << (unsigned int)this->cpu->S << std::endl;
		std::cout << "CPU:       X  (va_addr=0x" << std::hex << (uint32_t)&this->cpu->X << ") value=0x" << std::hex << (unsigned int)this->cpu->X << std::endl;
		std::cout << "CPU:       Y  (va_addr=0x" << std::hex << (uint32_t)&this->cpu->Y << ") value=0x" << std::hex << (unsigned int)this->cpu->Y << std::endl;
		std::cout << "CPU:       PC (va_addr=0x" << std::hex << (uint32_t)&this->cpu->PC << ") value=0x" << std::hex << (unsigned int)this->cpu->PC << std::endl;
		std::cout << "CPU: Cycles (va_addr=0x" << std::hex << (uint32_t)&this->cycles << ") value=0x" << std::hex << (unsigned long)this->cycles << std::endl;
		std::cout << "CPU: Remaining Cycles (va_addr=0x" << std::hex << (uint32_t)&this->cycles_remaining << ") value=0x" << std::hex << (unsigned int)this->cycles_remaining << std::endl;
		std::cout << "CPU: " << std::endl;
		std::cout << "CPU: ===================================================================" << std::endl;
		std::cout << "CPU: " << std::endl;
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

				// Jumping to RESET vector
				this->cpu->PC = this->MOS6502_CPU_MemReadWord(0xFFFC);

				this->cycles = 0;

				// PRINT DIAGNOSTICS
				this->MOS6502_CPU_DEV_PrintDiagnostics();

				std::cout << "CPU: Setting up the CPU was successfully!" << std::endl;
			}
			catch (const std::exception& e) {
				std::cout << "CPU: EXCEPTION: " << e.what() << std::endl;
			}
		}
	}

	// Status flag operations
	void MOS6502_CPU::MOS6502_CPU_SetFlags(MOS6502_CPU_STATUS_FLAGS flag, bool value) {
		std::cout << "CPU: SetFlag flag=0x" 
				  << (unsigned int)flag << ", value=" 
				  << (value ? (unsigned int)0x0001 : (unsigned int)0x0000) 
				  << std::endl;

		if (value) {
			std::cout << "CPU: value=0x1, setting flag now..." << std::endl;
			SET_FLAG(this->cpu->P, flag);
			std::cout << "CPU: Flag has been set. Current P register va_addr=0x" 
					  << std::hex << (uint32_t)&this->cpu->P 
					  << ", value=0x" << std::hex 
					  << (unsigned int)this->cpu->P << std::endl;
		}
		else {
			CLEAR_FLAG(this->cpu->P, flag);
		}
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

			this->
				cycles_remaining = this->MOS6502_CPU_GetInstructionCycles(this->opcode);

			this->MOS6502_CPU_ExecuteInstruction(this->opcode);
		}
	}

	uint8_t MOS6502_CPU::MOS6502_CPU_GetInstructionCycles(uint8_t opcode) {
		std::cout << "CPU: GetInstructionCycles: opcode = 0x" << std::hex << (unsigned int)opcode << std::endl;
		return 2;
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

	// Indirect addressing: ($0000)
	uint16_t MOS6502_CPU::MOS6502_CPU_AddrIndirect() {
		uint16_t ptr = this->MOS6502_CPU_FetchWord();

		// Simulate 6502 bug: if pointer is at page boundary
		// high byte is fetched from start of page rather than next page
		if ((ptr & 0xFF) == 0xFF) {
			uint8_t low = this->MOS6502_CPU_MemRead(ptr);
			uint8_t high = this->MOS6502_CPU_MemRead(ptr & 0xFF00);
			return (high << 8) | low;
		}

		return this->MOS6502_CPU_MemReadWord(ptr);
	}

	// Indirect,X addressing: ($00,X)
	uint16_t MOS6502_CPU::MOS6502_CPU_AddrIndirectX() {
		uint8_t ptr = this->MOS6502_CPU_Fetch();
		ptr += this->cpu->X;  // Zero-page wrap

		uint8_t low = this->MOS6502_CPU_MemRead(ptr & 0xFF);
		uint8_t high = this->MOS6502_CPU_MemRead((ptr + 1) & 0xFF);

		return (high << 8) | low;
	}

	// Indirect,Y addressing: ($00),Y
	uint16_t MOS6502_CPU::MOS6502_CPU_AddrIndirectY() {
		uint8_t ptr = this->MOS6502_CPU_Fetch();

		uint8_t low = this->MOS6502_CPU_MemRead(ptr);
		uint8_t high = this->MOS6502_CPU_MemRead((ptr + 1) & 0xFF);
		uint16_t base = (high << 8) | low;
		uint16_t addr = base + this->cpu->Y;

		// Some instructions add a cycle if page boundary is crossed
		if (this->MOS6502_CPU_PageBoundaryCrossed(base, addr)) {
			this->cycles_remaining++;
		}

		return addr;
	}

	void MOS6502_CPU::MOS6502_CPU_UpdateZeroAndNegativeFlags(uint8_t value) {
		this->MOS6502_CPU_SetFlags(ZERO, value == 0);
		this->MOS6502_CPU_SetFlags(NEGATIVE, value & 0x80);
	}

	///
	/// =======================================================================================================================
	///
	/// MOS6502 CPU INSTRUCTIONS EMULATE IMPLEMENTATION
	/// 
	/// FOLLOW INSTRUCTION SET - ALPHABETIC SEQUENCE FROM mos_6500_mpu_preliminary_may_1976.pdf
	/// 
	/// DATASET CAN BE FOUND AT: 
	/// https://web.archive.org/web/20221029042234if_/http://archive.6502.org/datasheets/mos_6500_mpu_preliminary_may_1976.pdf
	/// 
	/// =======================================================================================================================
	///
	
	// NOP
	void MOS6502_CPU::MOS6502_CPU_NOP() {
		// NOP instruction
		// Do nothing. We can print something here
		std::cout << "CPU: NOP Instruction" << std::endl;
	}

	// LDA: Load Accumulator with Memory
	void MOS6502_CPU::MOS6502_CPU_LDA(uint16_t addr) {
		// LDA instruction
		// Load Accumulator with Memory
		this->cpu->A = this->MOS6502_CPU_MemRead(addr);
		this->MOS6502_CPU_UpdateZeroAndNegativeFlags(this->cpu->A);
	}

	// LDX: Load Index X with Memory
	void MOS6502_CPU::MOS6502_CPU_LDX(uint16_t addr) {
		// LDX instruction
		// Load Index X with Memory
		this->cpu->X = this->MOS6502_CPU_MemRead(addr);
		this->MOS6502_CPU_UpdateZeroAndNegativeFlags(this->cpu->X);
	}

	// LDY
	void MOS6502_CPU::MOS6502_CPU_LDY(uint16_t addr) {
		this->cpu->Y = this->MOS6502_CPU_MemRead(addr);
		this->MOS6502_CPU_UpdateZeroAndNegativeFlags(this->cpu->Y);
	}

	// STA
	void MOS6502_CPU::MOS6502_CPU_STA(uint16_t addr) {
		this->MOS6502_CPU_MemWrite(addr, this->cpu->A);
	}

	// STX
	void MOS6502_CPU::MOS6502_CPU_STX(uint16_t addr) {
		this->MOS6502_CPU_MemWrite(addr, this->cpu->X);
	}

	// STY
	void MOS6502_CPU::MOS6502_CPU_STY(uint16_t addr) {
		this->MOS6502_CPU_MemWrite(addr, this->cpu->Y);
	}

	// TAX
	void MOS6502_CPU::MOS6502_CPU_TAX() {
		this->cpu->X = this->cpu->A;
		this->MOS6502_CPU_UpdateZeroAndNegativeFlags(this->cpu->X);
	}

	// TAY
	void MOS6502_CPU::MOS6502_CPU_TAY() {
		this->cpu->Y = this->cpu->A;
		this->MOS6502_CPU_UpdateZeroAndNegativeFlags(this->cpu->Y);
	}

	// TXA
	void MOS6502_CPU::MOS6502_CPU_TXA() {
		this->cpu->A = this->cpu->X;
		this->MOS6502_CPU_UpdateZeroAndNegativeFlags(this->cpu->A);
	}

	// TYA
	void MOS6502_CPU::MOS6502_CPU_TYA() {
		this->cpu->A = this->cpu->Y;
		this->MOS6502_CPU_UpdateZeroAndNegativeFlags(this->cpu->A);
	}

	// INX
	void MOS6502_CPU::MOS6502_CPU_INX() {
		this->cpu->X++;
		this->MOS6502_CPU_UpdateZeroAndNegativeFlags(this->cpu->X);
	}

	// INY
	void MOS6502_CPU::MOS6502_CPU_INY() {
		this->cpu->Y++;
		this->MOS6502_CPU_UpdateZeroAndNegativeFlags(this->cpu->Y);
	}

	// DEX
	void MOS6502_CPU::MOS6502_CPU_DEX() {
		this->cpu->X--;
		this->MOS6502_CPU_UpdateZeroAndNegativeFlags(this->cpu->X);
	}

	// DEY
	void MOS6502_CPU::MOS6502_CPU_DEY() {
		this->cpu->Y--;
		this->MOS6502_CPU_UpdateZeroAndNegativeFlags(this->cpu->Y);
	}

	//
	// Branch Instructions
	//
	void MOS6502_CPU::MOS6502_CPU_BNE(int8_t offset) {
		if (!this->MOS6502_CPU_GetFlags(ZERO)) {
			uint16_t old_pc = this->cpu->PC;
			this->cpu->PC += offset;

			// Add cycle when it down
			this->cycles_remaining++;

			// Add cycle if page boundary crossed
			if (this->MOS6502_CPU_PageBoundaryCrossed(old_pc, this->cpu->PC)) {
				this->cycles_remaining++;
			}
		}
	}

	// JMP
	void MOS6502_CPU::MOS6502_CPU_JMP(uint16_t addr) {
		this->cpu->PC = addr;
	}

	//
	// Execute Instruction module (contain: Decode Instruction and Execute Instruction)
	//
	void MOS6502_CPU::MOS6502_CPU_ExecuteInstruction(uint8_t opcode) {
		std::cout << "CPU: ExecuteInstruction: opcode = 0x" << std::hex << (unsigned int)opcode << std::endl;
		switch (opcode) {
			// NOP
			case 0xEA: this->MOS6502_CPU_NOP(); break;

			// LDA
			case 0xA9: this->MOS6502_CPU_LDA(this->MOS6502_CPU_AddrImmediate()); break;  // LDA Immediate
			case 0xA5: this->MOS6502_CPU_LDA(this->MOS6502_CPU_AddrZeroPage()); break;   // LDA Zero Page
			case 0xB5: this->MOS6502_CPU_LDA(this->MOS6502_CPU_AddrZeroPageX()); break;  // LDA Zero Page,X
			case 0xAD: this->MOS6502_CPU_LDA(this->MOS6502_CPU_AddrAbsolute()); break;   // LDA Absolute
			case 0xBD: this->MOS6502_CPU_LDA(this->MOS6502_CPU_AddrAbsoluteX()); break;  // LDA Absolute,X
			case 0xB9: this->MOS6502_CPU_LDA(this->MOS6502_CPU_AddrAbsoluteY()); break;  // LDA Absolute,Y
			case 0xA1: this->MOS6502_CPU_LDA(this->MOS6502_CPU_AddrIndirectX()); break;  // LDA (Indirect,X)
			case 0xB1: this->MOS6502_CPU_LDA(this->MOS6502_CPU_AddrIndirectY()); break;  // LDA (Indirect),Y

			case 0xA2: this->MOS6502_CPU_LDX(this->MOS6502_CPU_AddrImmediate()); break;  // LDX #$nn
			case 0xA6: this->MOS6502_CPU_LDX(this->MOS6502_CPU_AddrZeroPage()); break;   // LDX $nn
			case 0xB6: this->MOS6502_CPU_LDX(this->MOS6502_CPU_AddrZeroPageY()); break;  // LDX $nn,Y
			case 0xAE: this->MOS6502_CPU_LDX(this->MOS6502_CPU_AddrAbsolute()); break;   // LDX $nnnn
			case 0xBE: this->MOS6502_CPU_LDX(this->MOS6502_CPU_AddrAbsoluteY()); break;  // LDX $nnnn,Y

			case 0xA0: this->MOS6502_CPU_LDY(this->MOS6502_CPU_AddrImmediate()); break;  // LDY #$nn
			case 0xA4: this->MOS6502_CPU_LDY(this->MOS6502_CPU_AddrZeroPage()); break;   // LDY $nn
			case 0xB4: this->MOS6502_CPU_LDY(this->MOS6502_CPU_AddrZeroPageX()); break;  // LDY $nn,X
			case 0xAC: this->MOS6502_CPU_LDY(this->MOS6502_CPU_AddrAbsolute()); break;   // LDY $nnnn
			case 0xBC: this->MOS6502_CPU_LDY(this->MOS6502_CPU_AddrAbsoluteX()); break;  // LDY $nnnn,X

			// STA
			case 0x85: this->MOS6502_CPU_STA(this->MOS6502_CPU_AddrZeroPage()); break;   // STA Zero Page
			case 0x95: this->MOS6502_CPU_STA(this->MOS6502_CPU_AddrZeroPageX()); break;  // STA Zero Page,X
			case 0x8D: this->MOS6502_CPU_STA(this->MOS6502_CPU_AddrAbsolute()); break;   // STA Absolute
			case 0x9D: this->MOS6502_CPU_STA(this->MOS6502_CPU_AddrAbsoluteX()); break;  // STA Absolute,X
			case 0x99: this->MOS6502_CPU_STA(this->MOS6502_CPU_AddrAbsoluteY()); break;  // STA Absolute,Y
			case 0x81: this->MOS6502_CPU_STA(this->MOS6502_CPU_AddrIndirectX()); break;  // STA (Indirect,X)
			case 0x91: this->MOS6502_CPU_STA(this->MOS6502_CPU_AddrIndirectY()); break;  // STA (Indirect),Y

			// Transfer instructions
			case 0xAA: this->MOS6502_CPU_TAX(); break; // TAX
			case 0xA8: this->MOS6502_CPU_TAY(); break; // TAY
			case 0x8A: this->MOS6502_CPU_TXA(); break; // TXA
			case 0x98: this->MOS6502_CPU_TYA(); break; // TYA

			// Increment/Decrement
			case 0xE8: this->MOS6502_CPU_INX(); break; // INX
			case 0xC8: this->MOS6502_CPU_INY(); break; // INY
			case 0xCA: this->MOS6502_CPU_DEX(); break; // DEX
			case 0x88: this->MOS6502_CPU_DEY(); break; // DEY

			// Branch
			case 0xD0: this->MOS6502_CPU_BNE(this->MOS6502_CPU_Fetch()); break; // BNE $<addr>

			// Jump
			case 0x4C: this->MOS6502_CPU_JMP(this->MOS6502_CPU_AddrAbsolute()); break;

			default:
				std::cout << "CPU: CPU has received unknown opcode when trying execute instruction. Jumping to FAILBACK_DEFAULT_ACTION!" << std::endl;
				std::cout << "CPU: ExecutionInstruction: FAILBACK_DEFAULT_ACTION: Unknown opcode: 0x" << std::hex << (unsigned int)opcode << std::endl;
				break;
		}
	}
}													 