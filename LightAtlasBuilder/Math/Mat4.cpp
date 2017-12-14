#include <math.h>
#include "Math/Mat4.h"
#include "Math/Vec3.h"
#include "Math/Quat.h"

void Mat4::Set(
	Mat4* out, 
	float m0, float m1, float m2, float m3, 
	float m4, float m5, float m6, float m7, 
	float m8, float m9, float m10, float m11, 
	float m12, float m13, float m14, float m15)
{
	float* m = out->m;																
	m[0] = m0;		m[1] = m1;		m[2] = m2;		m[3] = m3;
	m[4] = m4;		m[5] = m5;		m[6] = m6;		m[7] = m7;
	m[8] = m8;		m[9] = m9;		m[10] = m10;	m[11] = m11;
	m[12] = m12;	m[13] = m13;	m[14] = m14;	m[15] = m15;
}

void Mat4::Copy(Mat4* out, const Mat4* src)
{
	for (int i = 0; i < 16; i++)
	{
		out->m[i] = src->m[i];
	}
}

void Mat4::CreateIdentity(Mat4* out)
{
	Mat4::Set(out,
		1.0f,	0.0f,	0.0f,	0.0f,
		0.0f,	1.0f,	0.0f,	0.0f, 
		0.0f,	0.0f,	1.0f,	0.0f,
		0.0f,	0.0f,	0.0f,	1.0f);
}

void Mat4::CreateTranslation(Mat4* out, Vec3* amount)
{
	Mat4::Set(out,
		1.0f,		0.0f,		0.0f,		0.0f,
		0.0f,		1.0f,		0.0f,		0.0f,
		0.0f,		0.0f,		1.0f,		0.0f,
		amount->x,	amount->y,	amount->z,	1.0f);
}

