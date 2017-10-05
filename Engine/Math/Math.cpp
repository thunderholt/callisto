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