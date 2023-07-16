#include "CPU6502.hpp"
#include "Bus.hpp"
#include "common/bitHelper.hpp"

#include <algorithm>
#include <iostream>
#include <cassert>
#include <sstream>

#ifdef EMUCPULOG
#include <fstream>
#include <iomanip>

void CPU6502::print(const int length)
{
	static std::ofstream file{"output.txt"};

	if (!file.is_open())
		file.open("output.txt");

	file << std::uppercase << std::hex << std::setfill('0') << std::setw(4) << (int)log_regs.PC << ' ';
	for (int i = 0; i < length; ++i, ++log_regs.PC) 
		file << ' ' << std::setw(2) << (int)read(log_regs.PC);
	file << "        ";
	file << " A:"  << std::setw(2) << (int)log_regs.A;
	file << " X:"  << std::setw(2) << (int)log_regs.X;
	file << " Y:"  << std::setw(2) << (int)log_regs.Y;
	file << " P:"  << std::setw(2) << (int)log_regs.Status;
	file << " SP:" << std::setw(2) << (int)log_regs.SP;
	file << " CYC:" << std::dec << log_cycles + 7;
	file << std::endl;
}
#endif

CPU6502::CPU6502()
{
	const auto unknown = [this]() { unknownOpcode(); };
	const auto none    = [this]() { this->none(); };
	const auto imm     = [this]() { this->imm(); };
	const auto zp      = [this]() { this->zp(); };
	const auto zpx     = [this]() { this->zpx(); };
	const auto zpy     = [this]() { this->zpy(); };
	const auto rel     = [this]() { this->rel(); };
	const auto ind     = [this]() { this->ind(); };
	const auto abs     = [this]() { this->abs(); };
	const auto abx     = [this]() { this->abx(); };
	const auto aby     = [this]() { this->aby(); };
	const auto idxInd  = [this]() { this->idxInd(); };
	const auto indIdx  = [this]() { this->indIdx(); };

	instrs_.resize(instrs_size, { none, unknown, 0, 0 });

	const auto ADC = [this]() { this->ADC(); };
	instrs_[0x69] = { imm,    ADC, 2, 0 };
	instrs_[0x65] = { zp ,    ADC, 3, 0 };
	instrs_[0x75] = { zpx,    ADC, 4, 0 };
	instrs_[0x6D] = { abs,    ADC, 4, 0 };
	instrs_[0x7D] = { abx,    ADC, 4, 1 };
	instrs_[0x79] = { aby,    ADC, 4, 1 };
	instrs_[0x61] = { idxInd, ADC, 6, 0 };
	instrs_[0x71] = { indIdx, ADC, 5, 1 };

	const auto AND = [this]() { this->AND(); };
	instrs_[0x29] = { imm,    AND, 2, 0 };
	instrs_[0x25] = { zp ,    AND, 3, 0 };
	instrs_[0x35] = { zpx,    AND, 4, 0 };
	instrs_[0x2D] = { abs,    AND, 4, 0 };
	instrs_[0x3D] = { abx,    AND, 4, 1 };
	instrs_[0x39] = { aby,    AND, 4, 1 };
	instrs_[0x21] = { idxInd, AND, 6, 0 };
	instrs_[0x31] = { indIdx, AND, 5, 1 };

	const auto ASL = [this]() { this->ASL(); };
	instrs_[0x0A] = { none, ASL, 2, 0 };
	instrs_[0x06] = { zp  , ASL, 5, 0 };
	instrs_[0x16] = { zpx , ASL, 6, 0 };
	instrs_[0x0E] = { abs , ASL, 6, 0 };
	instrs_[0x1E] = { abx , ASL, 7, 0 };

	const auto BCC = [this]() { this->BCC(); };
	instrs_[0x90] = { rel, BCC, 2, 1 };

	const auto BCS = [this]() { this->BCS(); };
	instrs_[0xB0] = { rel, BCS, 2, 1 };

	const auto BEQ = [this]() { this->BEQ(); };
	instrs_[0xF0] = { rel, BEQ, 2, 1 };

	const auto BIT = [this]() { this->BIT(); };
	instrs_[0x24] = { zp , BIT, 3, 0 };
	instrs_[0x2C] = { abs, BIT, 4, 0 };

	const auto BMI = [this]() { this->BMI(); };
	instrs_[0x30] = { rel, BMI, 2, 1 };

	const auto BNE = [this]() { this->BNE(); };
	instrs_[0xD0] = { rel, BNE, 2, 1 };

	const auto BPL = [this]() { this->BPL(); };
	instrs_[0x10] = { rel, BPL, 2, 1 };

	const auto BRK = [this]() { this->BRK(); };
	instrs_[0x00] = { none, BRK, 7, 0 };

	const auto BVC = [this]() { this->BVC(); };
	instrs_[0x50] = { rel, BVC, 2, 1 };

	const auto BVS = [this]() { this->BVS(); };
	instrs_[0x70] = { rel, BVS, 2, 1 };

	const auto CLC = [this]() { this->CLC(); };
	instrs_[0x18] = { none, CLC, 2, 0 };

	const auto CLD = [this]() { this->CLD(); };
	instrs_[0xD8] = { none, CLD, 2, 0 };

	const auto CLI = [this]() { this->CLI(); };
	instrs_[0x58] = { none, CLI, 2, 0 };

	const auto CLV = [this]() { this->CLV(); };
	instrs_[0xB8] = { none, CLV, 2, 0 };

	const auto CMP = [this]() { this->CMP(); };
	instrs_[0xC9] = { imm   , CMP, 2, 0 };
	instrs_[0xC5] = { zp    , CMP, 3, 0 };
	instrs_[0xD5] = { zpx   , CMP, 4, 0 };
	instrs_[0xCD] = { abs   , CMP, 4, 0 };
	instrs_[0xDD] = { abx   , CMP, 4, 1 };
	instrs_[0xD9] = { aby   , CMP, 4, 1 };
	instrs_[0xC1] = { idxInd, CMP, 6, 0 };
	instrs_[0xD1] = { indIdx, CMP, 5, 1 };

	const auto CPX = [this]() { this->CPX(); };
	instrs_[0xE0] = { imm, CPX, 2, 0 };
	instrs_[0xE4] = { zp , CPX, 3, 0 };
	instrs_[0xEC] = { abs, CPX, 4, 0 };

	const auto CPY = [this]() { this->CPY(); };
	instrs_[0xC0] = { imm, CPY, 2, 0 }; 
	instrs_[0xC4] = { zp , CPY, 3, 0 }; 
	instrs_[0xCC] = { abs, CPY, 4, 0 };

	const auto DEC = [this]() { this->DEC(); };
	instrs_[0xC6] = { zp , DEC, 5, 0 };
	instrs_[0xD6] = { zpx, DEC, 6, 0 };
	instrs_[0xCE] = { abs, DEC, 6, 0 };
	instrs_[0xDE] = { abx, DEC, 7, 0 };

	const auto DEX = [this]() { this->DEX(); };
	instrs_[0xCA] = { none, DEX, 2, 0 };

	const auto DEY = [this]() { this->DEY(); };
	instrs_[0x88] = { none, DEY, 2, 0 };
	
	const auto EOR = [this]() { this->EOR(); };
	instrs_[0x49] = { imm   , EOR, 2, 0 };
	instrs_[0x45] = { zp    , EOR, 3, 0 };
	instrs_[0x55] = { zpx   , EOR, 4, 0 };
	instrs_[0x4D] = { abs   , EOR, 4, 0 };
	instrs_[0x5D] = { abx   , EOR, 4, 1 };
	instrs_[0x59] = { aby   , EOR, 4, 1 };
	instrs_[0x41] = { idxInd, EOR, 6, 0 };
	instrs_[0x51] = { indIdx, EOR, 5, 1 };

	const auto INC = [this]() { this->INC(); };
	instrs_[0xE6] = { zp , INC, 5, 0 };
	instrs_[0xF6] = { zpx, INC, 6, 0 };
	instrs_[0xEE] = { abs, INC, 6, 0 };
	instrs_[0xFE] = { abx, INC, 7, 0 };

	const auto INX = [this]() { this->INX(); };
	instrs_[0xE8] = { none, INX, 2, 0 };

	const auto INY = [this]() { this->INY(); };
	instrs_[0xC8] = { none, INY, 2, 0 };

	const auto JMP = [this]() { this->JMP(); };
	instrs_[0x4C] = { abs, JMP, 3, 0 };
	instrs_[0x6C] = { ind, JMP, 5, 0 };

	const auto JSR = [this]() { this->JSR(); };
	instrs_[0x20] = { abs, JSR, 6, 0 };

	const auto LDA = [this]() { this->LDA(); };
	instrs_[0xA9] = { imm   , LDA, 2, 0 }; 
	instrs_[0xA5] = { zp    , LDA, 3, 0 }; 
	instrs_[0xB5] = { zpx   , LDA, 4, 0 }; 
	instrs_[0xAD] = { abs   , LDA, 4, 0 };
	instrs_[0xBD] = { abx   , LDA, 4, 1 }; 
	instrs_[0xB9] = { aby   , LDA, 4, 1 }; 
	instrs_[0xA1] = { idxInd, LDA, 6, 0 }; 
	instrs_[0xB1] = { indIdx, LDA, 5, 1 };;

	const auto LDX = [this]() { this->LDX(); };
	instrs_[0xA2] = { imm, LDX, 2, 0 }; 
	instrs_[0xA6] = { zp , LDX, 3, 0 }; 
	instrs_[0xB6] = { zpy, LDX, 4, 0 }; 
	instrs_[0xAE] = { abs, LDX, 4, 0 };
	instrs_[0xBE] = { aby, LDX, 4, 1 };

	const auto LDY = [this]() { this->LDY(); };
	instrs_[0xA0] = { imm, LDY, 2, 0 }; 
	instrs_[0xA4] = { zp , LDY, 3, 0 }; 
	instrs_[0xB4] = { zpx, LDY, 4, 0 }; 
	instrs_[0xAC] = { abs, LDY, 4, 0 };
	instrs_[0xBC] = { abx, LDY, 4, 1 };

	const auto LSR = [this]() { this->LSR(); };
	instrs_[0x4A] = { none, LSR, 2, 0 }; 
	instrs_[0x46] = { zp  , LSR, 5, 0 };
	instrs_[0x56] = { zpx , LSR, 6, 0 }; 
	instrs_[0x4E] = { abs , LSR, 6, 0 }; 
	instrs_[0x5E] = { abx , LSR, 7, 0 };

	const auto NOP = [this]() { this->NOP(); };
	instrs_[0xEA] = { none , NOP, 2, 0 };

	const auto ORA = [this]() { this->ORA(); };
	instrs_[0x09] = { imm   , ORA, 2, 0 }; 
	instrs_[0x05] = { zp    , ORA, 3, 0 }; 
	instrs_[0x15] = { zpx   , ORA, 4, 0 }; 
	instrs_[0x0D] = { abs   , ORA, 4, 0 };
	instrs_[0x1D] = { abx   , ORA, 4, 1 }; 
	instrs_[0x19] = { aby   , ORA, 4, 1 }; 
	instrs_[0x01] = { idxInd, ORA, 6, 0 }; 
	instrs_[0x11] = { indIdx, ORA, 5, 1 };

	const auto PHA = [this]() { this->PHA(); };
	instrs_[0x48] = { none, PHA, 3, 0 };

	const auto PHP = [this]() { this->PHP(); };
	instrs_[0x08] = { none, PHP, 3, 0 };

	const auto PLA = [this]() { this->PLA(); };
	instrs_[0x68] = { none, PLA, 4, 0 };

	const auto PLP = [this]() { this->PLP(); };
	instrs_[0x28] = { none, PLP, 4, 0 };

	const auto ROL = [this]() { this->ROL(); };
	instrs_[0x2A] = { none, ROL, 2, 0 }; 
	instrs_[0x26] = { zp  , ROL, 5, 0 };
	instrs_[0x36] = { zpx , ROL, 6, 0 }; 
	instrs_[0x2E] = { abs , ROL, 6, 0 }; 
	instrs_[0x3E] = { abx , ROL, 7, 0 };

	const auto ROR = [this]() { this->ROR(); };
	instrs_[0x6A] = { none, ROR, 2, 0 }; 
	instrs_[0x66] = { zp  , ROR, 5, 0 };
	instrs_[0x76] = { zpx , ROR, 6, 0 }; 
	instrs_[0x6E] = { abs , ROR, 6, 0 }; 
	instrs_[0x7E] = { abx , ROR, 7, 0 };

	const auto RTI = [this]() { this->RTI(); };
	instrs_[0x40] = { none, RTI, 6, 0 };

	const auto RTS = [this]() { this->RTS(); };
	instrs_[0x60] = { none, RTS, 6, 0 };

	const auto SBC = [this]() { this->SBC(); };
	instrs_[0xE9] = { imm   , SBC, 2, 0 }; 
	instrs_[0xE5] = { zp    , SBC, 3, 0 }; 
	instrs_[0xF5] = { zpx   , SBC, 4, 0 }; 
	instrs_[0xED] = { abs   , SBC, 4, 0 };
	instrs_[0xFD] = { abx   , SBC, 4, 1 }; 
	instrs_[0xF9] = { aby   , SBC, 4, 1 }; 
	instrs_[0xE1] = { idxInd, SBC, 6, 0 }; 
	instrs_[0xF1] = { indIdx, SBC, 5, 1 };

	const auto SEC = [this]() { this->SEC(); };
	instrs_[0x38] = { none, SEC, 2, 0 };

	const auto SED = [this]() { this->SED(); };
	instrs_[0xF8] = { none, SED, 2, 0 };

	const auto SEI = [this]() { this->SEI(); };
	instrs_[0x78] = { none, SEI, 2, 0 };

	const auto STA = [this]() { this->STA(); };
	instrs_[0x85] = { zp    , STA, 3, 0 }; 
	instrs_[0x95] = { zpx   , STA, 4, 0 }; 
	instrs_[0x8D] = { abs   , STA, 4, 0 };
	instrs_[0x9D] = { abx   , STA, 5, 0 }; 
	instrs_[0x99] = { aby   , STA, 5, 0 }; 
	instrs_[0x81] = { idxInd, STA, 6, 0 }; 
	instrs_[0x91] = { indIdx, STA, 6, 0 };

	const auto STX = [this]() { this->STX(); };
	instrs_[0x86] = { zp , STX, 3, 0 };
	instrs_[0x96] = { zpy, STX, 4, 0 }; 
	instrs_[0x8E] = { abs, STX, 4, 0 };

	const auto STY = [this]() { this->STY(); };
	instrs_[0x84] = { zp , STY, 3, 0 }; 
	instrs_[0x94] = { zpx, STY, 4, 0 }; 
	instrs_[0x8C] = { abs, STY, 4, 0 };

	const auto TAX = [this]() { this->TAX(); };
	instrs_[0xAA] = { none, TAX, 2, 0 };

	const auto TAY = [this]() { this->TAY(); };
	instrs_[0xA8] = { none, TAY, 2, 0 };

	const auto TSX = [this]() { this->TSX(); };
	instrs_[0xBA] = { none, TSX, 2, 0 };

	const auto TXA = [this]() { this->TXA(); };
	instrs_[0x8A] = { none, TXA, 2, 0 };

	const auto TXS = [this]() { this->TXS(); };
	instrs_[0x9A] = { none, TXS, 2, 0 };

	const auto TYA = [this]() { this->TYA(); };
	instrs_[0x98] = { none, TYA, 2, 0 };

	// unofficial instructions
	const auto LAX = [this]() { this->LAX(); };
	instrs_[0xA7] = { zp    , LAX, 3, 0 };
	instrs_[0xB7] = { zpy   , LAX, 4, 0 };
	instrs_[0xAF] = { abs   , LAX, 4, 0 };
	instrs_[0xBF] = { aby   , LAX, 4, 1 };
	instrs_[0xA3] = { idxInd, LAX, 6, 0 };
	instrs_[0xB3] = { indIdx, LAX, 5, 1 };

	const auto SAX = [this]() { this->SAX(); };
	instrs_[0x87] = { zp    , SAX, 3, 0 };
	instrs_[0x97] = { zpy   , SAX, 4, 0 };
	instrs_[0x8F] = { abs   , SAX, 4, 0 };
	instrs_[0x83] = { idxInd, SAX, 6, 0 };

	const auto DCP = [this]() { this->DCP(); };
	instrs_[0xC7] = { zp    , DCP, 5, 0 };
	instrs_[0xD7] = { zpx   , DCP, 6, 0 };
	instrs_[0xCF] = { abs   , DCP, 6, 0 };
	instrs_[0xDF] = { abx   , DCP, 7, 0 };
	instrs_[0xDB] = { aby   , DCP, 7, 0 };
	instrs_[0xC3] = { idxInd, DCP, 8, 0 };
	instrs_[0xD3] = { indIdx, DCP, 8, 0 };

	const auto ISC = [this]() { this->ISC(); };
	instrs_[0xE7] = { zp    , ISC, 5, 0 };
	instrs_[0xF7] = { zpx   , ISC, 6, 0 };
	instrs_[0xEF] = { abs   , ISC, 6, 0 };
	instrs_[0xFF] = { abx   , ISC, 7, 0 };
	instrs_[0xFB] = { aby   , ISC, 7, 0 };
	instrs_[0xE3] = { idxInd, ISC, 8, 0 };
	instrs_[0xF3] = { indIdx, ISC, 8, 0 };

	const auto RLA = [this]() { this->RLA(); };
	instrs_[0x27] = { zp    , RLA, 5, 0 };
	instrs_[0x37] = { zpx   , RLA, 6, 0 };
	instrs_[0x2F] = { abs   , RLA, 6, 0 };
	instrs_[0x3F] = { abx   , RLA, 7, 0 };
	instrs_[0x3B] = { aby   , RLA, 7, 0 };
	instrs_[0x23] = { idxInd, RLA, 8, 0 };
	instrs_[0x33] = { indIdx, RLA, 8, 0 };

	const auto RRA = [this]() { this->RRA(); };
	instrs_[0x67] = { zp    , RRA, 5, 0 };
	instrs_[0x77] = { zpx   , RRA, 6, 0 };
	instrs_[0x6F] = { abs   , RRA, 6, 0 };
	instrs_[0x7F] = { abx   , RRA, 7, 0 };
	instrs_[0x7B] = { aby   , RRA, 7, 0 };
	instrs_[0x63] = { idxInd, RRA, 8, 0 };
	instrs_[0x73] = { indIdx, RRA, 8, 0 };

	const auto SLO = [this]() { this->SLO(); };
	instrs_[0x07] = { zp    , SLO, 5, 0 };
	instrs_[0x17] = { zpx   , SLO, 6, 0 };
	instrs_[0x0F] = { abs   , SLO, 6, 0 };
	instrs_[0x1F] = { abx   , SLO, 7, 0 };
	instrs_[0x1B] = { aby   , SLO, 7, 0 };
	instrs_[0x03] = { idxInd, SLO, 8, 0 };
	instrs_[0x13] = { indIdx, SLO, 8, 0 };

	const auto SRE = [this]() { this->SRE(); };
	instrs_[0x47] = { zp    , SRE, 5, 0 };
	instrs_[0x57] = { zpx   , SRE, 6, 0 };
	instrs_[0x4F] = { abs   , SRE, 6, 0 };
	instrs_[0x5F] = { abx   , SRE, 7, 0 };
	instrs_[0x5B] = { aby   , SRE, 7, 0 };
	instrs_[0x43] = { idxInd, SRE, 8, 0 };
	instrs_[0x53] = { indIdx, SRE, 8, 0 };

	// USBC or *SBC
	instrs_[0xEB] = { imm, SBC, 2, 0 };

	// NOPs
	instrs_[0x1A] = { none, NOP, 2, 0 };
	instrs_[0x3A] = { none, NOP, 2, 0 };
	instrs_[0x5A] = { none, NOP, 2, 0 };
	instrs_[0x7A] = { none, NOP, 2, 0 };
	instrs_[0xDA] = { none, NOP, 2, 0 };
	instrs_[0xFA] = { none, NOP, 2, 0 };

	instrs_[0x80] = { imm , NOP, 2, 0 };
	instrs_[0x82] = { imm , NOP, 2, 0 };
	instrs_[0x89] = { imm , NOP, 2, 0 };
	instrs_[0xC2] = { imm , NOP, 2, 0 };
	instrs_[0xE2] = { imm , NOP, 2, 0 };

	instrs_[0x0C] = { abs , NOP, 4, 0 };

	instrs_[0x1C] = { abx , NOP, 4, 1 };
	instrs_[0x3C] = { abx , NOP, 4, 1 };
	instrs_[0x5C] = { abx , NOP, 4, 1 };
	instrs_[0x7C] = { abx , NOP, 4, 1 };
	instrs_[0xDC] = { abx , NOP, 4, 1 };
	instrs_[0xFC] = { abx , NOP, 4, 1 };

	instrs_[0x04] = { zp  , NOP, 3, 0 };
	instrs_[0x44] = { zp  , NOP, 3, 0 };
	instrs_[0x64] = { zp  , NOP, 3, 0 };

	instrs_[0x14] = { zpx , NOP, 4, 0 };
	instrs_[0x34] = { zpx , NOP, 4, 0 };
	instrs_[0x54] = { zpx , NOP, 4, 0 };
	instrs_[0x74] = { zpx , NOP, 4, 0 };
	instrs_[0xD4] = { zpx , NOP, 4, 0 };
	instrs_[0xF4] = { zpx , NOP, 4, 0 };
} 

