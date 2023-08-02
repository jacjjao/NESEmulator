#pragma once

#include "common/type.hpp"
#include "Mapper/Mapper.hpp"
#include <filesystem>
#include <memory>
#include <vector>


class Cartridge
{
public:
	bool loadiNESFile(const std::filesystem::path& path);

	bool cpuWrite(const u16 addr, const u8 data);
	std::optional<u8> cpuRead(const u16 addr);

	bool ppuWrite(const u16 addr, const u8 data);
	std::optional<u8> ppuRead(const u16 addr);

	MirrorType getMirrorType() const;

private:
	std::unique_ptr<Mapper> mapper_;
};