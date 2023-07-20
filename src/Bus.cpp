#include "Bus.hpp"

Bus::Bus() : 
	cpu_mem_(cpu_mem_size, 0)
{
	cpu.connectToBus(this);
}

void Bus::cpuWrite(u16 addr, const u8 data)
{
	addr = getMapper()->cpuMapWrite(addr);
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

u8 Bus::cpuRead(u16 addr)
{
	addr = getMapper()->cpuMapRead(addr);
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
	ppu.write(getMapper()->ppuMapWrite(addr), data);
}

u8 Bus::ppuRead(const u16 addr)
{
	return ppu.read(getMapper()->ppuMapRead(addr));
}

Mapper* Bus::getMapper()
{
	return cartridge_->getMapper();
}

void Bus::insertCartridge(std::unique_ptr<Cartridge> cartridge)
{
	cartridge_ = std::move(cartridge);

	const std::vector<u8>& prg_rom = cartridge_->getPRGRom();
	for (u16 i = 0; i < prg_rom.size(); ++i)
		cpuWrite(0x8000 + i, prg_rom[i]);

	const std::vector<u8>& chr_rom = cartridge_->getCHRRom();
	for (u16 i = 0; i < chr_rom.size(); ++i)
		ppuWrite(0x0000 + i, chr_rom[i]);
}
