#pragma once

#include <SFML/Graphics/Vertex.hpp>
#include <vector>
#include <array>


class Pixel // This is actually a rectangle with 1 pixel width and 1 pixel height
{
public:
	Pixel(sf::Vertex& vertex);

	void setColor(const sf::Color& color);
	void setPosition(sf::Vector2f pos);

	sf::Vertex& getVertex();

private:
	sf::Vertex* vertex_;
};

class PixelArray
{
public:
	PixelArray(std::size_t size);

	Pixel operator[](std::size_t index);

	std::size_t size() const;

	const std::vector<sf::Vertex>& getVertexArray() const;

private:
	std::size_t size_;
	std::vector<sf::Vertex> vertices_;
};
/*
class Tile
{
public:
	Tile(std::array<Pixel, 8 * 8> pixels);

	void setColor(const sf::Color& color);

private:
	std::array<Pixel, 8 * 8> pixels_;
};

class TileArray
{
public:
	TileArray(std::size_t size);

	Tile& operator[](std::size_t index);

	std::size_t size() const;

	const PixelArray& getPixelArray() const;

private:
	PixelArray pixel_arr_;
	std::vector<Tile> tiles_;
};
*/