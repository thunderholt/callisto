#pragma once

#include "Include/Callisto.h"

class StaticMeshAsset : public IStaticMeshAsset
{
public:
	StaticMeshAsset(const char* filePath, Buffer* fileData, bool isStayResident);
	virtual ~StaticMeshAsset();

	//virtual void BeginLoadingReferencedAssets();
	virtual bool ResolveReferencedAssets();

	virtual char* GetFilePath();
	virtual bool GetIsStayResident();
	virtual bool GetLoadedSuccessfully();
	virtual bool GetIsEvictable();
	virtual void SetIsEvictable(bool isEvictable);

	virtual StaticMeshBuffers* GetBuffers();
	virtual StaticMeshChunk* GetChunk(int index);
	virtual int GetNumberOfChunks();
	virtual AssetRef* GetMaterialAssetRef(int index);
	virtual int GetNumberOfMaterialAssetRefs();
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
	unsigned short* tempIndecies;

	bool bufferAreInitialised;
	bool collisionMeshIsInitialised;

	StaticMeshBuffers buffers;
	DynamicLengthArray<StaticMeshChunk> chunks;
	DynamicLengthArray<AssetRef> materialAssetRefs;
	ICollisionMesh* collisionMesh;
};