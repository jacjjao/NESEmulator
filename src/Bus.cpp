#include "Bus.hpp"

Bus::Bus() : 
	cpu_mem_(cpu_mem_size, 0),
	ppu_mem_(ppu_mem_size, 0),
	ppu_oam_(ppu_oam_size, 0)
{
	cpu.connectToBus(this);
}

void Bus::cpuWrite(const u16 addr, const u8 data)
{
	if (0x0800 <= addr && addr <= 0x1FFF)
	{
		cpu_mem_[addr & 0x07FF] = data;
	}
	else if (0x2000 <= addr && addr <= 0x3FFF)
	{
		ppu.reg[addr & 0x0007] = data;
	}
	else
	{
		cpu_mem_[addr] = data;
	}
}

u8 Bus::cpuRead(const u16 addr)
{
	if (0x0800 <= addr && addr <= 0x1FFF)
	{
		return cpu_mem_[addr & 0x07FF];
	}
	else if (0x2000 <= addr && addr <= 0x3FFF)
	{
		return ppu.reg[addr & 0x0007];
	}
	return cpu_mem_[addr];
}

void Bus::ppuWrite(const u16 addr, const u8 data)
{
	ppu_mem_[addr] = data;
}

u8 Bus::ppuRead(const u16 addr)
{
	return ppu_mem_[addr];
}
