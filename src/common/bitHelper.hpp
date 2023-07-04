#pragma once

template<typename T>
constexpr bool getBitN(const T var, const unsigned pos)
{
	return (var >> pos) & static_cast<T>(1);
}

template<typename T>
constexpr void setBitN(T& var, const unsigned pos, const bool val)
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