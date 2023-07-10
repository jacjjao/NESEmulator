#pragma once

#include "common/type.hpp"
#include <functional>
#include <array>

#define EMUCPULOG

#ifdef EMUCPULOG
#include <string>
#endif

class Bus;

class CPU6502
{
public:
	CPU6502();

	void update();

	void connectToBus(Bus* bus);

	void write(u16 addr, u8 data);
	u8 read(u16 addr);

	void irq();
	void nmi();

// private:
	struct Instruction
	{
		std::function<void(void)> addr_mode;
		std::function<void(void)> operate;
		u8 cycles;
		u8 penalty; // page crossed penalty
	};

	u8 immidiate_ = 0;
	u16 abs_addr_ = 0;
	u8 opcode_ = 0;

	void ADC();
	void AND();
	void ASL();
	void BCC();
	void BCS();
	void BEQ();
	void BIT();
	void BMI();
	void BNE();
	void BPL();
	void BRK();
	void BVC();
	void BVS();
	void CLC();
	void CLD();
	void CLI();
	void CLV();
	void CMP();
	void CPX();
	void CPY();
	void DEC();
	void DEX();
	void DEY();
	void EOR();
	void INC();
	void INX();
	void INY();
	void JMP();
	void JSR();
	void LDA();
	void LDX();
	void LDY();
	void LSR();
	void NOP();
	void ORA();
	void PHA();
	void PHP();
	void PLA();
	void PLP();
	void ROL();
	void ROR();
	void RTI();
	void RTS();
	void SBC(); 
	void SEC();
	void SED();
	void SEI();
	void STA();
	void STX();
	void STY();
	void TAX();
	void TAY();
	void TSX(); 
	void TXA();
	void TXS();
	void TYA();

	void unknownOpcode();

	void relativeDisplace();

	void none();
	void imm();
	void rel();
	void zp();
	void zpx();
	void zpy();
	void abs();
	void abx();
	void aby();
	void ind();
	void idxInd();
	void indIdx();

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
	void setZeroFlag(bool set);
	void setInterruptDisableFlag(bool set);
	void setDecimalModeFlag(bool set);
	void setBreakFlag(bool set);
	void setOverflowFlag(bool set);
	void setNegativeResultFlag(bool set);

	void pushStack(u8 val);
	void pushStack(u16 val);
	u8 popStack();
	u16 popStackTwoBytes();

	bool pageCrossed(u16 addr1, u16 addr2);

	struct Registers
	{
		u8 A = 0;
		u8 X = 0;
		u8 Y = 0;
		u8 Status = 0x34;
		u8 SP = 0xFD;
		u16 PC = 0;
	} reg_;

#ifdef EMUCPULOG
	Registers log_regs;
	u64 log_cycles = 7;
	void print(unsigned length); 
#endif

	static constexpr std::size_t instrs_size = 256;

	std::array<Instruction, instrs_size> instrs_;
	u8 cycles_ = 0;
	u64 total_cycles_ = 0;

	Bus* bus_ = nullptr;

	static constexpr u16 stack_low = 0x0100;
};