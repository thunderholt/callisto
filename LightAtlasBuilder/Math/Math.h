#pragma once

#include "Math/Vec2.h"
#include "Math/Vec2i.h"
#include "Math/Vec3.h"
#include "Math/Vec3i.h"
#include "Math/Ray3.h"
#include "Math/Plane.h"
#include "Math/AABB.h"
#include "Math/CollisionFace.h"
#include "Math/CollisionLine.h"
#include "Math/Colours.h"

class Math
{
public:
	static int GenerateRandomInt(int from, int to);
	static float GenerateRandomFloat();
};