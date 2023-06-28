#pragma once

template<typename T>
constexpr T getBitN(const T var, const unsigned pos)
{
	return (var >> pos) & static_cast<T>(1);
}