#pragma once

#include "Include/Callisto.h"

struct AssetLoadJob
{
	AssetType assetType;
	char filePath[AssetMaxFilePathLength];
	bool isStayResident;
	int loadingFileIndex;
	bool isComplete;
};

enum AssetManagerLoadingPhase
{
	AssetManagerLoadingPhaseNone = 0,
	AssetManagerLoadingPhaseLoadingMapAsset = 1,
	AssetManagerLoadingPhaseLoadingReferencedAssets = 2
};

class AssetManager : public IAssetManager
{
public:
	AssetManager();
	virtual ~AssetManager();

	virtual void BeginLoadingCoreAssets();
	virtual void HeartBeat();
	virtual void LoadMap(const char* filePath);
	virtual int FindAssetIndex(AssetType assetType, const char* filePath);
	virtual bool ResolveAssetRefIndex(AssetRef* assetRef);
	virtual IMapAsset* GetMapAsset(int assetIndex);
	virtual IActorAsset* GetActorAsset(int assetIndex);
	virtual IWorldMeshAsset* GetWorldMeshAsset(int assetIndex);
	virtual IStaticMeshAsset* GetStaticMeshAsset(int assetIndex);
	virtual ITextureAsset* GetTextureAsset(int assetIndex);
	virtual ISpriteSheetAsset* GetSpriteSheetAsset(int assetIndex);
	virtual ITextAsset* GetTextAsset(int assetIndex);
	virtual IMaterialAsset* GetMaterialAsset(int assetIndex);
	virtual IPVSAsset* GetPVSAsset(int assetIndex);
	virtual bool GetIsLoadingCoreAssets();
	virtual bool GetIsLoading();
	virtual bool GetLoadFailed();

private:
	void BeginLoadingAsset(AssetType assetType, const char* filePath, bool isStayResident);
	IAsset* FindExistingAsset(AssetType assetType, const char* filePath);
	bool ExecuteAssetLoadJobs();
	void HandleJustLoadedAsset(IAsset* asset);
	bool LoadAsset(IAsset** outAsset, AssetType assetType, const char* filePath, Buffer* fileData, bool isStayResident);
	void CleanUpAssets(bool includeStayResidentAssets, bool includeNonEvictableAssets);
	int FindAssetLoadJobIndex(const char* filePath);
	void MarkAllAssetsAsEvictable();

	DynamicLengthSlottedArray<IAsset*> assetStores[AssetManagerNumberOfStores];
	DynamicLengthArray<AssetLoadJob> assetLoadJobs;
	AssetManagerLoadingPhase loadingPhase;
	IMapAsset* justLoadedMapAsset;
	bool isLoadingCoreAssets;
	bool isLoading;
	bool assetLoadFailed;
	long bytesLoadedFromDisk;
};