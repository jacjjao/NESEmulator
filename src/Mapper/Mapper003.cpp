#include "Mapper003.hpp"
#include <limits>


Mapper003::Mapper003(std::size_t prg_rom_size_in_byte, std::size_t chr_rom_size_in_byte) :
	prg_banks_{ prg_rom_size_in_byte / 16_KB },
	use_chr_ram_{ chr_rom_size_in_byte == 0 }
{
}

bool Mapper003::cpuMapWrite(const u16 addr, const u8 data, usize& mapped_addr)
{
	if (!use_chr_ram_ && 0x8000 <= addr && addr <= 0xFFFF)
	{
		chr_bank_index_ = data & 0x03;
		return true;
	}
	return false;
}

bool Mapper003::cpuMapRead(const u16 addr, usize& mapped_addr)
{
	if (0x8000 <= addr && addr <= 0xFFFF)
	{
		mapped_addr = addr & (prg_banks_ > 1 ? 0x7FFF : 0x3FFF);
		return true;
	}
	return false;
}

bool Mapper003::ppuMapWrite(const u16 addr, const u8 data, usize& mapped_addr)
{
	if (use_chr_ram_ && addr <= 0x1FFF)
	{
		mapped_addr = addr;
		return true;
	}
	return false;
}

bool Mapper003::ppuMapRead(const u16 addr, usize& mapped_addr)
{
	if (addr <= 0x1FFF)
	{
		if (use_chr_ram_)
		{
			mapped_addr = addr;
		}
		else
		{
			mapped_addr = chr_bank_size * chr_bank_index_ + addr;
		}
		return true;
	}
	return false;
}
