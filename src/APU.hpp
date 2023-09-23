#pragma once

#include "common/type.hpp"


class APU
{
public:
	void clock();
	
	void regWrite(u16 addr, u8 data);

	u8 regRead(u16 addr);

private:
	u8 status_ = 0;
	bool frame_sequencer_mode_ = false;
	bool irq_inhibit_flag_ = false;
	bool frame_interrupt = false;
	int frame_sequencer_step_ = 0;
};