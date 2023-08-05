#pragma once

#include "Mapper.hpp"


class Mapper000 : public Mapper
{
public:
	Mapper000(Cartridge cart);
	~Mapper000() override = default;

	std::optional<u8> cpuMapRead(u16 addr) override;

	bool ppuMapWrite(u16 addr, u8 data) override;
	std::optional<u8> ppuMapRead(u16 addr) override;
};