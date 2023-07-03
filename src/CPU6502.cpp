#include "CPU6502.hpp"
#include "Bus.hpp"
#include "common/bitHelper.hpp"
#include "common/utility.hpp"

#include <algorithm>
#include <iostream>
#include <cassert>
#include <sstream>

#ifdef EMUCPULOG
#include <fstream>
#include <iomanip>

void CPU6502::print(const unsigned length)
{
	static std::ofstream file{"output.txt"};

	if (!file.is_open())
		file.open("output.txt");

	file << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << (int)log_regs.PC << ' ';
	for (unsigned i = 0; i < length; ++i, ++log_regs.PC) 
		file << ' ' << std::setw(2) << (int)read(log_regs.PC);
	file << "\n     ";
	file << " A:"  << std::setw(2) << (int)log_regs.A;
	file << " X:"  << std::setw(2) << (int)log_regs.X;
	file << " Y:"  << std::setw(2) << (int)log_regs.Y;
	file << " P:"  << std::setw(2) << (int)log_regs.Status;
	file << " SP:" << std::setw(2) << (int)log_regs.SP;
	file << '\n' << std::endl;
}
#endif

CPU6502::CPU6502()
{
	const auto unknown = [this](const u8 opcode) {
		unknownOpcode(opcode);
	};
	for (auto& instr : instrs_)
		instr = unknown;

	const auto ADC = [this](const u8 opcode) {
		instrADC(opcode);
	};
	instrs_[0x69] = instrs_[0x65] = instrs_[0x75] = instrs_[0x6D] = ADC;
	instrs_[0x7D] = instrs_[0x79] = instrs_[0x61] = instrs_[0x71] = ADC;

	const auto AND = [this](const u8 opcode) {
		instrAND(opcode);
	};
	instrs_[0x29] = instrs_[0x25] = instrs_[0x35] = instrs_[0x2D] = AND;
	instrs_[0x3D] = instrs_[0x39] = instrs_[0x21] = instrs_[0x31] = AND;

	const auto ASL = [this](const u8 opcode) {
		instrASL(opcode);
	};
	instrs_[0x0A] = instrs_[0x06] = ASL;
	instrs_[0x16] = instrs_[0x0E] = instrs_[0x1E] = ASL;

	// BCC - Branch if Carry Clear
	const auto BCC = [this](const u8 opcode) {
		instrBCC(opcode);
	};
	instrs_[0x90] = BCC;

	// BCS - Branch if Carry Set
	const auto BCS = [this](const u8 opcode) {
		instrBCS(opcode);
	};
	instrs_[0xB0] = BCS;

	// BEQ - Branch if Equal
	const auto BEQ = [this](const u8 opcode) {
		instrBEQ(opcode);
	};
	instrs_[0xF0] = BEQ;

	// BIT - Bit Test
	const auto BIT = [this](const u8 opcode) {
		instrBIT(opcode);
	};
	instrs_[0x24] = instrs_[0x2C] = BIT;

	// BMI - Branch if Minus
	const auto BMI = [this](const u8 opcode) {
		instrBMI(opcode);
	};
	instrs_[0x30] = BMI;

	// BNE - Branch if Not Equal
	const auto BNE = [this](const u8 opcode) {
		instrBNE(opcode);
	};
	instrs_[0xD0] = BNE;

	// BPL - Branch if Positive
	const auto BPL = [this](const u8 opcode) {
		instrBPL(opcode);
	};
	instrs_[0x10] = BPL;

	// BRK - Break
	const auto BRK = [this](const u8 opcode) {
		instrBRK(opcode);
	};
	instrs_[0x00] = BRK;

	// BVC - Branch if Overflow Clear
	const auto BVC = [this](const u8 opcode) {
		instrBVC(opcode);
	};
	instrs_[0x50] = BVC;

	// BVS - Branch if Overflow Set
	const auto BVS = [this](const u8 opcode) {
		instrBVS(opcode);
	};
	instrs_[0x70] = BVS;

	// CLC - Clear Carry Flag
	const auto CLC = [this](const u8 opcode) {
		instrCLC(opcode);
	};
	instrs_[0x18] = CLC;

	// CLD - Clear Decimal Mode
	const auto CLD = [this](const u8 opcode) {
		instrCLD(opcode);
	};
	instrs_[0xD8] = CLD;

	// CLI - Clear Interrupt Disable
	const auto CLI = [this](const u8 opcode) {
		instrCLI(opcode);
	};
	instrs_[0x58] = CLI;

	// CLV - Clear Overflow Flag
	const auto CLV = [this](const u8 opcode) {
		instrCLV(opcode);
	};
	instrs_[0xB8] = CLV;

	// CMP - Compare Accumulator with Memory
	const auto CMP = [this](const u8 opcode) {
		instrCMP(opcode);
	};
	instrs_[0xC9] = instrs_[0xC5] = instrs_[0xD5] = instrs_[0xCD] = CMP;
	instrs_[0xDD] = instrs_[0xD9] = instrs_[0xC1] = instrs_[0xD1] = CMP;

	// CPX - Compare X Register with Memory
	const auto CPX = [this](const u8 opcode) {
		instrCPX(opcode);
	};
	instrs_[0xE0] = instrs_[0xE4] = instrs_[0xEC] = CPX;

	// CPY - Compare Y Register with Memory
	const auto CPY = [this](const u8 opcode) {
		instrCPY(opcode);
	};
	instrs_[0xC0] = instrs_[0xC4] = instrs_[0xCC] = CPY;

	// DEC - Decrement Memory by One
	const auto DEC = [this](const u8 opcode) {
		instrDEC(opcode);
	};
	instrs_[0xC6] = instrs_[0xD6] = instrs_[0xCE] = instrs_[0xDE] = DEC;

	// DEX - Decrement X Register by One
	const auto DEX = [this](const u8 opcode) {
		instrDEX(opcode);
	};
	instrs_[0xCA] = DEX;

	// DEY - Decrement Y Register by One
	const auto DEY = [this](const u8 opcode) {
		instrDEY(opcode);
	};
	instrs_[0x88] = DEY;

	// EOR - Exclusive OR Memory with Accumulator
	const auto EOR = [this](const u8 opcode) {
		instrEOR(opcode);
	};
	instrs_[0x49] = instrs_[0x45] = instrs_[0x55] = instrs_[0x4D] = EOR;
	instrs_[0x5D] = instrs_[0x59] = instrs_[0x41] = instrs_[0x51] = EOR;

	// INC - Increment Memory by One
	const auto INC = [this](const u8 opcode) {
		instrINC(opcode);
	};
	instrs_[0xE6] = instrs_[0xF6] = instrs_[0xEE] = instrs_[0xFE] = INC;

	// INX - Increment X Register by One
	const auto INX = [this](const u8 opcode) {
		instrINX(opcode);
	};
	instrs_[0xE8] = INX;

	// INY - Increment Y Register by One
	const auto INY = [this](const u8 opcode) {
		instrINY(opcode);
	};
	instrs_[0xC8] = INY;

	// JMP - Jump to New Location
	const auto JMP = [this](const u8 opcode) {
		instrJMP(opcode);
	};
	instrs_[0x4C] = instrs_[0x6C] = JMP;

	// JSR - Jump to New Location Saving Return Address
	const auto JSR = [this](const u8 opcode) {
		instrJSR(opcode);
	};
	instrs_[0x20] = JSR;

	// LDA - Load Accumulator with Memory
	const auto LDA = [this](const u8 opcode) {
		instrLDA(opcode);
	};
	instrs_[0xA9] = instrs_[0xA5] = instrs_[0xB5] = instrs_[0xAD] = LDA;
	instrs_[0xBD] = instrs_[0xB9] = instrs_[0xA1] = instrs_[0xB1] = LDA;

	// LDX - Load X Register with Memory
	const auto LDX = [this](const u8 opcode) {
		instrLDX(opcode);
	};
	instrs_[0xA2] = instrs_[0xA6] = instrs_[0xB6] = instrs_[0xAE] = LDX;
	instrs_[0xBE] = LDX;

	// LDY - Load Y Register with Memory
	const auto LDY = [this](const u8 opcode) {
		instrLDY(opcode);
	};
	instrs_[0xA0] = instrs_[0xA4] = instrs_[0xB4] = instrs_[0xAC] = LDY;
	instrs_[0xBC] = LDY;

	// LSR - Logical Shift Right
	const auto LSR = [this](const u8 opcode) {
		instrLSR(opcode);
	};
	instrs_[0x4A] = instrs_[0x46] = LSR;
	instrs_[0x56] = instrs_[0x4E] = instrs_[0x5E] = LSR;

	// NOP - No Operation
	const auto NOP = [this](const u8 opcode) {
		instrNOP(opcode);
	};
	instrs_[0xEA] = NOP;

	// ORA - Logical OR Memory with Accumulator
	const auto ORA = [this](const u8 opcode) {
		instrORA(opcode);
	};
	instrs_[0x09] = instrs_[0x05] = instrs_[0x15] = instrs_[0x0D] = ORA;
	instrs_[0x1D] = instrs_[0x19] = instrs_[0x01] = instrs_[0x11] = ORA;

	// PHA - Push Accumulator on Stack
	const auto PHA = [this](const u8 opcode) {
		instrPHA(opcode);
	};
	instrs_[0x48] = PHA;

	// PHP - Push Processor Status on Stack
	const auto PHP = [this](const u8 opcode) {
		instrPHP(opcode);
	};
	instrs_[0x08] = PHP;

	// PLA - Pull Accumulator from Stack
	const auto PLA = [this](const u8 opcode) {
		instrPLA(opcode);
	};
	instrs_[0x68] = PLA;

	// PLP - Pull Processor Status from Stack
	const auto PLP = [this](const u8 opcode) {
		instrPLP(opcode);
	};
	instrs_[0x28] = PLP;

	// ROL - Rotate Left
	const auto ROL = [this](const u8 opcode) {
		instrROL(opcode);
	};
	instrs_[0x2A] = instrs_[0x26] = ROL;
	instrs_[0x36] = instrs_[0x2E] = instrs_[0x3E] = ROL;

	// ROR - Rotate Right
	const auto ROR = [this](const u8 opcode) {
		instrROR(opcode);
	};
	instrs_[0x6A] = instrs_[0x66] = ROR;
	instrs_[0x76] = instrs_[0x6E] = instrs_[0x7E] = ROR;

	// RTI - Return from Interrupt
	const auto RTI = [this](const u8 opcode) {
		instrRTI(opcode);
	};
	instrs_[0x40] = RTI;

	// RTS - Return from Subroutine
	const auto RTS = [this](const u8 opcode) {
		instrRTS(opcode);
	};
	instrs_[0x60] = RTS;

	// SBC - Subtract with Carry
	const auto SBC = [this](const u8 opcode) {
		instrSBC(opcode);
	};
	instrs_[0xE9] = instrs_[0xE5] = instrs_[0xF5] = instrs_[0xED] = SBC;
	instrs_[0xFD] = instrs_[0xF9] = instrs_[0xE1] = instrs_[0xF1] = SBC;

	// SEC - Set Carry Flag
	const auto SEC = [this](const u8 opcode) {
		instrSEC(opcode);
	};
	instrs_[0x38] = SEC;

	// SED - Set Decimal Flag
	const auto SED = [this](const u8 opcode) {
		instrSED(opcode);
	};
	instrs_[0xF8] = SED;

	// SEI - Set Interrupt Disable
	const auto SEI = [this](const u8 opcode) {
		instrSEI(opcode);
	};
	instrs_[0x78] = SEI;

	// STA - Store Accumulator in Memory
	const auto STA = [this](const u8 opcode) {
		instrSTA(opcode);
	};
	instrs_[0x85] = instrs_[0x95] = instrs_[0x8D] = STA;
	instrs_[0x9D] = instrs_[0x99] = instrs_[0x81] = instrs_[0x91] = STA;

	// STX - Store X Register in Memory
	const auto STX = [this](const u8 opcode) {
		instrSTX(opcode);
	};
	instrs_[0x86] = instrs_[0x96] = instrs_[0x8E] = STX;

	// STY - Store Y Register in Memory
	const auto STY = [this](const u8 opcode) {
		instrSTY(opcode);
	};
	instrs_[0x84] = instrs_[0x94] = instrs_[0x8C] = STY;

	// TAX - Transfer Accumulator to X
	const auto TAX = [this](const u8 opcode) {
		instrTAX(opcode);
	};
	instrs_[0xAA] = TAX;

	// TAY - Transfer Accumulator to Y
	const auto TAY = [this](const u8 opcode) {
		instrTAY(opcode);
	};
	instrs_[0xA8] = TAY;

	// TSX - Transfer Stack Pointer to X
	const auto TSX = [this](const u8 opcode) {
		instrTSX(opcode);
	};
	instrs_[0xBA] = TSX;

	// TXA - Transfer X to Accumulator
	const auto TXA = [this](const u8 opcode) {
		instrTXA(opcode);
	};
	instrs_[0x8A] = TXA;

	// TXS - Transfer X to Stack Pointer
	const auto TXS = [this](const u8 opcode) {
		instrTXS(opcode);
	};
	instrs_[0x9A] = TXS;

	// TYA - Transfer Y to Accumulator
	const auto TYA = [this](const u8 opcode) {
		instrTYA(opcode);
	};
	instrs_[0x98] = TYA;
}

