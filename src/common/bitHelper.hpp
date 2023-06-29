#pragma once

template<typename T>
constexpr T getBitN(const T var, const unsigned pos)
{
	return (var >> pos) & static_cast<T>(1);
}

template<typename T>
constexpr void setBitN(T& var, const bool val, const unsigned pos)
{
	if (val)
	{
		var = var | (static_cast<T>(1) << pos);
	}
	else
	{
		var = var & ~(static_cast<T>(1) << pos);
	}
}