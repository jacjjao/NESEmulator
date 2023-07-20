#pragma once

#include "common/type.hpp"
#include <SFML/Graphics/Vertex.hpp>
#include <array>
#include <vector>


class PPU2C02
{
public:
	PPU2C02();

	std::array<u8, 8> reg;

	u8 read(u16 addr) const;
	void write(u16 addr, u8 data);

	u8* getPatternTable(u16 index);
	u8* getNameTable(u16 index);

	const std::vector<sf::Vertex>& getOutput() const;

private:
	static constexpr std::size_t mem_size = 16 * 1024; // 16kB
	static constexpr std::size_t resolution = 256 * 240;

	std::vector<sf::Vertex> video_output_;
	std::vector<u8> mem_;
};