void CPU6502::update()
{	
#ifdef EMUCPULOG
	log_regs = reg_;
	log_addr_mode = AddrMode::Implicit;
#endif

	cycles_ = 0;
	const u8 opcode = getByteFromPC();
	instrs_[opcode](opcode);
	setBitN(reg_.Status, 5, true);

#ifdef EMUCPULOG
	switch (log_addr_mode)
	{
	case AddrMode::Implicit:
		print(1);
		break;

	case AddrMode::Imme: case AddrMode::ZP: case AddrMode::Relative:
	case AddrMode::IndexedIndirect: case AddrMode::IndirectIndexed:
		print(2);
		break;

	case AddrMode::Abs: case AddrMode::Indirect:
		print(3);
		break;
	}
#endif
}

void CPU6502::connectToBus(Bus* bus)
{
	bus_ = bus;
}

void CPU6502::write(const u16 addr, const u8 data)
{
	bus_->write(addr, data);
}

u8 CPU6502::read(const u16 addr)
{
	return bus_->read(addr);
}

void CPU6502::irq()
{
	if (getInterruptDisableFlag())
		return;
	pushStack(reg_.PC);
	pushStack(reg_.Status);
	reg_.PC = getTwoBytesFromMem(0xFFFE);
	setInterruptDisableFlag(true);
	cycles_ = 7;
}

