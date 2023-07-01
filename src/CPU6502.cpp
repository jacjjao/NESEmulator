#include "CPU6502.hpp"
#include "common/bitHelper.hpp"
#include "common/utility.hpp"

#include <algorithm>
#include <iostream>
#include <cstring>
#include <cassert>
#include <iomanip>


CPU6502::CPU6502() : 
	mem(new u8[mem_size])
{
	memset(mem, 0, mem_size);

	instrs.resize(instrs_size, [this](const u8 opcode) {
		unknownOpcode(opcode);
	});

	const auto ADC = [this](const u8 opcode) {
		instrADC(opcode);
	};
	instrs[0x69] = instrs[0x65] = instrs[0x75] = instrs[0x6D] = ADC;
	instrs[0x7D] = instrs[0x79] = instrs[0x61] = instrs[0x71] = ADC;

	const auto AND = [this](const u8 opcode) {
		instrAND(opcode);
	};
	instrs[0x29] = instrs[0x25] = instrs[0x35] = instrs[0x2D] = AND;
	instrs[0x3D] = instrs[0x39] = instrs[0x21] = instrs[0x31] = AND;

	const auto ASL = [this](const u8 opcode) {
		instrASL(opcode);
	};
	instrs[0x0A] = instrs[0x06] = ASL;
	instrs[0x16] = instrs[0x0E] = instrs[0x1E] = ASL;

	// BCC - Branch if Carry Clear
	const auto BCC = [this](const u8 opcode) {
		instrBCC(opcode);
	};
	instrs[0x90] = BCC;

	// BCS - Branch if Carry Set
	const auto BCS = [this](const u8 opcode) {
		instrBCS(opcode);
	};
	instrs[0xB0] = BCS;

	// BEQ - Branch if Equal
	const auto BEQ = [this](const u8 opcode) {
		instrBEQ(opcode);
	};
	instrs[0xF0] = BEQ;

	// BIT - Bit Test
	const auto BIT = [this](const u8 opcode) {
		instrBIT(opcode);
	};
	instrs[0x24] = instrs[0x2C] = BIT;

	// BMI - Branch if Minus
	const auto BMI = [this](const u8 opcode) {
		instrBMI(opcode);
	};
	instrs[0x30] = BMI;

	// BNE - Branch if Not Equal
	const auto BNE = [this](const u8 opcode) {
		instrBNE(opcode);
	};
	instrs[0xD0] = BNE;

	// BPL - Branch if Positive
	const auto BPL = [this](const u8 opcode) {
		instrBPL(opcode);
	};
	instrs[0x10] = BPL;

	// BRK - Break
	const auto BRK = [this](const u8 opcode) {
		instrBRK(opcode);
	};
	instrs[0x00] = BRK;

	// BVC - Branch if Overflow Clear
	const auto BVC = [this](const u8 opcode) {
		instrBVC(opcode);
	};
	instrs[0x50] = BVC;

	// BVS - Branch if Overflow Set
	const auto BVS = [this](const u8 opcode) {
		instrBVS(opcode);
	};
	instrs[0x70] = BVS;

	// CLC - Clear Carry Flag
	const auto CLC = [this](const u8 opcode) {
		instrCLC(opcode);
	};
	instrs[0x18] = CLC;

	// CLD - Clear Decimal Mode
	const auto CLD = [this](const u8 opcode) {
		instrCLD(opcode);
	};
	instrs[0xD8] = CLD;

	// CLI - Clear Interrupt Disable
	const auto CLI = [this](const u8 opcode) {
		instrCLI(opcode);
	};
	instrs[0x58] = CLI;

	// CLV - Clear Overflow Flag
	const auto CLV = [this](const u8 opcode) {
		instrCLV(opcode);
	};
	instrs[0xB8] = CLV;

	// CMP - Compare Accumulator with Memory
	const auto CMP = [this](const u8 opcode) {
		instrCMP(opcode);
	};
	instrs[0xC9] = instrs[0xC5] = instrs[0xD5] = instrs[0xCD] = CMP;
	instrs[0xDD] = instrs[0xD9] = instrs[0xC1] = instrs[0xD1] = CMP;

	// CPX - Compare X Register with Memory
	const auto CPX = [this](const u8 opcode) {
		instrCPX(opcode);
	};
	instrs[0xE0] = instrs[0xE4] = instrs[0xEC] = CPX;

	// CPY - Compare Y Register with Memory
	const auto CPY = [this](const u8 opcode) {
		instrCPY(opcode);
	};
	instrs[0xC0] = instrs[0xC4] = instrs[0xCC] = CPY;

	// DEC - Decrement Memory by One
	const auto DEC = [this](const u8 opcode) {
		instrDEC(opcode);
	};
	instrs[0xC6] = instrs[0xD6] = instrs[0xCE] = instrs[0xDE] = DEC;

	// DEX - Decrement X Register by One
	const auto DEX = [this](const u8 opcode) {
		instrDEX(opcode);
	};
	instrs[0xCA] = DEX;

	// DEY - Decrement Y Register by One
	const auto DEY = [this](const u8 opcode) {
		instrDEY(opcode);
	};
	instrs[0x88] = DEY;

	// EOR - Exclusive OR Memory with Accumulator
	const auto EOR = [this](const u8 opcode) {
		instrEOR(opcode);
	};
	instrs[0x49] = instrs[0x45] = instrs[0x55] = instrs[0x4D] = EOR;
	instrs[0x5D] = instrs[0x59] = instrs[0x41] = instrs[0x51] = EOR;

	// INC - Increment Memory by One
	const auto INC = [this](const u8 opcode) {
		instrINC(opcode);
	};
	instrs[0xE6] = instrs[0xF6] = instrs[0xEE] = instrs[0xFE] = INC;

	// INX - Increment X Register by One
	const auto INX = [this](const u8 opcode) {
		instrINX(opcode);
	};
	instrs[0xE8] = INX;

	// INY - Increment Y Register by One
	const auto INY = [this](const u8 opcode) {
		instrINY(opcode);
	};
	instrs[0xC8] = INY;

	// JMP - Jump to New Location
	const auto JMP = [this](const u8 opcode) {
		instrJMP(opcode);
	};
	instrs[0x4C] = instrs[0x6C] = JMP;

	// JSR - Jump to New Location Saving Return Address
	const auto JSR = [this](const u8 opcode) {
		instrJSR(opcode);
	};
	instrs[0x20] = JSR;

	// LDA - Load Accumulator with Memory
	const auto LDA = [this](const u8 opcode) {
		instrLDA(opcode);
	};
	instrs[0xA9] = instrs[0xA5] = instrs[0xB5] = instrs[0xAD] = LDA;
	instrs[0xBD] = instrs[0xB9] = instrs[0xA1] = instrs[0xB1] = LDA;

	// LDX - Load X Register with Memory
	const auto LDX = [this](const u8 opcode) {
		instrLDX(opcode);
	};
	instrs[0xA2] = instrs[0xA6] = instrs[0xB6] = instrs[0xAE] = LDX;
	instrs[0xBE] = LDX;

	// LDY - Load Y Register with Memory
	const auto LDY = [this](const u8 opcode) {
		instrLDY(opcode);
	};
	instrs[0xA0] = instrs[0xA4] = instrs[0xB4] = instrs[0xAC] = LDY;
	instrs[0xBC] = LDY;

	// LSR - Logical Shift Right
	const auto LSR = [this](const u8 opcode) {
		instrLSR(opcode);
	};
	instrs[0x4A] = instrs[0x46] = LSR;
	instrs[0x56] = instrs[0x4E] = instrs[0x5E] = LSR;

	// NOP - No Operation
	const auto NOP = [this](const u8 opcode) {
		instrNOP(opcode);
	};
	instrs[0xEA] = NOP;

	// ORA - Logical OR Memory with Accumulator
	const auto ORA = [this](const u8 opcode) {
		instrORA(opcode);
	};
	instrs[0x09] = instrs[0x05] = instrs[0x15] = instrs[0x0D] = ORA;
	instrs[0x1D] = instrs[0x19] = instrs[0x01] = instrs[0x11] = ORA;

	// PHA - Push Accumulator on Stack
	const auto PHA = [this](const u8 opcode) {
		instrPHA(opcode);
	};
	instrs[0x48] = PHA;

	// PHP - Push Processor Status on Stack
	const auto PHP = [this](const u8 opcode) {
		instrPHP(opcode);
	};
	instrs[0x08] = PHP;

	// PLA - Pull Accumulator from Stack
	const auto PLA = [this](const u8 opcode) {
		instrPLA(opcode);
	};
	instrs[0x68] = PLA;

	// PLP - Pull Processor Status from Stack
	const auto PLP = [this](const u8 opcode) {
		instrPLP(opcode);
	};
	instrs[0x28] = PLP;

	// ROL - Rotate Left
	const auto ROL = [this](const u8 opcode) {
		instrROL(opcode);
	};
	instrs[0x2A] = instrs[0x26] = ROL;
	instrs[0x36] = instrs[0x2E] = instrs[0x3E] = ROL;

	// ROR - Rotate Right
	const auto ROR = [this](const u8 opcode) {
		instrROR(opcode);
	};
	instrs[0x6A] = instrs[0x66] = ROR;
	instrs[0x76] = instrs[0x6E] = instrs[0x7E] = ROR;

	// RTI - Return from Interrupt
	const auto RTI = [this](const u8 opcode) {
		instrRTI(opcode);
	};
	instrs[0x40] = RTI;

	// RTS - Return from Subroutine
	const auto RTS = [this](const u8 opcode) {
		instrRTS(opcode);
	};
	instrs[0x60] = RTS;

	// SBC - Subtract with Carry
	const auto SBC = [this](const u8 opcode) {
		instrSBC(opcode);
	};
	instrs[0xE9] = instrs[0xE5] = instrs[0xF5] = instrs[0xED] = SBC;
	instrs[0xFD] = instrs[0xF9] = instrs[0xE1] = instrs[0xF1] = SBC;

	// SEC - Set Carry Flag
	const auto SEC = [this](const u8 opcode) {
		instrSEC(opcode);
	};
	instrs[0x38] = SEC;

	// SED - Set Decimal Flag
	const auto SED = [this](const u8 opcode) {
		instrSED(opcode);
	};
	instrs[0xF8] = SED;

	// SEI - Set Interrupt Disable
	const auto SEI = [this](const u8 opcode) {
		instrSEI(opcode);
	};
	instrs[0x78] = SEI;

	// STA - Store Accumulator in Memory
	const auto STA = [this](const u8 opcode) {
		instrSTA(opcode);
	};
	instrs[0x85] = instrs[0x95] = instrs[0x8D] = STA;
	instrs[0x9D] = instrs[0x99] = instrs[0x81] = instrs[0x91] = STA;

	// STX - Store X Register in Memory
	const auto STX = [this](const u8 opcode) {
		instrSTX(opcode);
	};
	instrs[0x86] = instrs[0x96] = instrs[0x8E] = STX;

	// STY - Store Y Register in Memory
	const auto STY = [this](const u8 opcode) {
		instrSTY(opcode);
	};
	instrs[0x84] = instrs[0x94] = instrs[0x8C] = STY;

	// TAX - Transfer Accumulator to X
	const auto TAX = [this](const u8 opcode) {
		instrTAX(opcode);
	};
	instrs[0xAA] = TAX;

	// TAY - Transfer Accumulator to Y
	const auto TAY = [this](const u8 opcode) {
		instrTAY(opcode);
	};
	instrs[0xA8] = TAY;

	// TSX - Transfer Stack Pointer to X
	const auto TSX = [this](const u8 opcode) {
		instrTSX(opcode);
	};
	instrs[0xBA] = TSX;

	// TXA - Transfer X to Accumulator
	const auto TXA = [this](const u8 opcode) {
		instrTXA(opcode);
	};
	instrs[0x8A] = TXA;

	// TXS - Transfer X to Stack Pointer
	const auto TXS = [this](const u8 opcode) {
		instrTXS(opcode);
	};
	instrs[0x9A] = TXS;

	// TYA - Transfer Y to Accumulator
	const auto TYA = [this](const u8 opcode) {
		instrTYA(opcode);
	};
	instrs[0x98] = TYA;
}

