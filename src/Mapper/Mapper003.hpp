#pragma once

#include "Mapper.hpp"


class Mapper003 : public Mapper
{
public:
	Mapper003(Cartridge cart);
	~Mapper003() override = default;

	bool cpuMapWrite(u16 addr, u8 data) override;
	std::optional<u8> cpuMapRead(u16 addr) override;

	bool ppuMapWrite(u16 addr, u8 data) override;
	std::optional<u8> ppuMapRead(u16 addr) override;

private:
	usize chr_bank_start_ = 0;
};