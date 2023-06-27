#pragma once

#include "common/type.hpp"
#include <bitset>
#include <array>
#include <functional>
#include <vector>

class CPU6502
{
public:
	CPU6502();

	void update();

private:
	void opcodeADC(u8 opcode);
	u8 ADCImmediate(u8 immediate);
	u8 ADCZeroPage(u8 address, u8 offset);
	u8 ADCAbsolute(u16 address, u8 offset);
	u8 ADCIndexedIndirect(u8 address);
	u8 ADCIndirectIndexed(u8 address);

	void unknownOpcode(u8 opcode);

	u8 getByteFromPC();
	u16 getTwoBytesFromPC();

	u8 getCarryFlag() const;
	u8 getZeroFlag() const;
	u8 getInterruptDisableFlag() const;
	u8 getDecimalModeFlag() const;
	u8 getBreakFlag() const;
	u8 getOverflowFlag() const;
	u8 getNegativeResultFlag() const;


	void setCarryFlag(bool set);
	void setZeroFlag(bool set);
	void setInterruptDisableFlag(bool set);
	void setDecimalModeFlag(bool set);
	void setBreakFlag(bool set);
	void setOverflowFlag(bool set);
	void setNegativeResultFlag(bool set);

	struct
	{
		u8 A = 0;
		u8 X = 0;
		u8 Y = 0;
		u8 S = 0;
		u16 PC = 0;
		u8 P = 0;
		std::bitset<8> status{};
	} reg;

	std::vector<std::function<void(u8)>> ins;
	std::vector<u8> mem;
};