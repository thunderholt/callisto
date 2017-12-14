#include "Core/UnitTester.h"

bool UnitTester::RunUnitTests()
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	logger->Write("Running unit tests...");

	strcpy(this->testName, "");
	this->numberOfFailedSubTests = 0;

	this->Test_Plane_FromPoints_LookingUpX();
	this->Test_Plane_FromPoints_LookingDownX();
	this->Test_Plane_FromPoints_LookingUpY();
	this->Test_Plane_FromPoints_LookingDownY();
	this->Test_Plane_FromPoints_LookingUpZ();
	this->Test_Plane_FromPoints_LookingDownZ();
	this->Test_Plane_CalculatePointDistance_LookingUpX_OnPlane();
	this->Test_Plane_CalculatePointDistance_LookingUpX_InfrontOfPlane();
	this->Test_Plane_CalculatePointDistance_LookingUpX_BehindPlane();
	this->Test_Ray3_CalculateIntersectionWithSphere();
	this->Test_Ray3_CalculateIntersectionWithPlane();
	this->Test_Ray3_CalculateNearestPointOnRayToOtherPoint();
	this->Test_Sphere_CalculateCollisionWithPlane_AngledPlane();
	this->Test_Sphere_CalculateCollisionWithPlane_SquareOn();
	this->Test_Sphere_CalculateCollisionWithCollisionFace_HitTest1();
	this->Test_Sphere_CalculateCollisionWithCollisionFace_HitTest2();
	this->Test_Sphere_CalculateCollisionWithCollisionFace_HitTest3();
	this->Test_CollisionFace_FindNearestPointOnCollisionFacePerimeterToPoint();
	this->Test_Mat4_FromQuat();
	this->Test_Math_CalculateTriangleArea2D();
	this->Test_Math_CalculateTriangleArea3D();
	this->Test_Math_CalculateBarycentricCoords2D();
	this->Test_Math_CalculateBarycentricCoords3D();
	this->Test_Math_CalculateWorldPositionFromUV();
	//this->Test_Math_CalculateNormalWithinHemisphere();

	if (this->numberOfFailedSubTests == 0)
	{
		logger->Write("... all unit tests passed.");
	}
	else
	{
		logger->Write("... %d unit tests failed.", this->numberOfFailedSubTests);
	}

	return this->numberOfFailedSubTests == 0;
}

void UnitTester::SetTest(const char* testName)
{
	strcpy(this->testName, testName);
}

void UnitTester::Assert(const char* subTestName, bool result)
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	if (result)
	{
		logger->Write("Unit Test -> %s #%s: Passed", this->testName, subTestName);
	}
	else
	{
		logger->Write("Unit Test -> %s #%s: Failed", this->testName, subTestName);
		this->numberOfFailedSubTests++;
	}
}

bool UnitTester::AreNearlyEqual(float v1, float v2) 
{
	float epsilon = 0.005f;
	return (v1 > (v2 - epsilon)) && (v1 < (v2 + epsilon));
}

bool UnitTester::AreNearlyEqual(Vec2 v1, Vec2 v2)
{
	float epsilon = 0.005f;
	return UnitTester::AreNearlyEqual(v1.x, v2.x) && UnitTester::AreNearlyEqual(v1.y, v2.y);
}

bool UnitTester::AreNearlyEqual(Vec3 v1, Vec3 v2)
{
	float epsilon = 0.005f;
	return UnitTester::AreNearlyEqual(v1.x, v2.x) && UnitTester::AreNearlyEqual(v1.y, v2.y) && UnitTester::AreNearlyEqual(v1.z, v2.z);
}

void UnitTester::Test_Plane_FromPoints_LookingUpX() 
{
	this->SetTest("Plane::FromPoints<LookingUpX>");

	// Sub test 1.
	Vec3 points[3];
	Vec3::Set(&points[0], 0, 1, 0);
	Vec3::Set(&points[1], 0, 0, 1);
	Vec3::Set(&points[2], 0, 0, 0);

	Plane plane;
	Plane::FromPoints(&plane, points);

	this->Assert("1.1", plane.normal.x == 1);
	this->Assert("1.2", plane.normal.y == 0);
	this->Assert("1.3", plane.normal.z == 0);
	this->Assert("1.4", plane.d == 0);

	// Sub test 2.
	Vec3::Set(&points[0], 1, 1, 0);
	Vec3::Set(&points[1], 1, 0, 1);
	Vec3::Set(&points[2], 1, 0, 0);

	Plane::FromPoints(&plane, points);

	this->Assert("2.1", plane.normal.x == 1);
	this->Assert("2.1", plane.normal.y == 0);
	this->Assert("2.1", plane.normal.z == 0);
	this->Assert("2.1", plane.d == -1);

	// Sub test 3.
	Vec3::Set(&points[0], -1, 1, 0);
	Vec3::Set(&points[1], -1, 0, 1);
	Vec3::Set(&points[2], -1, 0, 0);

	Plane::FromPoints(&plane, points);

	this->Assert("3.1", plane.normal.x == 1);
	this->Assert("3.2", plane.normal.y == 0);
	this->Assert("3.3", plane.normal.z == 0);
	this->Assert("3.4", plane.d == 1);
}

