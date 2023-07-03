#pragma once

#include "common/type.hpp"
#include "CPU6502.hpp"

#include <array>


class Bus
{
public:
	Bus();

	void write(u16 addr, u8 data);
	u8 read(u16 addr, bool read_only = false);

	CPU6502 cpu;

private:
	std::array<u8, 0xFFFF + 1> mem_;
};