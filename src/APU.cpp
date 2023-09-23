#include "APU.hpp"
#include "common/bitHelper.hpp"

#include <cassert>


void APU::clock()
{
	// clock frame sequencer
	
}

void APU::regWrite(u16 addr, const u8 data)
{
	addr &= 0x0FF;
	assert(addr <= 0x13 || addr == 0x15 || addr == 0x17);

	switch (addr)
	{
	case 0x17:
		// TODO implement delay write
		frame_sequencer_mode_ = getBitN(data, 7);
		irq_inhibit_flag_ = getBitN(data, 6);
		frame_sequencer_step_ = 0;
		break;
	}
}

u8 APU::regRead(const u16 addr)
{
	return u8();
}
