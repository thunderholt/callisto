#pragma once

#include "Include/LightAtlasBuilder.h"

struct WorldMeshChunk
{
	int startIndex;
	int numberOfFaces;
	int materialAssetRefIndex;
	//Vec2i lightIslandOffset;
	//Vec2i lightIslandSize;
};

class WorldMeshAsset : public IWorldMeshAsset
{
public:
	WorldMeshAsset();
	virtual ~WorldMeshAsset();

	virtual bool Load(const char* filePath);
	virtual ICollisionMesh* GetCollisionMesh();
	virtual WorldMeshLightIsland* GetLightIsland(int lightIslandIndex);
	virtual WorldMeshLightIsland* FindLightIslandForChunk(int chunkIndex);
	virtual int GetNumberOfLightIslands();
	
private:
	ICollisionMesh* collisionMesh;
	WorldMeshLightIsland* lightIslands;
	int numberOfLightIslands;
};