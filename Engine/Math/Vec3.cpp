#include "Math/Vec3.h"
#include "Math/Mat4.h"

void Vec3::TransformMat3(Vec3* out, const Vec3* v, const Mat4* transform)
{
	const float* m = transform->m;
	float x = v->x;
	float y = v->y;
	float z = v->z;
	
	out->x = m[0] * x + m[4] * y + m[8] * z;
	out->y = m[1] * x + m[5] * y + m[9] * z;
	out->z = m[2] * x + m[6] * y + m[10] * z;
};

void Vec3::TransformMat4(Vec3* out, const Vec3* v, const Mat4* transform)
{
	const float* m = transform->m;
	float x = v->x;
	float y = v->y;
	float z = v->z;
	float w = m[3] * x + m[7] * y + m[11] * z + m[15];

	if (w == 0.0f)
	{
		w = 1.0f;
	}

	out->x = (m[0] * x + m[4] * y + m[8] * z + m[12]) / w;
	out->y = (m[1] * x + m[5] * y + m[9] * z + m[13]) / w;
	out->z = (m[2] * x + m[6] * y + m[10] * z + m[14]) / w;
};