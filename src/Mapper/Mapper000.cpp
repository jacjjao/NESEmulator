#include "Mapper000.hpp"

Mapper000::Mapper000(Cartridge cart) :
	Mapper{ std::move(cart) }
{
}

std::optional<u8> Mapper000::cpuMapRead(const u16 addr)
{
	if (0x8000 <= addr && addr <= 0xFFFF)
	{
		return cart_.PRGRom()[addr & (cart_.PRGRomSize() > 16_KB ? 0x7FFF : 0x3FFF)];
	}
	return std::nullopt;
}

bool Mapper000::ppuMapWrite(const u16 addr, const u8 data)
{
	if (cart_.useCHRRam() && addr <= 0x1FFF)
	{
		cart_.CHRMem()[addr] = data;
		return true;
	}
	return false;
}

std::optional<u8> Mapper000::ppuMapRead(const u16 addr)
{
	if (addr <= 0x1FFF)
	{
		return cart_.CHRMem()[addr];
	}
	return std::nullopt;
}
