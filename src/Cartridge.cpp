#include "Cartridge.hpp"
#include "common/bitHelper.hpp"
#include "Mapper/Mapper000.hpp"
#include "Mapper/Mapper003.hpp"
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
        u8 flag6;
        u8 flag7;
        u8 flag8;
        u8 flag9;
        u8 flag10;
        u8 u11, u12, u13, u14, u15; // unused bytes
    } header{};

    header.cN   = data[0];
    header.cE   = data[1];
    header.cS   = data[2];
    header.cEOF = data[3];

    header.prg_rom_size = data[4];
    header.chr_rom_size = data[5];

    header.flag6  = data[ 6];
    header.flag7  = data[ 7];
    header.flag8  = data[ 8];
    header.flag9  = data[ 9];
    header.flag10 = data[10];

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

    if (getBitN(header.flag6, 3))
    {
        std::cerr << "[FAILED] trainer contains\n";
        return false;
    }

    mirror_type_ = getBitN(header.flag6, 0) ? MirrorType::Vertical : MirrorType::Horizontal;

    std::size_t prg_rom_size = static_cast<std::size_t>(header.prg_rom_size) * 16384;
    std::size_t chr_rom_size = static_cast<std::size_t>(header.chr_rom_size) * 8192;
    const u8 mapper_type = (header.flag7 & 0xF0) | ((header.flag6 & 0xF0) >> 4);
    switch (mapper_type)
    {
    case 0x00:
        mapper_.reset(new Mapper000{ prg_rom_size, chr_rom_size });
        break;
    
    case 0x03:
        mapper_.reset(new Mapper003{ prg_rom_size, chr_rom_size });
        break;
    
    default:
        std::cerr << "[FALIED] unsupport mapper " << (int)mapper_type << '\n';
        return false;
    }

    u8* it = &(*(data.begin() + 16));

    // load PRG ROM
    mapper_->loadPrgRom(it, it + prg_rom_size);

    it += prg_rom_size;

    // load CHR ROM
    if (header.chr_rom_size > 0)
    {
        mapper_->loadChrRom(it, it + chr_rom_size);
    }

    return true;
}

bool Cartridge::cpuWrite(const u16 addr, const u8 data)
{
    return mapper_->cpuMapWrite(addr, data);
}

std::optional<u8> Cartridge::cpuRead(const u16 addr)
{
    return mapper_->cpuMapRead(addr);
}

bool Cartridge::ppuWrite(const u16 addr, const u8 data)
{
    return mapper_->ppuMapWrite(addr, data);
}

std::optional<u8> Cartridge::ppuRead(const u16 addr)
{
    return mapper_->ppuMapRead(addr);
}

MirrorType Cartridge::getMirrorType() const
{
    return mirror_type_;
}
