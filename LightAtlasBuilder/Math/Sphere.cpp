#include "Math/Sphere.h"
#include "Math/Ray3.h"

bool Sphere::CheckIntersectsAABB(Sphere* sphere, AABB* aabb) 
{
	Vec3 nearestPointInAABB;
	AABB::ClampPoint(&nearestPointInAABB, aabb, &sphere->position);

	Vec3 nearestPointInAABBToSpherePosition;
	Vec3::Sub(&nearestPointInAABBToSpherePosition, &sphere->position, &nearestPointInAABB);

	float sphereRadiusSqr = sphere->radius * sphere->radius;
	float nearestPointInAABBToSpherePositionLengthSqr = Vec3::LengthSqr(&nearestPointInAABBToSpherePosition);

	return nearestPointInAABBToSpherePositionLengthSqr <= sphereRadiusSqr;
}