void CPU6502::update()
{	
#ifdef EMUCPULOG
	log_regs = reg_;
	log_cycles = total_cycles_;
#endif

	cycles_ = 0;
	opcode_ = getByteFromPC();
	instrs_[opcode_].addr_mode();

#ifdef EMUCPULOG
	print(reg_.PC - log_regs.PC);
#endif

	instrs_[opcode_].operate();
	cycles_ += instrs_[opcode_].cycles;
	setBitN(reg_.Status, 5, true);
	total_cycles_ += cycles_;
}

void CPU6502::connectToBus(Bus* bus)
{
	bus_ = bus;
}

void CPU6502::write(const u16 addr, const u8 data)
{
	bus_->cpuWrite(addr, data);
}

u8 CPU6502::read(const u16 addr)
{
	return bus_->cpuRead(addr);
}

void CPU6502::irq()
{
	if (getInterruptDisableFlag())
		return;
	pushStack(reg_.PC);
	pushStack(reg_.Status);
	reg_.PC = getTwoBytesFromMem(0xFFFE);
	setInterruptDisableFlag(true);
	cycles_ += 7;
}

void CPU6502::nmi()
{
	pushStack(reg_.PC);
	pushStack(reg_.Status);
	reg_.PC = getTwoBytesFromMem(0xFFFA);
	setInterruptDisableFlag(true);
	cycles_ += 7;
}

