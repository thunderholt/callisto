#pragma once
#include "Include/Callisto.h"

class UnitTester : public IUnitTester
{
public:
	bool RunUnitTests();

private:
	void SetTest(const char* testName);
	void Assert(const char* subTestName, bool result);
	bool AreNearlyEqual(float v1, float v2);
	bool AreNearlyEqual(Vec2 v1, Vec2 v2);
	bool AreNearlyEqual(Vec3 v1, Vec3 v2);

	void Test_Plane_FromPoints_LookingUpX();
	void Test_Plane_FromPoints_LookingDownX();
	void Test_Plane_FromPoints_LookingUpY();
	void Test_Plane_FromPoints_LookingDownY();
	void Test_Plane_FromPoints_LookingUpZ();
	void Test_Plane_FromPoints_LookingDownZ();
	void Test_Plane_CalculatePointDistance_LookingUpX_OnPlane();
	void Test_Plane_CalculatePointDistance_LookingUpX_InfrontOfPlane();
	void Test_Plane_CalculatePointDistance_LookingUpX_BehindPlane();
	void Test_Ray3_CalculateIntersectionWithSphere();
	void Test_Ray3_CalculateIntersectionWithPlane();
	void Test_Ray3_CalculateNearestPointOnRayToOtherPoint();
	void Test_Sphere_CalculateCollisionWithPlane_AngledPlane();
	void Test_Sphere_CalculateCollisionWithPlane_SquareOn();
	void Test_Sphere_CalculateCollisionWithCollisionFace_HitTest1();
	void Test_Sphere_CalculateCollisionWithCollisionFace_HitTest2();
	void Test_Sphere_CalculateCollisionWithCollisionFace_HitTest3();
	void Test_CollisionFace_FindNearestPointOnCollisionFacePerimeterToPoint();
	void Test_Mat4_FromQuat();
	void Test_Math_CalculateTriangleArea2D();
	void Test_Math_CalculateTriangleArea3D();
	void Test_Math_CalculateBarycentricCoords2D();
	void Test_Math_CalculateBarycentricCoords3D();
	void Test_Math_CalculateWorldPositionFromUV();
	void Test_Math_CalculateNormalWithinHemisphere();

	char testName[64];
	int numberOfFailedSubTests;
};