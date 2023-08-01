#include "Mapper003.hpp"
#include <limits>


std::optional<std::size_t> Mapper003::cpuMapWrite(const u16 addr, const u8 data)
{
	if (0x8000 <= addr && addr <= 0xFFFF)
	{
		bank_idx_ = data;
	}
	return std::nullopt;
}

std::optional<std::size_t> Mapper003::cpuMapRead(const u16 addr)
{
	if (0x8000 <= addr && addr <= 0xFFFF)
	{
		return addr & (prg_bank_ > 1 ? 0x7FFF : 0x3FFF);
	}
	return std::nullopt;
}

std::optional<std::size_t> Mapper003::ppuMapRead(const u16 addr)
{
	if (0x0000 <= addr && addr <= 0x1FFF)
	{
		return bank_idx_ * 8192 + addr;
	}
	return std::nullopt;
}