#pragma once

#include "common/type.hpp"
#include <vector>
#include <SFML/Graphics/Color.hpp>


class Palette
{
public:
	Palette();

	u8& operator[](u16 addr);

	sf::Color getColor(u8 index) const;

private:
	static constexpr std::size_t ram_size = 32;
	static constexpr std::size_t palette_size = 64;

	std::vector<u8> ram_;
	std::vector<sf::Color> palette_;
};