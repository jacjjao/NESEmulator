#pragma once

#include "Rom.hpp"
#include "Ram.hpp"
#include <optional>


class Mapper
{
public:
	Mapper(std::size_t prg_ram_size_in_byte = 0, std::size_t prg_ram_bank_size = 0,
		   std::size_t chr_ram_size_in_byte = 0, std::size_t chr_ram_bank_size = 0);

	virtual ~Mapper() = default;

	virtual bool cpuMapWrite(u16, u8) { return false; }
	virtual std::optional<u8> cpuMapRead(u16) { return std::nullopt; }
	
	virtual bool ppuMapWrite(u16, u8) { return false; }
	virtual std::optional<u8> ppuMapRead(u16) { return std::nullopt; }

	virtual void reset() {};

	void assignPrgRom(Rom prg_rom);
	void assignChrRom(Rom chr_rom);

protected:
	Rom prg_rom_, chr_rom_;
	Ram prg_ram_, chr_ram_;
};