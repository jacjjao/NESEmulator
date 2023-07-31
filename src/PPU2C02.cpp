#include "PPU2C02.hpp"
#include "common/bitHelper.hpp"
#include <cassert>

#include <SFML/System/Clock.hpp>


PPU2C02::PPU2C02() :
	mem_(mem_size, 0),
	primary_oam_(primary_oam_size, 0),
	pixels_(resolution),
	second_oam_(second_oam_size),
	sprite_buf_(8)
{
	std::size_t i = 0;
	for (int row = 0; row < 240; ++row)
		for (int col = 0; col < 256; ++col, ++i)
		{
			pixels_[i].setPosition({ static_cast<float>(col), static_cast<float>(row) });
			pixels_[i].setColor(palette_.getColor(0x00));
		}
}

void PPU2C02::reset()
{
	PPUCTRL.reg = 0;
	PPUMASK.reg = 0;
	PPUSTATUS.reg = 0;
	vram_addr_.reg = 0;
	tvram_addr_.reg = 0;
	write_latch_ = false;
	PPUSCROLL = 0;
	PPUDATA = 0;
	scanline_ = cycle_ = 0;
}

void PPU2C02::update()
{
	const auto fetchName = [this] {
		const u16 next_tile_addr = 0x2000 | (vram_addr_.reg & 0x0FFF);
		bg_latches_.tile_name = memRead(next_tile_addr);
	};
	const auto fetchAttribute = [this] {
		const u16 v = vram_addr_.reg;
		u16 next_attr_addr = 0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07);

		const u8 data = memRead(next_attr_addr);
		const bool is_top = (vram_addr_.scroll.coarse_y & 0x03) < 2;
		const bool is_left = (vram_addr_.scroll.coarse_x & 0x03) < 2;
		u8 offset = 0;
		if (is_top)
		{
			if (is_left) // top left
			{
				offset = 0;
			}
			else // top right
			{
				offset = 2;
			}
		}
		else
		{
			if (is_left) // bottom left
			{
				offset = 4;
			}
			else // bottom right
			{
				offset = 6;
			}
		}
		const u8 pal = (data >> offset);
		bg_latches_.attr_low = (getBitN(pal, 0) ? 0xFF : 0x00);
		bg_latches_.attr_high = (getBitN(pal, 1) ? 0xFF : 0x00);
	};
	const auto fetchLowerPattern = [this] {
		u16 next_pat_addr = PPUCTRL.bit.bg_patterntb_addr;
		next_pat_addr = (next_pat_addr << 8) | bg_latches_.tile_name;
		next_pat_addr = (next_pat_addr << 1) | 0x00;
		next_pat_addr = (next_pat_addr << 3) | vram_addr_.scroll.fine_y;
		bg_latches_.pat_low = memRead(next_pat_addr);
	};
	const auto fetchUpperPattern = [this] {
		u16 next_pat_addr = PPUCTRL.bit.bg_patterntb_addr;
		next_pat_addr = (next_pat_addr << 8) | bg_latches_.tile_name;
		next_pat_addr = (next_pat_addr << 1) | 0x01;
		next_pat_addr = (next_pat_addr << 3) | vram_addr_.scroll.fine_y;
		bg_latches_.pat_high = memRead(next_pat_addr);
	};
	const auto loadRegisters = [this] {
		shift_reg_.pat_high  = (shift_reg_.pat_high  & 0xFF00) | bg_latches_.pat_high;
		shift_reg_.pat_low   = (shift_reg_.pat_low   & 0xFF00) | bg_latches_.pat_low;
		shift_reg_.attr_high = (shift_reg_.attr_high & 0xFF00) | bg_latches_.attr_high;
		shift_reg_.attr_low  = (shift_reg_.attr_low  & 0xFF00) | bg_latches_.attr_low;
	};
	const auto drawBGPixel = [this](u8& pal, u8& pat) {
		if (cycle_ > 256 || scanline_ == 261 || cycle_ == 0) return 0;
		const u8 pos        = 15 - (fine_x & 0x07);
		const u8 pixel_high = getBitN(shift_reg_.pat_high, pos);
		const u8 pixel_low  = getBitN(shift_reg_.pat_low, pos);
		const u8 pal_high   = getBitN(shift_reg_.attr_high, pos);
		const u8 pal_low    = getBitN(shift_reg_.attr_low, pos);
		pat = (pixel_high << 1) | pixel_low;
		pal = (pal_high << 1)   | pal_low;
	};
	const auto shiftRegisters = [this] {
		shift_reg_.pat_high  <<= 1;
		shift_reg_.pat_low   <<= 1;
		shift_reg_.attr_high <<= 1;
		shift_reg_.attr_low  <<= 1;
	};
	const auto incCoarseX = [this] {
		if (vram_addr_.scroll.coarse_x == 31)
		{
			vram_addr_.scroll.coarse_x = 0;
			vram_addr_.scroll.nametable_x = ~vram_addr_.scroll.nametable_x;
		}
		else
			++vram_addr_.scroll.coarse_x;
	};
	const auto incY = [this] {
		if (cycle_ != 256) return;
		if (vram_addr_.scroll.fine_y < 7)
			++vram_addr_.scroll.fine_y;
		else
		{
			vram_addr_.scroll.fine_y = 0;
			u8 y = vram_addr_.scroll.coarse_y;
			if (y == 29)
			{
				y = 0;
				vram_addr_.scroll.nametable_y = ~vram_addr_.scroll.nametable_y;
			}
			else if (y == 31)
				y = 0;
			else
				++y;
			vram_addr_.scroll.coarse_y = y;
		}
	};
	const auto transferTtoV = [&] {
		if (cycle_ == 257)
		{
			loadRegisters();
			vram_addr_.scroll.nametable_x = tvram_addr_.scroll.nametable_x;
			vram_addr_.scroll.coarse_x = tvram_addr_.scroll.coarse_x;
		}
		if (scanline_ == 261 && (280 <= cycle_ && cycle_ <= 304))
		{
			vram_addr_.scroll.fine_y = tvram_addr_.scroll.fine_y;
			vram_addr_.scroll.nametable_y = tvram_addr_.scroll.nametable_y;
			vram_addr_.scroll.coarse_y = tvram_addr_.scroll.coarse_y;
		}
	};
	const auto fetch = [&] {
		if (!((cycle_ >= 2 && cycle_ < 258) || (cycle_ >= 321 && cycle_ < 338))) return;
		shiftRegisters();
		switch ((cycle_ - 1) % 8)
		{
		case 0:
			loadRegisters();
			fetchName();
			break;

		case 2:
			fetchAttribute();
			break;

		case 4:
			fetchLowerPattern();
			break;

		case 6:
			fetchUpperPattern();
			break;

		case 7:
			incCoarseX();
			break;
		}
	};
	const auto createSprites = [this] {
		if (scanline_ >= 240 || scanline_ == 0) return;
		const auto reverseByte = [](u8 b) {
			b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
			b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
			b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
			return b;
		};
		sprite_buf_.clear();
		for (std::size_t i = 0, j = 0; i < second_oam_.size(); i += 4, ++j)
		{
			sprite_buf_.emplace_back();
			const u8 y_coord = second_oam_[i] + 1;
			u16 tile_index = second_oam_[i + 1];
			const u8 attribute = second_oam_[i + 2];
			const bool flip_hor = getBitN(attribute, 6);
			const bool flip_vert = getBitN(attribute, 7);
			
			sprite_buf_[j].x = second_oam_[i + 3];
			sprite_buf_[j].palette = attribute & 0x03;
			sprite_buf_[j].priority = getBitN(attribute, 5);
			
			u8 tile_pos = (scanline_ - y_coord) % 17;
			if (flip_vert)
			{
				tile_pos ^= 0x0F;
			}
			tile_pos &= 0x0F;
			if (tile_pos >= 8)
			{
				tile_pos -= 8;
				++tile_index;
			}
			const u16 tile_pat_addr = PPUCTRL.bit.sp_patterntb_addr * 0x1000 + (tile_index << 4) + tile_pos;
			sprite_buf_[j].pat_low  = memRead(tile_pat_addr);
			sprite_buf_[j].pat_high = memRead(tile_pat_addr + 8);
			if (flip_hor)
			{
				sprite_buf_[j].pat_low  = reverseByte(sprite_buf_[j].pat_low);
				sprite_buf_[j].pat_high = reverseByte(sprite_buf_[j].pat_high);
			}
		}
	};
	const auto spriteEval = [this] {
		if (cycle_ % 2 == 0 || scanline_ >= 240 || cycle_ < 65 || cycle_ > 256) return;
		if (cycle_ > 65 && oam_byte.n == 0) // all spirte have been evaluate
		{
			return;
		}
		if (cycle_ == 65) // at the start of the evaluation
		{
			oam_byte.n = oam_byte.m = 0;
			second_oam_.clear();
		}

		u8 sprite_addr = oam_byte.n * 4 + oam_byte.m;
		const u8 y_coord = primary_oam_[sprite_addr];
		const bool sprite_in_range = ((y_coord <= scanline_) && ((scanline_ - y_coord) < (PPUCTRL.bit.sp_size ? 16 : 8)));
		const bool second_oam_full = (second_oam_.size() >= 32);
		if (second_oam_full)
		{
			if (sprite_in_range)
			{
				PPUSTATUS.bit.sp_overflow = 1;
			}
			else
			{
				++oam_byte.m; // hardware bug
			}
		}
		else 
		{
			if (sprite_in_range)
			{
				second_oam_.push_back(primary_oam_[sprite_addr++]);
				second_oam_.push_back(primary_oam_[sprite_addr++]);
				second_oam_.push_back(primary_oam_[sprite_addr++]);
				second_oam_.push_back(primary_oam_[sprite_addr]);
				assert(second_oam_.size() <= 32);
			}
		}
		++oam_byte.n;
	};
	const auto updateSpriteX = [this] {
		if (cycle_ > 256 || scanline_ >= 240) return;
		for (auto& sprite : sprite_buf_)
			if (sprite.x > 0)
				--sprite.x;
	};
	const auto drawSprite = [this](u8& pal, u8& pat, bool& priority, int& idx) {
		priority = true;
		pal = 0; 
		pat = 0;
		if (cycle_ > 256 || scanline_ >= 240 || scanline_ == 0) return;
		for (int i = 0; auto& sprite : sprite_buf_)
		{
			if (sprite.x > 0)
				continue;
			const u8 pat_low  = getBitN(sprite.pat_low, 7);
			const u8 pat_high = getBitN(sprite.pat_high, 7);
			const u8 pattern = (pat_high << 1) | pat_low;
			if (pattern > 0)
			{
				pat = pattern;
				pal = sprite.palette;
				priority = sprite.priority;
				idx = i;
				return;
			}
			++i;
		}
	};
	const auto shiftSpritePattern = [this] {
		if (cycle_ > 256 || scanline_ >= 240) return;
		for (auto& sprite : sprite_buf_)
			if (sprite.x == 0)
			{
				sprite.pat_low  <<= 1;
				sprite.pat_high <<= 1;
			}
	};
	const auto muxColor = [this](const u8 bg_pat, const u8 bg_pal, const u8 sp_pat, const u8 sp_pal, const bool priority) {
		if (cycle_ > 256 || scanline_ >= 240) return;
		bool is_sprite = false;
		u8 pal = 0, pat = 0;
		if (sp_pat == 0 || bg_pat == 0)
		{
			if (sp_pat == 0 && bg_pat == 0)
			{

			}
			else if (sp_pat == 0)
			{
				pat = bg_pat;
				pal = bg_pal;
			}
			else
			{
				is_sprite = true;
				pat = sp_pat;
				pal = sp_pal;
			}
		}
		else if (priority)
		{
			pat = bg_pat;
			pal = bg_pal;
		}
		else
		{
			is_sprite = true;
			pat = sp_pat;
			pal = sp_pal;
		}
		pixels_[scanline_ * 256 + cycle_ - 1].setColor(getColorFromPaletteRam(is_sprite, pal, pat));
	};


	if (cycle_ == 0)
	{
		createSprites();
		cycle_ = 1;
	}

	if (240 <= scanline_ && scanline_ <= 260)
	{
		if (scanline_ == 241 && cycle_ == 1)
		{
			PPUSTATUS.bit.vb_start = 1;
			nmi_occured = PPUCTRL.bit.gen_nmi;
		}
	}
	else
	{
		u8 bg_pat = 0, bg_pal = 0;
		if (PPUMASK.bit.render_bg)
		{
			fetch();
			incY();
			transferTtoV();
			drawBGPixel(bg_pal, bg_pat);
		}
		u8 sp_pat = 0, sp_pal = 0;
		bool sp_priority = false;
		int sp_index = 0;
		if (PPUMASK.bit.render_sp)
		{
			drawSprite(sp_pal, sp_pat, sp_priority, sp_index);
			shiftSpritePattern();
			updateSpriteX();
			spriteEval();
		}

		if (!PPUSTATUS.bit.sp0_hit && 
			PPUMASK.bit.render_sp && PPUMASK.bit.render_bg &&
			sp_index == 0 && sp_pat > 0 && bg_pat > 0)
		{
			PPUSTATUS.bit.sp0_hit = 1;
		}

		muxColor(bg_pat, bg_pal, sp_pat, sp_pal, sp_priority);

		if (scanline_ == 261 && cycle_ == 1)
		{
			nmi_occured = false;
			PPUSTATUS.bit.vb_start = 0;
			PPUSTATUS.bit.sp0_hit = 0;
			PPUSTATUS.bit.sp_overflow = 0;
		}
	}

	++cycle_;
	if (cycle_ > 340)
	{
		cycle_ = 0;
		++scanline_;
		if (scanline_ > 261)
		{
			frame_complete = true;
			scanline_ = 0;
		}
	}
}

