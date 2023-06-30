#pragma once

#include "common/type.hpp"
#include <functional>
#include <vector>

class CPU6502
{
public:
	CPU6502();
	~CPU6502();

	void update();

private:
	void instrADC(u8 opcode); // carry, overflow ??
	void instrAND(u8 opcode);
	void instrASL(u8 opcode);
	void instrBCC(u8 opcode);
	void instrBCS(u8 opcode);
	void instrBEQ(u8 opcode);
	void instrBIT(u8 opcode);
	void instrBMI(u8 opcode);
	void instrBNE(u8 opcode);
	void instrBPL(u8 opcode);
	void instrBRK(u8 opcode);
	void instrBVC(u8 opcode);
	void instrBVS(u8 opcode);
	void instrCLC(u8 opcode);
	void instrCLD(u8 opcode);
	void instrCLI(u8 opcode);
	void instrCLV(u8 opcode);
	void instrCMP(u8 opcode);
	void instrCPX(u8 opcode);
	void instrCPY(u8 opcode);
	void instrDEC(u8 opcode);
	void instrDEX(u8 opcode);
	void instrDEY(u8 opcode);
	void instrEOR(u8 opcode);
	void instrINC(u8 opcode);
	void instrINX(u8 opcode);
	void instrINY(u8 opcode);
	void instrJMP(u8 opcode);
	void instrJSR(u8 opcode);
	void instrLDA(u8 opcode);
	void instrLDX(u8 opcode);
	void instrLDY(u8 opcode);
	void instrLSR(u8 opcode);
	void instrNOP(u8 opcode);
	void instrORA(u8 opcode);
	void instrPHA(u8 opcode);
	void instrPHP(u8 opcode);
	void instrPLA(u8 opcode);
	void instrPLP(u8 opcode);
	void instrROL(u8 opcode);
	void instrROR(u8 opcode);
	void instrRTI(u8 opcode);
	void instrRTS(u8 opcode);
	void instrSBC(u8 opcode); // carry, overflow ??
	void instrSEC(u8 opcode);
	void instrSED(u8 opcode);
	void instrSEI(u8 opcode);
	void instrSTA(u8 opcode);
	void instrSTX(u8 opcode);
	void instrSTY(u8 opcode);
	void instrTAX(u8 opcode);
	void instrTAY(u8 opcode);
	void instrTSX(u8 opcode); // ?????
	void instrTXA(u8 opcode);
	void instrTXS(u8 opcode);
	void instrTYA(u8 opcode);

	void unknownOpcode(u8 opcode);

	u16 immediateAddr();
	u16 zeroPageAddr(u8 offset = 0);
	u16 absoluteAddr(u8 offset = 0);
	u16 indirectAddr();
	u16 indexedIndirectAddr();
	u16 indirectIndexedAddr();

	u8 getByteFromPC();
	u16 getTwoBytesFromPC();
	u16 getTwoBytesFromZP(u8 loc);
	u16 getTwoBytesFromMem(u16 loc);

	u8 getCarryFlag() const;
	u8 getZeroFlag() const;
	u8 getInterruptDisableFlag() const;
	u8 getDecimalModeFlag() const;
	u8 getBreakFlag() const;
	u8 getOverflowFlag() const;
	u8 getNegativeResultFlag() const;

	void setCarryFlag(bool set);
	void setZeroFlag(u8 result);
	void setInterruptDisableFlag(bool set);
	void setDecimalModeFlag(bool set);
	void setBreakFlag(bool set);
	void setOverflowFlag(bool set);
	void setNegativeResultFlag(u8 result);

	void pushStack(u8 val);
	void pushStack(u16 val);
	u8 popStack();
	u16 popStackTwoBytes();

	struct
	{
		u8 A = 0;
		u8 X = 0;
		u8 Y = 0;
		u8 Status = 0;
		u8 SP = 0;
		u16 PC = 0;
	} reg;

	std::vector<std::function<void(u8)>> instrs;
	u8* mem;

	static constexpr std::size_t mem_size    = 0xFFFF + 1;
	static constexpr std::size_t instrs_size = 0xFF + 1;

	static constexpr u16 stack_begin = 0x01FF;
};