void CPU6502::nmi()
{
	pushStack(reg_.PC);
	pushStack(reg_.Status);
	reg_.PC = getTwoBytesFromMem(0xFFFA);
	setInterruptDisableFlag(true);
	cycles_ = 7;
}

void CPU6502::instrADC(const u8 opcode)
{
	u8 M = 0;
	switch (opcode)
	{
	case 0x69:
		M = immediateAddr();
		cycles_ += 2;
		break;
	
	case 0x65:
		M = read(zeroPageAddr());
		cycles_ += 3;
		break;

	case 0x75:
		M = read(zeroPageAddr(reg_.X));
		cycles_ += 4;
		break;

	case 0x6D:
		M = read(absoluteAddr());
		cycles_ += 4;
		break;

	case 0x7D:
		M = read(absoluteAddr(reg_.X));
		cycles_ += 4;
		break;

	case 0x79:
		M = read(absoluteAddr(reg_.Y));
		cycles_ += 4;
		break;

	case 0x61:
		M = read(indexedIndirectAddr());
		cycles_ += 6;
		break;

	case 0x71:
		M = read(indirectIndexedAddr());
		cycles_ += 5;
		break;
	}
		
	const u8 C = getCarryFlag();
	const u8 result = reg_.A + M + C;

	setCarryFlag(result < reg_.A);
	setZeroFlag(result == 0);
	setNegativeResultFlag(getBitN(result, 7));

	setOverflowFlag(willAddOverflow(reg_.A, M, C));

	reg_.A = result;
}

