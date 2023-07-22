#include "PPU2C02.hpp"
#include "common/bitHelper.hpp"

#include <cstdlib>
#include <ctime>

PPU2C02::PPU2C02() :
	mem_(mem_size, 0),
	pixels_(resolution),
	palettes_(palette_size)
{
	srand(time(nullptr));

	palettes_[0x00] = sf::Color(84, 84, 84);
	palettes_[0x01] = sf::Color(0, 30, 116);
	palettes_[0x02] = sf::Color(8, 16, 144);
	palettes_[0x03] = sf::Color(48, 0, 136);
	palettes_[0x04] = sf::Color(68, 0, 100);
	palettes_[0x05] = sf::Color(92, 0, 48);
	palettes_[0x06] = sf::Color(84, 4, 0);
	palettes_[0x07] = sf::Color(60, 24, 0);
	palettes_[0x08] = sf::Color(32, 42, 0);
	palettes_[0x09] = sf::Color(8, 58, 0);
	palettes_[0x0A] = sf::Color(0, 64, 0);
	palettes_[0x0B] = sf::Color(0, 60, 0);
	palettes_[0x0C] = sf::Color(0, 50, 60);
	palettes_[0x0D] = sf::Color(0, 0, 0);
	palettes_[0x0E] = sf::Color(0, 0, 0);
	palettes_[0x0F] = sf::Color(0, 0, 0);

	palettes_[0x10] = sf::Color(152, 150, 152);
	palettes_[0x11] = sf::Color(8, 76, 196);
	palettes_[0x12] = sf::Color(48, 50, 236);
	palettes_[0x13] = sf::Color(92, 30, 228);
	palettes_[0x14] = sf::Color(136, 20, 176);
	palettes_[0x15] = sf::Color(160, 20, 100);
	palettes_[0x16] = sf::Color(152, 34, 32);
	palettes_[0x17] = sf::Color(120, 60, 0);
	palettes_[0x18] = sf::Color(84, 90, 0);
	palettes_[0x19] = sf::Color(40, 114, 0);
	palettes_[0x1A] = sf::Color(8, 124, 0);
	palettes_[0x1B] = sf::Color(0, 118, 40);
	palettes_[0x1C] = sf::Color(0, 102, 120);
	palettes_[0x1D] = sf::Color(0, 0, 0);
	palettes_[0x1E] = sf::Color(0, 0, 0);
	palettes_[0x1F] = sf::Color(0, 0, 0);

	palettes_[0x20] = sf::Color(236, 238, 236);
	palettes_[0x21] = sf::Color(76, 154, 236);
	palettes_[0x22] = sf::Color(120, 124, 236);
	palettes_[0x23] = sf::Color(176, 98, 236);
	palettes_[0x24] = sf::Color(228, 84, 236);
	palettes_[0x25] = sf::Color(236, 88, 180);
	palettes_[0x26] = sf::Color(236, 106, 100);
	palettes_[0x27] = sf::Color(212, 136, 32);
	palettes_[0x28] = sf::Color(160, 170, 0);
	palettes_[0x29] = sf::Color(116, 196, 0);
	palettes_[0x2A] = sf::Color(76, 208, 32);
	palettes_[0x2B] = sf::Color(56, 204, 108);
	palettes_[0x2C] = sf::Color(56, 180, 204);
	palettes_[0x2D] = sf::Color(60, 60, 60);
	palettes_[0x2E] = sf::Color(0, 0, 0);
	palettes_[0x2F] = sf::Color(0, 0, 0);

	palettes_[0x30] = sf::Color(236, 238, 236);
	palettes_[0x31] = sf::Color(168, 204, 236);
	palettes_[0x32] = sf::Color(188, 188, 236);
	palettes_[0x33] = sf::Color(212, 178, 236);
	palettes_[0x34] = sf::Color(236, 174, 236);
	palettes_[0x35] = sf::Color(236, 174, 212);
	palettes_[0x36] = sf::Color(236, 180, 176);
	palettes_[0x37] = sf::Color(228, 196, 144);
	palettes_[0x38] = sf::Color(204, 210, 120);
	palettes_[0x39] = sf::Color(180, 222, 120);
	palettes_[0x3A] = sf::Color(168, 226, 144);
	palettes_[0x3B] = sf::Color(152, 226, 180);
	palettes_[0x3C] = sf::Color(160, 214, 228);
	palettes_[0x3D] = sf::Color(160, 162, 160);
	palettes_[0x3E] = sf::Color(0, 0, 0);
	palettes_[0x3F] = sf::Color(0, 0, 0);
}

void PPU2C02::update()
{
	static int row = 0, col = 0;
	static std::size_t i = 0;
	if (i >= pixels_.size())
		return;

	pixels_[i].setPosition(sf::Vector2f{static_cast<float>(col), static_cast<float>(row)});
	pixels_[i].setColor(rand() % 2 ? sf::Color::White : sf::Color::Blue);

	if (++col == 256)
	{
		++row;
		col = 0;
	}
	++i;
}

void PPU2C02::insertCartridge(std::shared_ptr<Cartridge> cartridge)
{
	cart_ = std::move(cartridge);
}

u8 PPU2C02::regRead(u16 addr)
{
	addr &= 0x0007;

	u8 data = data_buf_;

	switch (addr)
	{
	case 0x00: case 0x01: case 0x03: 
	case 0x04: case 0x05: case 0x06:
		break;
		
	case 0x02:
		data = (0x1F & data_buf_) | (0xE0 & PPUSTATUS.reg);
		PPUSTATUS.vb_start = 0;
		scroll_latch = addr_latch = 0;
		data_buf_ = data;
		break;

	case 0x07:
		// TODO read from vram
		break;
	}

	return data;
}

void PPU2C02::regWrite(u16 addr, const u8 data)
{
	data_buf_ = data;

	addr &= 0x0007;

	switch (addr)
	{
	case 0x00:
		PPUCTRL.reg = data;
		break;

	case 0x01:
		PPUMASK.reg = data;
		break;

	case 0x02:
		break;

	case 0x03:
		// TODO
		break;

	case 0x04:
		// TODO
		break;

	case 0x05:
		// TODO
		break;

	case 0x06:
		if (addr_latch == 0)
		{
			vram_addr = (static_cast<u16>(data) << 8);
			addr_latch = 1;
		}
		else
		{
			vram_addr |= static_cast<u16>(data);
			addr_latch = 0;
		}
		break;

	case 0x07:
		// TODO
		break;
	}
}

u8 PPU2C02::memRead(const u16 addr) const
{
	if (const auto data = cart_->ppuRead(addr); data.has_value())
	{
		return data.value();
	}
	else if (0x3000 <= addr && addr <= 0x3EFF)
	{
		return mem_[addr & 0x2EFF];
	}
	else if (0x3F20 <= addr && addr <= 0x3FFF)
	{
		return mem_[addr & 0x3F1F];
	}
	return mem_[addr];
}

void PPU2C02::memWrite(const u16 addr, const u8 data)
{
	data_buf_ = data;

	if (cart_->ppuWrite(addr, data))
	{

	}
	else if (0x3000 <= addr && addr <= 0x3EFF)
	{
		mem_[addr & 0x2EFF] = data;
	}
	else if (0x3F20 <= addr && addr <= 0x3FFF)
	{
		mem_[addr & 0x3F1F] = data;
	}
	else
	{
		mem_[addr] = data;
	}
}

const std::vector<sf::Vertex>& PPU2C02::getOutput() const
{
	return pixels_.getVertexArray();
}

bool PPU2C02::isFrameComplete() const
{
	return frame_complete_;
}
