#include "Mapper.hpp"

Mapper::Mapper(const std::size_t prg_ram_size_in_byte, const std::size_t prg_ram_bank_size, 
	           const std::size_t chr_ram_size_in_byte, const std::size_t chr_ram_bank_size) :
	prg_ram_{ prg_ram_size_in_byte, prg_ram_bank_size },
	chr_ram_{ chr_ram_size_in_byte, chr_ram_bank_size }
{
}

void Mapper::assignPrgRom(Rom prg_rom)
{
	prg_rom_ = std::move(prg_rom);
}

void Mapper::assignChrRom(Rom chr_rom)
{
	chr_rom_ = std::move(chr_rom);
}
