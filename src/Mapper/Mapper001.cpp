#include "Mapper001.hpp"
#include "../common/bitHelper.hpp"
#include <stdexcept>


Mapper001::Mapper001(const std::size_t prg_rom_size_in_byte, const std::size_t chr_rom_size_in_byte) :
	prg_rom_{ 16_KB, prg_rom_size_in_byte },
	chr_rom_{ 8_KB, (chr_rom_size_in_byte ? chr_rom_size_in_byte : 32_KB) },
	is_chr_ram { chr_rom_size_in_byte == 0 }
{
}

bool Mapper001::cpuMapWrite(const u16 addr, const u8 data)
{
	if (0x8000 <= addr && addr <= 0xFFFF)
	{
		if (getBitN(data, 7))
		{
			reset();
			return true;
		}

		u8 cmd = 0;
		if (getBitN(shift_reg_, 0))
		{
			cmd = (getBitN(data, 0) << 4) | (shift_reg_ >> 1);
			shift_reg_ = 0x10;
		}
		else
		{
			shift_reg_ = (getBitN(data, 0) << 4) | (shift_reg_ >> 1);
		}

		if (addr <= 0x9FFF)
		{
			setControlMode(cmd);
		}
		else if (addr <= 0xBFFF)
		{
			if (chr_bank_mode_)
			{
				chr_bank0_ = cmd;
			}
			else // 8KB mode
			{
				chr_bank0_ = (cmd >> 1);
			}
		}
		else if (addr <= 0xDFFF)
		{
			if (chr_bank_mode_) // 4KB mode
			{
				chr_bank0_ = cmd;
			}
		}
		else
		{
			cmd &= 0x0F;
			switch (prg_bank_mode_)
			{
			case 0: case 1: // 32KB mode
				prg_bank0_ = (cmd >> 1);
				break;

			case 2: 
				prg_bank1_ = cmd;
				break;

			case 3:
				prg_bank0_ = cmd;
				break;
			}
		}
		return true;
	}
	return false;
}

std::optional<u8> Mapper001::cpuMapRead(const u16 addr)
{
	if (addr < 0x8000)
	{
		return std::nullopt;
	}
	
	if (prg_bank_mode_ <= 1) // 32KB mode
	{
		prg_rom_.switchBank(prg_bank0_);
		return prg_rom_[addr & 0x7FFF];
	}
	else
	{
		if (addr < 0xC000)
		{
			prg_rom_.switchBank(prg_bank0_);
		}
		else
		{
			prg_rom_.switchBank(prg_bank1_);
		}
		return prg_rom_[addr & 0x3FFF];
	}
}

bool Mapper001::ppuMapWrite(const u16 addr, const u8 data)
{
	if (!is_chr_ram || addr >= 0x2000)
	{
		return false;
	}
	
	chr_rom_.switchBank(chr_bank0_);
	chr_rom_[addr] = data;
	return true;
}

std::optional<u8> Mapper001::ppuMapRead(const u16 addr)
{
	if (addr >= 0x2000)
	{
		return std::nullopt;
	}

	if (chr_bank_mode_)
	{
		if (addr < 0x1000)
		{
			chr_rom_.switchBank(chr_bank0_);
		}
		else
		{
			chr_rom_.switchBank(chr_bank1_);
		}
		return chr_rom_[addr & 0x0FFF];
	}
	else // 8KB mode
	{
		chr_rom_.switchBank(chr_bank0_);
		return chr_rom_[addr];
	}
}

void Mapper001::reset()
{
	shift_reg_ = 0x10;
	prg_bank_mode_ = 3;
}

void Mapper001::loadPrgRom(u8* data_begin, u8* data_end)
{
	prg_rom_.assign(data_begin, data_end);
}

void Mapper001::loadChrRom(u8* data_begin, u8* data_end)
{
	chr_rom_.assign(data_begin, data_end);
}

void Mapper001::setControlMode(const u8 data)
{
	u8 mirroring = data & 0x03;
	switch (mirroring)
	{
	case 2:
		setMirrortype(MirrorType::Vertical);
		break;

	case 3:
		setMirrortype(MirrorType::Horizontal);
		break;

	default:
		throw std::runtime_error{ "Unimplement Mirror mode" };
	}

	prg_bank_mode_ = ((data & 0x0C) >> 2);
	switch (prg_bank_mode_)
	{
	case 0: case 1:
		prg_rom_.setBankSize(32_KB);
		break;

	case 2: case 3:
		prg_rom_.setBankSize(16_KB);
		break;
	}

	chr_bank_mode_ = getBitN(data, 4);
	switch (chr_bank_mode_)
	{
	case 0:
		chr_rom_.setBankSize(8_KB);
		break;

	case 1:
		prg_rom_.setBankSize(4_KB);
		break;
	}
}
