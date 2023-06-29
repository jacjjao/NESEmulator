#include "utility.hpp"
#include <cassert>

bool willAddOverflow(const u8 a, const u8 b, const u8 c)
{
	const i8 x = static_cast<i8>(a);
	assert(*reinterpret_cast<const u8*>(&x) == a);

	const i8 y = static_cast<i8>(b);
	assert(*reinterpret_cast<const u8*>(&y) == b);

	const i8 z = static_cast<i8>(c);

	i8 sum = 0;
	sum = x + (y + z); // 如果x+y overflow的話x+y+z的結果會是錯的
	return (x > 0 && y > 0 && sum <= 0) || (x < 0 && y < 0 && sum >= 0);
}