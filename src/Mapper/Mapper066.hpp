#pragma once

#include "Mapper.hpp"


class Mapper066 : public Mapper
{
public:
	Mapper066(Cartridge cart);

	bool cpuMapWrite(u16 addr, u8 data) override;
	std::optional<u8> cpuMapRead(u16 addr) override;

	std::optional<u8> ppuMapRead(u16 addr) override;

private:
	u8* chr_rom;
	u8* prg_rom;
};