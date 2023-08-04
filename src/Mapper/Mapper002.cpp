#include "Mapper002.hpp"

Mapper002::Mapper002(const std::size_t prg_rom_size_in_byte, const std::size_t chr_rom_size_in_byte) :
	nprg_banks_{ prg_rom_size_in_byte / 16_KB },
	use_chr_ram_{ chr_rom_size_in_byte == 0 }
{
	prg_high_ = nprg_banks_ - 1;
}

bool Mapper002::cpuMapWrite(const u16 addr, const u8 data, usize&)
{
	if (addr < 0x8000)
	{
		return false;
	}
	prg_low_ = (data & 0x07);
	return true;
}

bool Mapper002::cpuMapRead(const u16 addr, usize& mapped_addr)
{
	if (addr < 0x8000)
	{
		return false;
	}
	if (addr < 0xC000)
	{
		mapped_addr = prg_low_ * 16_KB  + (addr & 0x3FFF);
	}
	else
	{
		mapped_addr = prg_high_ * 16_KB + (addr & 0x3FFF);
	}
	return true;
}

bool Mapper002::ppuMapWrite(const u16 addr, const u8 data, usize& mapped_addr)
{
	if (!use_chr_ram_ || addr >= 0x2000)
	{
		return false;
	}
	mapped_addr = addr;
	return true;
}

bool Mapper002::ppuMapRead(const u16 addr, usize& mapped_addr)
{
	if (addr <= 0x1FFF)
	{
		mapped_addr = addr;
		return true;
	}
	return false;
}
