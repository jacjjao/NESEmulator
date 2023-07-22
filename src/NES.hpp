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

	void dbg_draw_pattern_tb();

	Bus bus_;
	u64 cycle_count_ = 0;

	sf::RenderWindow* window_;
	sf::Clock system_clock_;
	sf::Event event_;
};