void CPU6502::instrAND(const u8 opcode)
{
	u8 M = 0;
	switch (opcode)
	{
	case 0x29:
		M = immediateAddr();
		break;
	
	case 0x25:
		M = read(zeroPageAddr());
		break;
	
	case 0x35:
		M = read(zeroPageAddr(reg_.X));
		break;

	case 0x2D:
		M = read(absoluteAddr());
		break;

	case 0x3D:
		M = read(absoluteAddr(reg_.X));
		break;

	case 0x39:
		M = read(absoluteAddr(reg_.Y));
		break;

	case 0x21:
		M = read(indexedIndirectAddr());
		break;

	case 0x31:
		M = read(indirectIndexedAddr());
		break;
	}

	reg_.A = reg_.A & M;
	setZeroFlag(reg_.A == 0);
	setNegativeResultFlag(getBitN(reg_.A, 7));
}

void CPU6502::instrASL(u8 opcode)
{
	u8 val = 0;
	u16 addr = 0;
	switch (opcode)
	{
	case 0x0A: 
		val = reg_.A;
		break;

	case 0x06:
		val = read(zeroPageAddr());
		break;

	case 0x16:
		val = read(zeroPageAddr(reg_.X));
		break;

	case 0x0E:
		val = read(absoluteAddr());
		break;

	case 0x1E:
		val = read(absoluteAddr(reg_.X));
		break;
	}

	setCarryFlag(getBitN(val, 7));
	val <<= 1;
	setZeroFlag(val == 0);
	setNegativeResultFlag(getBitN(val, 7));

	if (opcode == 0x0A)
		reg_.A = val;
	else
		write(addr, val);
}

void CPU6502::instrBCC(u8)
{
	const u8 displacement = relativeAddr();
	if (!getCarryFlag())
		relativeDisplace(displacement);
}

void CPU6502::instrBCS(u8)
{
	const u8 displacement = relativeAddr();
	if (getCarryFlag())
		relativeDisplace(displacement);
}

void CPU6502::instrBEQ(u8)
{
	u8 displacement = relativeAddr();
	if (getZeroFlag())
		relativeDisplace(displacement);
}

void CPU6502::instrBIT(const u8 opcode)
{
	u8 M = 0;
	switch (opcode)
	{
	case 0x24:
		M = read(zeroPageAddr());
		break;

	case 0x2C:
		M = read(absoluteAddr());
		break;
	}
	setZeroFlag((reg_.A & M) == 0);
	setOverflowFlag(getBitN(M, 6));
	setNegativeResultFlag(getBitN(M, 7));
}

void CPU6502::instrBMI(u8)
{
	const u8 displacement = relativeAddr();
	if (getNegativeResultFlag())
		relativeDisplace(displacement);
}

void CPU6502::instrBNE(u8)
{
	u8 displacement = relativeAddr();
	if (!getZeroFlag())
		relativeDisplace(displacement);
}

void CPU6502::instrBPL(u8)
{
	const u8 displacement = relativeAddr();
	if (!getNegativeResultFlag())
		relativeDisplace(displacement);
}

void CPU6502::instrBRK(u8)
{
	pushStack(reg_.PC);
	pushStack(reg_.Status);
	reg_.PC = getTwoBytesFromMem(0xFFFE);
	setBreakFlag(true);
}

void CPU6502::instrBVC(u8)
{
	const u8 displacement = relativeAddr();
	if (!getOverflowFlag())
		relativeDisplace(displacement);
}

