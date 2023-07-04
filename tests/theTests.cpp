#include <gtest/gtest.h>
#include <numeric>
#include "../src/common/type.hpp"
#include "../src/common/bitHelper.hpp"


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