void UnitTester::Test_Plane_FromPoints_LookingDownX()
{
	this->SetTest("Plane::FromPoints<LookingDownX>");

	// Sub test 1.
	Vec3 points[3];
	Vec3::Set(&points[0], 0, 1, 0);
	Vec3::Set(&points[1], 0, 0, 0);
	Vec3::Set(&points[2], 0, 0, 1);

	Plane plane;
	Plane::FromPoints(&plane, points);

	this->Assert("1.1", plane.normal.x == -1);
	this->Assert("1.2", plane.normal.y == 0);
	this->Assert("1.3", plane.normal.z == 0);
	this->Assert("1.4", plane.d == 0);
}

void UnitTester::Test_Plane_FromPoints_LookingUpY() 
{
	this->SetTest("Plane::FromPoints<LookingUpY>");

	// Sub test 1.
	Vec3 points[3];
	Vec3::Set(&points[0], -1, -10, 0);
	Vec3::Set(&points[1], -1, -10, 1);
	Vec3::Set(&points[2], 1, -10, 0);

	Plane plane;
	Plane::FromPoints(&plane, points);

	this->Assert("1.1", plane.normal.x == 0);
	this->Assert("1.2", plane.normal.y == 1);
	this->Assert("1.3", plane.normal.z == 0);
	this->Assert("1.4", plane.d == 10);
}

void UnitTester::Test_Plane_FromPoints_LookingDownY()
{
	this->SetTest("Plane::FromPoints<ookingDownY>");

	// Sub test 1.
	Vec3 points[3];
	Vec3::Set(&points[0], -1, -10, 0);
	Vec3::Set(&points[1], 1, -10, 0);
	Vec3::Set(&points[2], -1, -10, 1);

	Plane plane;
	Plane::FromPoints(&plane, points);

	this->Assert("1.1", plane.normal.x == 0);
	this->Assert("1.2", plane.normal.y == -1);
	this->Assert("1.3", plane.normal.z == 0);
	this->Assert("1.4", plane.d == -10);
}

void UnitTester::Test_Plane_FromPoints_LookingUpZ()
{
	this->SetTest("Plane::FromPoints<LookingUpZ>");

	// Sub test 1.
	Vec3 points[3];
	Vec3::Set(&points[0], 0, 1, 1);
	Vec3::Set(&points[1], 0, 0, 1);
	Vec3::Set(&points[2], 1, 0, 1);

	Plane plane;
	Plane::FromPoints(&plane, points);

	this->Assert("1.1", plane.normal.x == 0);
	this->Assert("1.2", plane.normal.y == 0);
	this->Assert("1.3", plane.normal.z == 1);
	this->Assert("1.4", plane.d == -1);
}

void UnitTester::Test_Plane_FromPoints_LookingDownZ() 
{
	this->SetTest("Plane::FromPoints<LookingDownZ>");

	// Sub test 1.
	Vec3 points[3];
	Vec3::Set(&points[0], 0, 1, 1);
	Vec3::Set(&points[1], 1, 0, 1);
	Vec3::Set(&points[2], 0, 0, 1);

	Plane plane;
	Plane::FromPoints(&plane, points);

	this->Assert("1.1", plane.normal.x == 0);
	this->Assert("1.2", plane.normal.y == 0);
	this->Assert("1.3", plane.normal.z == -1);
	this->Assert("1.4", plane.d == 1);
}

void UnitTester::Test_Plane_CalculatePointDistance_LookingUpX_OnPlane()
{
	this->SetTest("Plane::CalculatePointDistance<LookingUpX:OnPlane>");

	// Sub test 1.
	Vec3 points[3];
	Vec3::Set(&points[0], 0, 1, 0);
	Vec3::Set(&points[1], 0, 0, 1);
	Vec3::Set(&points[2], 0, 0, 0);

	Plane plane;
	Plane::FromPoints(&plane, points);

	Vec3 point = Vec3::Create(0, 0, 0);
	float distance = Plane::CalculatePointDistance(&plane, &point);
	this->Assert("1.1", distance == 0);
}

void UnitTester::Test_Plane_CalculatePointDistance_LookingUpX_InfrontOfPlane() 
{
	this->SetTest("Plane::CalculatePointDistance<LookingUpX:InfrontOfPlane>");

	// Sub test 1.
	Vec3 points[3];
	Vec3::Set(&points[0], 0, 1, 0);
	Vec3::Set(&points[1], 0, 0, 1);
	Vec3::Set(&points[2], 0, 0, 0);

	Plane plane;
	Plane::FromPoints(&plane, points);

	Vec3 point = Vec3::Create(1.5, 0, 0);
	float distance = Plane::CalculatePointDistance(&plane, &point);
	this->Assert("1.1", distance == 1.5);

	// Sub test 2.
	Vec3::Set(&points[0], 1, 1, 0);
	Vec3::Set(&points[1], 1, 0, 1);
	Vec3::Set(&points[2], 1, 0, 0);

	Plane::FromPoints(&plane, points);

	point = Vec3::Create(1.5, 0, 0);
	distance = Plane::CalculatePointDistance(&plane, &point);
	this->Assert("2.1", distance == 0.5);
}

