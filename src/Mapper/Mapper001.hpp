#pragma once

#include "Mapper.hpp"
#include <vector>


class Mapper001 : public Mapper
{
public:
	Mapper001(Cartridge cart);
	~Mapper001() override = default;

	bool cpuMapWrite(u16 addr, u8 data) override;
	std::optional<u8> cpuMapRead(u16 addr) override;

	bool ppuMapWrite(u16 addr, u8 data) override;
	std::optional<u8> ppuMapRead(u16 addr) override;

	void reset() override;

private:
	const usize nprg_banks_;

	u8 prg16_bank_low_ = 0, prg16_bank_high_ = 0;
	u8 prg32_bank_ = 0;
	u8 prg_bank_mode_ = 0;

	u8 chr4_bank_low_ = 0, chr4_bank_high_ = 0;
	u8 chr8_bank_ = 0;
	bool chr_bank_mode_ = 0;
	
	u8 shift_reg_ = 0x10, load_reg_ = 0;

	std::vector<u8> prg_ram_;
};