CPU6502::~CPU6502()
{
	delete[] mem;
}

void CPU6502::update()
{
	static u16 prev_pc = reg.PC;
	prev_pc = reg.PC;

	const u8 opcode = getByteFromPC();
	instrs[opcode](opcode);

	if (std::abs((int)reg.PC - (int)prev_pc) > 3) // it's a branch 
		return;
	std::cout << std::hex << (int)prev_pc << ' ';
	for (u8* it = &mem[prev_pc]; it < &mem[reg.PC]; ++it)
		std::cout << ' ' << std::hex << (int)*it;
	std::cout << '\n';
	std::cout << "A:" << std::hex << (int)reg.A;
	std::cout << " X:" << std::hex << (int)reg.X;
	std::cout << " Y:" << std::hex << (int)reg.Y;
	std::cout << " P:" << std::hex << (int)reg.Status;
	std::cout << " SP:" << std::hex << (int)reg.SP << "\n\n";
}

void CPU6502::instrADC(const u8 opcode)
{
	u16 addr = 0;
	switch (opcode)
	{
	case 0x69:
		addr = immediateAddr();
		break;
	
	case 0x65:
		addr = zeroPageAddr();
		break;

	case 0x75:
		addr = zeroPageAddr(reg.X);
		break;

	case 0x6D:
		addr = absoluteAddr();
		break;

	case 0x7D:
		addr = absoluteAddr(reg.X);
		break;

	case 0x79:
		addr = absoluteAddr(reg.Y);
		break;

	case 0x61:
		addr = indexedIndirectAddr();
		break;

	case 0x71:
		addr = indirectIndexedAddr();
		break;
	}

	const u8 M = mem[addr];
	const u8 C = getCarryFlag();
	const u8 result = reg.A + M + C;

	setCarryFlag(result < reg.A);
	setZeroFlag(result);
	setNegativeResultFlag(result);

	setOverflowFlag(willAddOverflow(reg.A, M, C));

	reg.A = result;
}

