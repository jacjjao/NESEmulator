#pragma once

#include "common/type.hpp"
#include "PPU2C02.hpp"
#include "CPU6502.hpp"

#include <vector>


class Bus
{
public:
	Bus();

	void cpuWrite(u16 addr, u8 data);
	u8 cpuRead(u16 addr);

	void ppuWrite(u16 addr, u8 data);
	u8 ppuRead(u16 addr);

	PPU2C02 ppu;
	CPU6502 cpu;

private:
	static constexpr std::size_t cpu_mem_size = 64 * 1024; // 64 kB
	static constexpr std::size_t ppu_mem_size = 16 * 1024; // 16 kB
	static constexpr std::size_t ppu_oam_size = 64;

	std::vector<u8> cpu_mem_;
	std::vector<u8> ppu_oam_;
	std::vector<u8> ppu_mem_;
};