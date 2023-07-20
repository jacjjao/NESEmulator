#pragma once

#include "common/type.hpp"
#include "Mapper.hpp"
#include <filesystem>
#include <memory>
#include <vector>

class Cartridge
{
public:
	bool loadiNESFile(const std::filesystem::path& path);
	
	Mapper* getMapper();

	const std::vector<u8>& getPRGRom() const;
	const std::vector<u8>& getCHRRom() const;

private:
	std::vector<u8> prg_rom_;
	std::vector<u8> chr_rom_;

	std::unique_ptr<Mapper> mapper_;
};