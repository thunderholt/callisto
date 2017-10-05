#pragma once

#include "Include/Callisto.h"

class TextAsset : public ITextAsset
{
public:
	TextAsset(const char* filePath, Buffer* fileData,  bool isStayResident);
	virtual ~TextAsset();

	//virtual void BeginLoadingReferencedAssets();
	virtual bool ResolveReferencedAssets();

	virtual char* GetFilePath();
	virtual bool GetIsStayResident();
	virtual bool GetLoadedSuccessfully();
	virtual bool GetIsEvictable();
	virtual void SetIsEvictable(bool isEvictable);

	virtual char* GetText();

private:
	char filePath[AssetMaxFilePathLength];
	bool isStayResident;
	bool loadedSuccessfully;
	bool isEvictable;

	char* text;
};