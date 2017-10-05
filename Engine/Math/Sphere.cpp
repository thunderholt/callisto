#include "Math/Sphere.h"
#include "Math/Ray3.h"

bool Sphere::CalculateCollisionWithPlane(Vec3* out, Sphere* sphere, Plane* plane, Vec3* sphereMovementDirection) 
{
	float currentDistanceToPlane = Plane::CalculatePointDistance(plane, &sphere->position);
	if (currentDistanceToPlane < sphere->radius) 
	{
		return false;
	}

	Ray3 sphereMovementRay;
	Ray3::Set(&sphereMovementRay, &sphere->position, sphereMovementDirection);

	Vec3 sphereMovementRayPlaneIntersection;
	bool sphereMovementRayIntersectsPlane = Ray3::CalculateIntersectionWithPlane(&sphereMovementRayPlaneIntersection, &sphereMovementRay, plane);
	if (!sphereMovementRayIntersectsPlane) 
	{
		return false;
	}

	Vec3 invPlaneNormal;
	Vec3::Scale(&invPlaneNormal, &plane->normal, -1.0f);

	float cosA = Vec3::Dot(&sphereMovementRay.normal, &invPlaneNormal);

	if (cosA == 1.0f) 
	{
		*out = sphereMovementRayPlaneIntersection;
		return true;
	}

	float hypotenuse = sphere->radius / cosA;

	float t = Vec3::Distance(&sphereMovementRayPlaneIntersection, &sphere->position) - hypotenuse;

	Vec3 spherePlaneIntersection;
	Vec3::ScaleAndAdd(&spherePlaneIntersection, &sphere->position, &sphereMovementRay.normal, t);
	Vec3::ScaleAndAdd(&spherePlaneIntersection, &spherePlaneIntersection, &invPlaneNormal, sphere->radius);

	*out = spherePlaneIntersection;

	return true;
}

bool Sphere::CalculateCollisionWithCollisionFace(FaceCollisionResult* out, Sphere* sphere, CollisionFace* face, Vec3* movementDirection)
{
	// If the face plane faces away from the sphere movement direction, then there is no collision.
	if (Vec3::Dot(movementDirection, &face->facePlane.normal) >= 0.0f) 
	{
		return false;
	}

	// Find the sphere/face-plane intersection.
	bool sphereIntersectsFacePlane = false;
	Vec3 sphereFacePlaneIntersection;

	float distanceFromSpherePositionToFacePlane = Plane::CalculatePointDistance(&face->facePlane, &sphere->position);

	if (distanceFromSpherePositionToFacePlane <= sphere->radius && distanceFromSpherePositionToFacePlane >= 0.0f) 
	{
		Ray3 facePlaneIntersectionRay;
		facePlaneIntersectionRay.origin = sphere->position;
		Vec3::Scale(&facePlaneIntersectionRay.normal, &face->facePlane.normal, -1.0f);

		sphereIntersectsFacePlane = Ray3::CalculateIntersectionWithPlane(
			&sphereFacePlaneIntersection, &facePlaneIntersectionRay, &face->facePlane);
	}
	else 
	{
		sphereIntersectsFacePlane = Sphere::CalculateCollisionWithPlane(
			&sphereFacePlaneIntersection, sphere, &face->facePlane, movementDirection);
	}

	if (!sphereIntersectsFacePlane) 
	{
		return false;
	}

	// Determine if the sphere/face-plane intersection is within the face.
	if (CollisionFace::DetermineIfPointOnFacePlaneIsWithinCollisionFace(face, &sphereFacePlaneIntersection)) 
	{
		out->intersection = sphereFacePlaneIntersection;
		out->distance = Vec3::Distance(&sphereFacePlaneIntersection, &sphere->position) - sphere->radius;

		// Handle the case where the face-plane already intersects the sphere.
		if (out->distance < 0.0f) 
		{
			out->distance = 0.0f;
		}

		out->collisionPlane = face->facePlane;
	}
	else 
	{
		// Nope, find the nearest point on the face's perimeter to the sphere/face-plane intersection.
		Vec3 nearestPointOnFacePerimeter;
		CollisionFace::FindNearestPointOnCollisionFacePerimeterToPoint(&nearestPointOnFacePerimeter, face, &sphereFacePlaneIntersection);

		// Cast a ray from the nearest point on face perimeter back to the sphere, using the reverse of the 
		// movement direction, to find the point on the sphere where the intersection will happen.
		Ray3 sphereIntersectionRay;
		sphereIntersectionRay.origin = nearestPointOnFacePerimeter;
		Vec3::Scale(&sphereIntersectionRay.normal, movementDirection, -1.0f);

		float intersectionDistance = 0.0f;
		if (!Ray3::CalculateIntersectionWithSphereDistance(&intersectionDistance, &sphereIntersectionRay, sphere)) 
		{
			return false;
		}

		out->intersection = nearestPointOnFacePerimeter;
		out->distance = intersectionDistance;

		// Handle the case where the face-plane already intersects the sphere.
		if (out->distance < 0.0f) 
		{
			out->distance = 0.0f;
		}

		// Calculate the collision plane.
		Vec3 sphereOriginAfterAllowedMovement;
		Vec3::ScaleAndAdd(&sphereOriginAfterAllowedMovement, &sphere->position, movementDirection, out->distance);

		Vec3 collisionPlaneNormal;
		Vec3::Sub(&collisionPlaneNormal, &sphereOriginAfterAllowedMovement, &out->intersection);
		Vec3::Normalize(&collisionPlaneNormal, &collisionPlaneNormal);

		Plane::FromNormalAndPoint(&out->collisionPlane, &collisionPlaneNormal, &out->intersection);
	}

	return true;
}

bool Sphere::CheckIntersectsAABB(Sphere* sphere, AABB* aabb) 
{
	Vec3 nearestPointInAABB;
	AABB::ClampPoint(&nearestPointInAABB, &sphere->position, aabb);

	Vec3 nearestPointInAABBToSpherePosition;
	Vec3::Sub(&nearestPointInAABBToSpherePosition, &sphere->position, &nearestPointInAABB);

	float sphereRadiusSqr = sphere->radius * sphere->radius;
	float nearestPointInAABBToSpherePositionLengthSqr = Vec3::LengthSqr(&nearestPointInAABBToSpherePosition);

	return nearestPointInAABBToSpherePositionLengthSqr <= sphereRadiusSqr;
}