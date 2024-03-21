#pragma once

#include "../Cartridge.hpp"

#include <memory>
#include <optional>


class Mapper
{
public:
	Mapper(Cartridge cart);
	virtual ~Mapper() = default;

	virtual bool cpuMapWrite(u16, u8) { return false; }
	virtual std::optional<u8> cpuMapRead(u16) { return false; }
	
	virtual bool ppuMapWrite(u16, u8) { return false; }
	virtual std::optional<u8> ppuMapRead(u16) { return false; }

	virtual void updateIRQCounter() {};

	MirrorType getMirrorType();

	bool irq = false;

protected:
	Cartridge cart_;
};