#pragma once

#include "Mapper.hpp"
#include <vector>


class Mapper001 : public Mapper
{
public:
	Mapper001(std::size_t chr_rom_size_in_byte);
	~Mapper001() override = default;

	bool cpuMapWrite(u16 addr, u8 data, usize& mapped_addr) override;
	bool cpuMapRead(u16 addr, usize& mapped_addr) override;

	bool ppuMapWrite(u16 addr, u8 data, usize& mapped_addr) override;
	bool ppuMapRead(u16 addr, usize& mapped_addr) override;

	void reset() override;

private:
	void setControlMode(u8 data);

	u8 prg_bank0_ = 0, prg_bank1_ = 0;
	unsigned prg_bank_mode_ = 3;

	u8 chr_bank0_ = 0, chr_bank1_ = 0;
	unsigned chr_bank_mode_ = 0;
	
	u8 shift_reg_ = 0x10;
	bool use_chr_ram_;
	
	bool prg_ram_disable_ = false;
};