void UnitTester::Test_Plane_CalculatePointDistance_LookingUpX_BehindPlane()
{
	this->SetTest("Plane::CalculatePointDistance<LookingUpX:BehindPlane>");

	// Sub test 1.
	Vec3 points[3];
	Vec3::Set(&points[0], 0, 1, 0);
	Vec3::Set(&points[1], 0, 0, 1);
	Vec3::Set(&points[2], 0, 0, 0);

	Plane plane;
	Plane::FromPoints(&plane, points);

	Vec3 point = Vec3::Create(-3.7f, 0, 0);
	float distance = Plane::CalculatePointDistance(&plane, &point);
	this->Assert("1.1", distance == -3.7f);

	// Sub test 2.
	Vec3::Set(&points[0], -2, 1, 0);
	Vec3::Set(&points[1], -2, 0, 1);
	Vec3::Set(&points[2], -2, 0, 0);

	Plane::FromPoints(&plane, points);

	point = Vec3::Create(-3, 0, 0);
	distance = Plane::CalculatePointDistance(&plane, &point);
	this->Assert("2.1", distance == -1);
}

void UnitTester::Test_Ray3_CalculateIntersectionWithSphere()
{
	this->SetTest("Ray3::CalculateIntersectionWithSphere");

	// Sub test 1.
	Sphere sphere = Sphere::Create(2, 3, -10, 1);

	Ray3 ray = Ray3::Create(4, 3, -10, -1, 0, 0);

	Vec3 result;

	Ray3::CalculateIntersectionWithSphere(&result, &ray, &sphere);

	this->Assert("1.1", result.x == 3);
	this->Assert("1.2", result.y == 3);
	this->Assert("1.3", result.z == -10);

	// Sub test 2.
	ray.origin = Vec3::Create(2, 5, -10);
	ray.normal = Vec3::Create(0, -1, 0);

	Ray3::CalculateIntersectionWithSphere(&result, &ray, &sphere);

	this->Assert("2.1", result.x == 2);
	this->Assert("2.2", result.y == 4);
	this->Assert("2.3", result.z == -10);

	// Sub test 3.
	ray.origin = Vec3::Create(2, 3, -8);
	ray.normal = Vec3::Create(0, 0, -1);

	Ray3::CalculateIntersectionWithSphere(&result, &ray, &sphere);

	this->Assert("3.1", result.x == 2);
	this->Assert("3.2", result.y == 3);
	this->Assert("3.3", result.z == -9);

	// Sub test 4.
	ray.origin = Vec3::Create(3, 2, -10);
	ray.normal = Vec3::Create(1, 0, 0);

	this->Assert("4.1", !Ray3::CalculateIntersectionWithSphere(&result, &ray, &sphere));

	// Sub test 5.
	ray.origin = Vec3::Create(2, 3, -10);
	ray.normal = Vec3::Create(0, 1, 0);

	Ray3::CalculateIntersectionWithSphere(&result, &ray, &sphere);

	this->Assert("5.1", result.x == 2);
	this->Assert("5.2", result.y == 4);
	this->Assert("5.3", result.z == -10);

	// Sub test 6.
	ray.origin = Vec3::Create(2, 4, -10);
	ray.normal = Vec3::Create(0, 1, 0);

	Ray3::CalculateIntersectionWithSphere(&result, &ray, &sphere);

	this->Assert("6.1", result.x == 2);
	this->Assert("6.2", result.y == 4);
	this->Assert("6.3", result.z == -10);
}

void UnitTester::Test_Ray3_CalculateIntersectionWithPlane()
{
	this->SetTest("Ray3::CalculateIntersectionWithPlane");

	// Sub test 1.
	Plane plane;
	Vec3 normal = Vec3::Create(1, 0, 0);
	Vec3 point = Vec3::Create(3, 0, 0);
	Plane::FromNormalAndPoint(&plane, &normal, &point);

	Ray3 ray = Ray3::Create(10, 5, 3, -1, 0, 0);

	Vec3 result;
	bool intersectionFound = Ray3::CalculateIntersectionWithPlane(&result, &ray, &plane);
	this->Assert("1.1", intersectionFound);
	this->Assert("1.2", result.x == 3);
	this->Assert("1.3", result.y == 5);
	this->Assert("1.4", result.z == 3);

	// Sub test 2.
	ray = Ray3::Create(2, 5, 3, -1, 0, 0);
	intersectionFound = Ray3::CalculateIntersectionWithPlane(&result, &ray, &plane);
	this->Assert("2.1", !intersectionFound);

	// Sub test 3.
	ray = Ray3::Create(0, 5, 3, 1, 0, 0);
	intersectionFound = Ray3::CalculateIntersectionWithPlane(&result, &ray, &plane);
	this->Assert("3.1", intersectionFound);
	this->Assert("3.2", result.x == 3);
	this->Assert("3.3", result.y == 5);
	this->Assert("3.4", result.z == 3);
}


