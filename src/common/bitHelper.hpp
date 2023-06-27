#pragma once

template<typename T>
constexpr T getBitN(T var, unsigned pos)
{
	return var >> pos;
}