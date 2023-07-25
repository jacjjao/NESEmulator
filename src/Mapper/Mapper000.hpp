#pragma once

#include "Mapper.hpp"


class Mapper000 : public Mapper
{
public:
	Mapper000(int bank) : banks_(bank) {}
	~Mapper000() override = default;

	std::optional<u16> cpuMapRead(const u16 addr) override
	{
		if (0x8000 <= addr && addr <= 0xFFFF)
		{
			return addr & (banks_ > 1 ? 0x7FFF : 0x3FFF);
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

private:
	int banks_;
};