void CPU6502::ADC()
{
	const u8 M = fetch();
	const u8 C = getCarryFlag();
	const u8 result = reg_.A + M + C;

	setCarryFlag(result < reg_.A);
	setZeroFlag(result == 0);
	setOverflowFlag((result ^ reg_.A) & (result ^ M) & 0x80);
	setNegativeResultFlag(getBitN(result, 7));

	reg_.A = result;
}

void CPU6502::AND()
{
	const u8 M = fetch();
	reg_.A = reg_.A & M;
	setZeroFlag(reg_.A == 0);
	setNegativeResultFlag(getBitN(reg_.A, 7));
}

void CPU6502::ASL()
{
	u8 M = fetch();

	setCarryFlag(getBitN(M, 7));
	M <<= 1;
	setZeroFlag(M == 0);
	setNegativeResultFlag(getBitN(M, 7));

	if (addr_mode_ == AddrMode::Imp)
		reg_.A = M;
	else
		write(abs_addr_, M);
}

void CPU6502::BCC()
{
	if (!getCarryFlag())
		relativeDisplace();
}

void CPU6502::BCS()
{
	if (getCarryFlag())
		relativeDisplace();
}

void CPU6502::BEQ()
{
	if (getZeroFlag())
		relativeDisplace();
}

void CPU6502::BIT()
{
	const u8 M = read(abs_addr_);
	setZeroFlag((reg_.A & M) == 0);
	setOverflowFlag(getBitN(M, 6));
	setNegativeResultFlag(getBitN(M, 7));
}

