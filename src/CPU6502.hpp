#pragma once

#include "pch.hpp"

// #define EMUCPULOG

class CPU6502
{
public:
	CPU6502();

	void cycle();

	void write(u16 addr, u8 data);
	u8 read(u16 addr);

	void reset();

	bool nmi();

	bool irq();

private:
	struct Instruction
	{
		std::function<void(void)> addr_mode;
		std::function<void(void)> operate;
		u8 cycles  = 0;
		u8 penalty = 0; // page crossed penalty
	};

	u16 abs_addr_ = 0;
	u8 opcode_ = 0;

	void ADC(); void AND(); void ASL(); void BCC();
	void BCS(); void BEQ(); void BIT(); void BMI();
	void BNE(); void BPL(); void BRK();	void BVC();
	void BVS();	void CLC();	void CLD();	void CLI();
	void CLV();	void CMP();	void CPX();	void CPY();
	void DEC();	void DEX();	void DEY();	void EOR();
	void INC();	void INX();	void INY();	void JMP();
	void JSR();	void LDA();	void LDX();	void LDY();
	void LSR();	void NOP();	void ORA();	void PHA();
	void PHP();	void PLA();	void PLP();	void ROL();
	void ROR();	void RTI();	void RTS();	void SBC(); 
	void SEC();	void SED();	void SEI();	void STA();
	void STX();	void STY();	void TAX();	void TAY();
	void TSX();	void TXA();	void TXS();	void TYA();

	// unofficial instructions
	void ALR();	void ANC(); void ARR(); void LAX();
	void SAX(); void DCP();	void ISC();	void RLA();	
	void RRA(); void SLO();	void SRE();

	void unknownOpcode();

	void relativeDisplace();
	
	enum class AddrMode
	{
		Imp, 
		Imm,
		ZP, ZPX, ZPY,
		Rel,
		Abs, Abx, Aby,
		Ind, IdxInd, IndIdx
	} addr_mode_ = AddrMode::Imp;
	
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

	u8 fetch();
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
	void print(int length); 
#endif

	static constexpr std::size_t instrs_size = 256;
	static constexpr u16 stack_low = 0x0100;

	std::vector<Instruction> instrs_;
	
	u64 total_cycles_ = 0;
	int cycle_remained_ = 0, penalty_ = 0;

	enum class CycleState
	{
		Fetch,
		Operate,
		WaitForPenalty
	} cycle_state_ = CycleState::Fetch;
};