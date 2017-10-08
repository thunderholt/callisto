#pragma once

#include "Include/Callisto.h"

/*struct DebugIconData
{
	int spriteSheetAssetIndex;
	int textureAssetIndex;
	int spriteIndexLookUp[DebugIconCount];
};*/

struct RenderStats
{
	int numberOfVisibleWorldMeshChunks;
};

class Renderer : public IRenderer
{
public:
	Renderer();

	//virtual bool PostCoreAssetLoadInit();
	virtual void RenderScene(RasterJob* rasterJob);

private:
	void ClearScreen();
	ISubActor* FindMainCamera();
	void SetupCamera(ISubActor* cameraSubActor);
	void DrawWorldMesh();
	void DrawActors();
	void DrawDebugVisualisations();
	void DrawHUD();
	void DrawStats();
	void DrawString(const char* str, Vec2 postion, int fontSpriteSheetAssetIndex, RgbaFloat tint);
	void DrawSprite3D(int spriteSheetAssetIndex, int textureAssetIndex, int spriteIndex, Vec3* position, float width, float height, bool enableZTest);
	void DrawLineSphere(Vec3* position, float radius);
	void DrawLineCube(Vec3* position, Vec3* size);
	int FindResidentSectorIndexFromWorldPosition(Vec3* worldPosition);

	RasterJob* rasterJob;
	//DebugIconData debugIcons;
	bool drawDebugVisualisations;
	bool drawVolumes;
	RenderStats stats;
};