#pragma once

#include "pch.hpp"

class Channel
{
public:
	virtual ~Channel() = default;

	void clockLenCnt();

	void setEnable(bool set);

	void setLenCntHalt(bool set);

	bool isSilenced() const;

	void loadLenCnt(u8 value);

	virtual uint8_t getOutput() { return 0; };

	virtual void clock() {};

private:
	u8 len_cnt_ = 0;
	bool enable_ = false;
	bool len_cnt_halt_ = false;
};

class Envelope{
	public:
		void clock();

		bool start_flag_ = false;

		uint8_t decay_level_ = 0;
		uint8_t divider_step_ = 0;
		uint8_t reset_level_ = 0;
		bool is_looping_ = false;
};

class Sweep{
	public:
		bool enabled_ = false;
		uint8_t divider_period_ = 0;
		uint8_t divider_counter_ = 0;
		bool negate_ = false;
		uint8_t shift_count_ = 0;
		bool reload_flag_ = false;
};

class PulseTable
{
public:
	constexpr PulseTable() : table_{ ~0 }
	{
		for (int i = 0; i < table_.size(); ++i)
		{
			table_[i] = 95.52f / (8128.0f / static_cast<float>(i) + 100.0f);
		}
	};

	constexpr float operator[](size_t index) const { return table_[index]; }

private:
	std::array<float, 31> table_;
};

class TriangleTable
{
public:
	constexpr TriangleTable() : table_{ ~0 }
	{
		for (int i = 0; i < table_.size(); ++i)
		{
			table_[i] = 163.67f / (24329.0f / static_cast<float>(i) + 100.0f);
		}
	};

	constexpr float operator[](size_t index) const { return table_[index]; }

private:
	std::array<float, 203> table_;
};

class PulseChannel : public Channel
{
public:
	PulseChannel(bool is_channel_1){this->is_channel_1 = is_channel_1;}
	~PulseChannel() override = default;

	uint8_t getOutput() override;

	static inline PulseTable table;

	void setDuty(u8 duty) { assert(duty < 4); duty_ = duty; };

	void clock() override { 
		timer--;
		if (timer == 0) {
			timer = timer_reset;
			step_ = (step_ + 1) % 8;
		}
	}

	void clockEnvelope();
	void clockSweep();

	void setStepReset() { step_ = 0; }

	void calculateSweepPeriod();
	bool isSweepMuted(){
		return timer_reset < 8 || target_period > 0x7FF;
	}
	
	bool is_constant = true;
	uint8_t constant_volume = 0;
	uint16_t timer = 0;
	uint16_t timer_reset = 0;
	int target_period = 0;

	Envelope envelope_;

	Sweep sweep_;


private:
	u8 duty_ = 0;
	int step_ = 0;
	bool is_channel_1 = false;
};

class TriangleChannel : public Channel
{
public:
	~TriangleChannel() override = default;

	uint8_t getOutput() override;

	static inline TriangleTable table;

	void clock() override { 
		timer--;
		if (timer == 0) {
			timer = timer_reset;
			position = (position + 1) % 32;
		}
	}

	void ClockLinearCounter();

	uint16_t timer = 0;
	uint16_t timer_reset = 0;

	bool counter_on_ = false;
	bool counter_reload_flag_ = false;
	uint8_t counter_reload_value_ = 0;
	uint8_t counter_value = 0;

	const uint8_t sequence[32] = {15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    uint8_t position=0;
};

class NoiseChannel : public Channel
{
public:
	~NoiseChannel() override = default;

	uint8_t getOutput() override;

	void clock() override { 
		timer--;
		if (timer == 0) {
			timer = timer_reset;

			feedback = (lfsr_ & 1) ^ ((mode_ ? (lfsr_ >> 6) : (lfsr_ >> 1)) & 1);
			lfsr_ = (lfsr_ >> 1) | (feedback << 14);
		}
	}

	void clockEnvelope();

	bool is_constant = true;
	bool mode_ = false;

	uint8_t constant_volume = 0;

	uint16_t timer = 0;
	uint16_t timer_reset = 0;
	uint16_t shift_reg = 1;
	uint16_t lfsr_ = 1;

	int feedback = 0;


	Envelope envelope_;
	const uint16_t noisePeriodTable[16] = {
    	4, 8, 16, 32, 64, 96, 128, 160,
    	202, 254, 380, 508, 762, 1016, 2034, 4068};

};

class AudioBuffer
{
public:
	AudioBuffer() { samples_.reserve(30000); }

	void write(i16 value);

	void copyAll(std::vector<i16>& buf);

private:
	std::vector<i16> samples_;
	std::mutex mutex_;
};

class APU
{
public:
	void clock();
	
	void regWrite(u16 addr, u8 data);

	u8 regRead(u16 addr);

	void reset();

	void mix();

	void getSamples(std::vector<i16>& buf);

private:
	void clockFrameCounter();
	void clockChannelsLen();
	void clockEnvelopes();
	void clockSweeps();

	u8 status_ = 0;
	bool frame_sequencer_mode_ = false;
	bool irq_inhibit_flag_ = false;
	bool frame_interrupt_ = false;
	int cpu_cycle_count_ = 0;

	PulseChannel pulse1_ = PulseChannel(true), pulse2_ = PulseChannel(false);

	TriangleChannel triangle_;

	NoiseChannel noise_;

	static constexpr std::array<u8, 32> LEN_CNT_TABLE = {
		10, 254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14,
		12,  16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30
	};

	AudioBuffer audio_buf_;
};