void UnitTester::Test_Ray3_CalculateNearestPointOnRayToOtherPoint()
{
	this->SetTest("Ray3::CalculateNearestPointOnRayToOtherPoint");

	// Sub test 1.
	Ray3 ray = Ray3::Create(5, 0, 0, 0, 1, 0);
	Vec3 point = Vec3::Create(2, 3, 0);

	Vec3 result;

	Ray3::CalculateNearestPointOnRayToOtherPoint(&result, &ray, &point, 10);

	this->Assert("1.1", result.x == 5);
	this->Assert("1.2", result.y == 3);
	this->Assert("1.3", result.z == 0);

	// Sub test 2.
	ray = Ray3::Create(5, 0, 0, 0, 1, 0);
	point = Vec3::Create(4, 10, 0);

	Ray3::CalculateNearestPointOnRayToOtherPoint(&result, &ray, &point, 10);

	this->Assert("2.1", result.x == 5);
	this->Assert("2.2", result.y == 10);
	this->Assert("2.3", result.z == 0);

	// Sub test 3.
	ray = Ray3::Create(5, 1, 0, 0, 1, 0);
	point = Vec3::Create(4, 0, 0);

	Ray3::CalculateNearestPointOnRayToOtherPoint(&result, &ray, &point, 10);

	this->Assert("3.1", result.x == 5);
	this->Assert("3.2", result.y == 1);
	this->Assert("3.3", result.z == 0);

	// Sub test 4.
	ray = Ray3::Create(5, 0, 0, 0, 1, 0);
	point = Vec3::Create(4, 11, 0);

	Ray3::CalculateNearestPointOnRayToOtherPoint(&result, &ray, &point, 10);

	this->Assert("4.1", result.x == 5);
	this->Assert("4.2", result.y == 10);
	this->Assert("4.3", result.z == 0);

	// Sub test 4.
	ray = Ray3::Create(5, 0, 0, 0, 1, 0);
	point = Vec3::Create(6, 5, 0);

	Ray3::CalculateNearestPointOnRayToOtherPoint(&result, &ray, &point, 10);

	this->Assert("5.1", result.x == 5);
	this->Assert("5.2", result.y == 5);
	this->Assert("5.3", result.z == 0);
}

void UnitTester::Test_Sphere_CalculateCollisionWithPlane_AngledPlane() 
{
	this->SetTest("Sphere::CalculateCollisionWithPlane<AngledPlane>");

	Vec3 planeNormal = Vec3::Create(-1, 1, 0);
	Vec3::Normalize(&planeNormal, &planeNormal);

	Plane plane;
	Vec3 point = Vec3::Create(2, 0, 0);
	Plane::FromNormalAndPoint(&plane, &planeNormal, &point);

	Sphere sphere = Sphere::Create(1, 2, 0, 1);

	// Sub test 1.
	Vec3 result;
	Vec3 sphereMovementNormal = Vec3::Create(1, 0, 0);
	Sphere::CalculateCollisionWithPlane(&result, &sphere, &plane, &sphereMovementNormal);

	this->Assert("1.1", this->AreNearlyEqual(result.x, 3.29289f));
	this->Assert("1.2", this->AreNearlyEqual(result.y, 1.29289f));
	this->Assert("1.3", result.z == 0.0f);

	// Sub test 2.
	sphereMovementNormal = Vec3::Create(-1, 0, 0);
	this->Assert("2.1", !Sphere::CalculateCollisionWithPlane(&result, &sphere, &plane, &sphereMovementNormal));

	// Sub test 3.
	sphereMovementNormal = Vec3::Create(0, 0, 1);
	this->Assert("3.1", !Sphere::CalculateCollisionWithPlane(&result, &sphere, &plane, &sphereMovementNormal));

	// Sub test 4.
	sphereMovementNormal = Vec3::Create(0, 0, -1);
	this->Assert("4.1", !Sphere::CalculateCollisionWithPlane(&result, &sphere, &plane, &sphereMovementNormal));
}

void UnitTester::Test_Sphere_CalculateCollisionWithPlane_SquareOn()
{
	this->SetTest("Sphere::CalculateCollisionWithPlane<SquareOn>");

	Vec3 planeNormal = Vec3::Create(-1, 0, 0);
	Vec3::Normalize(&planeNormal, &planeNormal);

	Plane plane;
	Vec3 point = Vec3::Create(3, 0, 0);
	Plane::FromNormalAndPoint(&plane, &planeNormal, &point);

	Sphere sphere = Sphere::Create(1, 2, 0, 1);

	// Sub test 1.
	Vec3 result;
	Vec3 sphereMovementNormal = Vec3::Create(1, 0, 0);
	Sphere::CalculateCollisionWithPlane(&result, &sphere, &plane, &sphereMovementNormal);

	this->Assert("1.1", this->AreNearlyEqual(result.x, 3));
	this->Assert("1.2", this->AreNearlyEqual(result.y, 2));
	this->Assert("1.3", result.z == 0);

	// Sub test 2.
	sphereMovementNormal = Vec3::Create(-1, 0, 0);
	this->Assert("2.1", !Sphere::CalculateCollisionWithPlane(&result, &sphere, &plane, &sphereMovementNormal));

	// Sub test 3.
	sphereMovementNormal = Vec3::Create(0, 0, 1);
	this->Assert("3.1", !Sphere::CalculateCollisionWithPlane(&result, &sphere, &plane, &sphereMovementNormal));

	// Sub test 4.
	sphereMovementNormal = Vec3::Create(0, 0, -1);
	this->Assert("4.1", !Sphere::CalculateCollisionWithPlane(&result, &sphere, &plane, &sphereMovementNormal));
}

