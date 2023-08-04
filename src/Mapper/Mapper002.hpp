#pragma once

#include "Mapper.hpp"


class Mapper002 : public Mapper
{
public:
	Mapper002(std::size_t prg_rom_size_in_byte, std::size_t chr_rom_size_in_byte);
	~Mapper002() override = default;

	bool cpuMapWrite(u16 addr, u8 data, usize& mapped_addr) override;
	bool cpuMapRead(u16 addr, usize& mapped_addr) override;

	bool ppuMapWrite(u16 addr, u8 data, usize& mapped_addr) override;
	bool ppuMapRead(u16 addr, usize& mapped_addr) override;

private:
	usize nprg_banks_;
	usize prg_low_ = 0;
	usize prg_high_;

	bool use_chr_ram_;
};