void CPU6502::instrBVS(u8)
{
	const u8 displacement = relativeAddr();
	if (getOverflowFlag())
		relativeDisplace(displacement);
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
	u8 M = 0;
	switch (opcode)
	{
	case 0xC9:
		M = immediateAddr();
		break;

	case 0xC5:
		M = read(zeroPageAddr());
		break;

	case 0xD5:
		M = read(zeroPageAddr(reg_.X));
		break;

	case 0xCD:
		M = read(absoluteAddr());
		break;
	
	case 0xDD:
		M = read(absoluteAddr(reg_.X));
		break;

	case 0xD9:
		M = read(absoluteAddr(reg_.Y));
		break;

	case 0xC1:
		M = read(indexedIndirectAddr());
		break;

	case 0xD1:
		M = read(indirectIndexedAddr());
		break;
	}

	const u8 result = reg_.A - M;
	setCarryFlag(reg_.A >= M);
	setZeroFlag(result == 0);
	setNegativeResultFlag(getBitN(result, 7));
}

void CPU6502::instrCPX(const u8 opcode)
{
	u8 M = 0;
	switch (opcode)
	{
	case 0xE0:
		M = immediateAddr();
		break;
	
	case 0xE4:
		M = read(zeroPageAddr());
		break;

	case 0xEC:
		M = read(absoluteAddr());
		break;
	}

	const u8 result = reg_.X - M;
	setCarryFlag(reg_.X >= M);
	setZeroFlag(result == 0);
	setNegativeResultFlag(getBitN(result, 7));
}

void CPU6502::instrCPY(const u8 opcode)
{
	u8 M = 0;
	switch (opcode)
	{
	case 0xC0:
		M = immediateAddr();
		break;

	case 0xC4:
		M = read(zeroPageAddr());
		break;

	case 0xCC:
		M = read(absoluteAddr());
		break;
	}

	const u8 result = reg_.Y - M;
	setCarryFlag(reg_.Y >= M);
	setZeroFlag(result == 0);
	setNegativeResultFlag(getBitN(result, 7));
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
		addr = zeroPageAddr(reg_.X);
		break;

	case 0xCE:
		addr = absoluteAddr();
		break;

	case 0xDE:
		addr = absoluteAddr(reg_.X);
		break;
	}

	u8 val = read(addr);
	write(addr, --val);
	setZeroFlag(val == 0);
	setNegativeResultFlag(getBitN(val, 7));
}

void CPU6502::instrDEX(u8)
{
	--reg_.X;
	setZeroFlag(reg_.X == 0);
	setNegativeResultFlag(getBitN(reg_.X, 7));
}

void CPU6502::instrDEY(u8)
{
	--reg_.Y;
	setZeroFlag(reg_.Y == 0);
	setNegativeResultFlag(getBitN(reg_.Y, 7));
}

void CPU6502::instrEOR(const u8 opcode)
{
	u8 M = 0;
	switch (opcode)
	{
	case 0x49:
		M = immediateAddr();
		break;

	case 0x45:
		M = read(zeroPageAddr());
		break;

	case 0x55:
		M = read(zeroPageAddr(reg_.X));
		break;

	case 0x4D:
		M = read(absoluteAddr());
		break;

	case 0x5D:
		M = read(absoluteAddr(reg_.X));
		break;

	case 0x59:
		M = read(absoluteAddr(reg_.Y));
		break;

	case 0x41:
		M = read(indexedIndirectAddr());
		break;

	case 0x51:
		M = read(indirectIndexedAddr());
		break;
	}

	reg_.A = reg_.A ^ M;
	setZeroFlag(reg_.A == 0);
	setNegativeResultFlag(getBitN(reg_.A, 7));
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
		addr = zeroPageAddr(reg_.X);
		break;

	case 0xEE:
		addr = absoluteAddr();
		break;

	case 0xFE:
		addr = absoluteAddr(reg_.X);
		break;
	}

	u8 val = read(addr);
	write(addr, ++val);
	setZeroFlag(val == 0);
	setNegativeResultFlag(getBitN(val, 7));
}

void CPU6502::instrINX(u8)
{
	++reg_.X;
	setZeroFlag(reg_.X == 0);
	setNegativeResultFlag(getBitN(reg_.X, 7));
}

void CPU6502::instrINY(u8)
{
	++reg_.Y;
	setZeroFlag(reg_.Y == 0);
	setNegativeResultFlag(getBitN(reg_.Y, 7));
}

void CPU6502::instrJMP(const u8 opcode)
{
	switch (opcode)
	{
	case 0x4C:
		reg_.PC = absoluteAddr();
		break;

	case 0x6C:
		reg_.PC = indirectAddr();
		break;
	}
}

void CPU6502::instrJSR(u8)
{
	const u16 destination = absoluteAddr();
	pushStack(--reg_.PC);
	reg_.PC = destination;
}

