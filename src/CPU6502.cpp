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

	instrs.resize(256, [this](const u8 opcode) {
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

	default:
		unknownOpcode(opcode);
		return;
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

	default:
		unknownOpcode(opcode);
		return;
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
	default:
		unknownOpcode(opcode);
		return;
	}

	assert(target);
	setCarryFlag(getBitN(*target, 7));
	*target <<= 1;
	setZeroFlag(*target == 0);
	setNegativeResultFlag(getBitN(*target, 7));
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
	u16 mem_loc = mem[table_loc];
	mem_loc += (mem[table_loc + 1] << 8); 
	return mem_loc;
}

u16 CPU6502::indirectIndexedAddr()
{
	const u8 mem_loc = mem[getByteFromPC()];
	const u16 base_addr = static_cast<u16>(mem[mem_loc]);
	const u16 offset = static_cast<u16>(mem[mem_loc + 1]);
	const u16 final_addr = base_addr + offset + static_cast<u16>(reg.Y);
	return final_addr;
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
