#pragma once
#include "Include/LightAtlasBuilder.h"

class LightAtlas : public ILightAtlas
{
public:
	LightAtlas();
	virtual ~LightAtlas();
	virtual void Allocate(int width, int height);
	virtual void WriteToPngFile(const char* filePath);
	virtual RgbFloat* GetTexels();
	virtual Vec2i GetSize();

private:
	RgbFloat* texels;
	Vec2i size;
};
