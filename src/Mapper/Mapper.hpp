#pragma once

#include "Memory.hpp"
#include <optional>


enum class MirrorType
{
	Vertical, Horizontal
};

class Mapper
{
public:
	virtual ~Mapper() = default;

	virtual bool cpuMapWrite(u16, u8) { return false; }
	virtual std::optional<u8> cpuMapRead(u16) { return std::nullopt; }
	
	virtual bool ppuMapWrite(u16, u8) { return false; }
	virtual std::optional<u8> ppuMapRead(u16) { return std::nullopt; }

	virtual void reset() {};

	virtual void loadPrgRom(u8* data_begin, u8* data_end) = 0;
	virtual void loadChrRom(u8* data_begin, u8* data_end) = 0;

	void setMirrortype(MirrorType type);
	MirrorType getMirrortype() const;

private:
	MirrorType mirror_type_;
};