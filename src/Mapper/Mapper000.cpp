#include "Mapper000.hpp"

Mapper000::Mapper000(const std::size_t prg_rom_size_in_byte, const std::size_t chr_rom_size_in_byte) :
	prg_rom_{ 0, prg_rom_size_in_byte },
	chr_mem_{ 0, (chr_rom_size_in_byte ? chr_rom_size_in_byte : 8 * 1024) },
	is_chr_ram_{ chr_rom_size_in_byte == 0 }
{
}

std::optional<u8> Mapper000::cpuMapRead(const u16 addr)
{
	if (0x8000 <= addr && addr <= 0xFFFF)
	{
		constexpr std::size_t eight_kiB = 8 * 1024;
		return prg_rom_[addr & (prg_rom_.getMemSize() > eight_kiB ? 0x7FFF : 0x3FFF)];
	}
	return std::nullopt;
}

bool Mapper000::ppuMapWrite(const u16 addr, const u8 data)
{
	if (is_chr_ram_ && 0x0000 <= addr && addr <= 0x1FFF)
	{
		chr_mem_[addr] = data;
		return true;
	}
	return false;
}

std::optional<u8> Mapper000::ppuMapRead(const u16 addr)
{
	if (0x0000 <= addr && addr <= 0x1FFF)
	{
		return chr_mem_[addr];
	}
	return std::nullopt;
}

void Mapper000::loadPrgRom(u8* data_begin, u8* data_end)
{
	prg_rom_.assign(data_begin, data_end);
}

void Mapper000::loadChrRom(u8* data_begin, u8* data_end)
{
	chr_mem_.assign(data_begin, data_end);
}

