#include "Cartridge.hpp"
#include "common/bitHelper.hpp"

#include <iostream>
#include <fstream>


bool Cartridge::loadiNESFile(const std::filesystem::path& path)
{
	std::ifstream file{path, std::ios::binary};
	
    if (!file) 
    {
        std::cerr << "[FAILED] cannot open the file: " << path << '\n';
        return false;
    }

    std::vector<u8> data(std::istreambuf_iterator<char>(file), {});
    if (data.size() >= 16)
    {
        std::cerr << "[FAILED] file is invalid, the file only contains " << data.size() << " bytes\n";
        return false;
    }

    header_.cN   = data[0];
    header_.cE   = data[1];
    header_.cS   = data[2];
    header_.cEOF = data[3];

    header_.prg_rom_size = data[4];
    header_.chr_rom_size = data[5];

    header_.mapper1 = data[6];
    header_.mapper2 = data[7];

    header_.prg_ram_size = data[8];

    header_.tv_system     = data[9];
    header_.tv_system_prg = data[10];

    header_.u11 = data[11];
    header_.u12 = data[12];
    header_.u13 = data[13];
    header_.u14 = data[14];
    header_.u15 = data[15];


    // verify the file
    if (header_.cN != 'N' ||
        header_.cE != 'E' ||
        header_.cS != 'S' ||
        static_cast<int>(header_.cEOF) != 0x1A)
    {
        std::cerr << "[FAILED] header is invalid\n";
        return false;
    }

    if (getBitN(header_.mapper1, 3))
    {
        std::cerr << "[FAILED] trainer contains\n";
        return false;
    }

    auto it = data.cbegin() + 16;

    // load PRG ROM
    std::size_t prg_rom_size = static_cast<std::size_t>(header_.prg_rom_size) * 16384;
    prg_rom_.assign(it, it + prg_rom_size);
    it += prg_rom_size;

    // load CHR ROM
    std::size_t chr_rom_size = static_cast<std::size_t>(header_.chr_rom_size) * 8192;
    chr_rom_.assign(it, it + chr_rom_size);

    return true;
}
