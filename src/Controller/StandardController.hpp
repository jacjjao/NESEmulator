#pragma once

#include "../common/type.hpp"


enum Botton : u8
{
	A      = 0b00000001,
	B      = 0b00000010,
	Select = 0b00000100,
	Start  = 0b00001000,
	Up     = 0b00010000,
	Down   = 0b00100000,
	Left   = 0b01000000,
	Right  = 0b10000000
};

class StandardController
{
public:
	void setStrobe(bool strobe);
	u8 report();

	void setBotton(Botton botton, bool set);

private:
	u8 reg_ = 0;

	bool strobe_   = false;
	u8 report_bit = 0;
};