void CPU6502::instrLDA(const u8 opcode)
{
	u8 M = 0;
	switch (opcode)
	{
	case 0xA9:
		M = immediateAddr();
		break;

	case 0xA5:
		M = read(zeroPageAddr());
		break;

	case 0xB5:
		M = read(zeroPageAddr(reg_.X));
		break;

	case 0xAD:
		M = read(absoluteAddr());
		break;

	case 0xBD:
		M = read(absoluteAddr(reg_.X));
		break;

	case 0xB9:
		M = read(absoluteAddr(reg_.Y));
		break;

	case 0xA1:
		M = read(indexedIndirectAddr());
		break;

	case 0xB1:
		M = read(indirectIndexedAddr());
		break;
	}
	reg_.A = M;
	setZeroFlag(reg_.A == 0);
	setNegativeResultFlag(getBitN(reg_.A, 7));
}

void CPU6502::instrLDX(const u8 opcode)
{
	u8 M = 0;
	switch (opcode)
	{
	case 0xA2:
		M = immediateAddr();
		break;

	case 0xA6:
		M = read(zeroPageAddr());
		break;

	case 0xB6:
		M = read(zeroPageAddr(reg_.Y));
		break;

	case 0xAE:
		M = read(absoluteAddr());
		break;

	case 0xBE:
		M = read(absoluteAddr(reg_.Y));
		break;
	}
	reg_.X = M;
	setZeroFlag(reg_.X == 0);
	setNegativeResultFlag(getBitN(reg_.X, 7));
}

void CPU6502::instrLDY(const u8 opcode)
{
	u8 M = 0;
	switch (opcode)
	{
	case 0xA0:
		M = immediateAddr();
		break;

	case 0xA4:
		M = read(zeroPageAddr());
		break;

	case 0xB4:
		M = read(zeroPageAddr(reg_.X));
		break;

	case 0xAC:
		M = read(absoluteAddr());
		break;

	case 0xBC:
		M = read(absoluteAddr(reg_.X));
		break;
	}
	reg_.Y = M;
	setZeroFlag(reg_.Y == 0);
	setNegativeResultFlag(getBitN(reg_.Y, 7));
}

void CPU6502::instrLSR(const u8 opcode)
{
	u8 val = 0;
	u16 addr = 0;
	switch (opcode)
	{
	case 0x4A:
		val = reg_.A;
		break;

	case 0x46:
		addr = zeroPageAddr();
		val = read(addr);
		break;

	case 0x56:
		addr = zeroPageAddr(reg_.X);
		val = read(addr);
		break;

	case 0x4E:
		addr = absoluteAddr();
		val = read(addr);
		break;

	case 0x5E:
		addr = absoluteAddr(reg_.X);
		val = read(addr);
		break;
	}

	setCarryFlag(getBitN(val, 0));
	val >>= 1;
	setNegativeResultFlag(getBitN(val, 7));

	if (opcode == 0x4A)
		reg_.A = val;
	else
		write(addr, val);
}

void CPU6502::instrNOP(u8)
{
	// nop
}

void CPU6502::instrORA(const u8 opcode)
{
	u8 M = 0;
	switch (opcode)
	{
	case 0x09:
		M = immediateAddr();
		break;

	case 0x05:
		M = read(zeroPageAddr());
		break;

	case 0x15:
		M = read(zeroPageAddr(reg_.X));
		break;

	case 0x0D:
		M = read(absoluteAddr());
		break;

	case 0x1D:
		M = read(absoluteAddr(reg_.X));
		break;

	case 0x19:
		M = read(absoluteAddr(reg_.Y));
		break;

	case 0x01:
		M = read(indexedIndirectAddr());
		break;

	case 0x11:
		M = read(indirectIndexedAddr());
		break;
	}

	reg_.A = reg_.A | M;
	setZeroFlag(reg_.A == 0);
	setNegativeResultFlag(getBitN(reg_.A, 7));
}

void CPU6502::instrPHA(u8)
{
	pushStack(reg_.A);
}

void CPU6502::instrPHP(u8)
{
	setBreakFlag(true);
	setBitN(reg_.Status, 5, true);
	pushStack(reg_.Status);
}

void CPU6502::instrPLA(u8)
{
	reg_.A = popStack();
	setZeroFlag(reg_.A == 0);
	setNegativeResultFlag(getBitN(reg_.A, 7));
}

void CPU6502::instrPLP(u8)
{
	reg_.Status = popStack();
	setBreakFlag(true);
}

void CPU6502::instrROL(const u8 opcode)
{
	u8 val = 0;
	u16 addr = 0;
	switch (opcode)
	{
	case 0x2A:
		val = reg_.A;
		break;

	case 0x26:
		addr = zeroPageAddr();
		val = read(addr);
		break;

	case 0x36:
		addr = zeroPageAddr(reg_.X);
		val = read(addr);
		break;

	case 0x2E:
		addr = absoluteAddr();
		val = read(addr);
		break;

	case 0x3E:
		addr = absoluteAddr(reg_.X);
		val = read(addr);
		break;
	}

	const u8 new_bit0 = getCarryFlag();
	setCarryFlag(getBitN(val, 7));
	val <<= 1;
	setBitN(val, 0, new_bit0);
	setNegativeResultFlag(getBitN(val, 7));

	if (opcode == 0x2A)
		reg_.A = val;
	else
		write(addr, val);
}