void CPU6502::BMI()
{
	if (getNegativeResultFlag())
		relativeDisplace();
}

void CPU6502::BNE()
{
	if (!getZeroFlag())
		relativeDisplace();
}

void CPU6502::BPL()
{
	if (!getNegativeResultFlag())
		relativeDisplace();
}

void CPU6502::BRK()
{
	pushStack(reg_.PC);
	pushStack(reg_.Status);
	reg_.PC = getTwoBytesFromMem(0xFFFE);
	setBreakFlag(true);
}

void CPU6502::BVC()
{
	if (!getOverflowFlag())
		relativeDisplace();
}

void CPU6502::BVS()
{
	if (getOverflowFlag())
		relativeDisplace();
}

void CPU6502::CLC()
{
	setCarryFlag(false);
}

void CPU6502::CLD()
{
	setDecimalModeFlag(false);
}

void CPU6502::CLI()
{
	setInterruptDisableFlag(false);
}

void CPU6502::CLV()
{
	setOverflowFlag(false);
}

void CPU6502::CMP()
{
	const u8 M = fetch();
	const u8 result = reg_.A - M;
	setCarryFlag(reg_.A >= M);
	setZeroFlag(reg_.A == M);
	setNegativeResultFlag(getBitN(result, 7));
}

void CPU6502::CPX()
{
	const u8 M = fetch();
	const u8 result = reg_.X - M;
	setCarryFlag(reg_.X >= M);
	setZeroFlag(result == 0);
	setNegativeResultFlag(getBitN(result, 7));
}

