#include "Math/Ray3.h" 
#include "Include/Common.h"

bool Ray3::CalculateIntersectionWithPlaneDistance(float* out, Ray3* ray, Plane* plane)
{
	/*
	O = Ray origin
	N = ray normal
	t = distance along ray
	P = intersection point
	S = plane normal
	d = plane d

	* Equation 1: Using P and values from the ray:
	O + Nt = P

	* Equation 2: Using P and values from the plane:
	P.S + d = 0

	* Substitute P from equation 1 into equation 2:
	(O + Nt).S + d = 0

	* We need to extract t, so use the dot product distributive law to get it out of the bracketed bit.
	O.S + Nt.S + d = 0

	* Extract t from the dot product:
	O.S + t(N.S) + d = 0

	* Rearrange to get t on the left hand side
	1) -t(N.S) = O.S + d
	2) -t = (O.S + d) / N.S
	3) t = -((O.S + d) / N.S)
	*/

	bool intersectionFound = true;

	float nDotS = Vec3::Dot(&ray->normal, &plane->normal);

	if (nDotS == 0) 
	{
		intersectionFound = false;
	}
	else
	{
		float t = -((Vec3::Dot(&ray->origin, &plane->normal) + plane->d) / nDotS);

		if (t < 0.0f)
		{
			intersectionFound = false;
		}

		if (out != null)
		{
			*out = t;
		}
	}

	return intersectionFound;
}

bool Ray3::CalculateIntersectionWithPlane(Vec3* out, Ray3* ray, Plane* plane)
{
	float t;

	bool intersectionFound = Ray3::CalculateIntersectionWithPlaneDistance(&t, ray, plane);

	if (intersectionFound) 
	{
		// Compute the intersection.
		Vec3::ScaleAndAdd(out, &ray->origin, &ray->normal, t);
	}

	return intersectionFound;
}

void Ray3::CalculateNearestPointOnRayToOtherPoint(Vec3* out, Ray3* ray, Vec3* point, float maxLengthAlongRay) 
{
	/*

	The ray, the point and the nearest intersection form a right-angled
	triangle. We can use trigonometry to find the intersection,

	H = Point - Ray Origin
	h = normalised(H)
	n = Ray normal

	cos(a) = n.h
	t = cos(a) * |H|

	*/

	Vec3 hypotenuse;
	Vec3::Sub(&hypotenuse, point, &ray->origin);

	float hypotenuseLength = Vec3::Length(&hypotenuse);

	Vec3 normalizedHypotenuse;
	Vec3::Scale(&normalizedHypotenuse, &hypotenuse, 1.0f / hypotenuseLength);

	float cosA = Vec3::Dot(&ray->normal, &normalizedHypotenuse);

	float t = cosA * hypotenuseLength;

	if (t < 0.0f) 
	{
		t = 0.0f;
	}
	else if (t > maxLengthAlongRay) 
	{
		t = maxLengthAlongRay;
	}

	Vec3::ScaleAndAdd(out, &ray->origin, &ray->normal, t);
}

FaceIntersectionType Ray3::CalculateIntersectionWithCollisionFace(Vec3* out, Ray3* ray, CollisionFace* face)
{
	FaceIntersectionType faceIntersectionType = FaceIntersectionTypeNone;

	bool isFrontSideCollision = Plane::CalculatePointDistance(&face->facePlane, &ray->origin) > 0.0f;

	Vec3 facePlaneIntersection;
	bool intersectionFound = Ray3::CalculateIntersectionWithPlane(&facePlaneIntersection, ray, &face->facePlane);

	if (intersectionFound)
	{
		if (CollisionFace::DetermineIfPointOnFacePlaneIsWithinCollisionFace(face, &facePlaneIntersection))
		{
			if (out != null)
			{
				*out = facePlaneIntersection;
			}

			faceIntersectionType = isFrontSideCollision ? FaceIntersectionTypeFrontSide : FaceIntersectionTypeBackSide;
		}
	}

	return faceIntersectionType;
}