#pragma once

#include "Include/Callisto.h"

class WorldMeshAsset : public IWorldMeshAsset
{
public:
	WorldMeshAsset(const char* filePath, Buffer* fileData, bool isStayResident);
	virtual ~WorldMeshAsset();

	virtual bool ResolveReferencedAssets();

	virtual char* GetFilePath();
	virtual bool GetIsStayResident();
	virtual bool GetLoadedSuccessfully();
	virtual bool GetIsEvictable();
	virtual void SetIsEvictable(bool isEvictable);

	virtual WorldMeshBuffers* GetBuffers();
	virtual WorldMeshChunk* GetChunk(int index);
	virtual int GetNumberOfChunks();
	virtual AssetRef* GetMaterialAssetRef(int index);
	virtual int GetNumberOfMaterialAssetRefs();
	virtual AssetRef* GetLightAtlasTextureAssetRef(int index);
	virtual int GetNumberOfLightAtlasTextureAssetRefs();
	//virtual AssetRef* GetLightAtlasTextureAssetRef();
	virtual AssetRef* GetPVSAssetRef();
	virtual ICollisionMesh* GetCollisionMesh();
	
private:
	char filePath[AssetMaxFilePathLength];
	bool isStayResident;
	bool loadedSuccessfully;
	bool isEvictable;

	int numberOfVerts;
	int numberOfIndecies;
	int numberOfFaces;

	float* tempPositions;
	float* tempMaterialTexCoords;
	float* tempLightAtlasTexCoords;
	unsigned short* tempIndecies;

	bool bufferAreInitialised;
	bool collisionMeshIsInitialised;

	WorldMeshBuffers buffers;
	DynamicLengthArray<WorldMeshChunk> chunks;
	DynamicLengthArray<AssetRef> materialAssetRefs;
	DynamicLengthArray<AssetRef> lightAtlasTextureAssetRefs;
	//AssetRef lightAtlasTextureAssetRef;
	AssetRef pvsAssetRef;
	ICollisionMesh* collisionMesh;
};