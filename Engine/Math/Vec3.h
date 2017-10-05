#pragma once

#include <math.h>

struct Mat4;

struct Vec3
{
	float x, y, z;

	static inline Vec3 Create()
	{
		Vec3 v;
		v.x = 0.0f;
		v.y = 0.0f;
		v.z = 0.0f;
		return v;
	}

	static inline Vec3 Create(float x, float y, float z)
	{
		Vec3 v;
		v.x = x;
		v.y = y;
		v.z = z;
		return v;
	}

	static inline void Zero(Vec3* out)
	{
		out->x = 0.0f;
		out->y = 0.0f;
		out->z = 0.0f;
	}

	static inline bool IsZero(Vec3* v)
	{
		return v->x == 0.0f && v->y == 0.0f && v->z == 0.0f;
	}

	static inline void Set(Vec3* out, float x, float y, float z)
	{
		out->x = x;
		out->y = y;
		out->z = z;
	}

	/*static inline void Copy(Vec3* out, Vec3* v)
	{
		out->x = v->x;
		out->y = v->y;
		out->z = v->z;
	}*/

	static inline void Add(Vec3* out, Vec3* a, Vec3* b)
	{
		out->x = a->x + b->x;
		out->y = a->y + b->y;
		out->z = a->z + b->z;
	}

	static inline void Add(Vec3* out, Vec3* a, float x, float y, float z)
	{
		out->x = a->x + x;
		out->y = a->y + y;
		out->z = a->z + z;
	}

	static inline void Sub(Vec3* out, Vec3* a, Vec3* b)
	{
		out->x = a->x - b->x;
		out->y = a->y - b->y;
		out->z = a->z - b->z;
	}

	static inline void Sub(Vec3* out, Vec3* a, float f)
	{
		out->x = a->x - f;
		out->y = a->y - f;
		out->z = a->z - f;
	}

	static inline void Mul(Vec3* out, Vec3* a, Vec3* b)
	{
		out->x = a->x * b->x;
		out->y = a->y * b->y;
		out->z = a->z * b->z;
	}

	static inline void Div(Vec3* out, Vec3* a, Vec3* b)
	{
		out->x = a->x / b->x;
		out->y = a->y / b->y;
		out->z = a->z / b->z;
	}

	static inline void Scale(Vec3* out, Vec3* a, float scale)
	{
		out->x = a->x * scale;
		out->y = a->y * scale;
		out->z = a->z * scale;
	}

	static inline void ScaleAndAdd(Vec3* out, Vec3* a, Vec3* b, float scale)
	{
		out->x = a->x + b->x * scale;
		out->y = a->y + b->y * scale;
		out->z = a->z + b->z * scale;
	}

	static inline float Dot(Vec3* a, Vec3* b)
	{
		return a->x * b->x + a->y * b->y + a->z * b->z;
	}

	static inline void Cross(Vec3* out, Vec3* a, Vec3* b)
	{
		out->x = a->y * b->z - a->z * b->y;
		out->y = a->z * b->x - a->x * b->z;
		out->z = a->x * b->y - a->y * b->x;
	}

	static inline float LengthSqr(Vec3* v)
	{
		return v->x * v->x + v->y * v->y + v->z * v->z;
	}

	static inline float Length(Vec3* v)
	{
		return sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
	}

	static inline float Distance(Vec3* a, Vec3* b)
	{
		Vec3 c;
		Vec3::Sub(&c, a, b);
		return Vec3::Length(&c);
	}

	static inline float DistanceSqr(Vec3* a, Vec3* b)
	{
		Vec3 c;
		Vec3::Sub(&c, a, b);
		return Vec3::LengthSqr(&c);
	}

	static inline void Normalize(Vec3* out, Vec3* v)
	{
		float length = Vec3::Length(v);
		if (length > 0)
		{
			out->x /= length;
			out->y /= length;
			out->z /= length;
		}
	}

	static inline void Min(Vec3* out, Vec3* v1, Vec3* v2)
	{
		out->x = v1->x < v2->x ? v1->x : v2->x;
		out->y = v1->y < v2->y ? v1->y : v2->y;
		out->z = v1->z < v2->z ? v1->z : v2->z;
	}

	static inline void Max(Vec3* out, Vec3* v1, Vec3* v2)
	{
		out->x = v1->x > v2->x ? v1->x : v2->x;
		out->y = v1->y > v2->y ? v1->y : v2->y;
		out->z = v1->z > v2->z ? v1->z : v2->z;
	}

	static inline bool Equals(Vec3* v1, Vec3* v2)
	{
		return 
			v1->x == v2->x &&
			v1->y == v2->y &&
			v1->z == v2->z;
	}

	static inline float CalculateYAxisFacingAngle(Vec3* from, Vec3* to) {

		return atan2f(
			to->x - from->x,
			to->z - from->z);
	}

	static void TransformMat3(Vec3* out, const Vec3* v, const Mat4* transform);
	static void TransformMat4(Vec3* out, const Vec3* v, const Mat4* transform);
};