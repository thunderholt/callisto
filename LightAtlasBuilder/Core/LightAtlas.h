#pragma once
#include "Include/LightAtlasBuilder.h"

class LightAtlas : public ILightAtlas
{
public:
	LightAtlas();
	virtual ~LightAtlas();
	virtual void Allocate(int width, int height);
	virtual void ResetLumelIntensityCacheItems();
	//virtual void ResetLumelIntensityCacheItemsForBounceNumber(int bounceNumber);
	//virtual void AverageLumelIntensityCacheItemsForBounceNumber(int bounceNumber);
	virtual void AccumulateLumelColoursFromIntensityCacheItems(RgbFloat* lightColour);
	virtual void WriteToPngFile(const char* filePath);
	virtual Lumel* GetLumels();
	virtual Vec2i GetSize();

private:
	Lumel* lumels;
	Vec2i size;
};
