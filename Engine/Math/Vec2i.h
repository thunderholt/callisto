#pragma once

#include <math.h>

struct Vec2i
{
	int x, y;

	static inline Vec2i Create(int x, int y)
	{
		Vec2i v;
		v.x = x;
		v.y = y;
		return v;
	}

	static inline void Set(Vec2i* out, int x, int y)
	{
		out->x = x;
		out->y = y;
	}

	static inline void Zero(Vec2i* out)
	{
		out->x = 0;
		out->y = 0;
	}

	static inline bool IsZero(Vec2i* v)
	{
		return v->x == 0 && v->y == 0;
	}

	static inline void Sub(Vec2i* out, Vec2i* a, Vec2i* b)
	{
		out->x = a->x - b->x;
		out->y = a->y - b->y;
	}
};