void PPU2C02::dummyUpdate()
{
	if (scanline_ == 261 && cycle_ == 1)
	{
		nmi_occured = false;
		PPUSTATUS.bit.vb_start = 0;
		scanline_ = cycle_ = 0;
		frame_complete = true;
	}

	if (scanline_ == 241 && cycle_ == 1)
	{
		PPUSTATUS.bit.vb_start = 1;
		nmi_occured = PPUCTRL.bit.gen_nmi;
	}

	++cycle_;
	if (cycle_ > 340)
	{
		cycle_ = 0;
		++scanline_;
	}
}

void PPU2C02::insertCartridge(std::shared_ptr<Cartridge> cartridge)
{
	cart_ = std::move(cartridge);
}

u8 PPU2C02::regRead(const u16 addr)
{
	assert(addr <= 7);

	u8 data = 0;

	switch (addr)
	{
	case 0x02:
		data = (0xE0 & PPUSTATUS.reg) | (0x1F & data_buf_);
		PPUSTATUS.bit.vb_start = 0;
		write_latch_ = false;
		break;

	case 0x04:
		if (1 <= cycle_ && cycle_ <= 64)
			data = 0xFF;
		else 
			data = primary_oam_[oam_addr_]; 
		break;

	case 0x07:
		data = data_buf_;
		data_buf_ = memRead(vram_addr_.reg);
		if (vram_addr_.reg >= 0x3F00)
			data = data_buf_;
		vram_addr_.reg += (PPUCTRL.bit.vram_addr_inc ? 32 : 1);
		break;
	}

	return data;
}

