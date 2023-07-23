#include "PPU2C02.hpp"
#include "common/bitHelper.hpp"

#include <cstdlib>
#include <ctime>
#include <cassert>

PPU2C02::PPU2C02() :
	mem_(mem_size, 0),
	pixels_(resolution)
{
	srand(time(nullptr));
	std::size_t i = 0;
	for (int row = 0; row < 240; ++row)
		for (int col = 0; col < 256; ++col, ++i)
		{
			pixels_[i].setPosition({ static_cast<float>(col), static_cast<float>(row) });
			pixels_[i].setColor(sf::Color::Black);
		}
}

void PPU2C02::reset()
{
	PPUCTRL.reg = 0;
	PPUMASK.reg = 0;
	scroll_latch = addr_latch = 0;
	PPUSCROLL = 0;
	PPUDATA = 0;
	odd_frame_ = false;
}

void PPU2C02::update()
{
	
}

void PPU2C02::insertCartridge(std::shared_ptr<Cartridge> cartridge)
{
	cart_ = std::move(cartridge);
}

u8 PPU2C02::regRead(const u16 addr)
{
	assert(addr <= 0x07);

	u8 data = 0;

	switch (addr)
	{		
	case 0x02:
		data = (0x1F & data_buf_) | (0xE0 & PPUSTATUS.reg);
		PPUSTATUS.bit.vb_start = 0;
		scroll_latch = addr_latch = 0;
		break;

	case 0x04:
		// TODO
		break;

	case 0x07:
		data = data_buf_;
		data_buf_ = memRead(vram_addr_);
		if (vram_addr_ >= 0x3F00)
			data = data_buf_;
		vram_addr_ += (PPUCTRL.bit.vram_addr_inc ? 32 : 1);
		break;
	}

	return data;
}

void PPU2C02::regWrite(const u16 addr, const u8 data)
{
	assert(addr <= 0x07);

	switch (addr)
	{
	case 0x00:
		PPUCTRL.reg = data;
		break;

	case 0x01:
		PPUMASK.reg = data;
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
			tmp_vram_addr_ = (static_cast<u16>(data & 0x3F) << 8) | (tmp_vram_addr_ & 0x00FF);
			addr_latch = 1;
		}
		else
		{
			tmp_vram_addr_ = (tmp_vram_addr_ & 0xFF00) | static_cast<u16>(data);
			vram_addr_ = tmp_vram_addr_;
			addr_latch = 0;
		}
		break;

	case 0x07:
		memWrite(vram_addr_, data);
		vram_addr_ += (PPUCTRL.bit.vram_addr_inc ? 32 : 1);
		break;
	}
}

u8 PPU2C02::memRead(u16 addr)
{
	addr &= 0x3FFF;
	if (const auto data = cart_->ppuRead(addr); data.has_value())
	{
		return data.value();
	}
	return *mirroring(addr);
}

void PPU2C02::memWrite(u16 addr, const u8 data)
{
	addr &= 0x3FFF;
	if (cart_->ppuWrite(addr, data))
	{
		return;
	}
	*mirroring(addr) = data;
}

const std::vector<sf::Vertex>& PPU2C02::getVideoOutput() const
{
	return pixels_.getVertexArray();
}

bool PPU2C02::isFrameComplete() const
{
	return frame_complete_;
}

sf::Color PPU2C02::getPalette(const bool sprite_select, const u8 pixel, const u8 palette)
{
	u8 index = (sprite_select << 5) | (palette << 2) | pixel;
	index &= (PPUMASK.bit.grey_sacle ? 0x30 : 0x3F);

	sf::Color color = palette_.getColor(index);

	if (index == 0x0F)
		return color;

	if (PPUMASK.bit.empha_red)
	{
		color.r += (255 - color.r) / 2;
	}
	if (PPUMASK.bit.empha_green)
	{
		color.g += (255 - color.g) / 2;
	}
	if (PPUMASK.bit.empha_blue)
	{
		color.b += (255 - color.b) / 2;
	}

	return color;
}

u8* PPU2C02::mirroring(u16 addr)
{
	if (0x2000 <= addr && addr <= 0x2FFF)
	{
		if (cart_->getMirrorType() == MirrorType::Vertical)
		{
			if (0x2800 <= addr && addr <= 0x2FFF)
			{
				addr -= 0x0800;
			}
		}
		else if (cart_->getMirrorType() == MirrorType::Horizontal)
		{
			if ((0x2400 <= addr && addr <= 0x27FF) ||
				(0x2C00 <= addr && addr <= 0x2FFF))
			{
				addr -= 0x0400;
			}
		}
	}
	else if (0x3000 <= addr && addr <= 0x3EFF)
	{
		addr &= 0x2EFF;
	}
	else if (0x3F00 <= addr && addr <= 0x3F1F)
	{
		return &palette_[addr];
	}
	else if (0x3F20 <= addr && addr <= 0x3FFF)
	{
		addr &= 0x3F1F;
	}
	return &mem_[addr];
}