#include "Math/AABB.h"

void AABB::ClampPoint(Vec3* out, AABB* aabb, Vec3* point) 
{
	*out = *point;

	if (point->x < aabb->from.x) 
	{
		out->x = aabb->from.x;
	}

	if (point->y < aabb->from.y) 
	{
		out->y = aabb->from.y;
	}

	if (point->z < aabb->from.z) 
	{
		out->z = aabb->from.z;
	}

	if (point->x > aabb->to.x) 
	{
		out->x = aabb->to.x;
	}

	if (point->y > aabb->to.y)
	{
		out->y = aabb->to.y;
	}

	if (point->z > aabb->to.z)
	{
		out->z = aabb->to.z;
	}
}

bool AABB::CheckContainsPoint(AABB* aabb, Vec3* point)
{
	bool containsPoint = true;

	if (Vec3::IsLessThan(point, &aabb->from))
	{
		containsPoint = false;
	}
	else if (Vec3::IsGreaterThan(point, &aabb->to))
	{
		containsPoint = false;
	}

	return containsPoint;
}