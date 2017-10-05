#include "Math/Plane.h"
#include "Math/Ray3.h"

void Plane::FromPoints(Plane* out, Vec3* points)
{
	Vec3 vecA;
	Vec3 vecB;
	Vec3::Sub(&vecA, &points[1], &points[0]);
	Vec3::Sub(&vecB, &points[2], &points[0]);

	Vec3::Cross(&out->normal, &vecA, &vecB);
	Vec3::Normalize(&out->normal, &out->normal);

	out->d = -Vec3::Dot(&out->normal, &points[0]);
}

void Plane::FromNormalAndPoint(Plane* out, Vec3* normal, Vec3* point) 
{
	out->normal = *normal;
	out->d = -Vec3::Dot(normal, point);
}

float Plane::CalculatePointDistance(Plane* plane, Vec3* point) 
{
	return Vec3::Dot(&plane->normal, point) + plane->d;
}

bool Plane::CalculatePlaneCollisionSlideReaction(PlaneCollisionSlideReaction* out, Plane* plane, Vec3* intersection, Vec3* desiredDirection, float desiredDistance) 
{
	Vec3 targetPoint;
	Vec3::ScaleAndAdd(&targetPoint, intersection, desiredDirection, desiredDistance);

	Ray3 targetPointToProjectionPointRay;
	Ray3::Set(&targetPointToProjectionPointRay, &targetPoint, &plane->normal);

	Vec3 projectionPoint;
	bool targetPointToProjectionPointRayIntersectsPlane = Ray3::CalculateIntersectionWithPlane(
		&projectionPoint, &targetPointToProjectionPointRay, plane);

	if (!targetPointToProjectionPointRayIntersectsPlane) 
	{
		return false;
	}

	Vec3 slideVector;
	Vec3::Sub(&slideVector, &projectionPoint, intersection);
	float slideDistance = Vec3::Length(&slideVector);
	Vec3::Normalize(&slideVector, &slideVector);

	// Copy out the results.
	out->direction = slideVector;
	out->distance = slideDistance;

	return true;
}