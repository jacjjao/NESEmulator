#include "Mapper066.hpp"

Mapper066::Mapper066(Cartridge cart) :
	Mapper{ std::move(cart) }
{
	chr_rom = cart_.CHRMem();
	prg_rom = cart_.PRGRom();
}

bool Mapper066::cpuMapWrite(const u16 addr, const u8 data)
{
	if (addr < 0x8000)
	{
		return false;
	}
	const usize chr_bank = data & 0x03;
	const usize prg_bank = data & 0x30;
	chr_rom = &cart_.CHRMem()[chr_bank *  8_KB];
	prg_rom = &cart_.PRGRom()[prg_bank * 32_KB];
	return true;
}

std::optional<u8> Mapper066::cpuMapRead(const u16 addr)
{
	if (addr < 0x8000)
	{
		return std::nullopt;
	}
	return prg_rom[addr & 0x7FFF];
}

std::optional<u8> Mapper066::ppuMapRead(u16 addr)
{
	if (addr > 0x1FFF)
	{
		return std::nullopt;
	}
	return chr_rom[addr];
}