void CPU6502::instrROR(const u8 opcode)
{
	u8 val = 0;
	u16 addr = 0;
	switch (opcode)
	{
	case 0x6A:
		val = reg_.A;
		break;

	case 0x66:
		addr = zeroPageAddr();
		val = read(addr);
		break;

	case 0x76:
		addr = zeroPageAddr(reg_.X);
		val = read(addr);
		break;

	case 0x6E:
		addr = absoluteAddr();
		val = read(addr);
		break;

	case 0x7E:
		addr = absoluteAddr(reg_.X);
		val = read(addr);
		break;
	}

	const u8 new_bit7 = getCarryFlag();
	setCarryFlag(getBitN(val, 0));
	val >>= 1;
	setBitN(val, 7, new_bit7);
	setNegativeResultFlag(getBitN(val, 7));

	if (opcode == 0x6A)
		reg_.A = val;
	else
		write(addr, val);
}

void CPU6502::instrRTI(u8)
{
	reg_.Status = popStack();
	reg_.PC = popStackTwoBytes();
}

void CPU6502::instrRTS(u8)
{
	reg_.PC = popStackTwoBytes();
	++reg_.PC;
}

void CPU6502::instrSBC(const u8 opcode)
{
	u8 M = 0;
	switch (opcode)
	{
	case 0xE9:
		M = immediateAddr();
		break;

	case 0xE5:
		M = read(zeroPageAddr());
		break;

	case 0xF5:
		M = read(zeroPageAddr(reg_.X));
		break;

	case 0xED:
		M = read(absoluteAddr());
		break;

	case 0xFD:
		M = read(absoluteAddr(reg_.X));
		break;

	case 0xF9:
		M = read(absoluteAddr(reg_.Y));
		break;

	case 0xE1:
		M = read(indexedIndirectAddr());
		break;

	case 0xF1:
		M = read(indirectIndexedAddr());
		break;
	}

	const u8 one = static_cast<u8>(1);

	const u8 C = getCarryFlag();
	const u8 result = reg_.A - M - (one - C);

	setZeroFlag(result == 0);
	setNegativeResultFlag(getBitN(result, 7));

	const bool overflow = willAddOverflow(reg_.A, ~M, C);
	setOverflowFlag(overflow);
	if (overflow)
		setCarryFlag(false);
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
		addr = zeroPageAddr(reg_.X);
		break;

	case 0x8D:
		addr = absoluteAddr();
		break;

	case 0x9D:
		addr = absoluteAddr(reg_.X);
		break;

	case 0x99:
		addr = absoluteAddr(reg_.Y);
		break;

	case 0x81:
		addr = indexedIndirectAddr();	
		break;

	case 0x91:
		addr = indirectIndexedAddr();
		break;
	}

	write(addr, reg_.A);
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
		addr = zeroPageAddr(reg_.Y);
		break;

	case 0x8E:
		addr = absoluteAddr();
		break;
	}

	write(addr, reg_.X);
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
		addr = zeroPageAddr(reg_.X);
		break;

	case 0x8C:
		addr = absoluteAddr();
		break;
	}

	write(addr, reg_.Y);
}

void CPU6502::instrTAX(u8)
{
	reg_.X = reg_.A;
	setZeroFlag(reg_.X == 0);
	setNegativeResultFlag(getBitN(reg_.X, 7));
}

void CPU6502::instrTAY(u8)
{
	reg_.Y = reg_.A;
	setZeroFlag(reg_.Y == 0);
	setNegativeResultFlag(getBitN(reg_.Y, 7));
}

void CPU6502::instrTSX(u8)
{
	reg_.X = reg_.SP;	
	setZeroFlag(reg_.X == 0);
	setNegativeResultFlag(getBitN(reg_.X, 7));
}

void CPU6502::instrTXA(u8)
{
	reg_.A = reg_.X;
	setZeroFlag(reg_.A == 0);
	setNegativeResultFlag(getBitN(reg_.A, 7));
}

void CPU6502::instrTXS(u8)
{
	reg_.SP = reg_.X;
}

void CPU6502::instrTYA(u8)
{
	reg_.A = reg_.Y;
	setZeroFlag(reg_.A == 0);
	setNegativeResultFlag(getBitN(reg_.A, 7));
}

void CPU6502::unknownOpcode(const u8 opcode)
{
	std::stringstream ss;
	ss << "Unknown opcode: " << std::hex << (int)opcode << '\n';
	ss << "PC: " << std::hex << (int)reg_.PC << '\n';
	throw std::runtime_error{ss.str()};
}

void CPU6502::relativeDisplace(u8 displacement)
{
	const bool is_negative = getBitN(displacement, 7);
	if (is_negative)
	{
		displacement = ~displacement + static_cast<u8>(1);
		reg_.PC -= displacement;
	}
	else
		reg_.PC += displacement;
}

