#include <stdlib.h>
#include "Math/Math.h"

int Math::GenerateRandomInt(int from, int to)
{
	float f = Math::GenerateRandomFloat();
	int i = from + (int)((to - from) * f);
	return i;
}

float Math::GenerateRandomFloat()
{
	return rand() / (float)RAND_MAX;
}

float Math::CalculateTriangleArea(Vec2* points)
{
	return Math::CalculateTriangleArea(&points[0], &points[1], &points[2]);
}

float Math::CalculateTriangleArea(Vec2* v1, Vec2* v2, Vec2* v3)
{
	Vec2 edge1, edge2;
	Vec2::Sub(&edge1, v2, v1);
	Vec2::Sub(&edge2, v3, v1);
	return Math::CalculateTriangleArea(&edge1, &edge2);
}

float Math::CalculateTriangleArea(Vec2* edge1, Vec2* edge2)
{
	Vec3 expandedEdge1, expandedEdge2;
	Vec3::Set(&expandedEdge1, edge1->x, edge1->y, 0.0f);
	Vec3::Set(&expandedEdge2, edge2->x, edge2->y, 0.0f);

	Vec3 cross;
	Vec3::Cross(&cross, &expandedEdge1, &expandedEdge2);
	return Vec3::Length(&cross) / 2.0f;
}

float Math::CalculateTriangleArea(Vec3* points)
{
	return Math::CalculateTriangleArea(&points[0], &points[1], &points[2]);
}

float Math::CalculateTriangleArea(Vec3* v1, Vec3* v2, Vec3* v3)
{
	Vec3 edge1, edge2;
	Vec3::Sub(&edge1, v2, v1);
	Vec3::Sub(&edge2, v3, v1);
	return Math::CalculateTriangleArea(&edge1, &edge2);
}

float Math::CalculateTriangleArea(Vec3* edge1, Vec3* edge2)
{
	Vec3 cross;
	Vec3::Cross(&cross, edge1, edge2);
	return Vec3::Length(&cross) / 2.0f;
}

void Math::CalculateBarycentricCoords(Vec3* out, Vec2* trianglePoints, Vec2* point)
{
	float areaABC = Math::CalculateTriangleArea(trianglePoints);
	float areaBCP = Math::CalculateTriangleArea(&trianglePoints[1], &trianglePoints[2], point);
	float areaCAP = Math::CalculateTriangleArea(&trianglePoints[2], &trianglePoints[0], point);
	float areaABP = Math::CalculateTriangleArea(&trianglePoints[0], &trianglePoints[1], point);

	out->x = areaBCP / areaABC;
	out->y = areaCAP / areaABC;
	out->z = areaABP / areaABC;
}

void Math::CalculateBarycentricCoords(Vec3* out, Vec3* trianglePoints, Vec3* point)
{
	/*float areaABC = Math::CalculateTriangleArea(trianglePoints);
	float areaBCP = Math::CalculateTriangleArea(&trianglePoints[1], &trianglePoints[2], point);
	float areaCAP = Math::CalculateTriangleArea(&trianglePoints[2], &trianglePoints[0], point);

	out->x = areaBCP / areaABC;
	out->y = areaCAP / areaABC;
	out->z = 1.0f - out->x - out->y;*/
	/*float areaABC = Math::CalculateTriangleArea(trianglePoints);
	float areaPBC = Math::CalculateTriangleArea(point, &trianglePoints[1], &trianglePoints[2]);
	float areaPCA = Math::CalculateTriangleArea(point, &trianglePoints[2], &trianglePoints[0]);

	out->x = areaPBC / areaABC;
	out->y = areaPCA / areaABC;
	out->z = 1.0f - out->x - out->y;*/
	float areaABC = Math::CalculateTriangleArea(trianglePoints);
	float areaBCP = Math::CalculateTriangleArea(&trianglePoints[1], &trianglePoints[2], point);
	float areaCAP = Math::CalculateTriangleArea(&trianglePoints[2], &trianglePoints[0], point);
	float areaABP = Math::CalculateTriangleArea(&trianglePoints[0], &trianglePoints[1], point);

	out->x = areaBCP / areaABC;
	out->y = areaCAP / areaABC;
	out->z = areaABP / areaABC;
}

void Math::BarycentricMix(Vec2* out, Vec2* values, Vec3* barycentricCoords)
{
	Vec2::Scale(out, &values[0], barycentricCoords->x);
	Vec2::ScaleAndAdd(out, out, &values[1], barycentricCoords->y);
	Vec2::ScaleAndAdd(out, out, &values[2], barycentricCoords->z);
}

void Math::BarycentricMix(Vec3* out, Vec3* values, Vec3* barycentricCoords)
{
	Vec3::Scale(out, &values[0], barycentricCoords->x);
	Vec3::ScaleAndAdd(out, out, &values[1], barycentricCoords->y);
	Vec3::ScaleAndAdd(out, out, &values[2], barycentricCoords->z);
}

bool Math::CheckBarycentricCoordsAreWithinTriangle(Vec3* barycentricCoords)
{
	float sum = barycentricCoords->x + barycentricCoords->y + barycentricCoords->z;
	float epsilon = 0.0001f;

	return sum >= 1.0f - epsilon && sum <= 1.0f + epsilon;
	/*barycentricCoords->x >= 0.0f && barycentricCoords->x <= 1.0f &&
	barycentricCoords->y >= 0.0f && barycentricCoords->y <= 1.0f &&
	barycentricCoords->z >= 0.0f && barycentricCoords->z <= 1.0f;*/
}

bool Math::CalculateWorldPositionAndNormalFromUV(Vec3* outWorldPosition, Vec3* outNormal, Vec3* positions, Vec3* normals, Vec2* uvs, Vec2* uv)
{
	Vec3 barycentricCoords;
	Math::CalculateBarycentricCoords(&barycentricCoords, uvs, uv);

	bool uvIsWithinFace = Math::CheckBarycentricCoordsAreWithinTriangle(&barycentricCoords);
	if (uvIsWithinFace)
	{
		Math::BarycentricMix(outWorldPosition, positions, &barycentricCoords);
		Math::BarycentricMix(outNormal, normals, &barycentricCoords);
		Vec3::Normalize(outNormal, outNormal);
	}

	return uvIsWithinFace;
}

float Math::Clamp(float f, float min, float max)
{
	if (f < min)
	{
		f = min;
	}
	else if (f > max)
	{
		f = max;
	}

	return f;
}

float Math::Lerp(float lerpFactor, float from, float to)
{
	return from + (to - from) * lerpFactor;
}

float Math::InverseLerp(float lerpedValue, float from, float to)
{
	return (lerpedValue - from) / (to - from);
}

float Math::Max(float f1, float f2)
{
	return f1 > f2 ? f1 : f2;
}