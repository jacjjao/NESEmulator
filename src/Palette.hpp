#pragma once

#include "common/type.hpp"
#include <vector>
#include <SFML/Graphics/Color.hpp>


class Palette
{
public:
private:
	std::vector<u8> ram_;
	std::vector<sf::Color> color_;
};