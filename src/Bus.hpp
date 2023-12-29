#pragma once

#include "common/type.hpp"
#include "Cartridge.hpp"
#include "PPU2C02.hpp"
#include "CPU6502.hpp"
#include "APU.hpp"
#include "Controller/StandardController.hpp"
#include "Mapper/Mapper.hpp"
#include <vector>


class Bus
{
public:
	static Bus& instance();

	void cpuWrite(u16 addr, u8 data);
	u8 cpuRead(u16 addr);

	void clock();

	void reset();

	PPU2C02 ppu;
	CPU6502 cpu;
	APU apu;
	StandardController joystick;
	Mapper& cartridge();

	void insertCartridge(std::unique_ptr<Mapper> cart);

private:
	static constexpr std::size_t cpu_mem_size = 2_KB;

	Bus();

	StandardController joystick_cache_;

	std::vector<u8> cpu_mem_;

	std::unique_ptr<Mapper> cart_;

	bool dma_transfer = false, dma_start = false;
	u16 dma_addr = 0;
	u8 oam_addr = 0;

	u64 cycle_ = 0;
};