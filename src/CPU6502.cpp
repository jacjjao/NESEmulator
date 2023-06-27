#include "CPU6502.hpp"
#include "common/bitHelper.hpp"

#include <algorithm>
#include <iostream>
#include <cstring>


CPU6502::CPU6502() : 
	mem(0xFFFF, 0)
{
	ins.resize(256, [this](const u8 opcode) {
		unknownOpcode(opcode);
	});

	const auto ADC = [this](const u8 opcode) {
		opcodeADC(opcode);
	};
	ins[0x69] = ins[0x65] = ins[0x75] = ins[0x6D] = ADC;
	ins[0x7D] = ins[0x79] = ins[0x61] = ins[0x71] = ADC;
}

void CPU6502::update()
{
	const u8 opcode = getByteFromPC();
	ins[opcode](opcode);
}

void CPU6502::opcodeADC(const u8 opcode)
{
	u8 result = 0;
	switch (opcode)
	{
	case 0x69:
		result = ADCImmediate(getByteFromPC());
		break;
	
	case 0x65:
		result = ADCZeroPage(getByteFromPC(), 0);
		break;

	case 0x75:
		result = ADCZeroPage(getByteFromPC(), reg.X);
		break;

	case 0x6D:
		result = ADCAbsolute(getTwoBytesFromPC(), 0);
		break;

	case 0x7D:
		result = ADCAbsolute(getTwoBytesFromPC(), reg.X);
		break;

	case 0x79:
		result = ADCAbsolute(getTwoBytesFromPC(), reg.Y);
		break;

	case 0x61:
		result = ADCIndexedIndirect(getByteFromPC());
		break;

	case 0x71:
		result = ADCIndirectIndexed(getByteFromPC());
		break;

	default:
		unknownOpcode(opcode);
		return;
	}

	setCarryFlag(result < reg.A);
	setOverflowFlag(getBitN(result, 7) != getBitN(reg.A, 7)); // if the sign bit is incorrect
	setZeroFlag(result == 0);
	setNegativeResultFlag(getBitN(result, 7));

	reg.A = result;
}

u8 CPU6502::ADCImmediate(const u8 immediate)
{
	return reg.A + immediate + getCarryFlag();
}

u8 CPU6502::ADCZeroPage(const u8 address, const u8 offset)
{
	const u8 mem_loc = address + offset;
	return reg.A + mem[mem_loc] + getCarryFlag();
}

u8 CPU6502::ADCAbsolute(const u16 address, u8 offset)
{
	const u16 mem_loc = address + static_cast<u16>(offset);
	return reg.A + mem[mem_loc] + getCarryFlag();
}

u8 CPU6502::ADCIndexedIndirect(const u8 address)
{
	const u8 table_loc = address + reg.X;
	u16 mem_loc = mem[table_loc];
	mem_loc += (mem[table_loc + 1] << 8); 
	return reg.A + mem[mem_loc] + getCarryFlag();
}

u8 CPU6502::ADCIndirectIndexed(const u8 address)
{
	const u8 mem_loc = mem[address];
	const u16 base_addr = static_cast<u16>(mem[mem_loc]);
	const u16 offset = static_cast<u16>(mem[mem_loc + 1]);
	const u16 final_addr = base_addr + offset + static_cast<u16>(reg.Y);
	return reg.A + mem[final_addr] + getCarryFlag();
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
