#pragma once

#include "../common/type.hpp"
#include <vector>


constexpr std::size_t operator ""_KB(unsigned long long val)
{
	return val * 1024;
}

class Memory
{
public:
	Memory(std::size_t bank_size, std::size_t size_in_byte);

	void setBankSize(std::size_t bank_size);
	void switchBank(std::size_t bank);

	std::size_t getMemSize() const;

	u8& operator[](std::size_t addr);

	const u8& operator[](std::size_t addr) const;

	template<typename Iter>
	void assign(Iter data_begin, Iter data_end)
	{
		mem_.assign(data_begin, data_end);
	}

private:
	std::size_t bank_size_;
	std::size_t bank_start_ = 0;
	std::vector<u8> mem_;
};