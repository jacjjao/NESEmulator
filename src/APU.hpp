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

	virtual float getOutput() { return 0.0f; };

	virtual void clock() {};

private:
	u8 len_cnt_ = 0;
	bool enable_ = false;
	bool len_cnt_halt_ = false;
};

class APUBuffers
{
public:
	void loadSamples(sf::Int16* buf, size_t count);

	void swapBuffer();

	sf::SoundBuffer& getBuffer();

private:
	bool latch_;
	sf::SoundBuffer buf1_, buf2_;
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

class PulseChannel : public Channel
{
public:
	~PulseChannel() override = default;

	float getOutput() override;

	static inline PulseTable table;

	void setDuty(u8 duty) { assert(duty < 4); duty_ = duty; };

	void clock() override { step_ = (step_ + 1) % 8; }

private:
	u8 duty_ = 0;
	int step_ = 0;
};

class APU
{
public:
	APU();

	void clock();
	
	void regWrite(u16 addr, u8 data);

	u8 regRead(u16 addr);

	void reset();

	sf::SoundBuffer& getBuffer();

	void mix();

private:
	void clockFrameCounter();
	void clockChannelsLen();

	u8 status_ = 0;
	bool frame_sequencer_mode_ = false;
	bool irq_inhibit_flag_ = false;
	bool frame_interrupt_ = false;
	int cpu_cycle_count_ = 0;

	PulseChannel pulse1_, pulse2_;

	Channel triangle_, noise_;

	static constexpr std::array<u8, 32> LEN_CNT_TABLE = {
		10, 254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14,
		12,  16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30
	};

	std::vector<sf::Int16> audio_buf_;
	size_t audio_buf_p_ = 0;

	APUBuffers bufs_;
};