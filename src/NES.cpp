#include "NES.hpp"

NES::NES()
{
	window_ = new sf::RenderWindow(sf::VideoMode(256 + 150, 240), "NES", sf::Style::Titlebar | sf::Style::Close);
	window_->setSize(sf::Vector2u(1024 + 600, 960));
	window_->setVerticalSyncEnabled(true);
}

NES::~NES()
{
	delete window_;
}

void NES::run()
{
    system_clock_.restart();
    while (window_->isOpen())
    {
        while (window_->pollEvent(event_))
        {
            onEvent();
        }

        onUpdate(system_clock_.getElapsedTime().asSeconds());

        window_->clear();
        onDraw();
        window_->display();
    }
}

void NES::onUpdate(const double elapsed_time)
{
    static double residual_time = 0.0;

    residual_time -= elapsed_time;
    if (residual_time > 0)
        return;
    
    residual_time += (1.0 / 60.0) - elapsed_time;
    /*
    bus_.ppu.update();
    if (cycle_count_ % 3 == 0)
    {
        bus_.cpu.update();
    }*/
    ++cycle_count_;
    system_clock_.restart();
}

void NES::onDraw()
{
    auto& video_output = bus_.ppu.getOutput();
    window_->draw(video_output.data(), video_output.size(), sf::Quads);
}

void NES::onEvent()
{
    switch (event_.type)
    {
    case sf::Event::Closed:
        window_->close();
        break;

    case sf::Event::KeyPressed:
        onKeyPressed();
        break;
    }
}

void NES::onKeyPressed()
{
    if (event_.key.code == sf::Keyboard::Escape)
        window_->close();
}
