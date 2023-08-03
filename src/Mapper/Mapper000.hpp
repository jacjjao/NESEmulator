#pragma once

#include "Mapper.hpp"


class Mapper000 : public Mapper
{
public:
	Mapper000(usize prg_rom_size_in_byte, usize chr_rom_size_in_byte);
	~Mapper000() override = default;

	bool cpuMapRead(u16 addr, usize& mapped_addr) override;

	bool ppuMapWrite(u16 addr, u8 data, usize& mapped_addr) override;
	bool ppuMapRead(u16 addr, usize& mapped_addr) override;

private:
	usize prg_banks_;
	bool use_chr_ram_;
};