#pragma once

#include "common/type.hpp"
#include "CPU6502.hpp"

#include <vector>


class Bus
{
public:
	Bus();

	void write(u16 addr, u8 data);
	u8 cpuRead(u16 addr);

	CPU6502 cpu;

private:
	static constexpr std::size_t mem_size = 0xFFFF + 1;
	std::vector<u8> mem_;
};