void UnitTester::Test_Sphere_CalculateCollisionWithCollisionFace_HitTest1()
{
	this->SetTest("Sphere::CalculateCollisionWithCollisionFace<HitTest1>");

	// * Sphere is infront of face-plane.
	// * Heading towards face-plane.
	// * Face-plane intersection point is within face.

	Vec3 points[3];
	Vec3::Set(&points[0], 2, 4, 0);
	Vec3::Set(&points[1], 2, 1, 0);
	Vec3::Set(&points[2], 4, 1, 0);

	CollisionFace collisionFace;
	CollisionFace::BuildFromPoints(&collisionFace, points);

	Sphere sphere = Sphere::Create(3, 2, 10, 1);

	FaceCollisionResult result;
	Vec3 movementDirection = Vec3::Create(0, 0, -1);
	Sphere::CalculateCollisionWithCollisionFace(&result, &sphere, &collisionFace, &movementDirection);

	this->Assert("1.1", result.intersection.x == 3);
	this->Assert("1.2", result.intersection.y == 2);
	this->Assert("1.3", result.intersection.z == 0);
	this->Assert("1.4", result.distance == 9);
}

void UnitTester::Test_Sphere_CalculateCollisionWithCollisionFace_HitTest2()
{
	this->SetTest("Sphere::CalculateCollisionWithCollisionFace<HitTest2>");

	// * Sphere is infront of face-plane.
	// * Heading towards face-plane.
	// * Face-plane intersection point is outside of face.
	// * Sphere/face intersection point is on face edge.

	Vec3 points[3];
	Vec3::Set(&points[0], 2, 4, 0);
	Vec3::Set(&points[1], 2, 1, 0);
	Vec3::Set(&points[2], 4, 1, 0);

	CollisionFace collisionFace;
	CollisionFace::BuildFromPoints(&collisionFace, points);

	Sphere sphere = Sphere::Create(1.5, 2, 10, 1);

	FaceCollisionResult result;
	Vec3 movementDirection = Vec3::Create(0, 0, -1);
	Sphere::CalculateCollisionWithCollisionFace(&result, &sphere, &collisionFace, &movementDirection);

	this->Assert("1.1", result.intersection.x == 2);
	this->Assert("1.2", result.intersection.y == 2);
	this->Assert("1.3", result.intersection.z == 0);
	this->Assert("1.4", result.distance > 9 && result.distance < 10);
}

void UnitTester::Test_Sphere_CalculateCollisionWithCollisionFace_HitTest3()
{
	this->SetTest("Sphere::CalculateCollisionWithCollisionFace<HitTest3>");

	// * Sphere intersects face-plane.
	// * Heading towards face-plane.
	// * Sphere/face intersection point is on face edge.

	Vec3 points[3];
	Vec3::Set(&points[0], 2, 4, 0);
	Vec3::Set(&points[1], 2, 1, 0);
	Vec3::Set(&points[2], 4, 1, 0);

	CollisionFace collisionFace;
	CollisionFace::BuildFromPoints(&collisionFace, points);

	Sphere sphere = Sphere::Create(1.5f, 2, 0.9f, 1);

	FaceCollisionResult result;
	Vec3 movementDirection = Vec3::Create(0, 0, -1);
	Sphere::CalculateCollisionWithCollisionFace(&result, &sphere, &collisionFace, &movementDirection);

	this->Assert("1.1", result.intersection.x == 2);
	this->Assert("1.2", result.intersection.y == 2);
	this->Assert("1.3", result.intersection.z == 0);
	this->Assert("1.4", result.distance > 0 && result.distance < 1);
}

void UnitTester::Test_CollisionFace_FindNearestPointOnCollisionFacePerimeterToPoint()
{
	this->SetTest("CollisionFace::FindNearestPointOnCollisionFacePerimeterToPoint");

	Vec3 points[3];
	Vec3::Set(&points[0], 0, 10, 3);
	Vec3::Set(&points[1], 0, 0, 3);
	Vec3::Set(&points[2], 5, 0, 3);

	CollisionFace face;
	CollisionFace::BuildFromPoints(&face, points);

	// Sub test 1.
	Vec3 result;
	Vec3 point;

	Vec3::Set(&point, -1, 5, 0);
	CollisionFace::FindNearestPointOnCollisionFacePerimeterToPoint(&result, &face, &point);

	this->Assert("1.1", this->AreNearlyEqual(result.x, 0));
	this->Assert("1.2", this->AreNearlyEqual(result.y, 5));
	this->Assert("1.3", this->AreNearlyEqual(result.z, 3));

	// Sub test 2.
	Vec3::Set(&point, -100, 20, 30);
	CollisionFace::FindNearestPointOnCollisionFacePerimeterToPoint(&result, &face, &point);

	this->Assert("2.1", this->AreNearlyEqual(result.x, 0));
	this->Assert("2.2", this->AreNearlyEqual(result.y, 10));
	this->Assert("2.3", this->AreNearlyEqual(result.z, 3));

	// Sub test 3.
	Vec3::Set(&point, 3, -20, 10);
	CollisionFace::FindNearestPointOnCollisionFacePerimeterToPoint(&result, &face, &point);

	this->Assert("3.1", this->AreNearlyEqual(result.x, 3));
	this->Assert("3.2", this->AreNearlyEqual(result.y, 0));
	this->Assert("3.3", this->AreNearlyEqual(result.z, 3));
}

