#include <math.h>
#include "Math/Mat2.h"

void Mat2::Set(
	Mat2* out,
	float m0, float m1, float m2, float m3)
{
	float* m = out->m;
	m[0] = m0;		m[1] = m1;		m[2] = m2;		m[3] = m3;
}

void Mat2::CreateIdentity(Mat2* out)
{
	Mat2::Set(out,
		1.0f, 0.0f,
		0.0f, 1.0f);
}

void Mat2::CreateRotation(Mat2* out, float radians)
{
	float s = sinf(radians);
	float c = cosf(radians);

	Mat2::Set(out,
		c, s,
		-s, c);
}