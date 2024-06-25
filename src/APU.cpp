#include "APU.hpp"
#include "Bus.hpp"


void APU::clock()
{
	/*clock frame sequencer*/
	triangle_.clock();
	dmc_.clock();
	if (cpu_cycle_count_ % 2 == 0)
	{
		pulse1_.clock();
		pulse2_.clock();		
		noise_.clock();
		clockFrameCounter();
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
		pulse1_.is_constant = (data >> 4) & 1;
		pulse1_.constant_volume = data & 0xF;
		pulse1_.envelope_.is_looping_ = (data >> 5) & 1;
		pulse1_.envelope_.reset_level_ = data & 0xF;
		pulse1_.setLenCntHalt(data & 0x20);
		pulse1_.setDuty((data & 0xC0) >> 6);
		break;
	
	case 0x4001:
		pulse1_.sweep_.enabled_ = data & 0x80;
		pulse1_.sweep_.divider_period_ = (data & 0x70) >> 4;
		pulse1_.sweep_.negate_ = data & 0x08;
		pulse1_.sweep_.shift_count_ = data & 0x07;
		pulse1_.sweep_.reload_flag_ = true;
		break;

	case 0x4002:
		pulse1_.timer_reset = (pulse1_.timer_reset & 0x700) | data;
		break;

	case 0x4003:
		pulse1_.timer_reset = (pulse1_.timer_reset & 0xFF) | ((data & 0x7) << 8);
		pulse1_.timer = pulse1_.timer_reset;
		pulse1_.envelope_.start_flag_ = true;
		pulse1_.setStepReset();
		pulse1_.loadLenCnt(getLenCntValue(data));
		break;

	case 0x4004:
		pulse2_.is_constant = (data >> 4) & 1;
		pulse2_.constant_volume = data & 0xF;
		pulse2_.envelope_.is_looping_ = (data >> 5) & 1;
		pulse2_.envelope_.reset_level_ = data & 0xF;
		pulse2_.setLenCntHalt(data & 0x20);
		pulse2_.setDuty((data & 0xC0) >> 6);
		break;

	case 0x4005:
		pulse2_.sweep_.enabled_ = data & 0x80;
		pulse2_.sweep_.divider_period_ = (data & 0x70) >> 4;
		pulse2_.sweep_.negate_ = data & 0x08;
		pulse2_.sweep_.shift_count_ = data & 0x07;
		pulse2_.sweep_.reload_flag_ = true;
		break;

	case 0x4006:
		pulse2_.timer_reset = (pulse2_.timer_reset & 0x700) | data;
		break;

	case 0x4007:
		pulse2_.timer_reset = (pulse2_.timer_reset & 0xFF) | ((data & 0x7) << 8);
		pulse2_.timer = pulse2_.timer_reset;
		pulse2_.envelope_.start_flag_ = true;
		pulse2_.setStepReset();
		pulse2_.loadLenCnt(getLenCntValue(data));
		break;

	case 0x4008:
		triangle_.counter_on_ = (data & 0x80) == 0;
		triangle_.counter_reload_value_ = data & 0x7F;
		triangle_.setLenCntHalt(data & 0x80);
		break;

	case 0x400A:
		triangle_.timer_reset = (triangle_.timer_reset & 0x700) | data;
		break;

	case 0x400B:
		triangle_.timer_reset = (triangle_.timer_reset & 0x00FF) | ((data & 0x7) << 8);
		triangle_.counter_reload_flag_ = true;
		triangle_.loadLenCnt(getLenCntValue(data));
		break;

	case 0x400C:
		noise_.setLenCntHalt(data & 0x20);
		noise_.envelope_.is_looping_ = (data >> 5) & 1;
		noise_.envelope_.reset_level_ = data & 0xF;
		noise_.is_constant = (data >> 4) & 1;
		noise_.constant_volume = data & 0xF;
		break;
	
	case 0x400E:
		noise_.mode_ = data & 0x80;
		noise_.timer_reset = noise_.noisePeriodTable[data & 0xF];
		break;

	case 0x400F:
		noise_.envelope_.start_flag_ = true;
		noise_.loadLenCnt(getLenCntValue(data));
		break;

	case 0x4010:
		dmc_.writeFlags(data);
		break;

	case 0x4011:
		dmc_.writeLoad(data);
		break;

	case 0x4012:
		dmc_.writeSampleAddr(data);
		break;

	case 0x4013:
		dmc_.writeSampleLen(data);
		break;

	case 0x4015:
		noise_.setEnable(data & 0x8);
		triangle_.setEnable(data & 0x4);
		pulse2_.setEnable(data & 0x2);
		pulse1_.setEnable(data & 0x1);
		dmc_.setEnable(getBitN(data, 4));
		break;

	case 0x4017:
		// TODO implement delay write
		frame_sequencer_mode_ = data & 0x80;
		if (frame_sequencer_mode_)
		{
			clockChannelsLen();
			clockEnvelopes();
			clockSweeps();
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
		setBitN(data, 7, dmc_.irq());
		setBitN(data, 4, dmc_.isSilenced());
		frame_interrupt_ = false;
		break;
	}
	}
	return data;
}

