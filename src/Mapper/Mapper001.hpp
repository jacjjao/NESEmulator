#pragma once

#include "Mapper.hpp"


class Mapper001 : public Mapper
{
public:
	Mapper001(std::size_t prg_rom_size_in_byte, std::size_t chr_rom_size_in_byte);
	~Mapper001() override = default;

	bool cpuMapWrite(u16 addr, u8 data) override;
	std::optional<u8> cpuMapRead(u16 addr) override;

	bool ppuMapWrite(u16 addr, u8 data) override;
	std::optional<u8> ppuMapRead(u16 addr) override;

	void reset() override;

private:
	void setControlMode(u8 data);

	u8 prg_bank1_ = 0, prg_bank2_ = 0;
	unsigned prg_bank_mode_ = 3;

	u8 chr_bank1_ = 0, chr_bank2_ = 0;
	unsigned chr_bank_mode_ = false;
	
	u8 shift_reg_ = 0x10;

	Memory prg_rom_, chr_rom_;
};