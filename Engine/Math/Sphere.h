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

	static inline void Zero(Sphere* out)
	{
		Vec3::Zero(&out->position);
		out->radius = 0.0f;
	}

	static inline void Translate(Sphere* out, Sphere* sphere, Vec3* amount)
	{
		Vec3::Add(&out->position, &sphere->position, amount);
		out->radius = sphere->radius;
	}

	static bool CalculateCollisionWithPlane(Vec3* out, Sphere* sphere, Plane* plane, Vec3* sphereMovementDirection);
	static bool CalculateCollisionWithCollisionFace(FaceCollisionResult* out, Sphere* sphere, CollisionFace* face, Vec3* movementDirection);
	static bool CheckIntersectsAABB(Sphere* sphere, AABB* aabb);
};