void CPU6502::instrAND(const u8 opcode)
{
	u16 addr = 0;
	switch (opcode)
	{
	case 0x29:
		addr = immediateAddr();
		break;
	
	case 0x25:
		addr = zeroPageAddr();
		break;
	
	case 0x35:
		addr = zeroPageAddr(reg.X);
		break;

	case 0x2D:
		addr = absoluteAddr();
		break;

	case 0x3D:
		addr = absoluteAddr(reg.X);
		break;

	case 0x39:
		addr = absoluteAddr(reg.Y);
		break;

	case 0x21:
		addr = indexedIndirectAddr();
		break;

	case 0x31:
		addr = indirectIndexedAddr();
		break;
	}

	reg.A = reg.A & mem[addr];

	setZeroFlag(reg.A);
	setNegativeResultFlag(reg.A);
}

void CPU6502::instrASL(u8 opcode)
{
	u8* target = nullptr;
	switch (opcode)
	{
	case 0x0A: 
		target = &reg.A;
		break;

	case 0x06:
	{
		const u16 addr = zeroPageAddr();
		target = &mem[addr];
		break;
	}
	case 0x16:
	{
		const u16 addr = zeroPageAddr(reg.X);
		target = &mem[addr];
		break;
	}
	case 0x0E:
	{
		const u16 addr = absoluteAddr();
		target = &mem[addr];
		break;
	}
	case 0x1E:
	{
		const u16 addr = absoluteAddr(reg.X);
		target = &mem[addr];
		break;
	}
	}

	assert(target);
	setCarryFlag(getBitN(*target, 7));
	*target <<= 1;
	setZeroFlag(*target);
	setNegativeResultFlag(*target);
}

