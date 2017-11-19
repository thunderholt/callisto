#pragma once

#include "Include/LightAtlasBuilder.h"

class MaterialAsset : public IMaterialAsset
{
public:
	MaterialAsset();
	virtual ~MaterialAsset();

	virtual bool Load(const char* filePath);

private:
};