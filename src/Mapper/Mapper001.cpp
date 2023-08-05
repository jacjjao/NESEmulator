#include "Mapper001.hpp"
#include "../common/bitHelper.hpp"



Mapper001::Mapper001(Cartridge cart) :
	Mapper{ cart },
	nprg_banks_{ cart.PRGRomSize() / 16_KB },
	prg_ram_(8_KB)
{
}

bool Mapper001::cpuMapWrite(const u16 addr, const u8 data)
{
	if (addr < 0x6000)
	{
		return false;
	}

	if (addr < 0x8000)
	{
		prg_ram_[addr & 0x1FFF] = data;
		return true;
	}

	if (getBitN(data, 7))
	{
		reset();
		return true;
	}

	if (!getBitN(shift_reg_, 0))
	{
		shift_reg_ = ((data & 0x01) << 4) | (shift_reg_ >> 1);
		return true;
	}

	const u8 cmd = ((data & 0x01) << 4) | (shift_reg_ >> 1);
	shift_reg_ = 0x10;
	if (addr <= 0x9FFF)
	{
		switch (cmd & 0x03)
		{
		case 0:
			cart_.mirror_type = MirrorType::OneScreenLow;
			break;

		case 1:
			cart_.mirror_type = MirrorType::OneScreenHigh;
			break;

		case 2:
			cart_.mirror_type = MirrorType::Vertical;
			break;

		case 3:
			cart_.mirror_type = MirrorType::Horizontal;
			break;
		}

		prg_bank_mode_ = ((cmd >> 2) & 0x03);
		if (prg_bank_mode_ == 2)
		{
			prg16_bank_low_ = 0;
		}
		else if (prg_bank_mode_ == 3)
		{
			prg16_bank_high_ = nprg_banks_ - 1;
		}

		chr_bank_mode_ = getBitN(cmd, 4);
	}
	else if (addr <= 0xBFFF)
	{
		if (chr_bank_mode_)
		{
			chr4_bank_low_ = (cmd & 0x1F);
		}
		else // 8KB mode
		{
			chr8_bank_ = (cmd & 0x1E);
		}
	}
	else if (addr <= 0xDFFF)
	{
		if (chr_bank_mode_) // 4KB mode
		{
			chr4_bank_high_ = (cmd & 0x1F);
		}
	}
	else
	{
		switch (prg_bank_mode_)
		{
		case 0: case 1: // 32KB mode
			prg32_bank_ = (cmd & 0x0E);
			break;

		case 2: 
			prg16_bank_high_ = (cmd & 0x0F);
			break;

		case 3:
			prg16_bank_low_  = (cmd & 0x0F);
			break;
		}
	}
	return true;
}

std::optional<u8> Mapper001::cpuMapRead(const u16 addr)
{
	if (addr < 0x6000)
	{
		return std::nullopt;
	}

	if (addr < 0x8000)
	{
		return prg_ram_[addr & 0x1FFF];
	}
	if (prg_bank_mode_ <= 1) // 32KB mode
	{
		return cart_.PRGRom()[prg32_bank_ * 32_KB + (addr & 0x7FFF)];
	}
	if (addr < 0xC000)
	{
		return cart_.PRGRom()[prg16_bank_low_ * 16_KB + (addr & 0x3FFF)];
	}
	return cart_.PRGRom()[prg16_bank_high_ * 16_KB + (addr & 0x3FFF)];
}

bool Mapper001::ppuMapWrite(const u16 addr, const u8 data)
{
	if (cart_.useCHRRam() && addr <= 0x1FFF)
	{
		cart_.CHRMem()[addr] = data;
		return true;
	}
	return false;
}

std::optional<u8> Mapper001::ppuMapRead(const u16 addr)
{
	if (addr >= 0x2000)
	{
		return std::nullopt;
	}
	if (cart_.useCHRRam())
	{
		return cart_.CHRMem()[addr];
	}
	if (chr_bank_mode_)
	{
		if (addr < 0x1000)
		{
			return cart_.CHRMem()[chr4_bank_low_ * 4_KB + (addr & 0x0FFF)];
		}
		return cart_.CHRMem()[chr4_bank_high_ * 4_KB + (addr & 0x0FFF)];
	}
	return cart_.CHRMem()[chr8_bank_ * 8_KB + addr];
}

void Mapper001::reset()
{
	shift_reg_ = 0x10;
	prg_bank_mode_ = 3;
	prg16_bank_high_ = nprg_banks_ - 1;
}