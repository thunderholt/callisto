#pragma once
#include "Math/Vec3.h"

struct AABB
{
	Vec3 from;
	Vec3 to;

	static inline void Zero(AABB* out)
	{
		Vec3::Zero(&out->from);
		Vec3::Zero(&out->to);
	}

	/*static inline void Copy(AABB* out, AABB* aabb)
	{
		Vec3::Copy(&out->from, &aabb->from);
		Vec3::Copy(&out->to, &aabb->to);
	}*/

	static inline void CalculateSize(Vec3* out, AABB* aabb)
	{
		Vec3::Sub(out, &aabb->to, &aabb->from);
	}

	static inline void Translate(AABB* out, AABB* aabb, Vec3* amount)
	{
		Vec3::Add(&out->from, &aabb->from, amount);
		Vec3::Add(&out->to, &aabb->to, amount);
	}

	static void ClampPoint(Vec3* out, Vec3* point, AABB* aabb);
};