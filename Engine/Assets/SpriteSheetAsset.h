#pragma once

#include "Include/Callisto.h"

class SpriteSheetAsset : public ISpriteSheetAsset
{
public:
	SpriteSheetAsset(const char* filePath, Buffer* fileData, bool isStayResident);
	virtual ~SpriteSheetAsset();

	//virtual void BeginLoadingReferencedAssets();
	virtual bool ResolveReferencedAssets();

	virtual char* GetFilePath();
	virtual bool GetIsStayResident();
	virtual bool GetLoadedSuccessfully();
	virtual bool GetIsEvictable();
	virtual void SetIsEvictable(bool isEvictable);

	//virtual AssetRef* GetTextureAssetRef();
	virtual int FindSpriteIndex(const char* name);
	virtual Sprite* GetSprite(int index);
	virtual int GetNumberOfSprites();

private:
	char filePath[AssetMaxFilePathLength];
	bool isStayResident;
	bool loadedSuccessfully;
	bool isEvictable;

	//AssetRef textureAssetRef;
	DynamicLengthArray<Sprite> sprites;
};