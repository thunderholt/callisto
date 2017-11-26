#include "Core/LightAtlas.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Lib/stb_image_write.h"

LightAtlas::LightAtlas()
{
	this->texels = null;
	Vec2i::Zero(&this->size);
}

LightAtlas::~LightAtlas()
{
	SafeDeleteArrayAndNull(this->texels);
}

void LightAtlas::Allocate(int width, int height)
{
	this->size.x = width;
	this->size.y = height;

	SafeDeleteArrayAndNull(this->texels);

	int numbeOfTexels = this->size.x * this->size.y;
	this->texels = new RgbFloat[numbeOfTexels];

	memset(this->texels, 0, sizeof(RgbFloat) * numbeOfTexels);
}

void LightAtlas::WriteToPngFile(const char* filePath)
{
	RgbUByte* rgbTexels = new RgbUByte[this->size.x * this->size.y];

	for (int y = 0; y < this->size.y; y++)
	{
		for (int x = 0; x < this->size.x; x++)
		{
			int texelIndex = y * this->size.x + x;
			RgbUByte* rgbTexel = &rgbTexels[texelIndex];
			RgbFloat* texel = &this->texels[texelIndex];

			rgbTexel->r = (unsigned char)(Math::Clamp(texel->r, 0.0f, 1.0f) * 255);
			rgbTexel->g = (unsigned char)(Math::Clamp(texel->g, 0.0f, 1.0f) * 255);
			rgbTexel->b = (unsigned char)(Math::Clamp(texel->b, 0.0f, 1.0f) * 255);
		}
	}

	stbi_write_png(filePath, this->size.x, this->size.y, 3, rgbTexels, this->size.x * sizeof(RgbUByte));

	SafeDeleteArrayAndNull(rgbTexels);
}

RgbFloat* LightAtlas::GetTexels()
{
	return this->texels;
}

Vec2i LightAtlas::GetSize()
{
	return this->size;
}