void UnitTester::Test_Mat4_FromQuat()
{
	this->SetTest("Mat4::FromQuat");

	Vec3 axis;
	Quat q;
	Mat4 m;
	Vec3 v;
	Vec3 result;

	// Sub test 1.
	Vec3::Set(&axis, 0.0f, 1.0f, 0.0f);
	Quat::FromAxisRotation(&q, &axis, PI / 2.0f);
	Mat4::FromQuat(&m, &q);
	Vec3::Set(&v, 1.0f, 0.0f, 0.0f);
	
	Vec3::TransformMat4(&result, &v, &m);
	this->Assert("1.1", this->AreNearlyEqual(result, Vec3::Create(0.0f, 0.0f, -1.0f)));

	// Sub test 2.
	Vec3::Set(&axis, 1.0f, 0.0f, 0.0f);
	Quat::FromAxisRotation(&q, &axis, PI);
	Mat4::FromQuat(&m, &q);
	Vec3::Set(&v, 0.0f, 0.0f, 1.0f);

	Vec3::TransformMat4(&result, &v, &m);
	this->Assert("2.1", this->AreNearlyEqual(result, Vec3::Create(0.0f, 0.0f, -1.0f)));

	// Sub test 3.
	Vec3::Set(&axis, 0.0f, 1.0f, 1.0f);
	Vec3::Normalize(&axis, &axis);
	Quat::FromAxisRotation(&q, &axis, PI);
	Mat4::FromQuat(&m, &q);
	Vec3::Set(&v, 1.0f, 0.0f, 0.0f);

	Vec3::TransformMat4(&result, &v, &m);
	this->Assert("3.1", this->AreNearlyEqual(result, Vec3::Create(-1.0f, 0.0f, 0.0f)));
}

void UnitTester::Test_Math_CalculateTriangleArea2D()
{
	this->SetTest("Math::CalculateTriangleArea2D");

	// Sub test 1.
	Vec2 points[3];
	Vec2::Set(&points[0], 0, 0);
	Vec2::Set(&points[1], 0, 1);
	Vec2::Set(&points[2], 1, 0);

	float area = Math::CalculateTriangleArea(points);

	this->Assert("1.1", this->AreNearlyEqual(area, 0.5f));

	// Sub test 2.
	Vec2::Set(&points[0], 0, 0);
	Vec2::Set(&points[1], 0, 2);
	Vec2::Set(&points[2], 1, 0);

	area = Math::CalculateTriangleArea(points);

	this->Assert("2.1", this->AreNearlyEqual(area, 1.0f));

	// Sub test 3.
	Vec2::Set(&points[0], 0, 0);
	Vec2::Set(&points[1], 3, 0);
	Vec2::Set(&points[2], 0, 2);

	area = Math::CalculateTriangleArea(points);

	this->Assert("3.1", this->AreNearlyEqual(area, 3.0f));
}

void UnitTester::Test_Math_CalculateTriangleArea3D()
{
	this->SetTest("Math::CalculateTriangleArea3D");

	// Sub test 1.
	Vec3 points[3];
	Vec3::Set(&points[0], 0, 0, 0);
	Vec3::Set(&points[1], 0, 1, 0);
	Vec3::Set(&points[2], 1, 0, 0);

	float area = Math::CalculateTriangleArea(points);

	this->Assert("1.1", this->AreNearlyEqual(area, 0.5f));

	// Sub test 2.
	Vec3::Set(&points[0], 0, 0, 0);
	Vec3::Set(&points[1], 0, 0, 2);
	Vec3::Set(&points[2], 1, 0, 0);

	area = Math::CalculateTriangleArea(points);

	this->Assert("2.1", this->AreNearlyEqual(area, 1.0f));

	// Sub test 3.
	Vec3::Set(&points[0], 0, 0, 0);
	Vec3::Set(&points[1], 0, 3, 0);
	Vec3::Set(&points[2], 0, 0, 2);

	area = Math::CalculateTriangleArea(points);

	this->Assert("3.1", this->AreNearlyEqual(area, 3.0f));

	// Sub test 4.
	Vec3::Set(&points[0], 0, 0, 0);
	Vec3::Set(&points[1], 1, 1, 0);
	Vec3::Set(&points[2], 1, 0, 0);

	area = Math::CalculateTriangleArea(points);

	this->Assert("4.1", this->AreNearlyEqual(area, 0.5f));
}

