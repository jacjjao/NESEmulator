#include "Cartridge.hpp"
#include "common/bitHelper.hpp"

#include <stdexcept>
#include <iostream>
#include <fstream>


u8 Cartridge::loadiNESFile(const std::filesystem::path& path)
{
	std::ifstream file{path, std::ios::binary};
    file.exceptions(std::ios::failbit | std::ios::badbit);
	
    if (!file.is_open()) 
    {
        throw std::runtime_error{ "Cannot open the file" };
    }

    std::vector<u8> data(std::istreambuf_iterator<char>(file), {});
    if (data.size() < 16)
    {
        throw std::runtime_error{ "File is too small(less than 16 bytes)" };
    }

    struct
    {
        u8 cN, cE, cS, cEOF;
        u8 prg_rom_size;
        u8 chr_rom_size;
        u8 flag6;
        u8 flag7;
        u8 prg_ram_size;
        u8 flag9;
        u8 flag10;
    } header{};

    header.cN   = data[0];
    header.cE   = data[1];
    header.cS   = data[2];
    header.cEOF = data[3];

    header.prg_rom_size = data[4];
    header.chr_rom_size = data[5];

    header.flag6 = data[6];
    header.flag7 = data[7];

    header.prg_ram_size = data[ 8];

    header.flag9  = data[ 9];
    header.flag10 = data[10];


    // verify the file
    if (header.cN   != 'N' ||
        header.cE   != 'E' ||
        header.cS   != 'S' ||
        header.cEOF != 0x1A)
    {
        throw std::runtime_error{ "File does not follow .nes file format" };
    }

    if (getBitN(header.flag6, 3))
    {
        throw std::runtime_error{ "Trainer data contains(Unsupported yet)" };
    }

    std::size_t prg_rom_size = static_cast<std::size_t>(header.prg_rom_size) * 16_KB;
    std::size_t chr_rom_size = static_cast<std::size_t>(header.chr_rom_size) * 8_KB;

    const u8* it = &data[16];

    // load PRG ROM
    prg_rom_.assign(it, it + prg_rom_size);

    it += prg_rom_size;

    // load CHR ROM
    if (header.chr_rom_size > 0)
    {
        chr_mem_.assign(it, it + chr_rom_size);
    }
    else
    {
        chr_mem_.resize(8_KB);
        use_chr_ram_ = true;
    }

    if (const bool use_prg_ram = getBitN(header.flag6, 1); use_prg_ram)
    {
        prg_ram_.resize(8_KB);
    }

    if (const bool ignore_mirror_control = getBitN(header.flag6, 3); !ignore_mirror_control)
    {
        mirror_type = (getBitN(header.flag6, 0) ? MirrorType::Vertical : MirrorType::Horizontal);
    }

    const u8 mapper_type = (header.flag7 & 0xF0) | ((header.flag6 & 0xF0) >> 4);
    return mapper_type;
}

bool Cartridge::useCHRRam() const
{
    return use_chr_ram_;
}

u8* const Cartridge::PRGRom()
{
    return prg_rom_.data();
}

u8* const Cartridge::CHRMem()
{
    return chr_mem_.data();
}

u8* const Cartridge::PRGRam()
{
    return prg_ram_.data();
}

usize Cartridge::PRGRomSize() const
{
    return prg_rom_.size();
}

usize Cartridge::CHRMemSize() const
{
    return chr_mem_.size();
}

usize Cartridge::PRGRamSize() const
{
    return prg_ram_.size();
}

