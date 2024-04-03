#include "NES.hpp"

NES::NES() : 
    bus{ Bus::instance() }
{
#ifdef DEBUG_WINDOW
	window_ = new sf::RenderWindow(sf::VideoMode(s_dbg_nes_width, s_dbg_nes_height), "NES", sf::Style::Titlebar | sf::Style::Close);
#else 
    window_ = new sf::RenderWindow(sf::VideoMode(s_nes_width, s_nes_height), "NES", sf::Style::Titlebar | sf::Style::Close);
#endif
    scaleWindow();

    constexpr unsigned channel_count = 1;
    initialize(channel_count, s_sample_rate);
    setProcessingInterval(sf::Time::Zero);
}

NES::~NES()
{
    stop();
	delete window_;
}

void NES::run()
{
    constexpr float update_interval = 1.0f / static_cast<float>(s_frame_rate);
    sf::Clock clock;

    onUpdate();

    play();
    while (window_->isOpen())
    {
        while (window_->pollEvent(event_))
        {
            onEvent();
        }

        onDraw();
        onUpdate();

        auto dt = clock.restart();
        while (dt.asSeconds() < update_interval)
            dt += clock.restart();
    }
}

bool NES::onUpdate()
{
    if (pause_) return true;

    do
    {
        bus.clock();
    } while (!bus.ppu.frame_complete);
    bus.ppu.frame_complete = false;
    
    return true;
}

void NES::onDraw()
{
    window_->clear(sf::Color{ 0, 0, 50 });
    
    // bus_.ppu.dbgDrawNametb(0);

    auto& video_output = bus.ppu.getVideoOutput();
    window_->draw(video_output.data(), video_output.size(), sf::Quads);
#ifdef DEBUG_WINDOW
    auto& patterntb1 = bus.ppu.dbgGetPatterntb(0, bus.ppu.dbg_pal);
    auto& patterntb2 = bus.ppu.dbgGetPatterntb(1, bus.ppu.dbg_pal);
    window_->draw(patterntb1.data(), patterntb1.size(), sf::Quads);
    window_->draw(patterntb2.data(), patterntb2.size(), sf::Quads);
    
    for (u8 i = 0; i < 8; ++i)
    {
        auto& palette = bus.ppu.dbgGetFramePalette(i);
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
    else if (event_.key.code == sf::Keyboard::Delete)
        bus.reset();
#ifdef DEBUG_WINDOW
    else if (event_.key.code == sf::Keyboard::Right)
        ++bus.ppu.dbg_pal &= 0x07;
    else if (event_.key.code == sf::Keyboard::Left)
        --bus.ppu.dbg_pal &= 0x07;
    else if (event_.key.code == sf::Keyboard::Space)
        pause_ = !pause_;
#endif
    else if (event_.key.code == sf::Keyboard::W)
        bus.joystick.setBotton(Botton::Up, true);
    else if (event_.key.code == sf::Keyboard::S)
        bus.joystick.setBotton(Botton::Down, true);
    else if (event_.key.code == sf::Keyboard::A)
        bus.joystick.setBotton(Botton::Left, true);
    else if (event_.key.code == sf::Keyboard::D)
        bus.joystick.setBotton(Botton::Right, true);
    else if (event_.key.code == sf::Keyboard::G)
        bus.joystick.setBotton(Botton::Select, true);
    else if (event_.key.code == sf::Keyboard::H)
        bus.joystick.setBotton(Botton::Start, true);
    else if (event_.key.code == sf::Keyboard::J)
        bus.joystick.setBotton(Botton::B, true);
    else if (event_.key.code == sf::Keyboard::K)
        bus.joystick.setBotton(Botton::A, true);
    else if (event_.key.code == sf::Keyboard::PageUp)
    {
        win_scale_ = std::min(win_scale_ + 1u, 10u);
        scaleWindow();
    }
    else if (event_.key.code == sf::Keyboard::PageDown)
    {
        win_scale_ = std::max(win_scale_ - 1u, 1u);
        scaleWindow();
    }
}

void NES::onKeyReleased()
{
     if (event_.key.code == sf::Keyboard::W)
         bus.joystick.setBotton(Botton::Up, false);
     else if (event_.key.code == sf::Keyboard::S)
         bus.joystick.setBotton(Botton::Down, false);
     else if (event_.key.code == sf::Keyboard::A)
         bus.joystick.setBotton(Botton::Left, false);
     else if (event_.key.code == sf::Keyboard::D)
         bus.joystick.setBotton(Botton::Right, false);
     else if (event_.key.code == sf::Keyboard::G)
         bus.joystick.setBotton(Botton::Select, false);
     else if (event_.key.code == sf::Keyboard::H)
         bus.joystick.setBotton(Botton::Start, false);
     else if (event_.key.code == sf::Keyboard::J)
         bus.joystick.setBotton(Botton::B, false);
     else if (event_.key.code == sf::Keyboard::K)
         bus.joystick.setBotton(Botton::A, false);
}

bool NES::onGetData(Chunk& data)
{
    static std::vector<i16> samples(1000);
    bus.apu.getSamples(samples);
    if (samples.empty())
    {
        constexpr int n = s_sample_rate / s_frame_rate;
        std::fill_n(std::back_inserter(samples), n, 0);
    }
    data.samples = samples.data();
    data.sampleCount = samples.size();
    return true;
}

void NES::onSeek(sf::Time)
{
    /* empty */
}

void NES::scaleWindow()
{
#ifdef DEBUG_WINDOW
    auto size = sf::Vector2u(s_dbg_nes_width, s_dbg_nes_height);
#else 
    auto size = sf::Vector2u(s_nes_width, s_nes_height);
#endif
    size *= win_scale_;
    window_->setSize(size);
    const auto desktop = sf::VideoMode::getDesktopMode();
    sf::Vector2i pos(desktop.width / 2 - size.x / 2, desktop.height / 2 - size.y / 2);
    window_->setPosition(pos);
}
