#include "Mapper.hpp"


Mapper::Mapper(Cartridge cart) : 
	cart_{ std::move(cart) }
{
}

MirrorType Mapper::getMirrorType()
{
	return cart_.mirror_type;
}

std::shared_ptr<Mapper> createMapper()
{
	return nullptr;
}
