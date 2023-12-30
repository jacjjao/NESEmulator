#include "APU.hpp"
#include "Bus.hpp"


void APU::clock()
{
	// clock frame sequencer
	if (cpu_cycle_count_ % 2 == 0)
	{
		clockFrameCounter();
	}
	++cpu_cycle_count_;
}

void APU::regWrite(const u16 addr, const u8 data)
{
	assert(addr <= 0x4013 || addr == 0x4015 || addr == 0x4017);

	const auto getLenCntValue = [](u8 data) -> u8 {
		data >>= 3;
		return LEN_CNT_TABLE[data];
	};

	switch (addr)
	{
	case 0x4000:
		pulse1_.setLenCntHalt(data & 0x20);
		break;

	case 0x4003:
		pulse1_.loadLenCnt(getLenCntValue(data));
		break;

	case 0x4004:
		pulse2_.setLenCntHalt(data & 0x20);
		break;

	case 0x4007:
		pulse2_.loadLenCnt(getLenCntValue(data));
		break;

	case 0x4008:
		triangle_.setLenCntHalt(data & 0x80);
		break;

	case 0x400B:
		triangle_.loadLenCnt(getLenCntValue(data));
		break;

	case 0x400C:
		noise_.setLenCntHalt(data & 0x20);
		break;

	case 0x400F:
		noise_.loadLenCnt(getLenCntValue(data));
		break;

	case 0x4015:
		noise_.setEnable(data & 0x8);
		triangle_.setEnable(data & 0x4);
		pulse2_.setEnable(data & 0x2);
		pulse1_.setEnable(data & 0x1);
		break;

	case 0x4017:
		// TODO implement delay write
		frame_sequencer_mode_ = data & 0x80;
		if (frame_sequencer_mode_)
		{
			clockChannelsLen();
		}
		if (irq_inhibit_flag_ != static_cast<bool>(data & 0x40))
		{
			frame_interrupt_ = false; // reset the flag
		}
		irq_inhibit_flag_ = data & 0x40;
		cpu_cycle_count_ = 0;
		break;
	}
}

u8 APU::regRead(u16 addr)
{
	addr &= 0x0FF;
	assert(addr <= 0x13 || addr == 0x15);

	u8 data = 0;
	switch (addr)
	{
	case 0x15:
	{
		data = (data << 1 | static_cast<u8>(!noise_.isSilenced()));
		data = (data << 1 | static_cast<u8>(!triangle_.isSilenced()));
		data = (data << 1 | static_cast<u8>(!pulse2_.isSilenced()));
		data = (data << 1 | static_cast<u8>(!pulse1_.isSilenced()));
		data |= (static_cast<u8>(frame_interrupt_) << 6);
		frame_interrupt_ = false;
		break;
	}
	}
	return data;
}

void APU::reset()
{
}

void APU::clockFrameCounter()
{
	if (!frame_sequencer_mode_)
	{
		if (cpu_cycle_count_ == 3728)
		{
		}
		else if (cpu_cycle_count_ == 7456)
		{
			clockChannelsLen();
		}
		else if (cpu_cycle_count_ == 11186)
		{
			// TODO
		}
		else if (cpu_cycle_count_ == 14914)
		{
			clockChannelsLen();
			if (!irq_inhibit_flag_)
			{
				frame_interrupt_ = true;
				Bus::instance().cpu.requestInterrupt();
			}
			cpu_cycle_count_ = 0;
			return;
		}

		if (cpu_cycle_count_ >= 14914)
		{
			cpu_cycle_count_ = 0;
		}
	}
	else
	{
		if (cpu_cycle_count_ == 3728)
		{
		}
		else if (cpu_cycle_count_ == 7456)
		{
			clockChannelsLen();
		}
		else if (cpu_cycle_count_ == 11186)
		{
			// TODO
		}
		else if (cpu_cycle_count_ == 14914)
		{

		}
		else if (cpu_cycle_count_ == 18640)
		{
			clockChannelsLen();
			cpu_cycle_count_ = 0;
			return;
		}

		if (cpu_cycle_count_ >= 18640)
		{
			cpu_cycle_count_ = 0;
		}
	}
}

void APU::clockChannelsLen()
{
	pulse1_.clockLenCnt();
	pulse2_.clockLenCnt();
	triangle_.clockLenCnt();
	noise_.clockLenCnt();
}

void Channel::clockLenCnt()
{
	if (!isSilenced() && !len_cnt_halt_)
	{
		--len_cnt_;
	}
}

void Channel::setEnable(bool set)
{
	enable_ = set;
	if (!enable_)
	{
		len_cnt_ = 0;
	}
}

void Channel::setLenCntHalt(bool set)
{
	len_cnt_halt_ = set;
}

bool Channel::isSilenced() const
{
	return !enable_ || len_cnt_ <= 0;
}

void Channel::loadLenCnt(const u8 value)
{
	if (enable_)
	{
		len_cnt_ = value;
	}
}
