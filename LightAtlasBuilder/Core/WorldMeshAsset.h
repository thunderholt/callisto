#pragma once

#include "Include/LightAtlasBuilder.h"

class WorldMeshAsset : public IWorldMeshAsset
{
public:
	WorldMeshAsset();
	virtual ~WorldMeshAsset();

	virtual bool Load(const char* filePath);
	virtual ICollisionMesh* GetCollisionMesh();
	virtual WorldMeshLightAtlas* GetLightAtlas(int lightAtlasIndex);
	virtual int GetNumberOfLightAtlases();
	virtual WorldMeshLightIsland* GetLightIsland(int lightIslandIndex);
	virtual WorldMeshLightIsland* FindLightIslandForChunk(int chunkIndex);
	virtual int GetNumberOfLightIslands();
	virtual WorldMeshLight* GetLight(int lightIndex);
	virtual int GetNumberOfLights();
	
private:
	ICollisionMesh* collisionMesh;
	WorldMeshLightAtlas* lightAtlases;
	int numberOfLightAtlases;
	WorldMeshLightIsland* lightIslands;
	int numberOfLightIslands;
	WorldMeshLight* lights;
	int numberOfLights;
};