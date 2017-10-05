#pragma once

#include "Include/Callisto.h"

class TextureAsset : public ITextureAsset
{
public:
	TextureAsset(const char* filePath, Buffer* fileData, bool isStayResident);
	virtual ~TextureAsset();

	//virtual void BeginLoadingReferencedAssets();
	virtual bool ResolveReferencedAssets();

	virtual char* GetFilePath();
	virtual bool GetIsStayResident();
	virtual bool GetLoadedSuccessfully();
	virtual bool GetIsEvictable();
	virtual void SetIsEvictable(bool isEvictable);

	virtual Vec2* GetSize();
	virtual ProviderId GetTextureId();

private:
	char filePath[AssetMaxFilePathLength];
	bool isStayResident;
	bool loadedSuccessfully;
	bool isEvictable;

	Vec2 size;
	ProviderId textureId;
};