void Mat4::CreateXRotation(Mat4* out, float radians)
{
	float s = sinf(radians);
	float c = cosf(radians);

	Mat4::Set(out,
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, c, s, 0.0f,
		0.0f, -s, c, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}

void Mat4::CreateYRotation(Mat4* out, float radians)
{
	float s = sinf(radians);
	float c = cosf(radians);

	Mat4::Set(out,
		c,		0.0f,	-s,		0.0f,
		0.0f,	1.0f,	0.0f,	0.0f, 
		s,		0.0f,	c,		0.0f,
		0.0f,	0.0f,	0.0f,	1.0f);
}

void Mat4::CreateZRotation(Mat4* out, float radians)
{
	float s = sinf(radians);
	float c = cosf(radians);

	Mat4::Set(out,
		c,		s,		0.0f,	0.0f,
		-s,		c,		0.0f,	0.0f,
		0.0f,	0.0f,	1.0f,	0.0f,
		0.0f,	0.0f,	0.0f,	1.0f);
}

void Mat4::CreateLookAt(Mat4* out, Vec3* eye, Vec3* lookAt, Vec3* up)
{
	Vec3 xAxis, yAxis, zAxis;
	
	// Calculate the z-axis.
	Vec3::Sub(&zAxis, lookAt, eye);
	Vec3::Normalize(&zAxis, &zAxis);

	// Calculate the x-axis.
	Vec3::Cross(&xAxis, up, &zAxis);
	Vec3::Normalize(&xAxis, &xAxis);

	// Calculate the y-axis.
	Vec3::Cross(&yAxis, &zAxis, &xAxis);

	// Build the matrix.
	Mat4::Set(out,
		xAxis.x,					yAxis.x,					zAxis.x,					0.0f,
		xAxis.y,					yAxis.y,					zAxis.y,					0.0f,
		xAxis.z,					yAxis.z,					zAxis.z,					0.0f,
		-Vec3::Dot(&xAxis, eye),	-Vec3::Dot(&yAxis, eye),	-Vec3::Dot(&zAxis, eye),	1.0f);
}

void Mat4::CreatePerspective(Mat4* out, float fov, float aspectRatio, float near, float far) 
{
	float f = 1.0f / tanf(fov / 2.0f);
	float nf = 1.0f / (near - far);

	Mat4::Set(out,
		f / aspectRatio,	0.0f,			0.0f,						0.0f,
		0.0f,				f,				0.0f,						0.0f,
		0.0f,				0.0f,			-(far + near) * nf,			1.0f,
		0.0f,				0.0f,			(2.0f * far * near) * nf,	0.0f);
}
/*void Mat4::CreatePerspective(Mat4* out, float fov, float aspectRatio, float near, float far) 
{
	float f = 1.0f / tanf(fov / 2.0f);
	float nf = 1.0f / (near - far);

	Mat4::Set(out,
		f / aspectRatio,	0.0f,			0.0f,						0.0f,
		0.0f,				f,				0.0f,						0.0f,
		0.0f,				0.0f,			(far + near) * nf,			-1.0f,
		0.0f,				0.0f,			(2.0f * far * near) * nf,	0.0f);
}*/

void Mat4::FromQuat(Mat4* out, Quat* quat)
{
	float x = quat->x, y = quat->y, z = quat->z, w = quat->w;
	float x2 = x + x;
	float y2 = y + y;
	float z2 = z + z;
	float xx = x * x2;
	float yx = y * x2;
	float yy = y * y2;
	float zx = z * x2;
	float zy = z * y2;
	float zz = z * z2;
	float wx = w * x2;
	float wy = w * y2;
	float wz = w * z2;

	Mat4::Set(out,
		1.0f - yy - zz,		yx + wz,			zx - wy,			0.0f,
		yx - wz,			1.0f - xx - zz,		zy + wx,			0.0f,
		zx + wy,			zy - wx,			1.0f - xx - yy,		0.0f,
		0.0f,				0.0f,				0.0f,				1.0f);
}

bool Mat4::Invert(Mat4* out, Mat4* a)
{
	bool success = false;

	float* m = a->m;
	float a00 = m[0], a01 = m[1], a02 = m[2], a03 = m[3],
		a10 = m[4], a11 = m[5], a12 = m[6], a13 = m[7],
		a20 = m[8], a21 = m[9], a22 = m[10], a23 = m[11],
		a30 = m[12], a31 = m[13], a32 = m[14], a33 = m[15],
		b00 = a00 * a11 - a01 * a10,
		b01 = a00 * a12 - a02 * a10,
		b02 = a00 * a13 - a03 * a10,
		b03 = a01 * a12 - a02 * a11,
		b04 = a01 * a13 - a03 * a11,
		b05 = a02 * a13 - a03 * a12,
		b06 = a20 * a31 - a21 * a30,
		b07 = a20 * a32 - a22 * a30,
		b08 = a20 * a33 - a23 * a30,
		b09 = a21 * a32 - a22 * a31,
		b10 = a21 * a33 - a23 * a31,
		b11 = a22 * a33 - a23 * a32;


	float det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;
	if (det > 0.0f)
	{
		det = 1.0f / det;
		out->m[0] = (a11 * b11 - a12 * b10 + a13 * b09) * det;
		out->m[1] = (a02 * b10 - a01 * b11 - a03 * b09) * det;
		out->m[2] = (a31 * b05 - a32 * b04 + a33 * b03) * det;
		out->m[3] = (a22 * b04 - a21 * b05 - a23 * b03) * det;
		out->m[4] = (a12 * b08 - a10 * b11 - a13 * b07) * det;
		out->m[5] = (a00 * b11 - a02 * b08 + a03 * b07) * det;
		out->m[6] = (a32 * b02 - a30 * b05 - a33 * b01) * det;
		out->m[7] = (a20 * b05 - a22 * b02 + a23 * b01) * det;
		out->m[8] = (a10 * b10 - a11 * b08 + a13 * b06) * det;
		out->m[9] = (a01 * b08 - a00 * b10 - a03 * b06) * det;
		out->m[10] = (a30 * b04 - a31 * b02 + a33 * b00) * det;
		out->m[11] = (a21 * b02 - a20 * b04 - a23 * b00) * det;
		out->m[12] = (a11 * b07 - a10 * b09 - a12 * b06) * det;
		out->m[13] = (a00 * b09 - a01 * b07 + a02 * b06) * det;
		out->m[14] = (a31 * b01 - a30 * b03 - a32 * b00) * det;
		out->m[15] = (a20 * b03 - a21 * b01 + a22 * b00) * det;
	}

	return success;
};

void Mat4::Multiply(Mat4* out, Mat4* a, Mat4* b) 
{
	float* am = a->m;
	float* bm = b->m;

	float 
		a00 = am[0], a01 = am[1], a02 = am[2], a03 = am[3],
		a10 = am[4], a11 = am[5], a12 = am[6], a13 = am[7],
		a20 = am[8], a21 = am[9], a22 = am[10], a23 = am[11],
		a30 = am[12], a31 = am[13], a32 = am[14], a33 = am[15];

	// Cache only the current line of the second matrix.
	float b0 = bm[0], b1 = bm[1], b2 = bm[2], b3 = bm[3];
	out->m[0] = b0*a00 + b1*a10 + b2*a20 + b3*a30;
	out->m[1] = b0*a01 + b1*a11 + b2*a21 + b3*a31;
	out->m[2] = b0*a02 + b1*a12 + b2*a22 + b3*a32;
	out->m[3] = b0*a03 + b1*a13 + b2*a23 + b3*a33;
	b0 = bm[4]; b1 = bm[5]; b2 = bm[6]; b3 = bm[7];
	out->m[4] = b0*a00 + b1*a10 + b2*a20 + b3*a30;
	out->m[5] = b0*a01 + b1*a11 + b2*a21 + b3*a31;
	out->m[6] = b0*a02 + b1*a12 + b2*a22 + b3*a32;
	out->m[7] = b0*a03 + b1*a13 + b2*a23 + b3*a33;
	b0 = bm[8]; b1 = bm[9]; b2 = bm[10]; b3 = bm[11];
	out->m[8] = b0*a00 + b1*a10 + b2*a20 + b3*a30;
	out->m[9] = b0*a01 + b1*a11 + b2*a21 + b3*a31;
	out->m[10] = b0*a02 + b1*a12 + b2*a22 + b3*a32;
	out->m[11] = b0*a03 + b1*a13 + b2*a23 + b3*a33;
	b0 = bm[12]; b1 = bm[13]; b2 = bm[14]; b3 = bm[15];
	out->m[12] = b0*a00 + b1*a10 + b2*a20 + b3*a30;
	out->m[13] = b0*a01 + b1*a11 + b2*a21 + b3*a31;
	out->m[14] = b0*a02 + b1*a12 + b2*a22 + b3*a32;
	out->m[15] = b0*a03 + b1*a13 + b2*a23 + b3*a33;
};

void Mat4::Translate(Mat4* out, Vec3* amount)
{
	float* m = out->m;
	m[12] += amount->x; 
	m[13] += amount->y; 
	m[14] += amount->z;
}