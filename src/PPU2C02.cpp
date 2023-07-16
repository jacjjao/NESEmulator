#include "PPU2C02.hpp"
#include <cassert>


PPU2C02::PPU2C02() : mem_(mem_size, 0)
{
}

u8 PPU2C02::read(const u16 addr) const
{
	if (0x3000 <= addr && addr <= 0x3EFF)
	{
		return mem_[addr & 0x2EFF];
	}
	else if (0x3F20 <= addr && addr <= 0x3FFF)
	{
		return mem_[addr & 0x3F1F];
	}
	return mem_[addr];
}

void PPU2C02::write(const u16 addr, const u8 data)
{
	if (0x3000 <= addr && addr <= 0x3EFF)
	{
		mem_[addr & 0x2EFF] = data;
	}
	else if (0x3F20 <= addr && addr <= 0x3FFF)
	{
		mem_[addr & 0x3F1F] = data;
	}
	else
	{
		mem_[addr] = data;
	}
}

u8* PPU2C02::getPatternTable(const u16 index)
{
	assert(index <= 1);
	const u16 addr_start = (index) ? 0x1000 : 0x0000;
	return &mem_[addr_start];
}

u8* PPU2C02::getNameTable(const u16 index)
{
	assert(index <= 3);
	const u16 addr_start = 0x2000 + 0x0400 * index;
	return &mem_[addr_start];
}
