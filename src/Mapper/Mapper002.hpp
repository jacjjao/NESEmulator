#pragma once

#include "Mapper.hpp"


class Mapper002 : public Mapper
{
public:
	Mapper002(Cartridge cart);
	~Mapper002() override = default;

	bool cpuMapWrite(u16 addr, u8 data) override;
	std::optional<u8> cpuMapRead(u16 addr) override;

	bool ppuMapWrite(u16 addr, u8 data) override;
	std::optional<u8> ppuMapRead(u16 addr) override;

private:
	usize prg_low_ = 0;
	usize prg_high_;
};