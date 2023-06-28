#include "CPU6502.hpp"
#include "common/bitHelper.hpp"

#include <algorithm>
#include <iostream>
#include <cstring>
#include <cassert>


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
}

CPU6502::~CPU6502()
{
	delete[] mem;
}

void CPU6502::update()
{
	const u8 opcode = getByteFromPC();
	instrs[opcode](opcode);
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

	u8 result = reg.A + mem[addr] + getCarryFlag();

	setCarryFlag(result < reg.A);
	setOverflowFlag(getBitN(result, 7) != getBitN(reg.A, 7));
	setZeroFlag(result == 0);
	setNegativeResultFlag(getBitN(result, 7));

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

	setZeroFlag(reg.A == 0);
	setNegativeResultFlag(getBitN(reg.A, 7));
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
	setZeroFlag(*target == 0);
	setNegativeResultFlag(getBitN(*target, 7));
}

void CPU6502::instrBBC(u8)
{
	if (getCarryFlag())
		return;
	const u8 displacement = getByteFromPC();
	reg.PC += displacement;
}

void CPU6502::instrBCS(u8)
{
	if (!getCarryFlag())
		return;
	const u8 displacement = getByteFromPC();
	reg.PC += displacement;
}

void CPU6502::instrBEQ(u8)
{
	if (!getZeroFlag())
		return;
	const u8 displacement = getByteFromPC();
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
	setZeroFlag((reg.A & M) == 0);
	setOverflowFlag(getBitN(M, 6));
	setNegativeResultFlag(getBitN(M, 7));
}

void CPU6502::instrBMI(u8)
{
	if (!getNegativeResultFlag())
		return;
	const u8 displacement = getByteFromPC();
	reg.PC += displacement;
}

void CPU6502::instrBNE(u8)
{
	if (getZeroFlag())
		return;
	const u8 displacement = getByteFromPC();
	reg.PC += displacement;
}

void CPU6502::instrBPL(u8)
{
	if (!getZeroFlag())
		return;
	const u8 displacement = getByteFromPC();
	reg.PC += displacement;
}

void CPU6502::instrBRK(u8)
{
	// !TODO
}

void CPU6502::instrBVC(u8)
{
	if (getOverflowFlag())
		return;
	const u8 displacement = getByteFromPC();
	reg.PC += displacement;
}

void CPU6502::instrBVS(u8)
{
	if (!getOverflowFlag())
		return;
	const u8 displacement = getByteFromPC();
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
	setCarryFlag(reg.A >= M);
	setZeroFlag(reg.A == M);
	setNegativeResultFlag(getBitN(reg.A - M, 7));
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
	setCarryFlag(reg.X >= M);
	setZeroFlag(reg.X == M);
	setNegativeResultFlag(getBitN(reg.X - M, 7));
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
	setCarryFlag(reg.Y >= M);
	setZeroFlag(reg.Y == M);
	setNegativeResultFlag(getBitN(reg.Y - M, 7));
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
	setZeroFlag(mem[addr] == 0);
	setNegativeResultFlag(getBitN(mem[addr], 7));
}

void CPU6502::instrDEX(u8)
{
	--reg.X;
	setZeroFlag(reg.X == 0);
	setNegativeResultFlag(getBitN(reg.X, 7));
}

void CPU6502::instrDEY(u8)
{
	--reg.Y;
	setZeroFlag(reg.Y == 0);
	setNegativeResultFlag(getBitN(reg.Y, 7));
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
	setZeroFlag(reg.A == 0);
	setNegativeResultFlag(getBitN(reg.A, 7));
}

u16 CPU6502::immediateAddr()
{
	return getByteFromPC();
}

u16 CPU6502::zeroPageAddr(const u8 offset)
{
	return getByteFromPC() + offset;
}

u16 CPU6502::absoluteAddr(u8 offset)
{
	return getTwoBytesFromPC() + static_cast<u16>(offset);
}

u16 CPU6502::indexedIndirectAddr()
{
	const u8 table_loc = getByteFromPC() + reg.X;
	return getTwoBytesFromZP(table_loc);
}

u16 CPU6502::indirectIndexedAddr()
{
	const u8 mem_loc = getByteFromPC();
	return getTwoBytesFromZP(mem_loc) + static_cast<u16>(reg.Y);
}

void CPU6502::unknownOpcode(const u8 opcode)
{
	std::cerr << "Unknown opcode " << opcode << '\n';
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
	return static_cast<u8>(reg.status[0]);
}

u8 CPU6502::getZeroFlag() const
{
	return static_cast<u8>(reg.status[1]);
}

u8 CPU6502::getInterruptDisableFlag() const
{
	return static_cast<u8>(reg.status[2]);
}

u8 CPU6502::getDecimalModeFlag() const
{
	return static_cast<u8>(reg.status[3]);
}

u8 CPU6502::getBreakFlag() const
{
	return static_cast<u8>(reg.status[4]);
}

u8 CPU6502::getOverflowFlag() const
{
	return static_cast<u8>(reg.status[6]);
}

u8 CPU6502::getNegativeResultFlag() const
{
	return static_cast<u8>(reg.status[7]);
}


void CPU6502::setCarryFlag(const bool set)
{
	reg.status.set(0, set);
}

void CPU6502::setZeroFlag(const bool set)
{
	reg.status.set(1, set);
}

void CPU6502::setInterruptDisableFlag(const bool set)
{
	reg.status.set(2, set);
}

void CPU6502::setDecimalModeFlag(const bool set)
{
	reg.status.set(3, set);
}

void CPU6502::setBreakFlag(const bool set)
{
	reg.status.set(4, set);
}

void CPU6502::setOverflowFlag(const bool set)
{
	reg.status.set(6, set);
}

void CPU6502::setNegativeResultFlag(const bool set)
{
	reg.status.set(7, set);
}
