#pragma once

#include <SFML/Graphics.hpp>
#include "Bus.hpp"

class NES
{
public:
	NES();
	~NES();

	void run();

private:
	void onUpdate(double elapsed_time);
	void onDraw();
	void onEvent();
	void onKeyPressed();

	Bus bus_;
	u64 cycle_count_ = 0;

	sf::RenderWindow* window_;
	sf::Clock system_clock_;
	sf::Event event_;
};