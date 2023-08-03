#include "Mapper000.hpp"

Mapper000::Mapper000(const usize prg_rom_size_in_byte, const usize chr_rom_size_in_byte) :
	prg_banks_{ prg_rom_size_in_byte / 16_KB },
	use_chr_ram_{ chr_rom_size_in_byte == 0 }
{
}

bool Mapper000::cpuMapRead(const u16 addr, usize& mapped_addr)
{
	if (0x8000 <= addr && addr <= 0xFFFF)
	{
		mapped_addr = addr & (prg_banks_ > 1 ? 0x7FFF : 0x3FFF);
		return true;
	}
	return false;
}

bool Mapper000::ppuMapWrite(const u16 addr, u8, usize& mapped_addr)
{
	if (use_chr_ram_ && addr <= 0x1FFF)
	{
		mapped_addr = addr;
		return true;
	}
	return false;
}

bool Mapper000::ppuMapRead(const u16 addr, usize& mapped_addr)
{
	if (addr <= 0x1FFF)
	{
		mapped_addr = addr;
		return true;
	}
	return false;
}
