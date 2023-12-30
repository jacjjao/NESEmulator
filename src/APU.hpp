#pragma once

#include "pch.hpp"


class Channel
{
public:
	void clockLenCnt();

	void setEnable(bool set);

	void setLenCntHalt(bool set);

	bool isSilenced() const;

	void loadLenCnt(u8 value);

private:
	u8 len_cnt_ = 0;
	bool enable_ = false;
	bool len_cnt_halt_ = false;
};

class APU
{
public:
	void clock();
	
	void regWrite(u16 addr, u8 data);

	u8 regRead(u16 addr);

	void reset();

private:
	void clockFrameCounter();
	void clockChannelsLen();

	u8 status_ = 0;
	bool frame_sequencer_mode_ = false;
	bool irq_inhibit_flag_ = false;
	bool frame_interrupt_ = false;
	int cpu_cycle_count_ = 0;

	Channel pulse1_, pulse2_, triangle_, noise_;

	static constexpr std::array<u8, 32> LEN_CNT_TABLE = {
		10, 254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14,
		12,  16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30
	};
};