void CPU6502::instrBCC(u8)
{
	const u8 displacement = getByteFromPC(); // get the byte no matter it will branch or not
	if (!getCarryFlag())
		reg.PC += displacement;
}

void CPU6502::instrBCS(u8)
{
	const u8 displacement = getByteFromPC();
	if (getCarryFlag())
		reg.PC += displacement;
}

void CPU6502::instrBEQ(u8)
{
	const u8 displacement = getByteFromPC();
	if (getZeroFlag())
		reg.PC += displacement;
}

void CPU6502::instrBIT(const u8 opcode)
{
	u16 addr = 0;
	switch (opcode)
	{
	case 0x24:
		addr = zeroPageAddr();
		break;

	case 0x2C:
		addr = absoluteAddr();
		break;
	}
	const u8 M = mem[addr];
	setZeroFlag(reg.A & M);
	setOverflowFlag(getBitN(M, 6));
	setNegativeResultFlag(M);
}

void CPU6502::instrBMI(u8)
{
	const u8 displacement = getByteFromPC();
	if (getNegativeResultFlag())
		reg.PC += displacement;
}

void CPU6502::instrBNE(u8)
{
	const u8 displacement = getByteFromPC();
	if (!getZeroFlag())
		reg.PC += displacement;
}

void CPU6502::instrBPL(u8)
{
	const u8 displacement = getByteFromPC();
	if (getZeroFlag())
		reg.PC += displacement;
}

void CPU6502::instrBRK(u8)
{
	pushStack(reg.PC);
	pushStack(reg.Status);
	reg.PC = getTwoBytesFromMem(0xFFFE);
	setBreakFlag(true);
}

void CPU6502::instrBVC(u8)
{
	const u8 displacement = getByteFromPC();
	if (!getOverflowFlag())
		reg.PC += displacement;
}

void CPU6502::instrBVS(u8)
{
	const u8 displacement = getByteFromPC();
	if (getOverflowFlag())
		reg.PC += displacement;
}

void CPU6502::instrCLC(u8)
{
	setCarryFlag(false);
}

void CPU6502::instrCLD(u8)
{
	setDecimalModeFlag(false);
}

void CPU6502::instrCLI(u8)
{
	setInterruptDisableFlag(false);
}

void CPU6502::instrCLV(u8)
{
	setOverflowFlag(false);
}

