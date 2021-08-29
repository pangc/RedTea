#include <gtest/gtest.h>
#include "math/vector.h"
#include "math/matrix.h"

TEST(MATH_TEST, vector)
{
	using namespace redtea::math;
	
	Vector2f v1 = {1.0, 2.0};
	EXPECT_EQ(v1.x, 1.0);
	EXPECT_EQ(v1[0], 1.0);

	Vector2i v2 = {3, 4};

	auto v = v1 + v2;
	EXPECT_EQ(v.x, v1.x + v2.x);

	auto c = cross(v1, v2);
	EXPECT_EQ(c, -2);

	Vector2f v3 = { 1, 1 };
	Vector2f v4 = { 1, 1 };
	EXPECT_EQ(v3 == v4, true);
	EXPECT_EQ(distance(v3, v4), 0);

	Vector3f v5 = { 1, 1, 1};
	Vector3f v6 = { v3, 1 };
	EXPECT_EQ(v5 == v6, true);
	 
	Vector4f v7 = { 1, 1, 1, 4};
	Vector4f v8 = { v6, 4 };
	EXPECT_EQ(v7 == v8, true);
}

TEST(MATH_TEST, matrix)
{
	using namespace redtea::math;
	Mat4i m1(
		0, 1, 2, 3,
		4, 5, 6, 7,
		8, 9, 10, 11,
		12, 13, 14, 15
	);
	
	Mat4i m2 = transpose(m1);
	bool success = true;
	for (int i = 0; i < 4; i++)
	{
		success = success & m1[i][0] == m2[0][i];
		success = success & m1[i][1] == m2[1][i];
		success = success & m1[i][2] == m2[2][i];
		success = success & m1[i][3] == m2[3][i];
	}
	EXPECT_EQ(success, true);

	Mat4i identity;
	Mat4i m3 = m1 * identity;
	EXPECT_EQ(m1 == m3, true);
}