void APU::getSamples(std::vector<i16>& buf)
{
	audio_buf_.copyAll(buf);
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
			clockEnvelopes();
		}
		else if (cpu_cycle_count_ == 7456)
		{
			clockChannelsLen();
			clockEnvelopes();
			clockSweeps();
		}
		else if (cpu_cycle_count_ == 11186)
		{
			clockEnvelopes();
		}
		else if (cpu_cycle_count_ == 14914)
		{
			clockChannelsLen();
			clockEnvelopes();
			clockSweeps();
			if (!irq_inhibit_flag_)
			{
				frame_interrupt_ = true;
				// Bus::instance().cpu.requestInterrupt();
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
			clockEnvelopes();
		}
		else if (cpu_cycle_count_ == 7456)
		{
			clockEnvelopes();
			clockChannelsLen();
			clockSweeps();
		}
		else if (cpu_cycle_count_ == 11186)
		{
			clockEnvelopes();
		}
		else if (cpu_cycle_count_ == 14914)
		{

		}
		else if (cpu_cycle_count_ == 18640)
		{
			clockEnvelopes();
			clockChannelsLen();
			clockSweeps();
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

void APU::clockEnvelopes(){
	pulse1_.clockEnvelope();
	pulse2_.clockEnvelope();
	triangle_.ClockLinearCounter();
	noise_.clockEnvelope();
}

void APU::clockSweeps(){
	pulse1_.clockSweep();
	pulse2_.clockSweep();
}

void APU::mix()
{
	uint8_t p1 = pulse1_.getOutput();
	uint8_t p2 = pulse2_.getOutput();
	uint8_t t = triangle_.getOutput();
	uint8_t n = noise_.getOutput();
	uint8_t d = dmc_.getOutput();
	float value = pulse1_.table[(p1 + p2) & 0x1F] + triangle_.table[3 * t + 2 * n + d];
	//float value = triangle_.table[2 * n];
	audio_buf_.write(static_cast<i16>(value * 32767.0f));
}

void Channel::clockLenCnt()
{
	if (len_cnt_ > 0 && !len_cnt_halt_)
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

uint8_t PulseChannel::getOutput()
{
	static u8 sequences[4][8] =
	{
		{ 0, 1, 0, 0, 0, 0, 0, 0 }, // 12.5%
		{ 0, 1, 1, 0, 0, 0, 0, 0 }, // 25%
		{ 0, 1, 1, 1, 1, 0, 0, 0 }, // 50%
		{ 1, 0, 0, 1, 1, 1, 1, 1 }  // 25% negated
	};

	if (isSilenced() || !(sequences[duty_][step_]) || isSweepMuted())
		return 0.0f;

	if (is_constant){
		return constant_volume;
	}
	else{
		return envelope_.decay_level_;
	}
	//return sequences[duty_][step_];
}

uint8_t TriangleChannel::getOutput(){
	if (counter_value == 0 || isSilenced()){
		return 0;
	}

	return sequence[position];
}

uint8_t NoiseChannel::getOutput(){
	if (isSilenced() || lfsr_ & 1){
		return 0;
	}
	else if (is_constant){
		return constant_volume;
	}
	else{
		return envelope_.decay_level_;
	}
}

void AudioBuffer::write(const i16 value)
{
	static int max_sample_count = 40;
	static int sample_count = 0;
	static double sample_sum = 0.0;

	sample_sum += static_cast<double>(value);
	++sample_count;
	if (sample_count >= max_sample_count)
	{
		sample_sum /= static_cast<double>(sample_count);

		{
			std::lock_guard lock{ mutex_ };
			samples_.push_back(sample_sum);
		}

		// for(int i = 0; i < samples_.size(); i++){
		// 	std::cout<<samples_[i]<<" "<<std::endl;
		// }
		// std::cout<<"================================"<<std::endl;

		sample_count = 0;
		sample_sum = 0.0;
		max_sample_count = (max_sample_count == 40) ? 41 : 40;
	}
}

void AudioBuffer::copyAll(std::vector<i16>& buf)
{
	buf.clear();
	std::lock_guard lock{ mutex_ };
	buf.assign(samples_.cbegin(), samples_.cend());
	samples_.clear();
}

void PulseChannel::clockEnvelope(){
	envelope_.clock();
}

void NoiseChannel::clockEnvelope(){
	envelope_.clock();
}

void TriangleChannel::ClockLinearCounter(){
	if (counter_reload_flag_){
		counter_value = counter_reload_value_;
	}
	else if (counter_value > 0){
		counter_value -= 1;
	}
	if (counter_on_){
		counter_reload_flag_ = false;
	}
}

void Envelope::clock(){
	if (start_flag_){
		decay_level_ = 15;
		divider_step_ = reset_level_;
		start_flag_ = false;
	}
	else{
		if (divider_step_ > 0){
			divider_step_ -= 1;
		}
		else{
			divider_step_ = reset_level_;
			if (decay_level_ > 0){
				decay_level_ -= 1;
			}
			else{
				if (is_looping_){
					decay_level_ = 15;
				}
			}
		}
	}
}

void PulseChannel::clockSweep(){
	calculateSweepPeriod();

	if (sweep_.divider_counter_ == 0 && sweep_.enabled_ && sweep_.shift_count_ > 0){
		if (!isSweepMuted()){
			timer_reset = target_period;
		}
		else{
			sweep_.divider_counter_ = sweep_.divider_period_ + 1;
		}
	}
	if(sweep_.divider_counter_ == 0 || sweep_.reload_flag_){
		sweep_.divider_counter_ = sweep_.divider_period_ + 1;
		sweep_.reload_flag_ = false;
	}
	else{
		sweep_.divider_counter_ -= 1;
	}
//===============================================
	// if (sweep_.reload_flag_){
	// 	if (sweep_.enabled_ && sweep_.divider_counter_ == 0){
	// 		timer_reset = target_period;
	// 	}
	// 	sweep_.reload_flag_ = false;
	// 	sweep_.divider_counter_ = sweep_.divider_period_;
	// }
	// else if (sweep_.divider_counter_ > 0){
	// 	sweep_.divider_counter_ -= 1;
	// }
	// else{
	// 	if (sweep_.enabled_){
	// 		timer_reset = target_period;
	// 	}
	// 	sweep_.divider_counter_ = sweep_.divider_period_;
	// }
//===============================================
	// if (sweep_.enabled_){
	// 	if (sweep_.reload_flag_){
	// 		sweep_.divider_counter_ = sweep_.divider_period_ + 1;
	// 		sweep_.reload_flag_ = false;
	// 	}
	// 	if (sweep_.divider_counter_ > 0){
	// 		sweep_.divider_counter_ -= 1;
	// 	}
	// 	else{
	// 		if (!isSweepMuted()){
	// 			std::cout<<timer_reset<<std::endl;
	// 			timer_reset = target_period;
	// 		}
	// 		sweep_.divider_counter_ = sweep_.divider_period_ + 1;
	// 	}
	// }
}

void PulseChannel::calculateSweepPeriod(){
	uint16_t current_period = timer_reset;
	uint16_t delta = current_period >> sweep_.shift_count_;

	if (sweep_.negate_){
		if (is_channel_1){
			target_period = current_period - (delta + 1);
			//timer_reset -= delta + 1;
		}
		else{
			target_period = current_period - delta;
			//timer_reset -= delta;
		}
	}
	else{
		target_period = current_period + delta;
		//timer_reset += delta;
	}
	if (target_period < 0){
		target_period = 0;
	}
}

void DMAReader::writeSampleAddr(const u8 data)
{
	const auto A = static_cast<u16>(data);
	sample_addr_ = cur_addr_ = 0xC000 + (A * 64);
}

void DMAReader::writeSampleLen(const u8 data)
{
	const auto L = static_cast<u16>(data);
	sample_len_ = cur_len_ = (L * 16) + 1;
}

void DMAReader::setLoop(const bool loop)
{
	loop_ = loop;
}

void DMAReader::setIrqEnable(const bool enable)
{
	irq_enable_ = true;
}

u8 DMAReader::getSample()
{
	const auto sample = Bus::instance().cpuRead(cur_addr_);
	if (cur_len_ > 0)
	{
		if (cur_addr_ == 0xFFFF)
			cur_addr_ = 0x8000;
		else
			++cur_addr_;
		--cur_len_;
	}
	else if (loop_)
	{
		restart();
	}
	else if (!disable_)
	{
		irq_ = true;
		Bus::instance().apu.dmc_irq = true;
	}
	return sample;
}

bool DMAReader::isEmpty() const
{
	return cur_len_ == 0;
}

bool DMAReader::irq()
{
	auto b = irq_;
	irq_ = false;
	return b;
}

void DMAReader::clear()
{
	loop_ = false;
	disable_ = true;
}

void DMAReader::restart()
{
	cur_addr_ = sample_addr_;
	cur_len_ = sample_len_;
	disable_ = false;
}

void DMCOutputUnit::update()
{
	if (!silence_)
	{
		bool b0 = getBitN(shift_reg_, 0);
		if (b0 && volume_ + 2 <= 127)
		{
			volume_ += 2;
		}
		else if (!b0 && volume_ >= 2)
		{
			volume_ -= 2;
		}
	}
	shift_reg_ >>= 1;
	--counter_;
	if (counter_ <= 0)
	{
		counter_ = 8;
		if (reader_.isEmpty())
		{
			if (restart_)
				reader_.restart();
			else
				silence_ = true;
		}
		else
		{
			silence_ = false;
			shift_reg_ = reader_.getSample();
		}
	}
}

void DMCOutputUnit::setVolume(const u8 volume)
{
	volume_ = volume;
}

u8 DMCOutputUnit::getOutput() const
{
	return silence_ ? 0 : volume_;
}

void DMCOutputUnit::restart()
{
	restart_ = true;
}

bool Divider::clock()
{
	if (counter_ == 0)
	{
		counter_ = period_;
		return true;
	}
	--counter_;
	return false;
}

void Divider::setPeriod(const u16 period)
{
	period_ = period;
}

void Divider::reset()
{
	counter_ = period_;
}

void Divider::reset(const u16 period)
{
	counter_ = period_ = period;
}

u16 Divider::getPeriod() const
{
	return period_;
}

void DMC::clock()
{
	if (divider_.clock())
		output_.update();
}

void DMC::writeFlags(const u8 data)
{
	output_.reader_.setIrqEnable(getBitN(data, 7));
	output_.reader_.setLoop(getBitN(data, 6));
	divider_.reset(s_period_table[data & 0x0F] - 1);
}

void DMC::writeLoad(const u8 data)
{
	output_.setVolume(data & 0x7F);
}

void DMC::writeSampleAddr(const u8 data)
{
	output_.reader_.writeSampleAddr(data);
}

void DMC::writeSampleLen(const u8 data)
{
	output_.reader_.writeSampleLen(data);
}

void DMC::setEnable(const bool enable)
{
	if (!enable)
		output_.reader_.clear();
	else
		output_.reader_.restart();
}

u8 DMC::getOutput() const
{
	return output_.getOutput();
}

bool DMC::isSilenced() const
{
	return output_.reader_.isEmpty();
}

bool DMC::irq()
{
	return output_.reader_.irq();
}