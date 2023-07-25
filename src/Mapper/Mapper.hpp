#pragma once

#include "common/type.hpp"
#include <optional>


class Mapper
{
public:
	virtual ~Mapper() = default;

	virtual std::optional<u16> cpuMapWrite(u16) { return std::nullopt; }
	virtual std::optional<u16> cpuMapRead(u16) { return std::nullopt; }
	
	virtual std::optional<u16> ppuMapWrite(u16) { return std::nullopt; }
	virtual std::optional<u16> ppuMapRead(u16) { return std::nullopt; }
};