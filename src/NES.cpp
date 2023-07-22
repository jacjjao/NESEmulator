#include "NES.hpp"

NES::NES()
{
	window_ = new sf::RenderWindow(sf::VideoMode(256 + 150, 240), "NES", sf::Style::Titlebar | sf::Style::Close);
	window_->setSize(sf::Vector2u(1024 + 600, 960));
}

NES::~NES()
{
	delete window_;
}

void NES::insertCartridge(std::shared_ptr<Cartridge> cartridge)
{
    bus_.insertCartridge(std::move(cartridge));
    bus_.cpu.reset();
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

        while (!onUpdate(system_clock_.getElapsedTime().asSeconds()))
        { }

        window_->clear();
        onDraw();
        window_->display();
    }
}

bool NES::onUpdate(const float elapsed_time)
{
    static constexpr float frame_time_interval = 1.0f / 60.0f;

    if (elapsed_time < frame_time_interval)
        return false;
    
    bus_.ppu.update();
    if (cycle_count_ % 3 == 0)
    {
        bus_.cpu.update();
    }
    ++cycle_count_;
    system_clock_.restart();

    return true;
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

void NES::dbg_draw_pattern_tb()
{
    
}
