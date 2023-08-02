#pragma once

#include "Memory.hpp"


class Ram
{
public:
	Ram(std::size_t size_in_byte, std::size_t bank_size);

	void switchBank(std::size_t bank);

	std::size_t getBankSize() const;

	u8& operator[](std::size_t addr);

	const u8& operator[](std::size_t addr) const;

private:
	Memory mem_;
};