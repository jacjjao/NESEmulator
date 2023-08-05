#include "Mapper003.hpp"
#include <limits>


Mapper003::Mapper003(Cartridge cart) :
	Mapper{ std::move(cart) }
{
}

bool Mapper003::cpuMapWrite(const u16 addr, const u8 data)
{
	if (!cart_.useCHRRam() && 0x8000 <= addr && addr <= 0xFFFF)
	{
		chr_bank_start_ = (data & 0x03) * 8_KB;
		return true;
	}
	return false;
}

std::optional<u8> Mapper003::cpuMapRead(const u16 addr)
{
	if (0x8000 <= addr && addr <= 0xFFFF)
	{
		return cart_.PRGRom()[addr & (cart_.PRGRomSize() > 16_KB ? 0x7FFF : 0x3FFF)];
	}
	return std::nullopt;
}

bool Mapper003::ppuMapWrite(const u16 addr, const u8 data)
{
	if (cart_.useCHRRam() && addr <= 0x1FFF)
	{
		cart_.CHRMem()[addr] = data;
		return true;
	}
	return false;
}

std::optional<u8> Mapper003::ppuMapRead(const u16 addr)
{
	if (addr <= 0x1FFF)
	{
		if (cart_.useCHRRam())
		{
			return cart_.CHRMem()[addr];
		}
		return cart_.CHRMem()[chr_bank_start_ + addr];
	}
	return std::nullopt;
}
