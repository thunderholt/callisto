#pragma once

struct BgrUByte
{
	unsigned char b;
	unsigned char g;
	unsigned char r;
};

struct RgbaUByte
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

struct RgbFloat
{
	float r, g, b;

	static RgbFloat Create(float r, float g, float b)
	{
		RgbFloat colour;
		RgbFloat::Set(&colour, r, g, b);
		return colour;
	}

	static void Zero(RgbFloat* out)
	{
		out->r = 0.0f;
		out->g = 0.0f;
		out->b = 0.0f;
	}

	static void Set(RgbFloat* out, float r, float g, float b)
	{
		out->r = r;
		out->g = g;
		out->b = b;
	}

	static void Add(RgbFloat* out, RgbFloat* c1, RgbFloat* c2)
	{
		out->r = c1->r + c2->r;
		out->g = c1->g + c2->g;
		out->b = c1->b + c2->b;
	}

	static void Scale(RgbFloat* out, RgbFloat* c, float amount)
	{
		out->r = c->r * amount;
		out->g = c->g * amount;
		out->b = c->b * amount;
	}

	static void Saturate(RgbFloat* out, RgbFloat* c)
	{
		out->r = c->r > 1.0f ? 1.0f : c->r;
		out->g = c->g > 1.0f ? 1.0f : c->g;
		out->b = c->b > 1.0f ? 1.0f : c->b;
	}
};

struct RgbaFloat
{
	float r, g, b, a;

	static RgbaFloat Create(float r, float g, float b, float a)
	{
		RgbaFloat colour;
		RgbaFloat::Set(&colour, r, g, b, a);
		return colour;
	}

	static void Set(RgbaFloat* colour, float r, float g, float b, float a)
	{
		colour->r = r;
		colour->g = g;
		colour->b = b;
		colour->a = a;
	}

	static void SetRed(RgbaFloat* colour)
	{
		Set(colour, 1.0f, 0.0f, 0.0f, 1.0f);
	}

	static void SetGreen(RgbaFloat* colour)
	{
		Set(colour, 0.0f, 1.0f, 0.0f, 1.0f);
	}

	static void SetBlack(RgbaFloat* colour)
	{
		Set(colour, 0.0f, 0.0f, 0.0f, 1.0f);
	}

	static void SetWhite(RgbaFloat* colour)
	{
		Set(colour, 1.0f, 1.0f, 1.0f, 1.0f);
	}

	static RgbaFloat GetBlack()
	{
		return RgbaFloat::Create(0.0f, 0.0f, 0.0f, 1.0f);
	}

	static RgbaFloat GetWhite()
	{
		return RgbaFloat::Create(1.0f, 1.0f, 1.0f, 1.0f);
	}

	static RgbaFloat GetWhite(float alpha)
	{
		return RgbaFloat::Create(1.0f, 1.0f, 1.0f, alpha);
	}

	static RgbaFloat GetRed()
	{
		return RgbaFloat::Create(1.0f, 0.0f, 0.0f, 1.0f);
	}

	static RgbaFloat GetRed(float alpha)
	{
		return RgbaFloat::Create(1.0f, 0.0f, 0.0f, alpha);
	}
};