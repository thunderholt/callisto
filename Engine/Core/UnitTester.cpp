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