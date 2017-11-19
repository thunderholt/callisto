#pragma once

#include <math.h>

struct Vec2
{
	float x, y;

	static inline Vec2 Create(int x, int y)
	{
		return Vec2::Create((float)x, (float)y);
	}

	static inline Vec2 Create(float x, float y)
	{
		Vec2 v;
		v.x = x;
		v.y = y;
		return v;
	}

	static inline void Set(Vec2* out, float x, float y)
	{
		out->x = x;
		out->y = y;
	}

	static inline void Zero(Vec2* out)
	{
		out->x = 0.0f;
		out->y = 0.0f;
	}

	static inline bool IsZero(Vec2* v)
	{
		return v->x == 0.0f && v->y == 0.0f;
	}

	static inline void Sub(Vec2* out, Vec2* a, Vec2* b)
	{
		out->x = a->x - b->x;
		out->y = a->y - b->y;
	}

	static inline void Scale(Vec2* out, Vec2* a, float scale)
	{
		out->x = a->x * scale;
		out->y = a->y * scale;
	}

	static inline float Length(Vec2* v)
	{
		return sqrtf(v->x * v->x + v->y * v->y);
	}

	static inline float LengthSqr(Vec2* v)
	{
		return v->x * v->x + v->y * v->y;
	}

	static inline float Distance(Vec2* a, Vec2* b)
	{
		Vec2 c;
		Vec2::Sub(&c, a, b);
		return Vec2::Length(&c);
	}

	static inline float DistanceSqr(Vec2* a, Vec2* b)
	{
		Vec2 c;
		Vec2::Sub(&c, a, b);
		return Vec2::LengthSqr(&c);
	}

	static inline void Normalize(Vec2* out, Vec2* v)
	{
		float length = Vec2::Length(v);
		if (length > 0)
		{
			out->x /= length;
			out->y /= length;
		}
	}
};