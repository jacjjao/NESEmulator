#include "Tile.hpp"

Pixel::Pixel(sf::Vertex& vertex)
{
	vertex_ = &vertex;
}

void Pixel::setColor(const sf::Color& color)
{
	for (std::size_t i = 0; i < 4; ++i)
		vertex_[i].color = color;
}

void Pixel::setPosition(const sf::Vector2f pos)
{
	vertex_[0].position = pos;
	vertex_[1].position = { pos.x + 1.0f, pos.y };
	vertex_[2].position = { pos.x + 1.0f, pos.y + 1.0f };
	vertex_[3].position = { pos.x       , pos.y + 1.0f };
}

sf::Vertex& Pixel::getVertex()
{
	return *vertex_;
}

PixelArray::PixelArray(const std::size_t size) :
	vertices_(size * 4),
	size_(size)
{
}

Pixel PixelArray::operator[](const std::size_t index)
{
	return Pixel{ vertices_[index * 4] };
}

std::size_t PixelArray::size() const
{
	return size_;
}

const std::vector<sf::Vertex>& PixelArray::getVertexArray() const
{
	return vertices_;
}

/*
Tile::Tile(const std::array<Pixel, 8> pixels)
{
	for (int row = 0; row < 8; ++row)
	{
		for (int col = 0; col < 8; ++col)

			pixels_[row * 8 + col] = pixels[row];
	}
}

void Tile::setColor(const sf::Color& color)
{
	for (auto& pixel : pixels_)
		pixel->setColor(color);
}

TileArray::TileArray(const std::size_t size) : 
	pixel_arr_(size * 8 * 8),
	tiles_(size)
{
	for (std::size_t i = 0, row = 0; row < 240; row += 8)
		for (std::size_t col = 0; col < 256; col += 8, ++i)
			tiles_[i].setTile(pixel_arr_[row * 256 + col]);
}

Tile& TileArray::operator[](const std::size_t index)
{
	return tiles_[index];
}

std::size_t TileArray::size() const
{
	return tiles_.size();
}

const PixelArray& TileArray::getPixelArray() const
{
	return pixel_arr_;
}
*/