#pragma once

#include "../common/type.hpp"
#include <optional>


class Mapper
{
public:
	virtual ~Mapper() = default;

	virtual std::optional<std::size_t> cpuMapWrite(u16, u8) { return std::nullopt; }
	virtual std::optional<std::size_t> cpuMapRead(u16) { return std::nullopt; }
	
	virtual std::optional<std::size_t> ppuMapWrite(u16, u8) { return std::nullopt; }
	virtual std::optional<std::size_t> ppuMapRead(u16) { return std::nullopt; }

	virtual void reset() {};
};