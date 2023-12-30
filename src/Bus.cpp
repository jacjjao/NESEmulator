#include "Bus.hpp"

Bus::Bus() : 
	cpu_mem_(cpu_mem_size)
{
}

Bus& Bus::instance()
{
	static Bus bus;
	return bus;
}

void Bus::cpuWrite(const u16 addr, const u8 data)
{
	if (cart_->cpuMapWrite(addr, data))
	{
	}
	else if (addr <= 0x1FFF)
	{
		cpu_mem_[addr & 0x07FF] = data;
	}
	else if (0x2000 <= addr && addr <= 0x3FFF)
	{
		ppu.regWrite(addr & 0x0007, data);
	}
	else if (addr == 0x4014)
	{
		dma_transfer = true;
		dma_addr = (static_cast<u16>(data) << 8);
		oam_addr = ppu.getOAMAddr();
		dma_start = false;
	}
	else if (addr <= 0x4013 || addr == 0x4015 || addr == 0x4017)
	{
		apu.regWrite(addr, data);
	}
	else if (addr == 0x4016)
	{
		joystick.setStrobe(data);
		joystick_cache_ = joystick;
	}
}

u8 Bus::cpuRead(const u16 addr)
{
	if (const auto data = cart_->cpuMapRead(addr); data.has_value())
	{
		return *data;
	}
	else if (addr <= 0x1FFF)
	{
		return cpu_mem_[addr & 0x07FF];
	}
	else if (0x2000 <= addr && addr <= 0x3FFF)
	{
		return ppu.regRead(addr & 0x0007);
	}
	else if (addr == 0x4015)
	{
		return apu.regRead(addr);
	}
	else if (addr == 0x4016)
	{
		return joystick_cache_.report();
	}
	return 0;
}

void Bus::clock()
{
	ppu.cycle();

	if (cycle_ % 3 == 0)
	{
		apu.clock();

		if (dma_transfer)
		{
			if (!dma_start)
			{
				if (cycle_ % 2 == 1)
				{
					dma_start = true;
				}
			}
			else
			{
				if (cycle_ % 2 == 1)
				{
					const u8 data = cpuRead(dma_addr++);
					ppu.OAM()[oam_addr++] = data;
					if ((dma_addr & 0x00FF) == 0x0000)
					{
						dma_transfer = false;
					}
				}
			}
		}
		else
		{
			cpu.cycle();
		}
	}

	++cycle_;
}

void Bus::reset()
{
	cycle_ = 0;
	cpu.reset();
	ppu.reset();
}

Mapper& Bus::cartridge()
{
	return *cart_;
}

void Bus::insertCartridge(std::unique_ptr<Mapper> cart)
{
	cart_ = std::move(cart);
	reset();
}
