#pragma once

#include "Math/Vec2.h"

struct Rect
{
	Vec2 from;
	Vec2 to;

	static bool PointIsWithin(Rect* rect, Vec2* point)
	{
		return
			point->x >= rect->from.x &&
			point->y >= rect->from.y &&
			point->x <= rect->to.x &&
			point->y <= rect->to.y;
	}
};