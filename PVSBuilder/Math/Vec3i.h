#pragma once

struct Vec3i
{
	int x, y, z;

	static inline void Zero(Vec3i* out)
	{
		out->x = 0;
		out->y = 0;
		out->z = 0;
	}

	static inline void Set(Vec3i* out, int x, int y, int z)
	{
		out->x = x;
		out->y = y;
		out->z = z;
	}

	static inline void Add(Vec3i* out, Vec3i* v1, Vec3i* v2)
	{
		out->x = v1->x + v2->x;
		out->y = v1->y + v2->y;
		out->z = v1->z + v2->z;
	}
};