#include "Mapper.hpp"


void Mapper::setMirrortype(const MirrorType type)
{
	mirror_type_ = type;
}

MirrorType Mapper::getMirrortype() const
{
	return mirror_type_;
}
