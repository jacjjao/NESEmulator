#include "NES.hpp"

NES::NES()
{
	window_ = new sf::RenderWindow(sf::VideoMode(256 + 150, 240 + 50), "NES", sf::Style::Titlebar | sf::Style::Close);
	window_->setSize(sf::Vector2u(1024 + 600, 960 + 200));
    window_->setPosition({ 100, 100 });
}

NES::~NES()
{
	delete window_;
}

void NES::insertCartridge(std::shared_ptr<Cartridge> cartridge)
{
    bus_.insertCartridge(std::move(cartridge));
    reset();
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

        onUpdate(0.0f);

        onDraw();
    }
}

void NES::reset()
{
    bus_.reset();
    system_clock_.restart();
    cycle_count_ = 0;
}

bool NES::onUpdate(const float elapsed_time)
{
    if (pause_) return true;

    bus_.ppu.update();
    if (cycle_count_ % 3 == 0)
    {
        bus_.cpu.update();
    }

    if (bus_.ppu.nmi_occured)
    {
        bus_.ppu.nmi_occured = false;
        bus_.cpu.nmi();
    }

    ++cycle_count_;

    return true;
}

void NES::onDraw()
{
    static sf::Clock clock;
    static constexpr float frame_time_interval = 1.0f / 60.0f;

    if (clock.getElapsedTime().asSeconds() < frame_time_interval)
        return;
    clock.restart();

    window_->clear(sf::Color{0, 0, 50});

    bus_.ppu.dbgDrawNametb(1);

    auto& video_output = bus_.ppu.getVideoOutput();
    window_->draw(video_output.data(), video_output.size(), sf::Quads);

    auto& patterntb1 = bus_.ppu.dbgGetPatterntb(0, bus_.ppu.dbg_pal);
    auto& patterntb2 = bus_.ppu.dbgGetPatterntb(1, bus_.ppu.dbg_pal);
    window_->draw(patterntb1.data(), patterntb1.size(), sf::Quads);
    window_->draw(patterntb2.data(), patterntb2.size(), sf::Quads);
    
    for (int i = 0; i < 8; ++i)
    {
        auto& palette = bus_.ppu.dbgGetFramePalette(i);
        window_->draw(palette.data(), palette.size(), sf::Quads);
    }
    
    window_->display();
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
    else if (event_.key.code == sf::Keyboard::Right)
        ++bus_.ppu.dbg_pal &= 0x07;
    else if (event_.key.code == sf::Keyboard::Left)
        --bus_.ppu.dbg_pal &= 0x07;
    else if (event_.key.code == sf::Keyboard::Space)
        pause_ = !pause_;
}
