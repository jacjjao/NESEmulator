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

	void reset();

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

	bool sim_timing_ = false;
	bool pause_ = false;
};