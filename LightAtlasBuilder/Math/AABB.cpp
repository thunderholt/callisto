#include "Math/AABB.h"

void AABB::ClampPoint(Vec3* out, Vec3* point, AABB* aabb) 
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


void AABB::CalculateFromCollisionFaces(AABB* out, CollisionFace* collisionFaces, int numberOfCollisionFaces)
{
	bool initialised = false;

	AABB::Zero(out);

	for (int collisionFaceIndex = 0; collisionFaceIndex < numberOfCollisionFaces; collisionFaceIndex++)
	{
		CollisionFace* collisionFace = &collisionFaces[collisionFaceIndex];

		for (int pointIndex = 0; pointIndex < 3; pointIndex++)
		{
			Vec3* point = &collisionFace->points[pointIndex];

			if (!initialised)
			{
				out->from = *point;
				out->to = *point;
				initialised = true;
			}
			else
			{
				if (point->x < out->from.x)
				{
					out->from.x = point->x;
				}

				if (point->y < out->from.y)
				{
					out->from.y = point->y;
				}

				if (point->z < out->from.z)
				{
					out->from.z = point->z;
				}

				if (point->x > out->to.x)
				{
					out->to.x = point->x;
				}

				if (point->y > out->to.y)
				{
					out->to.y = point->y;
				}

				if (point->z > out->to.z)
				{
					out->to.z = point->z;
				}
			}
		}
	}
}

bool AABB::CheckIntersectsAABB(AABB* aabb1, AABB* aabb2) 
{
	bool intersects = true;

	if (aabb1->from.x > aabb2->to.x ||
		aabb1->from.y > aabb2->to.y ||
		aabb1->from.z > aabb2->to.z ||
		aabb1->to.x < aabb2->from.x ||
		aabb1->to.y < aabb2->from.y ||
		aabb1->to.z < aabb2->from.z)
	{
		intersects = false;
	}

	return intersects;
}