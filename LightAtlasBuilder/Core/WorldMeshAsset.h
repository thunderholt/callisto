#pragma once

#include "Include/LightAtlasBuilder.h"

struct WorldMeshChunk
{
	int startIndex;
	int numberOfFaces;
	int materialAssetRefIndex;
	Vec2 lightIslandOffset;
	Vec2 lightIslandSize;
};

class WorldMeshAsset : public IWorldMeshAsset
{
public:
	WorldMeshAsset();
	virtual ~WorldMeshAsset();

	virtual bool Load(const char* filePath);
	virtual ICollisionMesh* GetCollisionMesh();
	
private:
	ICollisionMesh* collisionMesh;
};