void CPU6502::CPY()
{
	const u8 M = fetch();
	const u8 result = reg_.Y - M;
	setCarryFlag(reg_.Y >= M);
	setZeroFlag(result == 0);
	setNegativeResultFlag(getBitN(result, 7));
}

void CPU6502::DEC()
{
	u8 M = read(abs_addr_);
	--M;
	write(abs_addr_, M);
	setZeroFlag(M == 0);
	setNegativeResultFlag(getBitN(M, 7));
}

void CPU6502::DEX()
{
	--reg_.X;
	setZeroFlag(reg_.X == 0);
	setNegativeResultFlag(getBitN(reg_.X, 7));
}

void CPU6502::DEY()
{
	--reg_.Y;
	setZeroFlag(reg_.Y == 0);
	setNegativeResultFlag(getBitN(reg_.Y, 7));
}

void CPU6502::EOR()
{
	const u8 M = fetch();
	reg_.A = reg_.A ^ M;
	setZeroFlag(reg_.A == 0);
	setNegativeResultFlag(getBitN(reg_.A, 7));
}

void CPU6502::INC()
{
	u8 M = read(abs_addr_);
	++M;
	write(abs_addr_, M);
	setZeroFlag(M == 0);
	setNegativeResultFlag(getBitN(M, 7));
}

