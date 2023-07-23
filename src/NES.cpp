#include "NES.hpp"

NES::NES()
{
	window_ = new sf::RenderWindow(sf::VideoMode(256 + 150, 240 + 50), "NES", sf::Style::Titlebar | sf::Style::Close);
	window_->setSize(sf::Vector2u(1024 + 600, 960 + 200));
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
    auto& video_output = bus_.ppu.getVideoOutput();
    window_->draw(video_output.data(), video_output.size(), sf::Quads);

    auto& patterntb1 = dbg_draw_pattern_tb(0, palette_);
    auto& patterntb2 = dbg_draw_pattern_tb(1, palette_);
    window_->draw(patterntb1.getVertexArray().data(), patterntb1.getVertexArray().size(), sf::Quads);
    window_->draw(patterntb2.getVertexArray().data(), patterntb2.getVertexArray().size(), sf::Quads);
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
        ++palette_ &= 0x07;
}

PixelArray& NES::dbg_draw_pattern_tb(const int index, const u8 palette)
{
    static PixelArray patterntb[2]{ (128 * 128), (128 * 128) };
    static bool is_init = false;

    if (!is_init)
    {
        std::size_t i = 0;
        for (int row = 0; row < 128; ++row)
            for (int col = 0; col < 128; ++col, ++i)
            {
                patterntb[0][i].setPosition({float(256 + 11 + col), float(11 + row)});
                patterntb[1][i].setPosition({float(256 + 11 + col), float(128 + 22 + row)});
            }

        is_init = true;
    }

    int row = 0, col = 0;
    for (u16 addr = (index ? 0x1000 : 0x0000); addr < (index ? 0x2000 : 0x1000); addr += 16)
    {
        std::array<u8, 8> first{}, second{};
        for (int i = 0; i < 8; ++i)
        {
            first[i] = bus_.ppuRead(addr + i);
            second[i] = bus_.ppuRead(addr + i + 8);
        }
        
        for (int i = 0; i < 8; ++i)
        {
            for (int j = 0; j < 8; ++j)
            {
                u8 lowerbit = first[i] & 0x01;
                u8 upperbit = second[i] & 0x01;

                first[i] >>= 1;
                second[i] >>= 1;

                u8 pixel = (upperbit << 1) | lowerbit;

                sf::Color color = bus_.ppu.getPalette(false, pixel, palette);
                patterntb[index][(row + i) * 128 + col + (7 - j)].setColor(color);
            }
        }

        col += 8;
        if (col >= 128)
        {
            col = 0;
            row += 8;
        }
    }


    return patterntb[index];
}