void CPU6502::instrCMP(const u8 opcode)
{
	u16 addr = 0;
	switch (opcode)
	{
	case 0xC9:
		addr = immediateAddr();
		break;

	case 0xC5:
		addr = zeroPageAddr();
		break;

	case 0xD5:
		addr = zeroPageAddr(reg.X);
		break;

	case 0xCD:
		addr = absoluteAddr();
		break;
	
	case 0xDD:
		addr = absoluteAddr(reg.X);
		break;

	case 0xD9:
		addr = absoluteAddr(reg.Y);
		break;

	case 0xC1:
		addr = indexedIndirectAddr();
		break;

	case 0xD1:
		addr = indirectIndexedAddr();
		break;
	}

	const u8 M = mem[addr];
	const u8 result = reg.A - M;

	setCarryFlag(reg.A >= M);
	setZeroFlag(result);
	setNegativeResultFlag(result);
}

void CPU6502::instrCPX(const u8 opcode)
{
	u16 addr = 0;
	switch (opcode)
	{
	case 0xE0:
		addr = immediateAddr();
		break;
	
	case 0xE4:
		addr = zeroPageAddr();
		break;

	case 0xEC:
		addr = absoluteAddr();
		break;
	}

	const u8 M = mem[addr];
	const u8 result = reg.X - M;

	setCarryFlag(reg.X >= M);
	setZeroFlag(result);
	setNegativeResultFlag(result);
}

void CPU6502::instrCPY(const u8 opcode)
{
	u16 addr = 0;
	switch (opcode)
	{
	case 0xC0:
		addr = immediateAddr();
		break;

	case 0xC4:
		addr = zeroPageAddr();
		break;

	case 0xCC:
		addr = absoluteAddr();
		break;
	}

	const u8 M = mem[addr];
	const u8 result = reg.Y - M;

	setCarryFlag(reg.Y >= M);
	setZeroFlag(result);
	setNegativeResultFlag(result);
}

void CPU6502::instrDEC(const u8 opcode)
{
	u16 addr = 0;
	switch (opcode)
	{
	case 0xC6:
		addr = zeroPageAddr();
		break;

	case 0xD6:
		addr = zeroPageAddr(reg.X);
		break;

	case 0xCE:
		addr = absoluteAddr();
		break;

	case 0xDE:
		addr = absoluteAddr(reg.X);
		break;
	}

	--mem[addr];
	setZeroFlag(mem[addr]);
	setNegativeResultFlag(mem[addr]);
}

void CPU6502::instrDEX(u8)
{
	--reg.X;
	setZeroFlag(reg.X);
	setNegativeResultFlag(reg.X);
}

void CPU6502::instrDEY(u8)
{
	--reg.Y;
	setZeroFlag(reg.Y);
	setNegativeResultFlag(reg.Y);
}

void CPU6502::instrEOR(const u8 opcode)
{
	u16 addr = 0;
	switch (opcode)
	{
	case 0x49:
		addr = immediateAddr();
		break;

	case 0x45:
		addr = zeroPageAddr();
		break;

	case 0x55:
		addr = zeroPageAddr(reg.X);
		break;

	case 0x4D:
		addr = absoluteAddr();
		break;

	case 0x5D:
		addr = absoluteAddr(reg.X);
		break;

	case 0x59:
		addr = absoluteAddr(reg.Y);
		break;

	case 0x41:
		addr = indexedIndirectAddr();
		break;

	case 0x51:
		addr = indirectIndexedAddr();
		break;
	}

	reg.A = reg.A ^ mem[addr];
	setZeroFlag(reg.A);
	setNegativeResultFlag(reg.A);
}

void CPU6502::instrINC(u8 opcode)
{
	u16 addr = 0;
	switch (opcode)
	{
	case 0xE6:
		addr = zeroPageAddr();
		break;

	case 0xF6:
		addr = zeroPageAddr(reg.X);
		break;

	case 0xEE:
		addr = absoluteAddr();
		break;

	case 0xFE:
		addr = absoluteAddr(reg.X);
		break;
	}

	++mem[addr];
	setZeroFlag(mem[addr]);
	setNegativeResultFlag(mem[addr]);
}

void CPU6502::instrINX(u8)
{
	++reg.X;
	setZeroFlag(reg.X);
	setNegativeResultFlag(reg.X);
}

void CPU6502::instrINY(u8)
{
	++reg.Y;
	setZeroFlag(reg.Y);
	setNegativeResultFlag(reg.Y);
}

void CPU6502::instrJMP(const u8 opcode)
{
	switch (opcode)
	{
	case 0x4C:
		reg.PC = absoluteAddr();
		break;

	case 0x6C:
		reg.PC = indirectAddr();
		break;
	}
}

