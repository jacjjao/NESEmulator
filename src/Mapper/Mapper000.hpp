#pragma once

#include "Mapper.hpp"


class Mapper000 : public Mapper
{
public:
	Mapper000(unsigned prg_bank, unsigned chr_bank) : prg_bank_{ prg_bank }, chr_bank_{ chr_bank } 
	{
	}
	~Mapper000() override = default;

	std::optional<u16> cpuMapWrite(const u16 addr) override
	{
		if (0x8000 <= addr && addr <= 0xFFFF)
		{
			return addr & (prg_bank_ > 1 ? 0x7FFF : 0x3FFF);
		}
		return std::nullopt;
	}

	std::optional<u16> cpuMapRead(const u16 addr) override
	{
		if (0x8000 <= addr && addr <= 0xFFFF)
		{
			return addr & (prg_bank_ > 1 ? 0x7FFF : 0x3FFF);
		}
		return std::nullopt;
	}

	std::optional<u16> ppuMapRead(const u16 addr) override
	{
		if (0x0000 <= addr && addr <= 0x1FFF)
		{
			return addr;
		}
		return std::nullopt;
	}

	std::optional<u16> ppuMapWrite(const u16 addr) override
	{
		if (0x0000 <= addr && addr <= 0x1FFF && chr_bank_ == 0)
		{
			return addr;
		}
		return std::nullopt;
	}

private:
	unsigned prg_bank_, chr_bank_;
};