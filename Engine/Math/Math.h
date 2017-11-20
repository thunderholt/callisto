#pragma once

#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Math/Mat2.h"
#include "Math/Mat4.h"
#include "Math/Rect.h"
#include "Math/AABB.h"
#include "Math/Sphere.h"
#include "Math/Ray3.h"
#include "Math/Plane.h"
#include "Math/CollisionFace.h"
#include "Math/CollisionLine.h"
#include "Math/Colours.h"

class Math
{
public:
	static inline float Min(float f1, float f2)
	{
		return f1 < f2 ? f1 : f2;
	}

	static inline float Max(float f1, float f2)
	{
		return f1 > f2 ? f1 : f2;
	}

	static void CreateTransformFromRotation(Mat4* out, Vec3* rotation);
	static void CreateTransformFromPostionAndRotation(Mat4* out, Vec3* position, Vec3* rotation);
	static float RotateTowardsTargetAngle(float currentAngle, float targetAngle, float maxDelta);
	static float Lerp(float from, float to, float amount);
	static float GenerateRandomFloat();
	static float GenerateRandomFloat(float from, float to);
	static float CalculateTriangleArea(Vec2* points);
	static float CalculateTriangleArea(Vec2* v1, Vec2* v2, Vec2* v3);
	static float CalculateTriangleArea(Vec2* edge1, Vec2* edge2);
	static float CalculateTriangleArea(Vec3* points);
	static float CalculateTriangleArea(Vec3* v1, Vec3* v2, Vec3* v3);
	static float CalculateTriangleArea(Vec3* edge1, Vec3* edge2);
	static void CalculateBarycentricCoords(Vec3* out, Vec2* trianglePoints, Vec2* point);
	static void CalculateBarycentricCoords(Vec3* out, Vec3* trianglePoints, Vec3* point);
	static void BarycentricMix(Vec2* out, Vec2* values, Vec3* barycentricCoords);
	static void BarycentricMix(Vec3* out, Vec3* values, Vec3* barycentricCoords);
};