void CPU6502::instrJSR(u8)
{
	const u16 destination = absoluteAddr();
	pushStack(reg.PC);
	reg.PC = destination;
}

void CPU6502::instrLDA(const u8 opcode)
{
	switch (opcode)
	{
	case 0xA9:
		reg.A = immediateAddr();
		break;

	case 0xA5:
		reg.A = mem[zeroPageAddr()];
		break;

	case 0xB5:
		reg.A = mem[zeroPageAddr(reg.X)];
		break;

	case 0xAD:
		reg.A = mem[absoluteAddr()];
		break;

	case 0xBD:
		reg.A = mem[absoluteAddr(reg.X)];
		break;

	case 0xB9:
		reg.A = mem[absoluteAddr(reg.Y)];
		break;

	case 0xA1:
		reg.A = mem[indexedIndirectAddr()];
		break;

	case 0xB1:
		reg.A = mem[indirectIndexedAddr()];
		break;
	}
	setZeroFlag(reg.A);
	setNegativeResultFlag(reg.A);
}

void CPU6502::instrLDX(const u8 opcode)
{
	switch (opcode)
	{
	case 0xA2:
		reg.X = immediateAddr();
		break;

	case 0xA6:
		reg.X = mem[zeroPageAddr()];
		break;

	case 0xB6:
		reg.X = mem[zeroPageAddr(reg.Y)];
		break;

	case 0xAE:
		reg.X = mem[absoluteAddr()];
		break;

	case 0xBE:
		reg.X = mem[absoluteAddr(reg.Y)];
		break;
	}
	setZeroFlag(reg.X);
	setNegativeResultFlag(reg.X);
}

void CPU6502::instrLDY(const u8 opcode)
{
	u16 addr = 0;
	switch (opcode)
	{
	case 0xA0:
		addr = immediateAddr();
		break;

	case 0xA4:
		reg.Y = mem[zeroPageAddr()];
		break;

	case 0xB4:
		reg.Y = mem[zeroPageAddr(reg.X)];
		break;

	case 0xAC:
		reg.Y = mem[absoluteAddr()];
		break;

	case 0xBC:
		reg.Y = mem[absoluteAddr(reg.X)];
		break;
	}
	setZeroFlag(reg.Y);
	setNegativeResultFlag(reg.Y);
}

void CPU6502::instrLSR(const u8 opcode)
{
	u8* target = nullptr;
	switch (opcode)
	{
	case 0x4A:
		target = &reg.A;
		break;

	case 0x46:
		target = &mem[zeroPageAddr()];
		break;

	case 0x56:
		target = &mem[zeroPageAddr(reg.X)];
		break;

	case 0x4E:
		target = &mem[absoluteAddr()];
		break;

	case 0x5E:
		target = &mem[absoluteAddr(reg.X)];
		break;
	}

	assert(target);
	setCarryFlag(getBitN(*target, 0));
	*target >>= 1;
	setNegativeResultFlag(*target);
}

void CPU6502::instrNOP(u8)
{
	// nop
}

void CPU6502::instrORA(const u8 opcode)
{
	u16 addr = 0;
	switch (opcode)
	{
	case 0x09:
		addr = immediateAddr();
		break;

	case 0x05:
		addr = zeroPageAddr();
		break;

	case 0x15:
		addr = zeroPageAddr(reg.X);
		break;

	case 0x0D:
		addr = absoluteAddr();
		break;

	case 0x1D:
		addr = absoluteAddr(reg.X);
		break;

	case 0x19:
		addr = absoluteAddr(reg.Y);
		break;

	case 0x01:
		addr = indexedIndirectAddr();
		break;

	case 0x11:
		addr = indirectIndexedAddr();
		break;
	}

	const u8 M = mem[addr];
	reg.A = reg.A | M;
	setZeroFlag(reg.A);
	setNegativeResultFlag(reg.A);
}

void CPU6502::instrPHA(u8)
{
	pushStack(reg.A);
}

void CPU6502::instrPHP(u8)
{
	pushStack(reg.Status);
}

void CPU6502::instrPLA(u8)
{
	reg.A = popStack();
	setZeroFlag(reg.A);
	setNegativeResultFlag(reg.A);
}

void CPU6502::instrPLP(u8)
{
	reg.Status = popStack();
}