u8 CPU6502::immediateAddr()
{
#ifdef EMUCPULOG
	log_addr_mode = AddrMode::Imme;
#endif
	return getByteFromPC();
}

u8 CPU6502::relativeAddr()
{
#ifdef EMUCPULOG
	log_addr_mode = AddrMode::Relative;
#endif
	return getByteFromPC();
}

u16 CPU6502::zeroPageAddr(const u8 offset)
{
#ifdef EMUCPULOG
	log_addr_mode = AddrMode::ZP;
#endif
	return getByteFromPC() + offset;
}

u16 CPU6502::absoluteAddr(const u8 offset)
{
#ifdef EMUCPULOG
	log_addr_mode = AddrMode::Abs;
#endif
	const u16 off = static_cast<u16>(offset);
	const u16 addr = getTwoBytesFromPC();
	const u16 abs_addr = addr + off;
	if ((abs_addr & 0xFF00) != (addr & 0xFF00))
		++cycles_;
	return getTwoBytesFromPC() + static_cast<u16>(offset);
}

u16 CPU6502::indirectAddr()
{
#ifdef EMUCPULOG
	log_addr_mode = AddrMode::Indirect;
#endif
	const u16 addr = getTwoBytesFromPC();
	if (static_cast<u8>(addr) == 0xFF) // this is a hardware bug
	{
		u16 result = static_cast<u16>(read(addr));
		u16 msb_loc = addr & 0xFF00;
		result |= (static_cast<u16>(read(msb_loc)) << 8);
		return result;
	}
	return getTwoBytesFromMem(addr);
}

u16 CPU6502::indexedIndirectAddr()
{
#ifdef EMUCPULOG
	log_addr_mode = AddrMode::IndexedIndirect;
#endif
	const u8 table_loc = getByteFromPC() + reg_.X;
	return getTwoBytesFromZP(table_loc);
}

u16 CPU6502::indirectIndexedAddr()
{
#ifdef EMUCPULOG
	log_addr_mode = AddrMode::IndirectIndexed;
#endif
	const u8 table_loc = getByteFromPC();
	const u16 addr = getTwoBytesFromZP(table_loc);
	const u16 offset = static_cast<u16>(reg_.Y);
	const u16 abs_addr = addr + offset;
	if ((abs_addr & 0xFF00) != (addr & 0xFF00))
		++cycles_;
	return abs_addr;
}

u8 CPU6502::getByteFromPC()
{
	return read(reg_.PC++);
}

u16 CPU6502::getTwoBytesFromPC()
{
	u16 result = read(reg_.PC++);
	result |= (static_cast<u16>(read(reg_.PC++)) << 8);
	return result;
}

u16 CPU6502::getTwoBytesFromZP(u8 loc)
{
	u16 result = read(loc++);
	result |= (static_cast<u16>(read(loc)) << 8);
	return result;
}

u16 CPU6502::getTwoBytesFromMem(const u16 loc)
{
	u16 result = read(loc);
	result |= (static_cast<u16>(read(loc + 1)) << 8);
	return result;
}

u8 CPU6502::getCarryFlag() const
{
	return getBitN(reg_.Status, 0);
}

u8 CPU6502::getZeroFlag() const
{
	return getBitN(reg_.Status, 1);
}

u8 CPU6502::getInterruptDisableFlag() const
{
	return getBitN(reg_.Status, 2);
}

u8 CPU6502::getDecimalModeFlag() const
{
	return getBitN(reg_.Status, 3);
}

u8 CPU6502::getBreakFlag() const
{
	return getBitN(reg_.Status, 4);
}

u8 CPU6502::getOverflowFlag() const
{
	return getBitN(reg_.Status, 6);
}

u8 CPU6502::getNegativeResultFlag() const
{
	return getBitN(reg_.Status, 7);
}


void CPU6502::setCarryFlag(const bool set)
{
	setBitN(reg_.Status, 0, set);
}

void CPU6502::setZeroFlag(const bool set)
{
	setBitN(reg_.Status, 1, set);
}

void CPU6502::setInterruptDisableFlag(const bool set)
{
	setBitN(reg_.Status, 2, set);
}

void CPU6502::setDecimalModeFlag(const bool set)
{
	setBitN(reg_.Status, 3, set);
}

void CPU6502::setBreakFlag(const bool set)
{
	setBitN(reg_.Status, 4, set);
}

void CPU6502::setOverflowFlag(const bool set)
{
	setBitN(reg_.Status, 6, set);
}

void CPU6502::setNegativeResultFlag(const bool set)
{
	setBitN(reg_.Status, 7, set);
}

void CPU6502::pushStack(const u8 val)
{
	assert(reg_.SP > 0);
	write(stack_low + reg_.SP, val);
	--reg_.SP;
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
	assert(reg_.SP < 255);
	++reg_.SP;
	return read(stack_low + reg_.SP);
}

u16 CPU6502::popStackTwoBytes()
{
	u16 result = popStack();
	result |= (static_cast<u16>(popStack()) << 8);
	return result;
}