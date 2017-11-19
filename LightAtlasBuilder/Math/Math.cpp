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