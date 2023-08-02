#include "Mapper001.hpp"
#include "../common/bitHelper.hpp"
#include <stdexcept>


Mapper001::Mapper001(const std::size_t prg_rom_size_in_byte, const std::size_t chr_rom_size_in_byte) :
	prg_rom_{ 16_KB, prg_rom_size_in_byte },
	chr_rom_{  8_KB, chr_rom_size_in_byte }
{
}

bool Mapper001::cpuMapWrite(const u16 addr, const u8 data)
{
	if (0x8000 <= addr && addr <= 0xFFFF)
	{
		if (getBitN(data, 7))
		{
			reset();
		}
		else
		{
			if (getBitN(shift_reg_, 0))
			{
				shift_reg_ = (getBitN(data, 0) << 4) | (shift_reg_ >> 1);
				setControlMode(shift_reg_);
				shift_reg_ = 0x10;
			}
			else
			{
				shift_reg_ = (getBitN(data, 0) << 4) | (shift_reg_ >> 1);
			}
		}
		return true;
	}
	return false;
}

std::optional<u8> Mapper001::cpuMapRead(const u16 addr)
{
	return std::optional<u8>();
}

bool Mapper001::ppuMapWrite(const u16 addr, const u8 data)
{
	return false;
}

std::optional<u8> Mapper001::ppuMapRead(const u16 addr)
{
	return std::optional<u8>();
}

void Mapper001::reset()
{
	shift_reg_ = 0x10;
	prg_bank_mode_ = 3;
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
