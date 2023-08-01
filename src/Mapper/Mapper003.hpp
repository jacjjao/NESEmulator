#pragma once

#include "Mapper.hpp"


class Mapper003 : public Mapper
{
public:
	Mapper003(unsigned prg_bank, unsigned chr_bank) : prg_bank_{ prg_bank }, chr_bank_{ chr_bank }
	{
	}
	~Mapper003() override = default;

	std::optional<std::size_t> cpuMapWrite(const u16 addr, const u8 data) override;
	std::optional<std::size_t> cpuMapRead(const u16 addr) override;
	std::optional<std::size_t> ppuMapRead(const u16 addr) override;

private:
	unsigned prg_bank_, chr_bank_;
	u8 bank_idx_ = 0;
};