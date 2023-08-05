#include "Mapper002.hpp"

Mapper002::Mapper002(Cartridge cart) :
	Mapper{ std::move(cart) }
{
	prg_high_ = (cart_.PRGRomSize() / 16_KB) - 1;
}

bool Mapper002::cpuMapWrite(const u16 addr, const u8 data)
{
	if (addr < 0x8000)
	{
		return false;
	}
	prg_low_ = (data & 0x07);
	return true;
}

std::optional<u8> Mapper002::cpuMapRead(const u16 addr)
{
	if (addr < 0x8000)
	{
		return std::nullopt;
	}
	if (addr < 0xC000)
	{
		return cart_.PRGRom()[prg_low_ * 16_KB  + (addr & 0x3FFF)];
	}
	return cart_.PRGRom()[prg_high_ * 16_KB + (addr & 0x3FFF)];
}

bool Mapper002::ppuMapWrite(const u16 addr, const u8 data)
{
	if (!cart_.useCHRRam() || addr >= 0x2000)
	{
		return false;
	}
	cart_.CHRMem()[addr] = data;
	return true;
}

std::optional<u8> Mapper002::ppuMapRead(const u16 addr)
{
	if (addr <= 0x1FFF)
	{
		return cart_.CHRMem()[addr];
	}
	return std::nullopt;
}