void CPU6502::instrROL(const u8 opcode)
{
	u8* target = nullptr;
	switch (opcode)
	{
	case 0x2A:
		target = &reg.A;
		break;

	case 0x26:
		target = &mem[zeroPageAddr()];
		break;

	case 0x36:
		target = &mem[zeroPageAddr(reg.X)];
		break;

	case 0x2E:
		target = &mem[absoluteAddr()];
		break;

	case 0x3E:
		target = &mem[absoluteAddr(reg.X)];
		break;
	}

	assert(target);
	const u8 new_bit0 = getCarryFlag();
	setCarryFlag(getBitN(*target, 7));
	*target <<= 1;
	setBitN(*target, 0, new_bit0);
	setNegativeResultFlag(*target);
}

void CPU6502::instrROR(const u8 opcode)
{
	u8* target = nullptr;
	switch (opcode)
	{
	case 0x6A:
		target = &reg.A;
		break;

	case 0x66:
		target = &mem[zeroPageAddr()];
		break;

	case 0x76:
		target = &mem[zeroPageAddr(reg.X)];
		break;

	case 0x6E:
		target = &mem[absoluteAddr()];
		break;

	case 0x7E:
		target = &mem[absoluteAddr(reg.X)];
		break;
	}

	assert(target);
	const u8 new_bit7 = getCarryFlag();
	setCarryFlag(getBitN(*target, 0));
	*target >>= 1;
	setBitN(*target, 7, new_bit7);
	setNegativeResultFlag(*target);
}

void CPU6502::instrRTI(u8)
{
	reg.Status = popStack();
	reg.PC = popStackTwoBytes();
}

void CPU6502::instrRTS(u8)
{
	reg.PC = popStackTwoBytes();
}

void CPU6502::instrSBC(const u8 opcode)
{
	u16 addr = 0;
	switch (opcode)
	{
	case 0xE9:
		addr = immediateAddr();
		break;

	case 0xE5:
		addr = zeroPageAddr();
		break;

	case 0xF5:
		addr = zeroPageAddr(reg.X);
		break;

	case 0xED:
		addr = absoluteAddr();
		break;

	case 0xFD:
		addr = absoluteAddr(reg.X);
		break;

	case 0xF9:
		addr = absoluteAddr(reg.Y);
		break;

	case 0xE1:
		addr = indexedIndirectAddr();
		break;

	case 0xF1:
		addr = indirectIndexedAddr();
		break;
	}

	const u8 M = mem[addr], C = getCarryFlag();
	const u8 result = reg.A - M - (static_cast<u8>(1) - C);

	setZeroFlag(result);
	setNegativeResultFlag(result);

	const bool overflow = willAddOverflow(reg.A, ~M + 1, C - static_cast<u8>(1));
	if (overflow)
		setCarryFlag(false);
	setOverflowFlag(overflow);
}

void CPU6502::instrSEC(u8)
{
	setCarryFlag(true);
}

void CPU6502::instrSED(u8)
{
	setDecimalModeFlag(true);
}

void CPU6502::instrSEI(u8)
{
	setInterruptDisableFlag(true);
}

void CPU6502::instrSTA(const u8 opcode)
{
	u16 addr = 0;
	switch (opcode)
	{
	case 0x85:
		addr = zeroPageAddr();
		break;

	case 0x95:
		addr = zeroPageAddr(reg.X);
		break;

	case 0x8D:
		addr = absoluteAddr();
		break;

	case 0x9D:
		addr = absoluteAddr(reg.X);
		break;

	case 0x99:
		addr = absoluteAddr(reg.Y);
		break;

	case 0x81:
		addr = indexedIndirectAddr();	
		break;

	case 0x91:
		addr = indirectIndexedAddr();
		break;
	}

	mem[addr] = reg.A;
}

void CPU6502::instrSTX(const u8 opcode)
{
	u16 addr = 0;
	switch (opcode)
	{
	case 0x86:
		addr = zeroPageAddr();
		break;

	case 0x96:
		addr = zeroPageAddr(reg.Y);
		break;

	case 0x8E:
		addr = absoluteAddr();
		break;
	}

	mem[addr] = reg.X;
}

void CPU6502::instrSTY(u8 opcode)
{
	u16 addr = 0;
	switch (opcode)
	{
	case 0x84:
		addr = zeroPageAddr();
		break;

	case 0x94:
		addr = zeroPageAddr(reg.X);
		break;

	case 0x8C:
		addr = absoluteAddr();
		break;
	}

	mem[addr] = reg.Y;
}

void CPU6502::instrTAX(u8)
{
	reg.X = reg.A;
	setZeroFlag(reg.X);
	setNegativeResultFlag(reg.X);
}

void CPU6502::instrTAY(u8)
{
	reg.Y = reg.A;
	setZeroFlag(reg.Y);
	setNegativeResultFlag(reg.Y);
}