void UnitTester::Test_Math_CalculateBarycentricCoords2D()
{
	this->SetTest("Math::CalculateBarycentricCoords2D");

	Vec2 points[3];
	Vec2 point;
	Vec3 barycentricCoords;
	Vec2 result;

	// Sub test 1.
	Vec2::Set(&points[0], 0, 0);
	Vec2::Set(&points[1], 0, 1);
	Vec2::Set(&points[2], 1, 0);

	Vec2::Set(&point, 0.1f, 0.1f);
	Math::CalculateBarycentricCoords(&barycentricCoords, points, &point);
	Math::BarycentricMix(&result, points, &barycentricCoords);

	this->Assert("1.1", this->AreNearlyEqual(point, result));
	this->Assert("1.2", Math::CheckBarycentricCoordsAreWithinTriangle(&barycentricCoords));
	this->Assert("1.5", this->AreNearlyEqual(barycentricCoords.x + barycentricCoords.y + barycentricCoords.z, 1.0f));

	// Sub test 2.
	Vec2::Set(&points[0], 0, 0);
	Vec2::Set(&points[1], 1, 1);
	Vec2::Set(&points[2], 1, 0);

	Vec2::Set(&point, 1.1f, 0.5f);
	Math::CalculateBarycentricCoords(&barycentricCoords, points, &point);
	Math::BarycentricMix(&result, points, &barycentricCoords);

	this->Assert("2.1", !Math::CheckBarycentricCoordsAreWithinTriangle(&barycentricCoords));
}

void UnitTester::Test_Math_CalculateBarycentricCoords3D()
{
	this->SetTest("Math::CalculateBarycentricCoords3D");

	Vec3 points[3];
	Vec3 point;
	Vec3 barycentricCoords;
	Vec3 result;

	// Sub test 1.
	Vec3::Set(&points[0], 0, 0, 0);
	Vec3::Set(&points[1], 0, 1, 0);
	Vec3::Set(&points[2], 1, 0, 0);

	Vec3::Set(&point, 0.1f, 0.1f, 0.0f);
	Math::CalculateBarycentricCoords(&barycentricCoords, points, &point);
	Math::BarycentricMix(&result, points, &barycentricCoords);

	this->Assert("1.1", this->AreNearlyEqual(point, result));
	this->Assert("1.2", Math::CheckBarycentricCoordsAreWithinTriangle(&barycentricCoords));
	this->Assert("1.3", this->AreNearlyEqual(barycentricCoords.x + barycentricCoords.y + barycentricCoords.z, 1.0f));

	// Sub test 2.
	Vec3::Set(&points[0], 0, 0, 0);
	Vec3::Set(&points[1], 0, 2, 0);
	Vec3::Set(&points[2], 1, 0, 0);

	Vec3::Set(&point, 0.0f, 2.0f, 0.0f);
	Math::CalculateBarycentricCoords(&barycentricCoords, points, &point);
	Math::BarycentricMix(&result, points, &barycentricCoords);

	this->Assert("2.1", this->AreNearlyEqual(point, result));
	this->Assert("2.2", Math::CheckBarycentricCoordsAreWithinTriangle(&barycentricCoords));
	this->Assert("2.3", this->AreNearlyEqual(barycentricCoords.x + barycentricCoords.y + barycentricCoords.z, 1.0f));

	// Sub test 3.
	Vec3::Set(&points[0], 0, 0, 0);
	Vec3::Set(&points[1], 0.5, 0, 1);
	Vec3::Set(&points[2], 1, 0, 0);

	Vec3::Set(&point, 0.5f, 0.0f, 0.5f);
	Math::CalculateBarycentricCoords(&barycentricCoords, points, &point);
	Math::BarycentricMix(&result, points, &barycentricCoords);

	this->Assert("3.1", this->AreNearlyEqual(point, result));
	this->Assert("3.2", Math::CheckBarycentricCoordsAreWithinTriangle(&barycentricCoords));
	this->Assert("3.3", this->AreNearlyEqual(barycentricCoords.x + barycentricCoords.y + barycentricCoords.z, 1.0f));

	// Sub test 4.
	Vec3::Set(&points[0], 0, 0, 0);
	Vec3::Set(&points[1], 0.5, 0, 1);
	Vec3::Set(&points[2], 1, 0, 0);

	Vec3::Set(&point, -0.001f, 0.0f, 0.0f);
	Math::CalculateBarycentricCoords(&barycentricCoords, points, &point);
	Math::BarycentricMix(&result, points, &barycentricCoords);

	this->Assert("4.1", !Math::CheckBarycentricCoordsAreWithinTriangle(&barycentricCoords));

	// Sub test 5.
	Vec3::Set(&points[0], 0, 0, 0);
	Vec3::Set(&points[1], 1, 1, 0);
	Vec3::Set(&points[2], 1, 0, 0);

	Vec3::Set(&point, 1.1f, 0.0f, 0);
	Math::CalculateBarycentricCoords(&barycentricCoords, points, &point);
	Math::BarycentricMix(&result, points, &barycentricCoords);

	this->Assert("5.1", !Math::CheckBarycentricCoordsAreWithinTriangle(&barycentricCoords));

	// Sub test 6.
	Vec3::Set(&points[0], 0, 0, 0);
	Vec3::Set(&points[1], 1000, 1000, 0);
	Vec3::Set(&points[2], 1000, 0, 0);

	Vec3::Set(&point, 1000.001f, 0.0f, 0);
	Math::CalculateBarycentricCoords(&barycentricCoords, points, &point);
	Math::BarycentricMix(&result, points, &barycentricCoords);

	this->Assert("6.1", !Math::CheckBarycentricCoordsAreWithinTriangle(&barycentricCoords));

	// Sub test 7.
	Vec3::Set(&points[0], 0, 0, 0);
	Vec3::Set(&points[1], 1000, 1000, 0);
	Vec3::Set(&points[2], 1000, 0, 0);

	Vec3::Set(&point, 999.999f, 0.0f, 0);
	Math::CalculateBarycentricCoords(&barycentricCoords, points, &point);
	Math::BarycentricMix(&result, points, &barycentricCoords);

	this->Assert("7.1", Math::CheckBarycentricCoordsAreWithinTriangle(&barycentricCoords));

	// Sub test 8.
	Vec3::Set(&points[0], 0, 0, 0);
	Vec3::Set(&points[1], 0.001f, 0.001f, 0);
	Vec3::Set(&points[2], 0.001f, 0, 0);

	Vec3::Set(&point, 0.00099f, 0.0f, 0);
	Math::CalculateBarycentricCoords(&barycentricCoords, points, &point);
	Math::BarycentricMix(&result, points, &barycentricCoords);

	this->Assert("8.1", Math::CheckBarycentricCoordsAreWithinTriangle(&barycentricCoords));
}

