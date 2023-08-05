#include "Mapper004.hpp"
#include "../common/bitHelper.hpp"


Mapper004::Mapper004(const usize prg_rom_size) : 
	nprg_banks_{ prg_rom_size / 8_KB }
{
}

bool Mapper004::cpuMapWrite(const u16 addr, const u8 data, usize& mapped_addr)
{
	if (addr < 0x6000)
	{
		return false;
	}
	
	if (addr < 0x8000)
	{
		return ram_write_protect_;
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
				chr_2kbanks_[0] = data;
				break;

			case 0b001:
				chr_2kbanks_[1] = data;
				break;

			case 0b010:
				chr_1kbanks_[0] = data;
				break;

			case 0b011:
				chr_1kbanks_[1] = data;
				break;

			case 0b100:
				chr_1kbanks_[2] = data;
				break;

			case 0b101:
				chr_1kbanks_[3] = data;
				break;

			case 0b110:
				prg_banks_[0] = data;
				break;

			case 0b111:
				prg_banks_[1] = data;
				break;
			}
		}
	}
	else if (addr <= 0xBFFF)
	{
		if (addr_is_even)
		{
			setMirrortype((data & 0x01 ? MirrorType::Horizontal : MirrorType::Vertical));
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

bool Mapper004::cpuMapRead(const u16 addr, usize& mapped_addr)
{
	if (addr < 0x8000)
	{
		return false;
	}

	if (addr <= 0x9FFF)
	{
		if (!prg_bank_mode_)
		{
			mapped_addr = prg_banks_[0] * 8_KB + (addr & 0x1FFF);
		}
		else
		{
			mapped_addr = (nprg_banks_ - 2) * 8_KB + (addr & 0x1FFF);
		}
	}
	else if (addr <= 0xBFFF)
	{
		mapped_addr = prg_banks_[1] * 8_KB + (addr & 0x1FFF);
	}
	else if (addr <= 0xDFFF)
	{
		if (!prg_bank_mode_)
		{
			mapped_addr = (nprg_banks_ - 2) * 8_KB + (addr & 0x1FFF);
		}
		else
		{
			mapped_addr = prg_banks_[0] * 8_KB + (addr & 0x1FFF);
		}
	}
	else
	{
		mapped_addr = (nprg_banks_ - 1) * 8_KB + (addr & 0x1FFF);
	}
	return true;
}

bool Mapper004::ppuMapRead(const u16 addr, usize& mapped_addr)
{
	if (addr >= 0x2000)
	{
		return false;
	}

	if (addr <= 0x07FF)
	{
		if (!chr_bank_inversion_)
		{
			mapped_addr = chr_2kbanks_[0] * 2_KB + addr;
		}
		else
		{
			if (addr <= 0x03FF)
			{
				mapped_addr = chr_1kbanks_[0] * 1_KB + addr;
			}
			else
			{
				mapped_addr = chr_1kbanks_[1] * 1_KB + (addr & 0x03FF);
			}
		}
	}
	else if (addr <= 0x0FFF)
	{
		if (!chr_bank_inversion_)
		{
			mapped_addr = chr_2kbanks_[1] * 2_KB + (addr & 0x07FF);
		}
		else
		{
			if (addr <= 0x03FF)
			{
				mapped_addr = chr_1kbanks_[2] * 1_KB + (addr & 0x03FF);
			}
			else
			{
				mapped_addr = chr_1kbanks_[3] * 1_KB + (addr & 0x03FF);
			}
		}
	}
	else if (addr <= 0x17FF)
	{
		if (chr_bank_inversion_)
		{
			mapped_addr = chr_2kbanks_[0] * 2_KB + (addr & 0x07FF);
		}
		else
		{
			if (addr <= 0x03FF)
			{
				mapped_addr = chr_1kbanks_[0] * 1_KB + (addr & 0x03FF);
			}
			else
			{
				mapped_addr = chr_1kbanks_[1] * 1_KB + (addr & 0x03FF);
			}
		}
	}
	else if (addr <= 0x1FFF)
	{
		if (chr_bank_inversion_)
		{
			mapped_addr = chr_2kbanks_[1] * 2_KB + (addr & 0x07FF);
		}
		else
		{
			if (addr <= 0x03FF)
			{
				mapped_addr = chr_1kbanks_[2] * 1_KB + (addr & 0x03FF);
			}
			else
			{
				mapped_addr = chr_1kbanks_[3] * 1_KB + (addr & 0x03FF);
			}
		}
	}
	return true;
}
