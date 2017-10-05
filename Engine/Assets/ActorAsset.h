#pragma once

#include "Include/Callisto.h"

class ActorAsset : public IActorAsset
{
public:
	ActorAsset(const char* filePath, Buffer* fileData, bool isStayResident);
	virtual ~ActorAsset();

	//virtual void BeginLoadingReferencedAssets();
	virtual bool ResolveReferencedAssets();

	virtual char* GetFilePath();
	virtual bool GetIsStayResident();
	virtual bool GetLoadedSuccessfully();
	virtual bool GetIsEvictable();
	virtual void SetIsEvictable(bool isEvictable);

	virtual AssetRef* GetStaticMeshAssetRef(int index);
	virtual int GetNumberOfStaticMeshAssetRefs();
	virtual AssetRef* GetSkinnedMeshAssetRef(int index);
	virtual int GetNumberOfSkinnedMeshAssetRefs();
	virtual AssetRef* GetSkinnedMeshAnimationAssetRef(int index);
	virtual int GetNumberOfSkinnedMeshAnimationAssetRefs();
	virtual AssetRef* GetSpriteSheetAssetRef(int index);
	virtual int GetNumberOfSpriteSheetAssetRefs();
	virtual AssetRef* GetTextureAssetRef(int index);
	virtual int GetNumberOfTextureAssetRefs();
	virtual ActorSpec* GetActorSpec();
	virtual SubActorSpec* GetSubActorSpec(int index);
	virtual int GetNumberOfSubActorSpecs();
	virtual IJsonValue* GetJsonConfig();

private:
	void LoadActorSpecFromJsonValue(IJsonValue* jsonActorSpec);
	void LoadSubActorSpecFromJsonValue(IJsonValue* jsonSubActorSpec);
	void LoadSubActorSpecTriggerSphereDetailsFromJsonValue(IJsonValue* jsonTriggerSphereDetails, SubActorSpec* subActorSpec);
	void LoadSubActorSpecCameraDetailsFromJsonValue(IJsonValue* jsonTriggerSphereDetails, SubActorSpec* subActorSpec);
	void LoadSubActorSpecStaticMeshDetailsFromJsonValue(IJsonValue* jsonTriggerSphereDetails, SubActorSpec* subActorSpec);
	void LoadSubActorSpecSprite3DFromJsonValue(IJsonValue* jsonSprite3DDetails, SubActorSpec* subActorSpec);
	void LoadSubActorSpecVolumeFromJsonValue(IJsonValue* jsonVolumeDetails, SubActorSpec* subActorSpec);
	SubActorType GetSubActorTypeFromTypeName(const char* subActorTypeName);
	VolumeType GetVolumeTypeFromTypeName(const char* volumeTypeName);
	DebugIcon GetDebugIconFromDebugIconName(const char* debugIconName);

	char filePath[AssetMaxFilePathLength];
	bool isStayResident;
	bool loadedSuccessfully;
	bool isEvictable;

	DynamicLengthArray<AssetRef> staticMeshAssetRefs;
	DynamicLengthArray<AssetRef> skinnedMeshAssetRefs;
	DynamicLengthArray<AssetRef> skinnedMeshAnimationAssetRefs;
	DynamicLengthArray<AssetRef> spriteSheetAssetRefs;
	DynamicLengthArray<AssetRef> textureAssetRefs;
	ActorSpec actorSpec;
	DynamicLengthArray<SubActorSpec> subActorSpecs;
	IJsonValue* jsonConfig;
};