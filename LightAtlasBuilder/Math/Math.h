#pragma once

#include <assert.h>

#include "Math/Vec2.h"
#include "Math/Vec2i.h"
#include "Math/Vec3.h"
#include "Math/Vec3i.h"
#include "Math/Ray3.h"
#include "Math/Plane.h"
#include "Math/AABB.h"
#include "Math/Sphere.h"
#include "Math/Quat.h"
#include "Math/Mat4.h"
#include "Math/CollisionFace.h"
#include "Math/CollisionLine.h"
#include "Math/Colours.h"

#define PI 3.14159265358979323846f
#define NormalWithinHemisphereCalculationMaxCircles 128

struct NormalWithinHemisphereCalculationMetrics
{
	int numberOfCircles;
	float maxCircleRotation;
	int segmentCountsByCircleIndex[NormalWithinHemisphereCalculationMaxCircles];
	float segmentStepsByCircleIndex[NormalWithinHemisphereCalculationMaxCircles];
	int outputNormalCount;
};

class Math
{
public:
	static inline bool AreAlmostEqual(float f1, float f2, float epsilon)
	{
		return f1 >= f2 - epsilon && f1 <= f2 + epsilon;
	}

	static int GenerateRandomInt(int from, int to);
	static float GenerateRandomFloat();
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
	static bool CheckBarycentricCoordsAreWithinTriangle(Vec3* barycentricCoords);
	static bool CalculateWorldPositionAndNormalFromUV(Vec3* outWorldPosition, Vec3* outNormal, Vec3* positions, Vec3* normals, Vec2* uvs, Vec2* uv);
	static float Clamp(float f, float min, float max);
	static float Lerp(float lerpFactor, float from, float to);
	static float InverseLerp(float lerpedValue, float from, float to);
	static float Max(float f1, float f2);
	static void BuildNormalWithinHemisphereCalculationMetrics(NormalWithinHemisphereCalculationMetrics* out, int numberOfCircles, float maxCircleRotation, int numberOfOutputNormals);
	static void CalculateNormalWithinHemisphere(Vec3* out, Vec3* normal, Vec3* binormal, NormalWithinHemisphereCalculationMetrics* metrics, int circleIndex, int segmentIndex);
	//static void CalculateNormalWithinHemisphere(Vec3* out, Vec3* normal, Vec3* binormal, int numberOfCircles, int numberOfSegments, int circleNumber, int segmentNumber, float maxCircleRotation);
};