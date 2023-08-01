#pragma once

#include <SFML/Graphics.hpp>
#include "Bus.hpp"

// #define DEBUG_WINDOW

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
	void onKeyReleased();

	Bus bus_{};

	sf::RenderWindow* window_ = nullptr;
	sf::Event event_{};

	bool sim_timing_ = false;
	bool pause_ = false;
};