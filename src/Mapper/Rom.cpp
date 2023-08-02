#include "Rom.hpp"


void Rom::switchBank(const std::size_t bank)
{
	mem_.switchBank(bank);
}

std::size_t Rom::getBankSize() const
{
	return mem_.getBankSize();
}

u8 Rom::operator[](std::size_t addr) const
{
	return mem_[addr];
}