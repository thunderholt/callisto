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
};