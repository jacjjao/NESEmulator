#pragma once

#include "Mapper.hpp"


class Mapper003 : public Mapper
{
public:
	Mapper003(std::size_t prg_rom_size_in_byte, std::size_t chr_rom_size_in_byte);
	~Mapper003() override = default;

	bool cpuMapWrite(u16 addr, u8 data, usize& mapped_addr) override;
	bool cpuMapRead(u16 addr, usize& mapped_addr) override;

	bool ppuMapWrite(u16 addr, u8 data, usize& mapped_addr) override;
	bool ppuMapRead(u16 addr, usize& mapped_addr) override;

private:
	usize chr_bank_index_ = 0, prg_banks_;
	bool use_chr_ram_;
};