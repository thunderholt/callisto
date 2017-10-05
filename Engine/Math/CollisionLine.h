#pragma once
#include "Math/Vec3.h"
#include "Math/Ray3.h"
#include "Math/CollisionFace.h"

struct CollisionLine
{
	Vec3 from;
	Vec3 to;
	Ray3 ray;
	float length;

	static void FromOwnFromAndToPoints(CollisionLine* out);
	static FaceIntersectionType CalculateIntersectionWithCollisionFace(Vec3* out, CollisionLine* line, CollisionFace* face);
	static void TransformMat4(CollisionLine* out, const CollisionLine* line, const Mat4* transform);
};