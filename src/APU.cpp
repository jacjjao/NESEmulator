#include "APU.hpp"
#include "Bus.hpp"


void APU::clock()
{
	// clock frame sequencer
	if (cpu_cycle_count_ % 2 == 0)
	{
		clockFrameCounter();
		pulse1_.clock();
		pulse2_.clock();
	}
	mix();
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
		pulse1_.setDuty((data & 0xC0) >> 6);
		break;

	case 0x4003:
		pulse1_.loadLenCnt(getLenCntValue(data));
		break;

	case 0x4004:
		pulse2_.setLenCntHalt(data & 0x20);
		pulse1_.setDuty((data & 0xC0) >> 6);
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

APU::APU() : 
	audio_buf_(44100 / 20)
{
}

sf::SoundBuffer& APU::getBuffer()
{
	bufs_.loadSamples(audio_buf_.data(), audio_buf_.size());
	auto& buf = bufs_.getBuffer();
	bufs_.swapBuffer();
	audio_buf_p_ = 0;
	return buf;
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

void APU::mix()
{
	float p1 = pulse1_.getOutput();
	float p2 = pulse2_.getOutput();
	
	if (audio_buf_p_ < audio_buf_.size())
		audio_buf_[audio_buf_p_++] = static_cast<sf::Int16>(10000.0f * PulseChannel::table[p1 + p2]);
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

void APUBuffers::loadSamples(sf::Int16* buf, size_t count)
{
	static constexpr unsigned CHANNEL_COUNT = 1;
	static constexpr unsigned SAMPLE_RATE = 44100;
	if (!getBuffer().loadFromSamples(buf, count, CHANNEL_COUNT, SAMPLE_RATE))
	{
		std::fprintf(stderr, "Failed to load samples\n");
	}
}

void APUBuffers::swapBuffer()
{
	latch_ = !latch_;
}

sf::SoundBuffer& APUBuffers::getBuffer()
{
	return latch_ ? buf1_ : buf2_;
}

float PulseChannel::getOutput()
{
	static u8 sequences[4][8] =
	{
		{ 0, 1, 0, 0, 0, 0, 0, 0 }, // 12.5%
		{ 0, 1, 1, 0, 0, 0, 0, 0 }, // 25%
		{ 0, 1, 1, 1, 1, 0, 0, 0 }, // 50%
		{ 1, 0, 0, 1, 1, 1, 1, 1 }  // 25% negated
	};

	return sequences[duty_][step_];
}
