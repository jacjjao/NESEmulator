#include "APU.hpp"
#include "common/bitHelper.hpp"
#include "Bus.hpp"

#include <cassert>


void APU::clock()
{
	// clock frame sequencer
	const auto updateLenCnts = [this] {
		pulse1_.clockLenCnt();
		pulse2_.clockLenCnt();
		triangle_.clockLenCnt();
		noise_.clockLenCnt();
	};
	
	if (!frame_sequencer_mode_)
	{
		if (frame_sequencer_step_ == 0)
		{
			// TODO
		}
		else if (frame_sequencer_step_ == 1)
		{
			updateLenCnts();
		}
		else if (frame_sequencer_step_ == 2)
		{
			// TODO
		}
		else
		{
			updateLenCnts();
			if (irq_inhibit_flag_)
			{
				Bus::instance().cpu.requestInterrupt();
			}
			frame_sequencer_step_ = 0;
			return;
		}
	}
	else
	{
		if (frame_sequencer_step_ == 0)
		{
			// TODO
		}
		else if (frame_sequencer_step_ == 1)
		{
			updateLenCnts();
		}
		else if (frame_sequencer_step_ == 2)
		{
			// TODO
		}
		else if (frame_sequencer_step_ == 3)
		{

		}
		else
		{
			updateLenCnts();
			frame_sequencer_step_ = 0;
			return;
		}
	}
	++frame_sequencer_step_;
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
	case 0x4003:
		pulse1_.loadLenCnt(getLenCntValue(data));
		break;

	case 0x4007:
		pulse2_.loadLenCnt(getLenCntValue(data));
		break;

	case 0x400B:
		triangle_.loadLenCnt(getLenCntValue(data));
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
		frame_sequencer_mode_ = getBitN(data, 7);
		irq_inhibit_flag_ = getBitN(data, 6);
		frame_sequencer_step_ = 0;
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
		data = (data << 1 | static_cast<u8>(!noise_.isHalted()));
		data = (data << 1 | static_cast<u8>(!triangle_.isHalted()));
		data = (data << 1 | static_cast<u8>(!pulse2_.isHalted()));
		data = (data << 1 | static_cast<u8>(!pulse1_.isHalted()));
		break;
	}
	}
	return data;
}

void APU::reset()
{
}

void Channel::clockLenCnt()
{
	if (!isHalted())
	{
		--len_cnt_;
	}
}

void Channel::setEnable(bool set)
{
	enable_ = set;
}

bool Channel::isHalted() const
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
