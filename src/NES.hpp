#pragma once

#include <SFML/Graphics.hpp>
#include "Bus.hpp"

#define DEBUG_WINDOW

class NES
{
public:
	NES();
	~NES();

	void run();

	Bus& bus;

private:
	bool onUpdate(float elapsed_time);
	void onDraw();
	void onEvent();
	void onKeyPressed();
	void onKeyReleased();

	sf::RenderWindow* window_ = nullptr;
	sf::Event event_{};

	bool pause_ = false;
};