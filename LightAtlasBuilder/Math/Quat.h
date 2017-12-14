#pragma once

#include <math.h>
#include "Math/Vec3.h"

struct Quat
{
	float x, y, z, w;

	static void FromAxisRotation(Quat* out, Vec3* axis, float angle)
	{
		float a = angle / 2.0f;
		float s = sinf(a);

		out->x = axis->x * s;
		out->y = axis->y * s;
		out->z = axis->z * s;
		out->w = cosf(a);
	}
};