#pragma once

#include "Mapper.hpp"


class Mapper000 : public Mapper
{
public:
	u16 cpuMapWrite(const u16 addr) override
	{
		if (0xC000 <= addr)
		{
			return 0xBFFF & addr;
		}
		return addr;
	}
	u16 cpuMapRead(const u16 addr) override
	{
		if (0xC000 <= addr)
		{
			return 0xBFFF & addr;
		}
		return addr;
	}
};