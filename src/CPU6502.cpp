#include "CPU6502.hpp"
#include "common/bitHelper.hpp"
#include "common/utility.hpp"

#include <algorithm>
#include <iostream>
#include <cstring>
#include <cassert>


CPU6502::CPU6502() : 
	mem(new u8[mem_size])
{
	memset(mem, 0, mem_size);

	reg.SP = &mem[stack_begin];

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
	setZeroFlag(reg.A & M);
	setOverflowFlag(getBitN(M, 6));
	setNegativeResultFlag(M);
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
	u16 addr = 0;
	switch (opcode)
	{
	case 0xA9:
		addr = immediateAddr();
		break;

	case 0xA5:
		addr = zeroPageAddr();
		break;

	case 0xB5:
		addr = zeroPageAddr(reg.X);
		break;

	case 0xAD:
		addr = absoluteAddr();
		break;

	case 0xBD:
		addr = absoluteAddr(reg.X);
		break;

	case 0xB9:
		addr = absoluteAddr(reg.Y);
		break;

	case 0xA1:
		addr = indexedIndirectAddr();
		break;

	case 0xB1:
		addr = indirectIndexedAddr();
		break;
	}

	reg.A = mem[addr];
	setZeroFlag(reg.A);
	setNegativeResultFlag(reg.A);
}

void CPU6502::instrLDX(const u8 opcode)
{
	u16 addr = 0;
	switch (opcode)
	{
	case 0xA2:
		addr = immediateAddr();
		break;

	case 0xA6:
		addr = zeroPageAddr();
		break;

	case 0xB6:
		addr = zeroPageAddr(reg.Y);
		break;

	case 0xAE:
		addr = absoluteAddr();
		break;

	case 0xBE:
		addr = absoluteAddr(reg.Y);
		break;
	}

	reg.X = mem[addr];
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
		addr = zeroPageAddr();
		break;

	case 0xB4:
		addr = zeroPageAddr(reg.X);
		break;

	case 0xAC:
		addr = absoluteAddr();
		break;

	case 0xBC:
		addr = absoluteAddr(reg.X);
		break;
	}

	reg.Y = mem[addr];
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
	setBitN(*target, new_bit0, 0);
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
	setBitN(*target, new_bit7, 7);
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
	setBitN(reg.Status, set, 0);
}

void CPU6502::setZeroFlag(const u8 result)
{
	setBitN(reg.Status, (result == 0), 1);
}

void CPU6502::setInterruptDisableFlag(const bool set)
{
	setBitN(reg.Status, set, 2);
}

void CPU6502::setDecimalModeFlag(const bool set)
{
	setBitN(reg.Status, set, 3);
}

void CPU6502::setBreakFlag(const bool set)
{
	setBitN(reg.Status, set, 4);
}

void CPU6502::setOverflowFlag(const bool set)
{
	setBitN(reg.Status, set, 6);
}

void CPU6502::setNegativeResultFlag(const u8 result)
{
	setBitN(reg.Status, getBitN(result, 7), 7);
}

void CPU6502::pushStack(const u8 val)
{
	assert(reg.SP > &mem[stack_end]);
	*reg.SP = val;
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
	assert(reg.SP < &mem[stack_begin]);
	++reg.SP;
	return *reg.SP;
}

u16 CPU6502::popStackTwoBytes()
{
	u16 result = popStack();
	result += (static_cast<u16>(popStack()) << 8);
	return result;
}
