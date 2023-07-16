#pragma once

#include "common/type.hpp"
#include <filesystem>
#include <vector>


class Cartridge
{
public:
	bool loadiNESFile(const std::filesystem::path& path);
	
private:
	struct Header
	{
		u8 cN, cE, cS, cEOF; 
		u8 prg_rom_size;
		u8 chr_rom_size;
		u8 mapper1;
		u8 mapper2;
		u8 prg_ram_size;
		u8 tv_system;
		u8 tv_system_prg;
		u8 u11, u12, u13, u14, u15; // unused bytes
	} header_;

	std::vector<u8> prg_rom_;
	std::vector<u8> chr_rom_;
};