#include "Math/CollisionLine.h"
#include "Include/Common.h"

void CollisionLine::FromOwnFromAndToPoints(CollisionLine* out)
{
	out->ray.origin = out->from;

	Vec3::Sub(&out->ray.normal, &out->to, &out->from);
	out->length = Vec3::Length(&out->ray.normal);

	Vec3::Normalize(&out->ray.normal, &out->ray.normal);
}

FaceIntersectionType CollisionLine::CalculateIntersectionWithCollisionFace(Vec3* out, CollisionLine* line, CollisionFace* face) 
{
	FaceIntersectionType faceIntersectionType = FaceIntersectionTypeNone;

	bool isFrontSideCollision = Plane::CalculatePointDistance(&face->facePlane, &line->from) > 0.0f;

	Vec3 facePlaneIntersection;
	bool intersectionFound = Ray3::CalculateIntersectionWithPlane(&facePlaneIntersection, &line->ray, &face->facePlane);

	if (intersectionFound) 
	{
		if (CollisionFace::DetermineIfPointOnFacePlaneIsWithinCollisionFace(face, &facePlaneIntersection))
		{
			float distanceToFacePlaneIntersectionSqr = Vec3::DistanceSqr(&line->from, &facePlaneIntersection);

			float lineLengthSqr = line->length * line->length;

			if (distanceToFacePlaneIntersectionSqr <= lineLengthSqr)
			{
				if (out != null)
				{
					*out = facePlaneIntersection;
				}

				faceIntersectionType = isFrontSideCollision ? FaceIntersectionTypeFrontSide : FaceIntersectionTypeBackSide;
			}
		}
	}

	return faceIntersectionType;
}