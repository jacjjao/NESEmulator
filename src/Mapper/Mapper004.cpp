#include "Mapper004.hpp"
#include "../Bus.hpp"
#include "../common/bitHelper.hpp"
#include <cassert>	


Mapper004::Mapper004(Cartridge cart) :
	Mapper{ std::move(cart) },
	prg_ram_(8_KB)
{
	const usize prg_banks = cart_.PRGRomSize() / 8_KB;
	fix_prg_second_last_ = &cart_.PRGRom()[(prg_banks - 2) * 8_KB];
	fix_prg_last_ = &cart_.PRGRom()[(prg_banks - 1) * 8_KB];
	prg_banks_[0] = prg_banks_[1] = &cart_.PRGRom()[0];
	chr_2kbanks_[0] = chr_2kbanks_[1] = &cart_.CHRMem()[0];
	chr_1kbanks_[0] = chr_1kbanks_[1] = &cart_.CHRMem()[0];
	chr_1kbanks_[2] = chr_1kbanks_[3] = &cart_.CHRMem()[0];
}

bool Mapper004::cpuMapWrite(const u16 addr, const u8 data)
{
	if (addr < 0x6000)
	{
		return false;
	}
	
	if (addr < 0x8000)
	{
		if (!ram_write_protect_)
		{
			prg_ram_[addr & 0x1FFF] = data;
		}
		return true;
	}
	
	const bool addr_is_even = (addr % 2 == 0);
	if (addr <= 0x9FFF)
	{
		if (addr_is_even)
		{
			bank_select_ = data & 0x07;
			prg_bank_mode_ = getBitN(data, 6);
			chr_bank_inversion_ = getBitN(data, 7);
		}
		else
		{
			switch (bank_select_)
			{
			case 0b000:
				chr_2kbanks_[0] = &cart_.CHRMem()[(data & 0xFE) * 1_KB];
				break;

			case 0b001:
				chr_2kbanks_[1] = &cart_.CHRMem()[(data & 0xFE) * 1_KB];
				break;

			case 0b010:
				chr_1kbanks_[0] = &cart_.CHRMem()[data * 1_KB];
				break;

			case 0b011:
				chr_1kbanks_[1] = &cart_.CHRMem()[data * 1_KB];
				break;

			case 0b100:
				chr_1kbanks_[2] = &cart_.CHRMem()[data * 1_KB];
				break;

			case 0b101:
				chr_1kbanks_[3] = &cart_.CHRMem()[data * 1_KB];
				break;

			case 0b110:
				prg_banks_[0] = &cart_.PRGRom()[(data & 0x3F) * 8_KB];
				break;

			case 0b111:
				prg_banks_[1] = &cart_.PRGRom()[(data & 0x3F) * 8_KB];
				break;
			}
		}
	}
	else if (addr <= 0xBFFF)
	{
		if (addr_is_even)
		{
			cart_.mirror_type = (data & 0x01 ? MirrorType::Horizontal : MirrorType::Vertical);
		}
		else
		{
			ram_write_protect_ = getBitN(data, 6);
			ram_enable_ = getBitN(data, 7);
		}
	}
	else if (addr <= 0xDFFF)
	{
		if (addr_is_even)
		{
			irq_latch_ = data;
		}
		else
		{
			irq_counter_ = 0;
			reload_flag_ = true;
		}
	}
	else
	{
		if (addr_is_even)
		{
			irq_enable_ = false;
		}
		else
		{
			irq_enable_ = true;
		}
	}
	return true;
}

std::optional<u8> Mapper004::cpuMapRead(const u16 addr)
{
	if (addr < 0x6000)
	{
		return std::nullopt;
	}

	if (addr < 0x8000)
	{
		return prg_ram_[addr & 0x1FFF];
	}

	if (addr <= 0x9FFF)
	{
		if (!prg_bank_mode_)
		{
			return prg_banks_[0][addr & 0x1FFF];
		}
		return fix_prg_second_last_[addr & 0x1FFF];
	}
	if (addr <= 0xBFFF)
	{
		return prg_banks_[1][addr & 0x1FFF];
	}
	if (addr <= 0xDFFF)
	{
		if (!prg_bank_mode_)
		{
			return fix_prg_second_last_[addr & 0x1FFF];
		}
		return prg_banks_[0][addr & 0x1FFF];
	}
	return fix_prg_last_[addr & 0x1FFF];
}

std::optional<u8> Mapper004::ppuMapRead(const u16 addr)
{
	if (addr >= 0x2000)
	{
		return std::nullopt;
	}

	if (addr <= 0x07FF)
	{
		if (chr_bank_inversion_)
		{
			if (addr <= 0x03FF)
			{
				return chr_1kbanks_[0][addr & 0x03FF];
			}
			return chr_1kbanks_[1][addr & 0x03FF];
		}
		return chr_2kbanks_[0][addr & 0x07FF];
	}
	if (addr <= 0x0FFF)
	{
		if (chr_bank_inversion_)
		{
			if (addr <= 0x0BFF)
			{
				return chr_1kbanks_[2][addr & 0x03FF];
			}
			return chr_1kbanks_[3][addr & 0x03FF];
		}
		return chr_2kbanks_[1][addr & 0x07FF];
	}
	if (addr <= 0x17FF)
	{
		if (!chr_bank_inversion_)
		{
			if (addr <= 0x13FF)
			{
				return chr_1kbanks_[0][addr & 0x03FF];
			}
			return chr_1kbanks_[1][addr & 0x03FF];
		}
		return chr_2kbanks_[0][addr & 0x07FF];
	}
	if (!chr_bank_inversion_)
	{
		if (addr <= 0x1BFF)
		{
			return chr_1kbanks_[2][addr & 0x03FF];
		}
		return chr_1kbanks_[3][addr & 0x03FF];
	}
	return chr_2kbanks_[1][addr & 0x07FF];
}

void Mapper004::updateIRQCounter(const u8 PPUCTRL, const unsigned sprite_count, const unsigned scanline, const unsigned cycle)
{
	if (scanline >= 240 || scanline_tracker_ == scanline) return;

	const bool sp_pat_tb = getBitN(PPUCTRL, 3);
	const bool bg_pat_tb = getBitN(PPUCTRL, 4);
	const bool sp_size = getBitN(PPUCTRL, 5);

	if (!sp_size) // 8x8 sprite
	{
		if (!bg_pat_tb && sp_pat_tb)
		{
			if (cycle != 260) return;
		}
		else if (bg_pat_tb && !sp_pat_tb)
		{
			if (cycle != 324) return;
		}
	}
	else
	{
		if (sprite_count >= 8) return;
	}

	updateIRQCounterNoCheck();
}

void Mapper004::updateIRQCounterNoCheck()
{
	if (reload_flag_ || irq_counter_ == 0)
	{
		irq_counter_ = irq_latch_;
	}
	else if (irq_counter_ > 0)
	{
		--irq_counter_;
	}
	
	if (irq_counter_ == 0 && irq_enable_)
	{
		Bus::instance().cpu.irq();
	}
}
