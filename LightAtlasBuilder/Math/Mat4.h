#pragma once

struct Vec3;
struct Quat;

struct Mat4
{
	float m[16];

	static void Set(Mat4* out, float m0, float m1, float m2, float m3, float m4, float m5, float m6, float m7, float m8, float m9, float m10, float m11, float m12, float m13, float m14, float m15);
	static void Copy(Mat4* out, const Mat4* src);
	static void CreateIdentity(Mat4* out);
	static void CreateTranslation(Mat4* out, Vec3* amount);
	static void CreateXRotation(Mat4* out, float radians);
	static void CreateYRotation(Mat4* out, float radians);
	static void CreateZRotation(Mat4* out, float radians);
	static void CreateLookAt(Mat4* out, Vec3* eye, Vec3* lookAt, Vec3* up);
	static void CreatePerspective(Mat4* out, float fov, float aspectRatio, float near, float far);
	static void FromQuat(Mat4* out, Quat* quat);
	static bool Invert(Mat4* out, Mat4* a);
	static void Multiply(Mat4* out, Mat4* a, Mat4* b);
	static void Translate(Mat4* out, Vec3* amount);
};