void CPU6502::INX()
{
	++reg_.X;
	setZeroFlag(reg_.X == 0);
	setNegativeResultFlag(getBitN(reg_.X, 7));
}

void CPU6502::INY()
{
	++reg_.Y;
	setZeroFlag(reg_.Y == 0);
	setNegativeResultFlag(getBitN(reg_.Y, 7));
}

void CPU6502::JMP()
{
	reg_.PC = abs_addr_;
}

void CPU6502::JSR()
{
	pushStack(--reg_.PC);
	reg_.PC = abs_addr_;
}

void CPU6502::LDA()
{
	const u8 M = fetch();
	reg_.A = M;
	setZeroFlag(reg_.A == 0);
	setNegativeResultFlag(getBitN(reg_.A, 7));
}

void CPU6502::LDX()
{
	const u8 M = fetch();
	reg_.X = M;
	setZeroFlag(reg_.X == 0);
	setNegativeResultFlag(getBitN(reg_.X, 7));
}

void CPU6502::LDY()
{
	const u8 M = fetch();
	reg_.Y = M;
	setZeroFlag(reg_.Y == 0);
	setNegativeResultFlag(getBitN(reg_.Y, 7));
}

void CPU6502::LSR()
{
	u8 M = fetch();

	setCarryFlag(getBitN(M, 0));
	M >>= 1;
	setZeroFlag(M == 0);
	setNegativeResultFlag(getBitN(M, 7));

	if (addr_mode_ == AddrMode::Imp)
		reg_.A = M;
	else
		write(abs_addr_, M);
}

void CPU6502::NOP()
{
}

void CPU6502::ORA()
{
	const u8 M = fetch();
	reg_.A = reg_.A | M;
	setZeroFlag(reg_.A == 0);
	setNegativeResultFlag(getBitN(reg_.A, 7));
}

void CPU6502::PHA()
{
	pushStack(reg_.A);
}

void CPU6502::PHP()
{
	const bool M = getBreakFlag();
	setBreakFlag(true);
	pushStack(reg_.Status);
	setBreakFlag(M);
}

void CPU6502::PLA()
{
	reg_.A = popStack();
	setZeroFlag(reg_.A == 0);
	setNegativeResultFlag(getBitN(reg_.A, 7));
}

void CPU6502::PLP()
{
	const bool M = getBreakFlag();
	reg_.Status = popStack();
	setBreakFlag(M);
}

void CPU6502::ROL()
{
	u8 M = fetch();

	const u8 new_bit0 = getCarryFlag();
	setCarryFlag(getBitN(M, 7));
	M <<= 1;
	setBitN(M, 0, new_bit0);
	setNegativeResultFlag(getBitN(M, 7));

	if (addr_mode_ == AddrMode::Imp)
		reg_.A = M;
	else
		write(abs_addr_, M);
}

