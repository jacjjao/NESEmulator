#pragma once

#include "Mapper.hpp"


class Mapper000 : public Mapper
{
public:
	Mapper000(unsigned prg_bank, unsigned chr_bank) : prg_bank_{ prg_bank }, chr_bank_{ chr_bank } 
	{
	}
	~Mapper000() override = default;

	std::optional<std::size_t> cpuMapRead(const u16 addr) override
	{
		if (0x8000 <= addr && addr <= 0xFFFF)
		{
			return addr & (prg_bank_ > 1 ? 0x7FFF : 0x3FFF);
		}
		return std::nullopt;
	}

	std::optional<std::size_t> ppuMapWrite(const u16 addr, u8) override
	{
		if (chr_bank_ == 0 && 0x0000 <= addr && addr <= 0x1FFF)
		{
			return addr;
		}
		return std::nullopt;
	}

	std::optional<std::size_t> ppuMapRead(const u16 addr) override
	{
		if (0x0000 <= addr && addr <= 0x1FFF)
		{
			return addr;
		}
		return std::nullopt;
	}

private:
	unsigned prg_bank_, chr_bank_;
};