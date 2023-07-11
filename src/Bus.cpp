#include "Bus.hpp"

Bus::Bus() : 
	mem_(mem_size, 0)
{
	cpu.connectToBus(this);
}

void Bus::write(const u16 addr, const u8 data)
{
	mem_[addr] = data;
}

u8 Bus::cpuRead(const u16 addr)
{
	return mem_[addr];
}