void PPU2C02::regWrite(const u16 addr, const u8 data)
{
	assert(addr <= 7);

	switch (addr)
	{
	case 0x00:
		PPUCTRL.reg = data;
		tvram_addr_.scroll.nametable_x = PPUCTRL.bit.nametable_x;
		tvram_addr_.scroll.nametable_y = PPUCTRL.bit.nametable_y;
		break;

	case 0x01:
		PPUMASK.reg = data;
		break;

	case 0x03:
		oam_addr_ = data;
		break;

	case 0x04:
	{
		const bool rendering = ((scanline_ < 240 || scanline_ == 261) && 
								(PPUMASK.bit.render_bg || PPUMASK.bit.render_sp));
		if (!rendering)
		{
			primary_oam_[oam_addr_] = data;
			++oam_addr_;
		}
		break;
	}
	case 0x05:
		if (!write_latch_) // first write
		{
			tvram_addr_.scroll.coarse_x = (data >> 3);
			fine_x = data & 0x07;
		}
		else // second write
		{
			tvram_addr_.scroll.coarse_y = (data >> 3);
			tvram_addr_.scroll.fine_y = (data & 0x07);
		}
		write_latch_ = !write_latch_;
		break;

	case 0x06:
		if (!write_latch_) // first write
		{
			tvram_addr_.reg = (static_cast<u16>(data & 0x3F) << 8) | (tvram_addr_.reg & 0x00FF);
		}
		else // second write
		{
			tvram_addr_.reg = (tvram_addr_.reg & 0xFF00) | data;
			vram_addr_ = tvram_addr_;
		}
		write_latch_ = !write_latch_;
		break;

	case 0x07:
		memWrite(vram_addr_.reg, data);
		vram_addr_.reg += (PPUCTRL.bit.vram_addr_inc ? 32 : 1);
		break;
	}
}

