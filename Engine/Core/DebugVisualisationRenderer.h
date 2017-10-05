#pragma once

#include "Include/Callisto.h"

struct DebugIconData
{
	int spriteSheetAssetIndex;
	int textureAssetIndex;
	int spriteIndexLookUp[DebugIconCount];
};

class DebugVisualisationRenderer : public IDebugVisualisationRenderer
{
public:
	virtual ~DebugVisualisationRenderer();
	virtual bool PostCoreAssetLoadInit();
	virtual void RenderDebugVisualisations(RasterJob* rasterJob);

private:
	bool InitDebugIcons();
	void RenderActorDebugVisualisations(RasterJob* rasterJob);
	void RenderPVSDebugVisualisations(RasterJob* rasterJob);
	void DrawActorAABB(RasterJob* rasterJob, IActor* actor);
	void DrawActorCollisionSphere(RasterJob* rasterJob, IActor* actor);
	void DrawSubActorTriggerSphere(RasterJob* rasterJob, ISubActor* subActor);
	void DrawSubActorTarget(RasterJob* rasterJob, ISubActor* subActor);
	void DrawDebugIcon(RasterJob* rasterJob, DebugIcon icon, Vec3* position);

	DebugIconData debugIcons;
};