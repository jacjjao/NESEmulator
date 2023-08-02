#include "Memory.hpp"

Memory::Memory(const std::size_t size_in_byte, const std::size_t bank_size) :
	mem_(size_in_byte),
	bank_size_{bank_size}
{
}

void Memory::switchBank(const std::size_t bank)
{
	bank_start_ = bank * bank_size_;
}

std::size_t Memory::getBankSize() const
{
	return bank_size_;
}

u8& Memory::operator[](std::size_t addr)
{
	return mem_[bank_start_ + addr];
}

const u8& Memory::operator[](std::size_t addr) const
{
	return mem_[bank_start_ + addr];
}
