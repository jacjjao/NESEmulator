#include "Cartridge.hpp"
#include "common/bitHelper.hpp"
#include "Mapper000.hpp"
#include <iostream>
#include <fstream>


bool Cartridge::loadiNESFile(const std::filesystem::path& path)
{
	std::ifstream file{path, std::ios::binary};
	
    if (!file.is_open()) 
    {
        std::cerr << "[FAILED] cannot open the file: " << path << '\n';
        return false;
    }

    std::vector<u8> data(std::istreambuf_iterator<char>(file), {});
    if (data.size() < 16)
    {
        std::cerr << "[FAILED] file is invalid, the file only contains " << data.size() << " bytes\n";
        return false;
    }

    struct
    {
        u8 cN, cE, cS, cEOF;
        u8 prg_rom_size;
        u8 chr_rom_size;
        u8 mapper1;
        u8 mapper2;
        u8 prg_ram_size;
        u8 tv_system;
        u8 tv_system_prg;
        u8 u11, u12, u13, u14, u15; // unused bytes
    } header{};

    header.cN   = data[0];
    header.cE   = data[1];
    header.cS   = data[2];
    header.cEOF = data[3];

    header.prg_rom_size = data[4];
    header.chr_rom_size = data[5];

    header.mapper1 = data[6];
    header.mapper2 = data[7];

    header.prg_ram_size = data[8];

    header.tv_system     = data[9];
    header.tv_system_prg = data[10];

    header.u11 = data[11];
    header.u12 = data[12];
    header.u13 = data[13];
    header.u14 = data[14];
    header.u15 = data[15];


    // verify the file
    if (header.cN != 'N' ||
        header.cE != 'E' ||
        header.cS != 'S' ||
        static_cast<int>(header.cEOF) != 0x1A)
    {
        std::cerr << "[FAILED] header is invalid\n";
        return false;
    }

    if (getBitN(header.mapper1, 3))
    {
        std::cerr << "[FAILED] trainer contains\n";
        return false;
    }

    switch (header.mapper1)
    {
    case 0:
        mapper_.reset(new Mapper000{ header.prg_rom_size });
        break;

    default:
        std::cerr << "[FAILED] unsupport mapper\n";
        return false;
    }

    auto it = data.cbegin() + 16;

    // load PRG ROM
    std::size_t prg_rom_size = static_cast<std::size_t>(header.prg_rom_size) * 16384;
    prg_rom_.assign(it, it + prg_rom_size);
    it += prg_rom_size;

    // load CHR ROM
    std::size_t chr_rom_size = static_cast<std::size_t>(header.chr_rom_size) * 8192;
    chr_rom_.assign(it, it + chr_rom_size);

    return true;
}

const std::vector<u8>& Cartridge::getPRGRom() const
{
    return prg_rom_;
}

const std::vector<u8>& Cartridge::getCHRRom() const
{
    return chr_rom_;
}

bool Cartridge::cpuWrite(const u16 addr, const u8 data)
{
    const auto adr = mapper_->cpuMapRead(addr);
    if (adr.has_value())
    {
        prg_rom_[adr.value()] = data;
        return true;
    }
    return false;
}

std::optional<u8> Cartridge::cpuRead(const u16 addr)
{
    const auto adr = mapper_->cpuMapRead(addr);
    if (adr.has_value())
    {
        return prg_rom_[adr.value()];
    }
    return std::nullopt;
}

bool Cartridge::ppuWrite(const u16 addr, const u8 data)
{
    const auto adr = mapper_->ppuMapWrite(addr);
    if (adr.has_value())
    {
        chr_rom_[adr.value()] = data;
        return true;
    }
    return false;
}

std::optional<u8> Cartridge::ppuRead(const u16 addr)
{
    const auto adr = mapper_->ppuMapRead(addr);
    if (adr.has_value())
    {
        return chr_rom_[adr.value()];
    }
    return std::nullopt;
}
