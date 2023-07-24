#pragma once

#include "common/type.hpp"
#include "Tile.hpp"
#include "Palette.hpp"
#include "Cartridge.hpp"
#include <SFML/Graphics/Vertex.hpp>
#include <array>
#include <vector>


class PPU2C02
{
public:
	PPU2C02();

	void reset();

	void update();

	void insertCartridge(std::shared_ptr<Cartridge> cartridge);

	u8 regRead(u16 addr);
	void regWrite(u16 addr, u8 data);
	u8 memRead(u16 addr);
	void memWrite(u16 addr, u8 data);

	const std::vector<sf::Vertex>& getVideoOutput() const;

	bool isFrameComplete() const;

	bool nmi_occured = false;

public: // for debug
	PixelArray& dbgGetPatterntb(int i, u8 palette);
	void dbgDrawNametb();
	std::vector<sf::Vertex>& dbgGetFramePalette(u8 index);
	sf::Color dbgGetColor(u16 palette, u16 pixel);
	u8 dbg_pal = 0;

private:
	u8* mirroring(u16 addr);

	static constexpr std::size_t mem_size = 16 * 1024; // 16kB
	static constexpr std::size_t resolution = 256 * 240;

	union
	{
		struct Foo
		{
			u8 nametb_addr : 2;
			u8 vram_addr_inc : 1;
			u8 sp_patterntb_addr : 1;
			u8 bg_patterntb_addr : 1;
			u8 sp_size : 1;
			u8 master_slave_select : 1;
			u8 gen_nmi : 1;
		} bit;
		u8 reg;
	} PPUCTRL;
	static_assert(sizeof(PPUCTRL) == 1);

	union
	{
		struct Foo
		{
			u8 grey_sacle : 1;
			u8 show_bg_lm_8pixels : 1;
			u8 show_sp_lm_8pixels : 1;
			u8 show_bg : 1;
			u8 show_sp : 1;
			u8 empha_red : 1;
			u8 empha_green : 1;
			u8 empha_blue : 1;
		} bit;
		u8 reg;
	} PPUMASK;
	static_assert(sizeof(PPUMASK) == 1);

	union
	{
		struct Foo
		{
			u8 open_bus : 5;
			u8 sp_overflow : 1;
			u8 sp0_hit : 1;
			u8 vb_start : 1;
		} bit;
		u8 reg;
	} PPUSTATUS;
	static_assert(sizeof(PPUSTATUS) == 1);

	u8 OAMADDR = 0;
	u8 OAMDATA = 0;
	u8 PPUSCROLL = 0;
	u8 PPUADDR = 0;
	u8 PPUDATA = 0;

	u8 scroll_latch = 0, addr_latch = 0;

	u8 fine_x_scroll_ = 0;

	PixelArray pixels_;
	Palette palette_;
	std::vector<u8> mem_;

	std::shared_ptr<Cartridge> cart_;

	bool frame_complete_ = false;
	bool odd_frame_ = false;

	u8 data_buf_ = 0;
	u16 tmp_vram_addr_ = 0;
	u16 vram_addr_ = 0;
	u16 pattb_shift_reg_[2] = {};
	u8 palattr_shift_reg_[2] = {};

	int cycle_ = 0;
	u64 total_cycle_ = 0;
};