#pragma once

#include "Mapper.hpp"


class Mapper000 : public Mapper
{
public:
	Mapper000(std::size_t prg_rom_size_in_byte, std::size_t chr_rom_size_in_byte);
	~Mapper000() override = default;

	std::optional<u8> cpuMapRead(const u16 addr) override;

	bool ppuMapWrite(const u16 addr, const u8 data) override;
	std::optional<u8> ppuMapRead(const u16 addr) override;

	void loadPrgRom(u8* data_begin, u8* data_end) override;
	void loadChrRom(u8* data_begin, u8* data_end) override;

private:
	Memory prg_rom_, chr_mem_;
	bool is_chr_ram_;
};