void CPU6502::instrTSX(u8)
{
	reg.X = reg.SP;	
	setZeroFlag(reg.X);
	setNegativeResultFlag(reg.X);
}

void CPU6502::instrTXA(u8)
{
	reg.A = reg.SP;
	setZeroFlag(reg.A);
	setNegativeResultFlag(reg.A);
}

void CPU6502::instrTXS(u8)
{
	reg.SP = reg.X;
}

void CPU6502::instrTYA(u8)
{
	reg.A = reg.Y;
	setZeroFlag(reg.A);
	setNegativeResultFlag(reg.A);
}

u16 CPU6502::immediateAddr()
{
	return getByteFromPC();
}

u16 CPU6502::zeroPageAddr(const u8 offset)
{
	return getByteFromPC() + offset;
}

u16 CPU6502::absoluteAddr(const u8 offset)
{
	return getTwoBytesFromPC() + static_cast<u16>(offset);
}

u16 CPU6502::indirectAddr()
{
	return mem[getTwoBytesFromPC()];
}

u16 CPU6502::indexedIndirectAddr()
{
	const u8 table_loc = getByteFromPC() + reg.X;
	return getTwoBytesFromZP(table_loc);
}

u16 CPU6502::indirectIndexedAddr()
{
	const u8 table_loc = getByteFromPC();
	return getTwoBytesFromZP(table_loc) + static_cast<u16>(reg.Y);
}

void CPU6502::unknownOpcode(const u8 opcode)
{
	std::cerr << "Unknown opcode: " << std::hex << (int)opcode << '\n'; 
	std::cerr << "PC: " << std::hex << (int)reg.PC << '\n';
	throw std::runtime_error{""};
}

u8 CPU6502::getByteFromPC()
{
	return mem[reg.PC++];
}

u16 CPU6502::getTwoBytesFromPC()
{
	u16 result = mem[reg.PC++];
	result += (static_cast<u16>(mem[reg.PC++]) << 8);
	return result;
}

u16 CPU6502::getTwoBytesFromZP(const u8 loc)
{
	u16 result = mem[loc];
	result += (mem[loc + 1] << 8);
	return result;
}

u16 CPU6502::getTwoBytesFromMem(const u16 loc)
{
	u16 result = mem[loc];
	result += (mem[loc + 1] << 8);
	return result;
}

u8 CPU6502::getCarryFlag() const
{
	return getBitN(reg.Status, 0);
}

u8 CPU6502::getZeroFlag() const
{
	return getBitN(reg.Status, 1);
}

u8 CPU6502::getInterruptDisableFlag() const
{
	return getBitN(reg.Status, 2);
}

u8 CPU6502::getDecimalModeFlag() const
{
	return getBitN(reg.Status, 3);
}

u8 CPU6502::getBreakFlag() const
{
	return getBitN(reg.Status, 4);
}

u8 CPU6502::getOverflowFlag() const
{
	return getBitN(reg.Status, 6);
}

u8 CPU6502::getNegativeResultFlag() const
{
	return getBitN(reg.Status, 7);
}


void CPU6502::setCarryFlag(const bool set)
{
	setBitN(reg.Status, 0, set);
}

void CPU6502::setZeroFlag(const u8 result)
{
	setBitN(reg.Status, 1, (result == 0));
}

void CPU6502::setInterruptDisableFlag(const bool set)
{
	setBitN(reg.Status, 2, set);
}

void CPU6502::setDecimalModeFlag(const bool set)
{
	setBitN(reg.Status, 3, set);
}

void CPU6502::setBreakFlag(const bool set)
{
	setBitN(reg.Status, 4, set);
}

void CPU6502::setOverflowFlag(const bool set)
{
	setBitN(reg.Status, 6, set);
}

void CPU6502::setNegativeResultFlag(const u8 result)
{
	setBitN(reg.Status, 7, getBitN(result, 7));
}

void CPU6502::pushStack(const u8 val)
{
	assert(reg.SP > 0);
	mem[stack_low + reg.SP] = val;
	--reg.SP;
}

void CPU6502::pushStack(const u16 val)
{
	const u8 msb = static_cast<u8>(val >> 8);
	const u8 lsb = static_cast<u8>(val);
	pushStack(msb);
	pushStack(lsb);
}

u8 CPU6502::popStack()
{
	assert(reg.SP < 255);
	++reg.SP;
	return mem[stack_low + reg.SP];
}

u16 CPU6502::popStackTwoBytes()
{
	u16 result = popStack();
	result += (static_cast<u16>(popStack()) << 8);
	return result;
}
