#include "Math/Ray3.h" 
#include "Include/Common.h"

bool Ray3::CalculateIntersectionWithSphereDistance(float* out, Ray3* ray, Sphere* sphere)
{
	/*
	O = Ray origin
	N = ray normal
	t = distance along ray
	P = intersection point
	S = Sphere position
	R = sphere radius

	* Equation 1: Using P and values from the ray:
	O + Nt = P

	* Equation 2: Using P and values from the sphere:
	(P - S).(P - S) = R^2

	* Substitute P from equation 1 into equation 2:
	(O + Nt - S).(O + Nt - S) = R^2

	* We need to extract t, so first of all expand the dot products using the dot product distributive law:
	(O + Nt - S).O + (O + Nt - S).Nt + (O + Nt - S).(-S) = R^2

	* Do a second round of extracting t using the dot product distributive law:
	(O - S).O + (O - S).Nt + (O - S).(-S) + Nt.O + Nt.Nt + Nt.(-S) = R^2

	* Collect terms (use the dot product distributive law in the opposite direction to
	turn Nt.O + Nt.(-S) into (O - S).Nt, which can then be collected into 2(O - S).Nt):
	(O - S).O + 2(O - S).Nt + (O - S).(-S) + Nt.Nt = R^2

	* Extract t from the dot products:
	(O - S).O + t(2(O - S).N) + (O - S).(-S) + t^2(N.N) = R^2

	* Move R^2 over to the left hand side.
	(O - S).O + t(2(O - S).N) + (O - S).(-S) + t^2(N.N) - R^2 = 0

	* This can be solved using the quadratic formula: ax^2 + bx + c = 0. Reaaranged for the quadratic formula, we get:
	t^2(N.N) + t(2(O - S).N) + (O - S).O + (O - S).(-S) - R^2 = 0

	where
	a = N.N
	b = 2(O - S).N
	c = (O - S).O + (O - S).(-S) - R^2 = (O - S).(O - S) - R^2

	* The quadratic formula can be re-arrange to solve x as:
	x = (-b [+-] sqrt(b^2 - 4ac)) / 2a

	We need to find both the + and - solution and find the shortest positive value (since negative
	means the sphere is behind the ray, which we aren't interested in).

	We also need to check if the calculation inside the sqrt is < 0, as that means
	there is no intersection.

	*/

	bool intersectionFound = true;

	// Compute O - S.
	Vec3 oMinusS;
	Vec3::Sub(&oMinusS, &ray->origin, &sphere->position);

	// Compute a.
	float a = Vec3::Dot(&ray->normal, &ray->normal);

	// Compute b.
	float b = 2.0f * Vec3::Dot(&oMinusS, &ray->normal);

	// Compute c.
	float c = Vec3::Dot(&oMinusS, &oMinusS) - sphere->radius * sphere->radius;

	// Compute the determinant (the bit inside the square root).
	float determinant = b * b - 4 * a * c;

	if (determinant < 0.0f) {
		return false;
	}

	// Compute the square root of the determinant.
	float determinantSqtr = sqrtf(determinant);

	// Compute t0 (the + version).
	float t0 = (-b + determinantSqtr) / (2.0f * a);

	// Compute t1 (the - version).
	float t1 = (-b - determinantSqtr) / (2.0f * a);

	// Find t (the lowest positive value of t0 and t1). Remember that t0 or t1, or both could be
	// negative, which we don't want.
	float t = t0;

	if (t0 < 0.0f) 
	{
		if (t1 < 0.0f) 
		{
			intersectionFound = false;
		}
		else 
		{
			t = t1;
		}
	}
	else if (t1 > 0.0f && t1 < t0) 
	{
		t = t1;
	}

	*out = t;

	return intersectionFound;
}

bool Ray3::CalculateIntersectionWithSphere(Vec3* out, Ray3* ray, Sphere* sphere)
{
	float t = 0.0f;

	bool intersectionFound = Ray3::CalculateIntersectionWithSphereDistance(&t, ray, sphere);

	if (intersectionFound) 
	{
		// Compute the intersection.
		Vec3::ScaleAndAdd(out, &ray->origin, &ray->normal, t);
	}

	// We're done!
	return intersectionFound;
}

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