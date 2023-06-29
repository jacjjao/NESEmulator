#include <gtest/gtest.h>
#include <numeric>
#include "../src/common/utility.hpp"

TEST(HelloTest, BasicAssertions) {
	i8 a = -1;
	i8 b = std::numeric_limits<i8>::min();
	i8 c = 0;

	u8 x = *reinterpret_cast<const u8*>(&a);
	u8 y = *reinterpret_cast<const u8*>(&b);
	EXPECT_EQ(true, willAddOverflow(x, y, c));
	EXPECT_EQ(true, willAddOverflow(y, x, c));

	c = 1;
	EXPECT_EQ(false, willAddOverflow(x, y, c));
	EXPECT_EQ(false, willAddOverflow(y, x, c));

	a = std::numeric_limits<i8>::max();
	b = 5;
	x = *reinterpret_cast<const u8*>(&a);
	y = *reinterpret_cast<const u8*>(&b);
	EXPECT_EQ(true, willAddOverflow(x, y, c));
	EXPECT_EQ(true, willAddOverflow(y, x, c));

	b = -5;
	y = *reinterpret_cast<const u8*>(&b);
	EXPECT_EQ(false, willAddOverflow(x, y, c));
	EXPECT_EQ(false, willAddOverflow(y, x, c));
}

