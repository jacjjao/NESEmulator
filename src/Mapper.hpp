#pragma once

#include "common/type.hpp"


class Mapper
{
public:
	virtual u16 cpuMapWrite(u16 addr) { return addr; }
	virtual u16 cpuMapRead(u16 addr) { return addr; }
	
	virtual u16 ppuMapWrite(u16 addr) { return addr; }
	virtual u16 ppuMapRead(u16 addr) { return addr; }
};