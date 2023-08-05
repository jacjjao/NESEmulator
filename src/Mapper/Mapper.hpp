#pragma once

#include "../common/type.hpp"


constexpr usize operator ""_KB(unsigned long long val)
{
	return val * 1024;
}

enum class MirrorType
{
	Vertical, Horizontal,
	OneScreenLow, OneScreenHigh,
	FourScreen
};

class Mapper
{
public:
	virtual ~Mapper() = default;

	virtual bool cpuMapWrite(u16, u8, usize&) { return false; }
	virtual bool cpuMapRead(u16, usize&) { return false; }
	
	virtual bool ppuMapWrite(u16, u8, usize&) { return false; }
	virtual bool ppuMapRead(u16, usize&) { return false; }

	virtual void reset() {};

	void setMirrortype(MirrorType type);
	MirrorType getMirrortype() const;

private:
	MirrorType mirror_type_ = MirrorType::FourScreen;
};