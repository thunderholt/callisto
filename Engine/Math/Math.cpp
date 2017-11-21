#include <stdlib.h>
#include "Math/Math.h"

void Math::CreateTransformFromRotation(Mat4* out, Vec3* rotation)
{
	Mat4 xRotationTransform, yRotationTransform, zRotationTransform;

	Mat4::CreateZRotation(&zRotationTransform, rotation->z);
	Mat4::CreateYRotation(&yRotationTransform, rotation->y);
	Mat4::CreateXRotation(&xRotationTransform, rotation->x);

	Mat4::Multiply(out, &yRotationTransform, &zRotationTransform);
	Mat4::Multiply(out, out, &xRotationTransform);
}

void Math::CreateTransformFromPostionAndRotation(Mat4* out, Vec3* position, Vec3* rotation)
{
	Math::CreateTransformFromRotation(out, rotation);
	Mat4::Translate(out, position);
}

float Math::RotateTowardsTargetAngle(float currentAngle, float targetAngle, float maxDelta)
{
	// TODO - fix twirly problem.

	if (currentAngle > targetAngle) 
	{
		currentAngle -= maxDelta;
		currentAngle = Math::Max(currentAngle, targetAngle);
	}
	else if (currentAngle < targetAngle) 
	{
		currentAngle += maxDelta;
		currentAngle = Math::Min(currentAngle, targetAngle);
	}

	return currentAngle;
}

float Math::Lerp(float from, float to, float amount)
{
	return from + (to * 2.0f * amount);
}

float Math::GenerateRandomFloat()
{
	return rand() / (float)RAND_MAX;
}

float Math::GenerateRandomFloat(float from, float to)
{
	float f = Math::GenerateRandomFloat();
	return Math::Lerp(from, to, f);
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
	float epsilon = 0.0000001f;

	return sum >= 1.0f - epsilon && sum <= 1.0f + epsilon;
		/*barycentricCoords->x >= 0.0f && barycentricCoords->x <= 1.0f &&
		barycentricCoords->y >= 0.0f && barycentricCoords->y <= 1.0f &&
		barycentricCoords->z >= 0.0f && barycentricCoords->z <= 1.0f;*/
}

bool Math::CalculateWorldPositionFromUV(Vec3* out, Vec3* positions, Vec2* uvs, Vec2* uv)
{
	Vec3 barycentricCoords;
	Math::CalculateBarycentricCoords(&barycentricCoords, uvs, uv);

	bool uvIsWithinFace = Math::CheckBarycentricCoordsAreWithinTriangle(&barycentricCoords);
	if (uvIsWithinFace)
	{
		Math::BarycentricMix(out, positions, &barycentricCoords);
	}

	return uvIsWithinFace;
}