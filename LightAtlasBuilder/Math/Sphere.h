#pragma once
#include "Math/Vec3.h"
#include "Math/Plane.h"
#include "Math/AABB.h"
#include "Math/CollisionFace.h"

struct Sphere
{
	Vec3 position;
	float radius;

	static inline Sphere Create(float x, float y, float z, float radius)
	{
		Sphere sphere;
		Sphere::Set(&sphere, x, y, z, radius);
		return sphere;
	}

	static inline void Set(Sphere* out, float x, float y, float z, float radius)
	{
		Vec3::Set(&out->position, x, y, z);
		out->radius = radius;
	}

	static inline void Set(Sphere* out, Vec3* position, float radius)
	{
		out->position = *position;
		out->radius = radius;
	}

	static inline void Zero(Sphere* out)
	{
		Vec3::Zero(&out->position);
		out->radius = 0.0f;
	}

	static bool CheckIntersectsAABB(Sphere* sphere, AABB* aabb);
};