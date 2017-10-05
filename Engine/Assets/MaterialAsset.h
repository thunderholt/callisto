#pragma once

#include "Include/Callisto.h"

class MaterialAsset : public IMaterialAsset
{
public:
	MaterialAsset(const char* filePath, Buffer* fileData, bool isStayResident);
	virtual ~MaterialAsset();

	virtual AssetRef* GetTextureAssetRef(MaterialTextureType textureType);
	virtual RgbFloat GetEmmissiveColour();

	//virtual void BeginLoadingReferencedAssets();
	virtual bool ResolveReferencedAssets();

	virtual char* GetFilePath();
	virtual bool GetIsStayResident();
	virtual bool GetLoadedSuccessfully();
	virtual bool GetIsEvictable();
	virtual void SetIsEvictable(bool isEvictable);

private:
	char filePath[AssetMaxFilePathLength];
	bool isStayResident;
	bool loadedSuccessfully;
	bool isEvictable;

	AssetRef textureAssetRefs[MaterialNumberOfTextureTypes];
	RgbFloat emissiveColour; // TEST code!
};