u8 PPU2C02::memRead(u16 addr)
{
	addr &= 0x3FFF;

	if (const auto data = cart_->ppuRead(addr); data.has_value())
	{
		return *data;
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

void PPU2C02::OAMDMA(u8* data)
{
	for (std::size_t i = 0; i < 256; ++i, ++oam_addr_, ++data)
		primary_oam_[oam_addr_] = *data;
}

const std::vector<sf::Vertex>& PPU2C02::getVideoOutput()
{
	return pixels_.getVertexArray();
}

u8* PPU2C02::mirroring(u16 addr)
{
	if (0x2000 <= addr && addr <= 0x3EFF)
	{
		addr &= 0x2FFF;
		if (cart_->getMirrorType() == MirrorType::Vertical)
		{
			if (0x2800 <= addr && addr <= 0x2FFF)
			{
				addr -= 0x0800;
			}
		}
		else if (cart_->getMirrorType() == MirrorType::Horizontal)
		{
			if (0x2400 <= addr && addr <= 0x27FF)
			{
				addr -= 0x0400;
			}
			else if (0x2800 <= addr && addr <= 0x2BFF)
			{
				addr -= 0x0400;
			}
			else if (0x2C00 <= addr && addr <= 0x2FFF)
			{
				addr -= 0x0800;
			}
		}
	}
	else if (0x3F00 <= addr && addr <= 0x3FFF)
	{
		return &palette_[addr & 0x00FF];
	}
	return &mem_[addr];
}

sf::Color PPU2C02::getColorFromPaletteRam(const bool sprite, const u16 palette, const u16 pixel)
{
	const u16 sp = sprite;
	const u16 addr = ((sp << 4) | (palette << 2) | pixel);
	return palette_.getColor(palette_[addr] & 0x3F);
}

const std::vector<sf::Vertex>& PPU2C02::dbgGetPatterntb(const int index, const u8 palette)
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

				patterntb[index][(row + i) * 128 + col + (7 - j)].setColor(getColorFromPaletteRam(false, palette, pixel));
			}
		}

		col += 8;
		if (col >= 128)
		{
			col = 0;
			row += 8;
		}
	}


	return patterntb[index].getVertexArray();
}

