#pragma once

#include "common/type.hpp"
#include "Tile.hpp"
#include "Palette.hpp"
#include "Cartridge.hpp"
#include <SFML/Graphics/Vertex.hpp>
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

	void OAMDMA(u8* data);

	bool frame_complete = false;

	const std::vector<sf::Vertex>& getVideoOutput();

	bool nmi_occured = false;

public: // for debug
	void dummyUpdate();
	const std::vector<sf::Vertex>& dbgGetPatterntb(int i, u8 palette);
	void dbgDrawNametb(u8 which);
	const std::vector<sf::Vertex>& dbgGetFramePalette(u8 index);
	u8 dbg_pal = 0;

private:
	sf::Color getColorFromPaletteRam(bool sprite, u16 palette, u16 pixel);
	u8* mirroring(u16 addr);

	static constexpr std::size_t mem_size = 16 * 1024; // 16kB
	static constexpr std::size_t primary_oam_size = 256;
	static constexpr std::size_t second_oam_size = 8 * 4;
	static constexpr std::size_t resolution = 256 * 240;

	union
	{
		struct Foo
		{
			u8 nametable_x : 1;
			u8 nametable_y : 1;
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
			u8 render_bg_lm_8pixels : 1;
			u8 render_sp_lm_8pixels : 1;
			u8 render_bg : 1;
			u8 render_sp : 1;
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

	union
	{
		struct Foo
		{
			u16 coarse_x : 5;
			u16 coarse_y : 5;
			u16 nametable_x : 1;
			u16 nametable_y : 1;
			u16 fine_y : 3;
		} scroll;
		u16 reg;
	} vram_addr_, tvram_addr_;
	static_assert(sizeof(vram_addr_) == 2);

	u8 OAMADDR = 0;
	u8 OAMDATA = 0;
	u8 PPUSCROLL = 0;
	u8 PPUADDR = 0;
	u8 PPUDATA = 0;

	bool write_latch_ = false;

	u8 fine_x = 0;

	PixelArray pixels_;
	Palette palette_;
	std::vector<u8> mem_, primary_oam_, second_oam_;
	u8 oam_addr_ = 0;

	struct Sprite
	{
		u8 palette;
		u8 x;
		u8 pat_high, pat_low;
		bool priority;
		bool is_sprite0;
	};

	std::vector<Sprite> sprite_buf_;
	bool sprite0_hit_protential_next_ = false;
	bool sprite0_hit_protential_cur_ = false;

	struct OAMByte
	{
		u8 n : 6;
		u8 m : 2;
	} oam_byte;

	std::shared_ptr<Cartridge> cart_;

	u8 data_buf_ = 0;

	struct BackGroundLatches
	{
		u8 pat_high = 0;
		u8 pat_low = 0;
		u8 attr_low = 0;
		u8 attr_high = 0;
		u8 tile_name = 0;
	} bg_latches_;

	struct BackGroundShiftRegister
	{
		u16 pat_high = 0;
		u16 pat_low = 0;
		u16 attr_low = 0;
		u16 attr_high = 0;
	} shift_reg_;

	unsigned scanline_ = 0;
	int cycle_ = 0;
	u64 total_cycle_ = 0;
};