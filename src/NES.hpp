#pragma once

#include <SFML/Graphics.hpp>
#include "Bus.hpp"

class NES
{
public:
	NES();
	~NES();

	void insertCartridge(std::shared_ptr<Cartridge> cartridge);
	void run();

private:
	bool onUpdate(float elapsed_time);
	void onDraw();
	void onEvent();
	void onKeyPressed();

	Bus bus_{};
	u64 cycle_count_ = 0;

	sf::RenderWindow* window_ = nullptr;
	sf::Clock system_clock_{};
	sf::Event event_{};

public: // for debug
	PixelArray& dbg_draw_pattern_tb(int i, u8 palette);
	u8 palette_ = 0;
};