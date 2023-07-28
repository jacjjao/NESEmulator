#include "StandardController.hpp"
#include "../common/bitHelper.hpp"

void StandardController::setStrobe(const bool strobe)
{
	if (strobe_ != strobe) // if strobe mode changed
	{
		report_bit = 0;
	}
	strobe_ = strobe;
}

u8 StandardController::report()
{
	if (report_bit >= 8)
	{
		return 0x01;
	}
	const u8 result = getBitN(reg_, report_bit);
	++report_bit;
	return result;
}

void StandardController::setBotton(const Botton botton, const bool set)
{
	//if (!strobe_)
		//return;
	if (set)
	{
		reg_ |= botton;
	}
	else
	{
		reg_ = reg_ & (~botton);
	}
}
