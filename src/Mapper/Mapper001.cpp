#include "Mapper001.hpp"
#include "../common/bitHelper.hpp"



Mapper001::Mapper001(const std::size_t chr_rom_size_in_byte) :
	use_chr_ram_{ chr_rom_size_in_byte == 0 }
{
}

bool Mapper001::cpuMapWrite(const u16 addr, const u8 data, usize&)
{
	if (addr < 0x6000)
	{
		return false;
	}
	if (addr < 0x8000)
	{
		return prg_ram_disable_;
	}
	if (getBitN(data, 7))
	{
		reset();
		return true;
	}

	if (!getBitN(shift_reg_, 0))
	{
		shift_reg_ = (data & 0x01 << 4) | (shift_reg_ >> 1);
		return true;
	}

	u8 cmd = (data & 0x01 << 4) | (shift_reg_ >> 1);
	shift_reg_ = 0x10;
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
			chr_bank1_ = cmd;
		}
	}
	else
	{
		prg_ram_disable_ = getBitN(cmd, 4);
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

bool Mapper001::cpuMapRead(const u16 addr, usize& mapped_addr)
{
	if (addr < 0x8000)
	{
		return false;
	}
	if (prg_bank_mode_ <= 1) // 32KB mode
	{
		mapped_addr = prg_bank0_ * 32_KB + (addr & 0x7FFF);
	}
	else
	{
		if (addr < 0xC000)
		{
			mapped_addr = prg_bank0_ * 16_KB + (addr & 0x3FFF);
		}
		else
		{
			mapped_addr = prg_bank1_ * 16_KB + (addr & 0x3FFF);
		}
	}
	return true;
}

bool Mapper001::ppuMapWrite(const u16 addr, const u8 data, usize& mapped_addr)
{
	if (use_chr_ram_ && addr <= 0x1FFF)
	{
		mapped_addr = addr;
		return true;
	}
	if (0xA000 <= addr && addr <= 0xDFFF)
	{
		if (addr <= 0xBFFF)
		{
			if (chr_bank_mode_)
			{
				chr_bank0_ = getBitN(data, 0);
			}
			prg_ram_disable_ = getBitN(data, 4);
		}
		else
		{
			if (chr_bank_mode_)
			{
				chr_bank1_ = getBitN(data, 0);
				prg_ram_disable_ = getBitN(data, 4);
			}
		}
		return true;
	}
	return false;
}

bool Mapper001::ppuMapRead(const u16 addr, usize& mapped_addr)
{
	if (addr >= 0x2000)
	{
		return false;
	}
	if (use_chr_ram_)
	{
		mapped_addr = addr;
		return true;
	}
	if (chr_bank_mode_)
	{
		if (addr < 0x1000)
		{
			mapped_addr = chr_bank0_ * 4_KB + (addr & 0x0FFF);
		}
		else
		{
			mapped_addr = chr_bank1_ * 4_KB + (addr & 0x0FFF);
		}
	}
	else // 8KB mode
	{
		mapped_addr = chr_bank0_ * 8_KB + addr;
	}
	return true;
}

void Mapper001::reset()
{
	shift_reg_ = 0x10;
	prg_bank_mode_ = 3;
}

void Mapper001::setControlMode(const u8 data)
{
	switch (data & 0x03)
	{
	case 0:
		setMirrortype(MirrorType::OneScreenLow);
		break;

	case 1:
		setMirrortype(MirrorType::OneScreenHigh);
		break;

	case 2:
		setMirrortype(MirrorType::Vertical);
		break;

	case 3:
		setMirrortype(MirrorType::Horizontal);
		break;
	}

	prg_bank_mode_ = ((data & 0x0C) >> 2);
	chr_bank_mode_ = getBitN(data, 4);
}
