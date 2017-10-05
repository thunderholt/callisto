#pragma once

struct Mat2
{
	float m[4];

	static void Set(Mat2* out, float m0, float m1, float m2, float m3);
	static void CreateIdentity(Mat2* out);
	static void CreateRotation(Mat2* out, float radians);
};