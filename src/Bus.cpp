#include "Bus.hpp"

Bus::Bus()
{
	cpu.connectToBus(this);
	for (auto& i : mem_)
		i = 0;
}

void Bus::write(const u16 addr, const u8 data)
{
	mem_[addr] = data;
}

u8 Bus::read(const u16 addr, bool)
{
	return mem_[addr];
}
