#pragma once
#include "Math/Vec3.h"
#include "Math/Sphere.h"
#include "Math/Plane.h"
#include "Math/CollisionFace.h"

struct Ray3
{
	Vec3 origin;
	Vec3 normal;

	static inline Ray3 Create(float originX, float originY, float originZ, float normalX, float normalY, float normalZ)
	{
		Ray3 ray;
		Vec3::Set(&ray.origin, originX, originY, originZ);
		Vec3::Set(&ray.normal, normalX, normalY, normalZ);
		return ray;
	}

	static inline void Set(Ray3* out, Vec3* origin, Vec3* normal)
	{
		out->origin = *origin;
		out->normal = *normal;
	}

	static bool CalculateIntersectionWithSphereDistance(float* out, Ray3* ray, Sphere* sphere);
	static bool CalculateIntersectionWithSphere(Vec3* out, Ray3* ray, Sphere* sphere);
	static bool CalculateIntersectionWithPlaneDistance(float* out, Ray3* ray, Plane* plane);
	static bool CalculateIntersectionWithPlane(Vec3* out, Ray3* ray, Plane* plane);
	static void CalculateNearestPointOnRayToOtherPoint(Vec3* out, Ray3* ray, Vec3* point, float maxLengthAlongRay);
	static FaceIntersectionType CalculateIntersectionWithCollisionFace(Vec3* out, Ray3* ray, CollisionFace* face);
};