#pragma once

#include "../common/type.hpp"
#include <vector>


class Memory
{
public:
	Memory() = default;

	Memory(std::size_t size_in_byte, std::size_t bank_size);

	template<typename Iter>
	Memory(Iter data_begin, Iter data_end, std::size_t bank_size) : 
		mem(data_begin, data_end),
		bank_size_{bank_size}
	{
	}
	
	void switchBank(std::size_t bank);

	std::size_t getBankSize() const;

	u8& operator[](std::size_t addr);

	const u8& operator[](std::size_t addr) const;

private:
	const std::size_t bank_size_;
	std::size_t bank_start_ = 0;
	std::vector<u8> mem_;
};