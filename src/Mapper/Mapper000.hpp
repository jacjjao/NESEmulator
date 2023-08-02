#pragma once

#include "Mapper.hpp"


class Mapper000 : public Mapper
{
public:
	Mapper000(unsigned prg_bank, unsigned chr_bank) : 
		Mapper{}
		prg_bank_{ prg_bank }, chr_bank_{ chr_bank } 
	{
	}
	~Mapper000() override = default;

	bool ppuMapWrite(const u16 addr, const u8 data) override
	{
		if (chr_bank_ == 0 && 0x0000 <= addr && addr <= 0x1FFF)
		{
			
		}
		return false;
	}

	std::optional<u8> cpuMapRead(const u16 addr) override
	{
		if (0x8000 <= addr && addr <= 0xFFFF)
		{
			
		}
		return std::nullopt;
	}

	std::optional<u8> ppuMapRead(const u16 addr) override
	{
		if (0x0000 <= addr && addr <= 0x1FFF)
		{
			
		}
		return std::nullopt;
	}

private:
	unsigned prg_bank_, chr_bank_;
};