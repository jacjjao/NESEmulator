#include "Ram.hpp"

Ram::Ram(const std::size_t size_in_byte, const std::size_t bank_size) :
	mem_{ size_in_byte, bank_size }
{
}

void Ram::switchBank(const std::size_t bank)
{
	mem_.switchBank(bank);
}

std::size_t Ram::getBankSize() const
{
	return mem_.getBankSize();
}

u8& Ram::operator[](std::size_t addr)
{
	return mem_[addr];
}

const u8& Ram::operator[](std::size_t addr) const
{
	return mem_[addr];
}
