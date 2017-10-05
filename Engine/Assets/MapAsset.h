#pragma once

#include "Include/Callisto.h"

class MapAsset : public IMapAsset
{
public:
	MapAsset(const char* filePath, Buffer* fileData, bool isStayResident);
	virtual ~MapAsset();

	virtual bool ResolveReferencedAssets();

	virtual char* GetFilePath();
	virtual bool GetIsStayResident();
	virtual bool GetLoadedSuccessfully();
	virtual bool GetIsEvictable();
	virtual void SetIsEvictable(bool isEvictable);

	virtual AssetRef* GetAssetRef(int index);
	virtual int GetNumberAssetRefs();
	virtual AssetRef* GetWorldMeshAssetRef();
	virtual IActorInstance* GetActorInstance(int index);
	virtual int GetNumberOfActorInstances();

private:
	void LoadAssetRefsFromJsonValue(IJsonValue* jsonAssetRefs, AssetType assetType);

	char filePath[AssetMaxFilePathLength];
	bool isStayResident;
	bool loadedSuccessfully;
	bool isEvictable;

	DynamicLengthArray<AssetRef> assetRefs;
	AssetRef worldMeshAssetRef;
	DynamicLengthArray<IActorInstance*> actorInstances;
};