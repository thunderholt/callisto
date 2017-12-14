#pragma once
#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Math/Plane.h"

enum FaceIntersectionType
{
	FaceIntersectionTypeNone = 0,
	FaceIntersectionTypeFrontSide = 1,
	FaceIntersectionTypeBackSide = 2
};

struct CollisionFace
{
	Vec3 points[3];
	Vec3 normals[3];
	Vec2 materialUVs[3];
	Vec2 lightAtlasUVs[3];
	Plane facePlane;
	Plane edgePlanes[3];
	Vec3 freeNormalisedEdges[3];
	float edgeLengths[3];

	static void BuildFromPoints(CollisionFace* out, Vec3* points, Vec3* normals, Vec2* materialUVs, Vec2* lightAtlasUVs);
	static bool DetermineIfPointOnFacePlaneIsWithinCollisionFace(CollisionFace* face, Vec3* point);
	static void FindNearestPointOnCollisionFacePerimeterToPoint(Vec3* out, CollisionFace* collisionFace, Vec3* point);
};

struct FaceCollisionResult
{
	Vec3 intersection;
	float distance;
	Plane collisionPlane;
};