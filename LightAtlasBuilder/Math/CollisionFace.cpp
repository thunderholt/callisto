#include "Math/CollisionFace.h"
#include "Math/Ray3.h"

void CollisionFace::BuildFromPoints(CollisionFace* out, Vec3* points, Vec3* normals, Vec2* uvs)
{
	// Copy over the points;
	out->points[0] = points[0];
	out->points[1] = points[1];
	out->points[2] = points[2];

	// Copy over the normals.
	out->normals[0] = normals[0];
	out->normals[1] = normals[1];
	out->normals[2] = normals[2];

	// Copy over the uvs.
	out->uvs[0] = uvs[0];
	out->uvs[1] = uvs[1];
	out->uvs[2] = uvs[2];

	// Calculate face normal.
	Vec3 freeEdgeAB, freeEdgeAC, faceNormal;
	Vec3::Sub(&freeEdgeAB, &points[1], &points[0]);
	Vec3::Sub(&freeEdgeAC, &points[2], &points[0]);
	Vec3::Cross(&faceNormal, &freeEdgeAB, &freeEdgeAC);
	Vec3::Normalize(&faceNormal, &faceNormal);

	Plane::FromNormalAndPoint(&out->facePlane, &faceNormal, &points[0]);

	// Calculate the edge planes and edge lengths.
	Vec3::Sub(&out->freeNormalisedEdges[0], &points[1], &points[0]);
	out->edgeLengths[0] = Vec3::Length(&out->freeNormalisedEdges[0]);
	Vec3::Normalize(&out->freeNormalisedEdges[0], &out->freeNormalisedEdges[0]);

	Vec3::Sub(&out->freeNormalisedEdges[1], &points[2], &points[1]);
	out->edgeLengths[1] = Vec3::Length(&out->freeNormalisedEdges[1]);
	Vec3::Normalize(&out->freeNormalisedEdges[1], &out->freeNormalisedEdges[1]);

	Vec3::Sub(&out->freeNormalisedEdges[2], &points[0], &points[2]);
	out->edgeLengths[2] = Vec3::Length(&out->freeNormalisedEdges[2]);
	Vec3::Normalize(&out->freeNormalisedEdges[2], &out->freeNormalisedEdges[2]);

	Vec3 edgePlaneNormals[3];
	Vec3::Cross(&edgePlaneNormals[0], &out->freeNormalisedEdges[0], &faceNormal);
	Vec3::Cross(&edgePlaneNormals[1], &out->freeNormalisedEdges[1], &faceNormal);
	Vec3::Cross(&edgePlaneNormals[2], &out->freeNormalisedEdges[2], &faceNormal);

	Plane::FromNormalAndPoint(&out->edgePlanes[0], &edgePlaneNormals[0], &points[0]);
	Plane::FromNormalAndPoint(&out->edgePlanes[1], &edgePlaneNormals[1], &points[1]);
	Plane::FromNormalAndPoint(&out->edgePlanes[2], &edgePlaneNormals[2], &points[2]);
}

bool CollisionFace::DetermineIfPointOnFacePlaneIsWithinCollisionFace(CollisionFace* face, Vec3* point) 
{
	bool isWithin = false;

	if (Plane::CalculatePointDistance(&face->edgePlanes[0], point) < 0 &&
		Plane::CalculatePointDistance(&face->edgePlanes[1], point) < 0 &&
		Plane::CalculatePointDistance(&face->edgePlanes[2], point) < 0)
	{
		isWithin = true;
	}

	return isWithin;
}

void CollisionFace::FindNearestPointOnCollisionFacePerimeterToPoint(Vec3* out, CollisionFace* collisionFace, Vec3* point) 
{
	float nearestPointDistanceSqr = -1.0f;

	for (int i = 0; i < 3; i++) 
	{
		Ray3 ray;
		ray.origin = collisionFace->points[i];
		ray.normal = collisionFace->freeNormalisedEdges[i];

		Vec3 potentialNearestPoint;
		Ray3::CalculateNearestPointOnRayToOtherPoint(&potentialNearestPoint, &ray, point, collisionFace->edgeLengths[i]);

		float potentialNearestPointDistanceSqr = Vec3::DistanceSqr(point, &potentialNearestPoint);

		if (nearestPointDistanceSqr == -1.0f || potentialNearestPointDistanceSqr < nearestPointDistanceSqr) 
		{
			nearestPointDistanceSqr = potentialNearestPointDistanceSqr;
			*out = potentialNearestPoint;
		}
	}
}