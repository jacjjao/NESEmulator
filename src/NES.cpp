#include "NES.hpp"

NES::NES()
{
#ifdef DEBUG_WINDOW
	window_ = new sf::RenderWindow(sf::VideoMode(256 + 150, 240 + 50), "NES", sf::Style::Titlebar | sf::Style::Close);
	window_->setSize(sf::Vector2u(1024 + 600, 960 + 200));
#else 
    window_ = new sf::RenderWindow(sf::VideoMode(256, 240), "NES", sf::Style::Titlebar | sf::Style::Close);
    window_->setSize(sf::Vector2u(256 * 5, 240 * 5));
#endif
    window_->setPosition({ 100, 100 });
}

NES::~NES()
{
	delete window_;
}

void NES::insertCartridge(std::shared_ptr<Mapper> cartridge)
{
    bus_.insertCartridge(std::move(cartridge));
    reset();
}

void NES::run()
{
    static sf::Clock clock;
    static constexpr float frame_time_interval = 1.0f / 50.0f;

    while (window_->isOpen())
    {
        while (window_->pollEvent(event_))
        {
            onEvent();
        }

        if (clock.getElapsedTime().asSeconds() >= frame_time_interval)
        {
            clock.restart();
            onUpdate(0.0f);
            onDraw();
        }
    }
}

void NES::reset()
{
    bus_.reset();
}

bool NES::onUpdate(float)
{
    if (pause_) return true;
    
    do
    {
        bus_.clock();
    } while (!bus_.ppu.frame_complete);
    bus_.ppu.frame_complete = false;
    
    return true;
}

void NES::onDraw()
{
    window_->clear(sf::Color{ 0, 0, 50 });
    
    // bus_.ppu.dbgDrawNametb(0);

    auto& video_output = bus_.ppu.getVideoOutput();
    window_->draw(video_output.data(), video_output.size(), sf::Quads);
#ifdef DEBUG_WINDOW
    auto& patterntb1 = bus_.ppu.dbgGetPatterntb(0, bus_.ppu.dbg_pal);
    auto& patterntb2 = bus_.ppu.dbgGetPatterntb(1, bus_.ppu.dbg_pal);
    window_->draw(patterntb1.data(), patterntb1.size(), sf::Quads);
    window_->draw(patterntb2.data(), patterntb2.size(), sf::Quads);
    
    for (int i = 0; i < 8; ++i)
    {
        auto& palette = bus_.ppu.dbgGetFramePalette(i);
        window_->draw(palette.data(), palette.size(), sf::Quads);
    }
#endif
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
#ifdef DEBUG_WINDOW
    else if (event_.key.code == sf::Keyboard::Right)
        ++bus_.ppu.dbg_pal &= 0x07;
    else if (event_.key.code == sf::Keyboard::Left)
        --bus_.ppu.dbg_pal &= 0x07;
    else if (event_.key.code == sf::Keyboard::Space)
        pause_ = !pause_;
#endif
    else if (event_.key.code == sf::Keyboard::W)
        bus_.joystick.setBotton(Botton::Up, true);
    else if (event_.key.code == sf::Keyboard::S)
        bus_.joystick.setBotton(Botton::Down, true);
    else if (event_.key.code == sf::Keyboard::A)
        bus_.joystick.setBotton(Botton::Left, true);
    else if (event_.key.code == sf::Keyboard::D)
        bus_.joystick.setBotton(Botton::Right, true);
    else if (event_.key.code == sf::Keyboard::G)
        bus_.joystick.setBotton(Botton::Select, true);
    else if (event_.key.code == sf::Keyboard::H)
        bus_.joystick.setBotton(Botton::Start, true);
    else if (event_.key.code == sf::Keyboard::J)
        bus_.joystick.setBotton(Botton::B, true);
    else if (event_.key.code == sf::Keyboard::K)
        bus_.joystick.setBotton(Botton::A, true);
}

void NES::onKeyReleased()
{
     if (event_.key.code == sf::Keyboard::W)
         bus_.joystick.setBotton(Botton::Up, false);
     else if (event_.key.code == sf::Keyboard::S)
         bus_.joystick.setBotton(Botton::Down, false);
     else if (event_.key.code == sf::Keyboard::A)
         bus_.joystick.setBotton(Botton::Left, false);
     else if (event_.key.code == sf::Keyboard::D)
         bus_.joystick.setBotton(Botton::Right, false);
     else if (event_.key.code == sf::Keyboard::G)
         bus_.joystick.setBotton(Botton::Select, false);
     else if (event_.key.code == sf::Keyboard::H)
         bus_.joystick.setBotton(Botton::Start, false);
     else if (event_.key.code == sf::Keyboard::J)
         bus_.joystick.setBotton(Botton::B, false);
     else if (event_.key.code == sf::Keyboard::K)
         bus_.joystick.setBotton(Botton::A, false);
}
