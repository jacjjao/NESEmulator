#pragma once

#include "pch.hpp"
#include "Bus.hpp"

#ifdef EMU_DEBUG
#define DEBUG_WINDOW
#endif

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

	sf::Sound sound_;

	bool pause_ = false;
};