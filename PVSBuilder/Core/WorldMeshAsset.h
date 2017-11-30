#pragma once

#include "Include/PVSBuilder.h"

struct WorldMeshChunk
{
	int startIndex;
	int numberOfFaces;
	int materialAssetRefIndex;
	//Vec2 lightIslandOffset;
	//Vec2 lightIslandSize;
};

class WorldMeshAsset : public IWorldMeshAsset
{
public:
	WorldMeshAsset();
	virtual ~WorldMeshAsset();

	virtual bool Load(const char* filePath);
	virtual ICollisionMesh* GetCollisionMesh();
	virtual AABB* GetPvsSectorAABB(int index);
	virtual int GetNumberOfPvsSectorAABBs();
	
private:
	ICollisionMesh* collisionMesh;
	AABB* pvsSectorAABBs;
	int numberOfPvsSectorAABBs;
};