#pragma once

#include "Include/PVSBuilder.h"

class MaterialAsset : public IMaterialAsset
{
public:
	MaterialAsset();
	virtual ~MaterialAsset();

	virtual bool Load(const char* filePath);
	virtual bool GetIsVisibilityOcculuder();

private:
	bool isVisibilityOccluder;
};