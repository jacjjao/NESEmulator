#include "PPU2C02.hpp"
#include "common/bitHelper.hpp"


PPU2C02::PPU2C02() :
	mem_(mem_size, 0),
	pixels_(resolution)
{
	std::size_t i = 0;
	for (int row = 0; row < 240; ++row)
		for (int col = 0; col < 256; ++col, ++i)
		{
			pixels_[i].setPosition({ static_cast<float>(col), static_cast<float>(row) });
			pixels_[i].setColor(palette_.getColor(0x00));
		}

	PPUSTATUS.reg = 0xA0;
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
	if (cycle_ == -1 || cycle_ == 1)
	{
		PPUSTATUS.bit.vb_start = 0;
	}

	if (cycle_ == 241)
	{
		PPUSTATUS.bit.vb_start = 1;
		nmi_occured = PPUCTRL.bit.gen_nmi;
	}

	if (cycle_ == 260)
	{
		cycle_ = -1;
		nmi_occured = false;
		return;
	}

	++cycle_;
}

void PPU2C02::insertCartridge(std::shared_ptr<Cartridge> cartridge)
{
	cart_ = std::move(cartridge);
}

u8 PPU2C02::regRead(u16 addr)
{
	addr &= 0x07;

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

void PPU2C02::regWrite(u16 addr, const u8 data)
{
	addr &= 0x07;

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

PixelArray& PPU2C02::dbgGetPatterntb(const int index, const u8 palette)
{
	static PixelArray patterntb[2]{ (128 * 128), (128 * 128) };
	static bool is_init = false;

	if (!is_init)
	{
		std::size_t i = 0;
		for (int row = 0; row < 128; ++row)
			for (int col = 0; col < 128; ++col, ++i)
			{
				patterntb[0][i].setPosition({ float(256 + 11 + col), float(11 + row) });
				patterntb[1][i].setPosition({ float(256 + 11 + col), float(128 + 22 + row) });
			}

		is_init = true;
	}

	int row = 0, col = 0;
	for (u16 addr = (index ? 0x1000 : 0x0000); addr < (index ? 0x2000 : 0x1000); addr += 16)
	{
		std::array<u8, 8> first{}, second{};
		for (int i = 0; i < 8; ++i)
		{
			first[i] = memRead(addr + i);
			second[i] = memRead(addr + i + 8);
		}
		
		for (int i = 0; i < 8; ++i)
		{
			for (int j = 0; j < 8; ++j)
			{
				const u8 lowerbit = first[i] & 0x01;
				const u8 upperbit = second[i] & 0x01;
				const u8 pixel = (upperbit << 1) | lowerbit;

				first[i] >>= 1;
				second[i] >>= 1;

				patterntb[index][(row + i) * 128 + col + (7 - j)].setColor(dbgGetColor(palette, pixel));
			}
		}

		col += 8;
		if (col >= 128)
		{
			col = 0;
			row += 8;
		}
	}


	return patterntb[index];
}

sf::Color PPU2C02::dbgGetColor(const u16 palette, const u16 pixel)
{
	return palette_.getColor(memRead(0x3F00 + (palette << 2) + pixel) & 0x3F);
}

void PPU2C02::dbgDrawNametb()
{
	for (int row = 0; row < 30; ++row)
	{
		for (int col = 0; col < 32; ++col)
		{
			const u8 tile = memRead(row * 32 + col);
			
		}
	}
}

std::vector<sf::Vertex>& PPU2C02::dbgGetFramePalette(const u8 index)
{
	static std::vector<sf::Vertex> frame_palette[8];
	static bool init = false;
	static float size = 7.5f;

	if (!init)
	{
		sf::Vector2f pos{ 50.0f, 250.0f };
		for (int i = 0; i < 8; ++i)
		{
			frame_palette[i].resize(16);
			for (int j = 0; j < 4; ++j)
			{
				frame_palette[i][j * 4 + 0].position = pos;
				frame_palette[i][j * 4 + 1].position = { pos.x + size, pos.y };
				frame_palette[i][j * 4 + 2].position = { pos.x + size, pos.y + size };
				frame_palette[i][j * 4 + 3].position = { pos.x, pos.y + size };
				pos.x += size;
			}

			pos.x += size;
			if (i == 3)
			{
				pos.y += size * 2;
				pos.x = 50.0f;
			}
		}
		init = true;
	}

	u16 addr = 0x3F01 + index * 4;
	for (int j = 0; j < 4; ++j)
	{
		sf::Color color = dbgGetColor(index, j);
		frame_palette[index][j * 4].color = color;
		frame_palette[index][j * 4 + 1].color = color;
		frame_palette[index][j * 4 + 2].color = color;
		frame_palette[index][j * 4 + 3].color = color;
	}
		
	return frame_palette[index];
}
