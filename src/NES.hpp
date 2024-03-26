#pragma once

#include "pch.hpp"
#include "Bus.hpp"

#ifdef EMU_DEBUG
#define DEBUG_WINDOW
#endif

class NES : public sf::SoundStream
{
public:
	NES();
	~NES();

	void run();

	Bus& bus;

private:
	static constexpr int s_sample_rate = 44100;
	static constexpr int s_frame_rate = 60;
	static constexpr unsigned s_nes_width = 256;
	static constexpr unsigned s_nes_height = 240;
	static constexpr unsigned s_dbg_nes_width = 256 + 150;
	static constexpr unsigned s_dbg_nes_height = 256 + 50;

	bool onUpdate();
	void onDraw();
	void onEvent();
	void onKeyPressed();
	void onKeyReleased();
	bool onGetData(Chunk& data) override;
	void onSeek(sf::Time) override;
	void scaleWindow();

	sf::RenderWindow* window_ = nullptr;
	sf::Event event_{};

	sf::Sound sound_;

	unsigned win_scale_ = 3;

	bool pause_ = false;
};