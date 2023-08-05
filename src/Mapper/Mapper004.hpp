#pragma once

#include "Mapper.hpp"

/*
class Mapper004 : public Mapper
{
public:
	Mapper004(usize prg_rom_size);
	~Mapper004() override = default;

	bool cpuMapWrite(u16 addr, u8 data, usize& mapped_addr) override;
	bool cpuMapRead(u16 addr, usize& mapped_addr) override;

	bool ppuMapRead(u16 addr, usize& mapped_addr) override;

private:
	const usize nprg_banks_;
	usize prg_banks_[2]{};

	usize chr_2kbanks_[2]{};
	usize chr_1kbanks_[4]{};

	u8 bank_select_ = 0;
	bool prg_bank_mode_ = false;
	bool chr_bank_inversion_ = false;

	bool ram_write_protect_ = false;
	bool ram_enable_ = true;

	u8 irq_counter_ = 0;
	u8 irq_latch_ = 0;
	bool reload_flag_ = false;
	bool irq_enable_ = true;
};
*/