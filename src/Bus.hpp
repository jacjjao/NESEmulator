#pragma once

#include "common/type.hpp"
#include "Cartridge.hpp"
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

	Mapper* getMapper();
	void insertCartridge(std::unique_ptr<Cartridge> cartridge);

private:
	static constexpr std::size_t cpu_mem_size = 64 * 1024; // 64 kB

	std::vector<u8> cpu_mem_;

	std::unique_ptr<Cartridge> cartridge_;
};