void PPU2C02::dbgDrawNametb(const u8 which)
{
	u16 addr = 0x2000;
	for (int row = 0; row < 30; ++row)
	{
		for (int col = 0; col < 32; ++col, ++addr)
		{
			std::array<u8, 8> lower{}, upper{};
			const u8 patterntb_index = memRead(addr);
			const u16 patterntb_addr = 0x1000 * which + u16(patterntb_index) * 16;

			for (int i = 0; i < 8; ++i)
			{
				lower[i] = memRead(patterntb_addr + i);
				upper[i] = memRead(patterntb_addr + i + 8);
			}

			const u16 attr_addr = 0x23C0 + (row / 4) * 8 + (col / 4);
			const u8 attr_val = memRead(attr_addr);
			const u8 offset = (((row % 4) & 0x10) | ((col % 4) > 1)) * 2;
			const u8 pal_sel = (attr_val >> offset) & 0x03;

			u16 tl_pixel_idx = row * 8 * 256 + col * 8;
			for (int i = 0; i < 8; ++i)
			{
				for (int j = 0; j < 8; ++j)
				{
					Pixel pixel = pixels_[tl_pixel_idx + i * 256 + (7 - j)];
					const u8 lowerbit = lower[i] & 0x01;
					const u8 upperbit = upper[i] & 0x01;
					const u8 index = (upperbit << 1) | lowerbit;

					upper[i] >>= 1;
					lower[i] >>= 1;

					pixel.setColor(getColorFromPaletteRam(false, pal_sel, index));
				}
			}
		}
	}
}

const std::vector<sf::Vertex>& PPU2C02::dbgGetFramePalette(const u8 index)
{
	static std::vector<sf::Vertex> frame_palette[8];
	static bool init = false;
	static float size = 7.5f;

	if (!init)
	{
		sf::Vector2f pos{ 50.0f, 250.0f };
		for (std::size_t i = 0; i < 8; ++i)
		{
			frame_palette[i].resize(16);
			for (std::size_t j = 0; j < 4; ++j)
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
	for (std::size_t j = 0; j < 4; ++j)
	{
		sf::Color color = getColorFromPaletteRam(false, index, j);
		frame_palette[index][j * 4].color = color;
		frame_palette[index][j * 4 + 1].color = color;
		frame_palette[index][j * 4 + 2].color = color;
		frame_palette[index][j * 4 + 3].color = color;
	}

	return frame_palette[index];
}