void CPU6502::ROR()
{
	u8 M = fetch();

	const u8 new_bit7 = getCarryFlag();
	setCarryFlag(getBitN(M, 0));
	M >>= 1;
	setBitN(M, 7, new_bit7);
	setNegativeResultFlag(getBitN(M, 7));

	if (addr_mode_ == AddrMode::Imp)
		reg_.A = M;
	else
		write(abs_addr_, M);
}

void CPU6502::RTI()
{
	reg_.Status = popStack();
	reg_.PC = popStackTwoBytes();
}

void CPU6502::RTS()
{
	reg_.PC = popStackTwoBytes();
	++reg_.PC;
}

void CPU6502::SBC()
{
	const u8  M = fetch();
	const u16 C = getCarryFlag();
	const u16 value = static_cast<u16>(M) ^ 0x00FF;
	const u16 A = reg_.A;
	const u16 result = A + value + C;
	
	setCarryFlag(result & 0xFF00);
	setZeroFlag((result & 0x00FF) == 0);
	setNegativeResultFlag(getBitN(result, 7));
	setOverflowFlag((result ^ A) & (result ^ value) & 0x0080);

	reg_.A = static_cast<u8>(result);
}

void CPU6502::SEC()
{
	setCarryFlag(true);
}

void CPU6502::SED()
{
	setDecimalModeFlag(true);
}

void CPU6502::SEI()
{
	setInterruptDisableFlag(true);
}

void CPU6502::STA()
{
	write(abs_addr_, reg_.A);
}

void CPU6502::STX()
{
	write(abs_addr_, reg_.X);
}

void CPU6502::STY()
{
	write(abs_addr_, reg_.Y);
}

void CPU6502::TAX()
{
	reg_.X = reg_.A;
	setZeroFlag(reg_.X == 0);
	setNegativeResultFlag(getBitN(reg_.X, 7));
}

void CPU6502::TAY()
{
	reg_.Y = reg_.A;
	setZeroFlag(reg_.Y == 0);
	setNegativeResultFlag(getBitN(reg_.Y, 7));
}

void CPU6502::TSX()
{
	reg_.X = reg_.SP;	
	setZeroFlag(reg_.X == 0);
	setNegativeResultFlag(getBitN(reg_.X, 7));
}

void CPU6502::TXA()
{
	reg_.A = reg_.X;
	setZeroFlag(reg_.A == 0);
	setNegativeResultFlag(getBitN(reg_.A, 7));
}

void CPU6502::TXS()
{
	reg_.SP = reg_.X;
}

void CPU6502::TYA()
{
	reg_.A = reg_.Y;
	setZeroFlag(reg_.A == 0);
	setNegativeResultFlag(getBitN(reg_.A, 7));
}

void CPU6502::LAX()
{
	LDA();
	TAX();
}

void CPU6502::SAX()
{
	const u8 M = reg_.X & reg_.A;
	write(abs_addr_, M);
}

void CPU6502::DCP()
{
	DEC();
	CMP();
}

void CPU6502::ISC()
{
	INC();
	SBC();
}

void CPU6502::RLA()
{
	ROL();
	AND();
}

void CPU6502::RRA()
{
	ROR();
	ADC();
}

void CPU6502::SLO()
{
	ASL();
	ORA();
}

void CPU6502::SRE()
{
	LSR();
	EOR();
}

void CPU6502::unknownOpcode()
{
}

void CPU6502::relativeDisplace()
{
	const u16 old_pc = reg_.PC;
	reg_.PC = abs_addr_;
	++cycles_; // branch succeeds penalty
	if (pageCrossed(old_pc, reg_.PC))
		cycles_ += instrs_[opcode_].penalty;
}

void CPU6502::none()
{
	addr_mode_ = AddrMode::Imp;
}

void CPU6502::imm()
{
	addr_mode_ = AddrMode::Imm;
	abs_addr_ = getByteFromPC();
}

void CPU6502::rel()
{
	addr_mode_ = AddrMode::Rel;
	u8 displacement = getByteFromPC();
	const bool is_negative = getBitN(displacement, 7);
	if (is_negative)
	{
		displacement = ~displacement + static_cast<u8>(1);
		abs_addr_ = reg_.PC - static_cast<u16>(displacement);
	}
	else
		abs_addr_ = reg_.PC + static_cast<u16>(displacement);
}

void CPU6502::zp()
{
	addr_mode_ = AddrMode::ZP;
	abs_addr_ = static_cast<u16>(getByteFromPC());
}

void CPU6502::zpx()
{
	addr_mode_ = AddrMode::ZPX;
	const u8 addr = getByteFromPC() + reg_.X;
	abs_addr_ = static_cast<u16>(addr);
}

void CPU6502::zpy()
{
	addr_mode_ = AddrMode::ZPY;
	const u8 addr = getByteFromPC() + reg_.Y;
	abs_addr_ = static_cast<u16>(addr);
}

void CPU6502::abs()
{
	addr_mode_ = AddrMode::Abs;
	abs_addr_ = getTwoBytesFromPC();
}

