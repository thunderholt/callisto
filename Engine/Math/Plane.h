#pragma once
#include "Math/Vec3.h"

struct PlaneCollisionSlideReaction
{
	Vec3 direction;
	float distance;
};

struct Plane
{
	Vec3 normal;
	float d;

	static void FromPoints(Plane* out, Vec3* points);
	static void FromNormalAndPoint(Plane* out, Vec3* normal, Vec3* point);
	static float CalculatePointDistance(Plane* plane, Vec3* point);
	static bool CalculatePlaneCollisionSlideReaction(PlaneCollisionSlideReaction* out, Plane* plane, Vec3* intersection, Vec3* desiredDirection, float desiredDistance);
};
