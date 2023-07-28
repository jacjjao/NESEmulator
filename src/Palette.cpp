#include "Palette.hpp"

Palette::Palette() : 
	ram_(ram_size),
	palette_(palette_size)
{
	palette_[0x00] = sf::Color(84, 84, 84);
	palette_[0x01] = sf::Color(0, 30, 116);
	palette_[0x02] = sf::Color(8, 16, 144);
	palette_[0x03] = sf::Color(48, 0, 136);
	palette_[0x04] = sf::Color(68, 0, 100);
	palette_[0x05] = sf::Color(92, 0, 48);
	palette_[0x06] = sf::Color(84, 4, 0);
	palette_[0x07] = sf::Color(60, 24, 0);
	palette_[0x08] = sf::Color(32, 42, 0);
	palette_[0x09] = sf::Color(8, 58, 0);
	palette_[0x0A] = sf::Color(0, 64, 0);
	palette_[0x0B] = sf::Color(0, 60, 0);
	palette_[0x0C] = sf::Color(0, 50, 60);
	palette_[0x0D] = sf::Color(0, 0, 0);
	palette_[0x0E] = sf::Color(0, 0, 0);
	palette_[0x0F] = sf::Color(0, 0, 0);

	palette_[0x10] = sf::Color(152, 150, 152);
	palette_[0x11] = sf::Color(8, 76, 196);
	palette_[0x12] = sf::Color(48, 50, 236);
	palette_[0x13] = sf::Color(92, 30, 228);
	palette_[0x14] = sf::Color(136, 20, 176);
	palette_[0x15] = sf::Color(160, 20, 100);
	palette_[0x16] = sf::Color(152, 34, 32);
	palette_[0x17] = sf::Color(120, 60, 0);
	palette_[0x18] = sf::Color(84, 90, 0);
	palette_[0x19] = sf::Color(40, 114, 0);
	palette_[0x1A] = sf::Color(8, 124, 0);
	palette_[0x1B] = sf::Color(0, 118, 40);
	palette_[0x1C] = sf::Color(0, 102, 120);
	palette_[0x1D] = sf::Color(0, 0, 0);
	palette_[0x1E] = sf::Color(0, 0, 0);
	palette_[0x1F] = sf::Color(0, 0, 0);

	palette_[0x20] = sf::Color(236, 238, 236);
	palette_[0x21] = sf::Color(76, 154, 236);
	palette_[0x22] = sf::Color(120, 124, 236);
	palette_[0x23] = sf::Color(176, 98, 236);
	palette_[0x24] = sf::Color(228, 84, 236);
	palette_[0x25] = sf::Color(236, 88, 180);
	palette_[0x26] = sf::Color(236, 106, 100);
	palette_[0x27] = sf::Color(212, 136, 32);
	palette_[0x28] = sf::Color(160, 170, 0);
	palette_[0x29] = sf::Color(116, 196, 0);
	palette_[0x2A] = sf::Color(76, 208, 32);
	palette_[0x2B] = sf::Color(56, 204, 108);
	palette_[0x2C] = sf::Color(56, 180, 204);
	palette_[0x2D] = sf::Color(60, 60, 60);
	palette_[0x2E] = sf::Color(0, 0, 0);
	palette_[0x2F] = sf::Color(0, 0, 0);

	palette_[0x30] = sf::Color(236, 238, 236);
	palette_[0x31] = sf::Color(168, 204, 236);
	palette_[0x32] = sf::Color(188, 188, 236);
	palette_[0x33] = sf::Color(212, 178, 236);
	palette_[0x34] = sf::Color(236, 174, 236);
	palette_[0x35] = sf::Color(236, 174, 212);
	palette_[0x36] = sf::Color(236, 180, 176);
	palette_[0x37] = sf::Color(228, 196, 144);
	palette_[0x38] = sf::Color(204, 210, 120);
	palette_[0x39] = sf::Color(180, 222, 120);
	palette_[0x3A] = sf::Color(168, 226, 144);
	palette_[0x3B] = sf::Color(152, 226, 180);
	palette_[0x3C] = sf::Color(160, 214, 228);
	palette_[0x3D] = sf::Color(160, 162, 160);
	palette_[0x3E] = sf::Color(0, 0, 0);
	palette_[0x3F] = sf::Color(0, 0, 0);
}

u8& Palette::operator[](u16 addr)
{
	addr &= 0x1F;

	if (addr == 0x10 || addr == 0x14 || addr == 0x18 || addr == 0x1C)
		addr &= 0x0F;

	return ram_[addr];
}

sf::Color Palette::getColor(const u8 index) const
{
	return palette_[index];
}
