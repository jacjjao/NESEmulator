#pragma once

#include "Mapper.hpp"


class Mapper004 : public Mapper
{
public:
	Mapper004(Cartridge cart);
	~Mapper004() override = default;

	bool cpuMapWrite(u16 addr, u8 data) override;
	std::optional<u8> cpuMapRead(u16 addr) override;

	std::optional<u8> ppuMapRead(u16 addr) override;

	void updateIRQCounter() override;

private:
	u8* prg_banks_[2]{};
	u8* fix_prg_second_last_;
	u8* fix_prg_last_;

	u8* chr_2kbanks_[2]{};
	u8* chr_1kbanks_[4]{};

	u8 bank_select_ = 0;
	bool prg_bank_mode_ = false;
	bool chr_bank_inversion_ = false;

	bool ram_write_protect_ = false;
	bool ram_enable_ = true;

	int irq_counter_ = 0;
	u8 irq_latch_ = 0;
	bool irq_enable_ = false;
	bool reload_flag_ = false;

	unsigned scanline_tracker_ = 100000;

	std::vector<u8> prg_ram_;
};
