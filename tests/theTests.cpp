#include <gtest/gtest.h>
#include <numeric>
#include "../src/common/utility.hpp"
#include "../src/common/bitHelper.hpp"

TEST(HelloTest, willAddOverflowTest) 
{
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

	a = std::numeric_limits<u8>::min() + 5;
	b = -6;
	c = 1;
	x = *reinterpret_cast<const u8*>(&a);
	y = *reinterpret_cast<const u8*>(&b);
	EXPECT_EQ(false, willAddOverflow(x, y, c));
	EXPECT_EQ(false, willAddOverflow(y, x, c));
}

TEST(HelloTest, getBitNTest)
{
	u8 a = 0b10101010;
	EXPECT_EQ(getBitN(a, 0), 0);
	EXPECT_EQ(getBitN(a, 1), 1);
	EXPECT_EQ(getBitN(a, 2), 0);
	EXPECT_EQ(getBitN(a, 3), 1);
	EXPECT_EQ(getBitN(a, 4), 0);
	EXPECT_EQ(getBitN(a, 5), 1);
	EXPECT_EQ(getBitN(a, 6), 0);
	EXPECT_EQ(getBitN(a, 7), 1);

	a = ~a;
	EXPECT_EQ(getBitN(a, 0), 1);
	EXPECT_EQ(getBitN(a, 1), 0);
	EXPECT_EQ(getBitN(a, 2), 1);
	EXPECT_EQ(getBitN(a, 3), 0);
	EXPECT_EQ(getBitN(a, 4), 1);
	EXPECT_EQ(getBitN(a, 5), 0);
	EXPECT_EQ(getBitN(a, 6), 1);
	EXPECT_EQ(getBitN(a, 7), 0);
}

TEST(HelloTest, setBitNTest)
{
	u8 a = 0;
	setBitN(a, 1, 1);
	setBitN(a, 3, 1);
	setBitN(a, 5, 1);
	setBitN(a, 7, 1);
	EXPECT_EQ(a, 0b10101010);

	setBitN(a, 1, 0);
	setBitN(a, 3, 0);
	setBitN(a, 5, 0);
	setBitN(a, 7, 0);
	setBitN(a, 0, 1);
	setBitN(a, 2, 1);
	setBitN(a, 4, 1);
	setBitN(a, 6, 1);
	EXPECT_EQ(a, 0b01010101);
}