void UnitTester::Test_Math_CalculateWorldPositionFromUV()
{
	this->SetTest("Math::CalculateWorldPositionFromUV");

	Vec3 positions[3];
	Vec2 uvs[3];
	Vec2 uv;
	Vec3 worldPosition;

	// Sub test 1.
	Vec3::Set(&positions[0], 0, 0, 0);
	Vec3::Set(&positions[1], 0, 10, 0);
	Vec3::Set(&positions[2], 10, 0, 0);

	Vec2::Set(&uvs[0], 0, 1);
	Vec2::Set(&uvs[1], 0, 0);
	Vec2::Set(&uvs[2], 1, 1);

	Vec2::Set(&uv, 0.5f, 1); 
	bool uvIsWithinFace = Math::CalculateWorldPositionFromUV(&worldPosition, positions, uvs, &uv);

	this->Assert("1.1", uvIsWithinFace);
	this->Assert("1.2", this->AreNearlyEqual(worldPosition, Vec3::Create(5, 0, 0)));

	// Sub test 2.
	Vec3::Set(&positions[0], 0, 0, 0);
	Vec3::Set(&positions[1], 0, 10, 0);
	Vec3::Set(&positions[2], 10, 0, 0);

	Vec2::Set(&uvs[0], 0, 1);
	Vec2::Set(&uvs[1], 0, 0);
	Vec2::Set(&uvs[2], 1, 1);

	Vec2::Set(&uv, 0, 0);
	uvIsWithinFace = Math::CalculateWorldPositionFromUV(&worldPosition, positions, uvs, &uv);

	this->Assert("2.1", uvIsWithinFace);
	this->Assert("2.2", this->AreNearlyEqual(worldPosition, Vec3::Create(0, 10, 0)));

	// Sub test 2.
	Vec3::Set(&positions[0], 0, 0, 0);
	Vec3::Set(&positions[1], 5, 10, 0);
	Vec3::Set(&positions[2], 10, 0, 0);

	Vec2::Set(&uvs[0], 0, 1);
	Vec2::Set(&uvs[1], 0.5, 0);
	Vec2::Set(&uvs[2], 1, 1);

	Vec2::Set(&uv, 0.5, 0.5);
	uvIsWithinFace = Math::CalculateWorldPositionFromUV(&worldPosition, positions, uvs, &uv);

	this->Assert("3.1", uvIsWithinFace);
	this->Assert("3.2", this->AreNearlyEqual(worldPosition, Vec3::Create(5, 5, 0)));
}

void UnitTester::Test_Math_CalculateNormalWithinHemisphere()
{
	/*this->SetTest("Math::CalculateNormalWithinHemisphere");

	Vec3 normal;
	Vec3 binormal;
	Vec3 result;

	// Sub test 1.
	Vec3::Set(&normal, 0.0f, 1.0f, 0.0f);
	Vec3::Set(&binormal, 1.0f, 0.0f, 0.0f);
	
	Math::CalculateNormalWithinHemisphere(&result, &normal, &binormal, 4, 4, 3, 3, PI / 2.0f);
	this->Assert("1.1", this->AreNearlyEqual(result, Vec3::Create(-1.0f, 0, 0.0f)));

	// Sub test 2.
	bool allResultsWereWithinHemisphere = true;
	for (int circleNumber = 0; circleNumber < 4; circleNumber++)
	{
		for (int segmentNumber = 0; segmentNumber < 4; segmentNumber++)
		{
			Vec3::Set(&normal, Math::GenerateRandomFloat(), Math::GenerateRandomFloat(), Math::GenerateRandomFloat());
			Vec3::Normalize(&normal, &normal);

			Math::CalculateNormalWithinHemisphere(&result, &normal, &binormal, 4, 4, circleNumber, segmentNumber, PI / 2.0f);
			allResultsWereWithinHemisphere = Vec3::Dot(&normal, &result) >= 0;
		}
	}

	this->Assert("1.2", allResultsWereWithinHemisphere);*/
}