void CPU6502::abx()
{
	addr_mode_ = AddrMode::Abx;
	const u16 addr = getTwoBytesFromPC();
	abs_addr_ = addr + static_cast<u16>(reg_.X);
	if (pageCrossed(addr, abs_addr_))
		cycles_ += instrs_[opcode_].penalty;
}

void CPU6502::aby()
{
	addr_mode_ = AddrMode::Aby;
	const u16 addr = getTwoBytesFromPC();
	abs_addr_ = addr + static_cast<u16>(reg_.Y);
	if (pageCrossed(addr, abs_addr_))
		cycles_ += instrs_[opcode_].penalty;
}

void CPU6502::ind()
{
	addr_mode_ = AddrMode::Ind;
	const u16 addr = getTwoBytesFromPC();
	if (static_cast<u8>(addr) == 0xFF) // this is a hardware bug
	{
		u16 result = static_cast<u16>(read(addr));
		const u16 msb_loc = addr & 0xFF00;
		result |= (static_cast<u16>(read(msb_loc)) << 8);
		abs_addr_ = result;
	}
	else 
		abs_addr_ = getTwoBytesFromMem(addr);
}

void CPU6502::idxInd()
{
	addr_mode_ = AddrMode::IdxInd;
	const u8 table_loc = getByteFromPC() + reg_.X;
	abs_addr_ = getTwoBytesFromZP(table_loc);
}

void CPU6502::indIdx()
{
	addr_mode_ = AddrMode::IndIdx;
	const u8 table_loc = getByteFromPC();
	const u16 addr = getTwoBytesFromZP(table_loc);
	abs_addr_ = addr + static_cast<u16>(reg_.Y);
	if (pageCrossed(addr, abs_addr_))
		cycles_ += instrs_[opcode_].penalty;
}

u8 CPU6502::fetch()
{
	if (addr_mode_ == AddrMode::Imm)
		return static_cast<u8>(abs_addr_);
	if (addr_mode_ == AddrMode::Imp)
		return reg_.A;
	return read(abs_addr_);
}

u8 CPU6502::getByteFromPC()
{
	return read(reg_.PC++);
}

u16 CPU6502::getTwoBytesFromPC()
{
	u16 result = read(reg_.PC++);
	result |= (static_cast<u16>(read(reg_.PC++)) << 8);
	return result;
}

u16 CPU6502::getTwoBytesFromZP(u8 loc)
{
	u16 result = read(loc++);
	result |= (static_cast<u16>(read(loc)) << 8);
	return result;
}

u16 CPU6502::getTwoBytesFromMem(u16 loc)
{
	u16 result = read(loc++);
	result |= (static_cast<u16>(read(loc)) << 8);
	return result;
}

u8 CPU6502::getCarryFlag() const
{
	return getBitN(reg_.Status, 0);
}

u8 CPU6502::getZeroFlag() const
{
	return getBitN(reg_.Status, 1);
}

u8 CPU6502::getInterruptDisableFlag() const
{
	return getBitN(reg_.Status, 2);
}

u8 CPU6502::getDecimalModeFlag() const
{
	return getBitN(reg_.Status, 3);
}

u8 CPU6502::getBreakFlag() const
{
	return getBitN(reg_.Status, 4);
}

u8 CPU6502::getOverflowFlag() const
{
	return getBitN(reg_.Status, 6);
}

u8 CPU6502::getNegativeResultFlag() const
{
	return getBitN(reg_.Status, 7);
}

void CPU6502::setCarryFlag(const bool set)
{
	setBitN(reg_.Status, 0, set);
}

void CPU6502::setZeroFlag(const bool set)
{
	setBitN(reg_.Status, 1, set);
}

void CPU6502::setInterruptDisableFlag(const bool set)
{
	setBitN(reg_.Status, 2, set);
}

void CPU6502::setDecimalModeFlag(const bool set)
{
	setBitN(reg_.Status, 3, set);
}

void CPU6502::setBreakFlag(const bool set)
{
	setBitN(reg_.Status, 4, set);
}

void CPU6502::setOverflowFlag(const bool set)
{
	setBitN(reg_.Status, 6, set);
}

void CPU6502::setNegativeResultFlag(const bool set)
{
	setBitN(reg_.Status, 7, set);
}

void CPU6502::pushStack(const u8 val)
{
	assert(reg_.SP > 0);
	write(stack_low + reg_.SP, val);
	--reg_.SP;
}

void CPU6502::pushStack(const u16 val)
{
	const u8 msb = static_cast<u8>(val >> 8);
	const u8 lsb = static_cast<u8>(val);
	pushStack(msb);
	pushStack(lsb);
}

u8 CPU6502::popStack()
{
	assert(reg_.SP < 255);
	++reg_.SP;
	return read(stack_low + reg_.SP);
}

u16 CPU6502::popStackTwoBytes()
{
	u16 result = popStack();
	result |= (static_cast<u16>(popStack()) << 8);
	return result;
}

bool CPU6502::pageCrossed(u16 addr1, u16 addr2)
{
	return (addr1 & 0xFF00) != (addr2 & 0xFF00);
}
