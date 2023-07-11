#include "Cartridge.hpp"
#include <iostream>
#include <fstream>
#include <cassert>


void Cartridge::readiNESFile(const std::filesystem::path& path)
{
	std::ifstream file{path, std::ios::binary};
	
    if (!file) 
    {
        std::cerr << "Failed to open the file: " << path << '\n';
        return;
    }

    data_.assign(std::istreambuf_iterator<char>(file), {});
    assert(data_.size() >= 16);

    
    header_.cN   = data_[0];
    header_.cE   = data_[1];
    header_.cS   = data_[2];
    header_.cEOF = data_[3];

    header_.prg_rom_size = data_[4];
    header_.chr_rom_size = data_[5];

    header_.mapper1 = data_[6];
    header_.mapper2 = data_[7];

    header_.prg_ram_size = data_[8];

    header_.tv_system     = data_[9];
    header_.tv_system_prg = data_[10];

    header_.u11 = data_[11];
    header_.u12 = data_[12];
    header_.u13 = data_[13];
    header_.u14 = data_[14];
    header_.u15 = data_[15];


    // verify the file
    assert(
        header_.cN == 'N' &&
        header_.cE == 'E' &&
        header_.cS == 'S' &&
        static_cast<int>(header_.cEOF) == 0x1A);


}
