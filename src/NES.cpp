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
}

bool NES::onUpdate(float)
{
    if (pause_) return true;

    bus_.joystick.setBotton(Botton::Up,     (event_.key.code == sf::Keyboard::W));
    bus_.joystick.setBotton(Botton::Down,   (event_.key.code == sf::Keyboard::S));
    bus_.joystick.setBotton(Botton::Left,   (event_.key.code == sf::Keyboard::A));
    bus_.joystick.setBotton(Botton::Right,  (event_.key.code == sf::Keyboard::D));
    bus_.joystick.setBotton(Botton::A,      (event_.key.code == sf::Keyboard::K));
    bus_.joystick.setBotton(Botton::B,      (event_.key.code == sf::Keyboard::J));
    bus_.joystick.setBotton(Botton::Start,  (event_.key.code == sf::Keyboard::H));
    bus_.joystick.setBotton(Botton::Select, (event_.key.code == sf::Keyboard::G));
    
    return true;
}

void NES::onDraw()
{
    static sf::Clock clock;
    static constexpr float frame_time_interval = 1.0f / 50.0f;
    static sf::Color background_color = sf::Color{ 0, 0, 50 };

    if (clock.getElapsedTime().asSeconds() < frame_time_interval)
        return;
    clock.restart();

    if (!pause_)
    {
        do
        {
            bus_.clock();
        } while (!bus_.ppu.frame_complete);
        bus_.ppu.frame_complete = false;
    }

    window_->clear(background_color);

    // bus_.ppu.dbgDrawNametb(1);
    
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

    case sf::Event::KeyReleased:
        onKeyReleased();
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

void NES::onKeyReleased()
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
