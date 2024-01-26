#include "pch.hpp"
#include "NES.hpp"
#include "Mapper/AllMapper.hpp"


std::unique_ptr<Mapper> createCartridge(const std::filesystem::path& path)
{
    Cartridge cart;
    u8 mapper_type = cart.loadiNESFile(path);
    std::unique_ptr<Mapper> mapper;
    switch (mapper_type)
    {
    case 000:
        mapper.reset(new Mapper000{ std::move(cart) });
        break;

    case 001:
        mapper.reset(new Mapper001{ std::move(cart) });
        break;

    case 002:
        mapper.reset(new Mapper002{ std::move(cart) });
        break;

    case 003:
        mapper.reset(new Mapper003{ std::move(cart) });
        break;

    case 004:
        mapper.reset(new Mapper004{ std::move(cart) });
        break;

    case 066:
        mapper.reset(new Mapper066{ std::move(cart) });
        break;

    default:
    {
        char err_msg[50] = "";
        std::snprintf(err_msg, 50, "Unsupport mapper %03d", (int)mapper_type);
        throw std::runtime_error{ err_msg };
    }
    }
    std::printf("Mapper %03d\n", (int)mapper_type);
    return mapper;
}

void trimPath(std::string& path)
{
    int start = 0;
    while (start < path.size() && !std::isalpha(path[start]))
        start++;
    int end = path.size() - 1;
    while (end >= start && !std::isalpha(path[end]))
        end--;
    if (start <= end)
        path = path.substr(start, end - start + 1);
}

int main()
{      
    std::string cmd;
    while (true)
    {
        std::cout << "Input rom directory or type \"exit\" to exit the program: ";
        std::getline(std::cin, cmd);

        if (cmd == "exit")
            break;

        trimPath(cmd);

        try
        {
            NES nes;
            nes.bus.insertCartridge(createCartridge(cmd));
            nes.run();
        }
        catch (std::exception& e)
        {
            std::fprintf(stderr, "%s\n", e.what());
        }
    }
    return 0;
}