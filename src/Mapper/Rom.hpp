#pragma once

#include "Memory.hpp"


class Rom
{
public:
	Rom() = default;

	template<typename Iter>
	Rom(Iter data_begin, Iter data_end, std::size_t bank_size) :
		mem_{data_begin, data_end, bank_size}
	{
	}

	void switchBank(std::size_t bank);

	std::size_t getBankSize() const;

	u8 operator[](std::size_t addr) const;

private:
	Memory mem_;
};