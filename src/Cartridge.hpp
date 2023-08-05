#pragma once

#include "common/type.hpp"
#include <filesystem>
#include <vector>


constexpr usize operator ""_KB(unsigned long long val)
{
	return val * 1024;
}

enum class MirrorType
{
	Vertical, Horizontal,
	OneScreenLow, OneScreenHigh,
	FourScreen
};

class Cartridge
{
public:
	u8 loadiNESFile(const std::filesystem::path& path);

	bool useCHRRam() const;

	u8* const PRGRom();
	u8* const CHRMem();

	usize PRGRomSize() const;
	usize CHRMemSize() const;

	MirrorType mirror_type = MirrorType::FourScreen;

private:
	bool use_chr_ram_ = false;
	std::vector<u8> prg_rom_, chr_mem_;
};