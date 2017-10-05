#include "Core/DebugVisualisationRenderer.h"

DebugVisualisationRenderer::~DebugVisualisationRenderer()
{

}

bool DebugVisualisationRenderer::PostCoreAssetLoadInit()
{
	bool success = false;

	if (this->InitDebugIcons())
	{
		success = true;
	}

	return success;
}

void DebugVisualisationRenderer::RenderDebugVisualisations(RasterJob* rasterJob)
{
	//this->RenderActorDebugVisualisations(rasterJob);
	this->RenderPVSDebugVisualisations(rasterJob);
}

bool DebugVisualisationRenderer::InitDebugIcons()
{
	bool success = false;

	IEngine* engine = GetEngine();
	IAssetManager* assetManager = engine->GetAssetManager();
	ILogger* logger = engine->GetLogger();
	
	this->debugIcons.spriteSheetAssetIndex = assetManager->FindAssetIndex(AssetTypeSpriteSheet, "sprite-sheets/icons/debug.ss");
	if (this->debugIcons.spriteSheetAssetIndex == -1)
	{
		logger->Write("Unable to find debug icon sprite sheet.");
	}
	else
	{
		this->debugIcons.textureAssetIndex = assetManager->FindAssetIndex(AssetTypeTexture, "textures/sprite-sheets/icons/debug.png");
		if (this->debugIcons.textureAssetIndex == -1)
		{
			logger->Write("Unable to find debug icon texture.");
		}
		else
		{
			ISpriteSheetAsset* spriteSheetAsset = engine->GetAssetManager()->GetSpriteSheetAsset(this->debugIcons.spriteSheetAssetIndex);

			this->debugIcons.spriteIndexLookUp[DebugIconWanderToPoint] = spriteSheetAsset->FindSpriteIndex("wander-to-pointer");
			this->debugIcons.spriteIndexLookUp[DebugIconTrigger] = spriteSheetAsset->FindSpriteIndex("trigger");
			this->debugIcons.spriteIndexLookUp[DebugIconEye] = spriteSheetAsset->FindSpriteIndex("eye");
			this->debugIcons.spriteIndexLookUp[DebugIconTarget] = spriteSheetAsset->FindSpriteIndex("target");

			// Check all of the sprites were found.
			bool allSpritesWereFound = true;
			for (int i = 0; i < DebugIconCount; i++)
			{
				if (this->debugIcons.spriteIndexLookUp[i] == -1)
				{
					logger->Write("Missing sprite at index %d", i);
					allSpritesWereFound = false;
					break;
				}
			}

			if (allSpritesWereFound)
			{
				success = true;
			}
		}
	}

	return success;
}

void DebugVisualisationRenderer::RenderActorDebugVisualisations(RasterJob* rasterJob)
{
	IEngine* engine = GetEngine();
	ISceneManager* sceneManager = engine->GetSceneManager();

	for (int actorIndex = 0; actorIndex < sceneManager->GetNumberOfActorIndexes(); actorIndex++)
	{
		IActor* actor = sceneManager->GetActor(actorIndex);
		if (actor != null)
		{
			this->DrawActorAABB(rasterJob, actor);
			this->DrawActorCollisionSphere(rasterJob, actor);

			for (int subActorIndex = 0; subActorIndex < actor->GetNumberOfSubActorIndexes(); subActorIndex++)
			{
				ISubActor* subActor = actor->GetSubActor(subActorIndex);
				if (subActor != null)
				{
					if (subActor->GetDebugIcon() != DebugIconNone)
					{
						this->DrawDebugIcon(rasterJob, subActor->GetDebugIcon(), subActor->GetWorldPosition());
					}

					if (subActor->GetType() == SubActorTypeTriggerSphere)
					{
						this->DrawSubActorTriggerSphere(rasterJob, subActor);
					}
					else if (subActor->GetType() == SubActorTypeTarget)
					{
						this->DrawSubActorTarget(rasterJob, subActor);
					}
				}
			}
		}
	}
}

void DebugVisualisationRenderer::RenderPVSDebugVisualisations(RasterJob* rasterJob)
{
	IEngine* engine = GetEngine();
	IAssetManager* assetManager = engine->GetAssetManager();
	AssetRef* mapAssetRef = engine->GetMapAssetRef();
	IMapAsset* mapAsset = assetManager->GetMapAsset(mapAssetRef->index);
	AssetRef* worldMeshAssetRef = mapAsset->GetWorldMeshAssetRef();
	IWorldMeshAsset* worldMeshAsset = assetManager->GetWorldMeshAsset(worldMeshAssetRef->index);
	AssetRef* pvsAssetRef = worldMeshAsset->GetPVSAssetRef();

	if (pvsAssetRef->index != -1)
	{
		IPVSAsset* pvsAsset = assetManager->GetPVSAsset(pvsAssetRef->index);
		PVSSectorMetrics* sectorMetrics = pvsAsset->GetSectorMetrics();
		int* visibleSectorIndexes = pvsAsset->GetVisibleSectorIndexes();

		for (int sectorIndex = 0; sectorIndex < sectorMetrics->numberOfSectors; sectorIndex++)
		{
			PVSSector* sector = pvsAsset->GetSector(sectorIndex);

			DrawLineCubeRasterJobItem* rasterJobItem = &rasterJob->drawLineCubeJobItems.PushAndGet();
			RgbFloat::Set(&rasterJobItem->unoccludedColour, 1.0f, 0.0f, 0.0f);
			RgbFloat::Set(&rasterJobItem->occludedColour, 0.0f, 0.0f, 1.0f);
			rasterJobItem->position = sector->origin;
			Vec3::Set(&rasterJobItem->size, sectorMetrics->sectorSize, sectorMetrics->sectorSize, sectorMetrics->sectorSize);
		}

		if (rasterJob->commonRasterizationParameters.residentPvsSectorIndex != -1)
		{
			PVSSector* sector = pvsAsset->GetSector(rasterJob->commonRasterizationParameters.residentPvsSectorIndex);
			/*

			DrawLineCubeRasterJobItem* rasterJobItem = &rasterJob->drawLineCubeJobItems.PushAndGet();
			RgbFloat::Set(&rasterJobItem->unoccludedColour, 0.0f, 1.0f, 0.0f);
			RgbFloat::Set(&rasterJobItem->occludedColour, 0.0f, 0.5f, 0.0f);
			rasterJobItem->position = sector->origin;
			Vec3::Set(&rasterJobItem->size, sectorMetrics->sectorSize, sectorMetrics->sectorSize, sectorMetrics->sectorSize);*/

			for (int i = 0; i < sector->numberOfVisibleSectors; i++)
			{
				int sectorIndex = visibleSectorIndexes[sector->visibleSectorIndexesOffset + i];
				PVSSector* sector = pvsAsset->GetSector(sectorIndex);

				DrawLineCubeRasterJobItem* rasterJobItem = &rasterJob->drawLineCubeJobItems.PushAndGet();
				RgbFloat::Set(&rasterJobItem->unoccludedColour, 0.0f, 1.0f, 0.0f);
				RgbFloat::Set(&rasterJobItem->occludedColour, 0.0f, 0.5f, 0.0f);
				rasterJobItem->position = sector->origin;
				Vec3::Set(&rasterJobItem->size, sectorMetrics->sectorSize, sectorMetrics->sectorSize, sectorMetrics->sectorSize);
			}
		}

		/*for (int z = 0; z < sectorMetrics->sectorCounts[2]; z++)
		{
			for (int y = 0; y < sectorMetrics->sectorCounts[1]; y++)
			{
				for (int x = 0; x < sectorMetrics->sectorCounts[0]; x++)
				{
					DrawLineCubeRasterJobItem* rasterJobItem = &rasterJob->drawLineCubeJobItems.PushAndGet();
					Vec3::Set(&rasterJobItem->position, x * sectorMetrics->sectorSize, y * sectorMetrics->sectorSize, z * sectorMetrics->sectorSize);
					Vec3::Add(&rasterJobItem->position, &rasterJobItem->position, &sectorMetrics->sectorOriginOffset);
					Vec3::Set(&rasterJobItem->size, sectorMetrics->sectorSize, sectorMetrics->sectorSize, sectorMetrics->sectorSize);
				}
			}
		}*/
	}
}

void DebugVisualisationRenderer::DrawActorAABB(RasterJob* rasterJob, IActor* actor)
{
	AABB* aabb = actor->GetWorldAABB();

	DrawLineCubeRasterJobItem* rasterJobItem = &rasterJob->drawLineCubeJobItems.PushAndGet();
	rasterJobItem->position = aabb->from;
	AABB::CalculateSize(&rasterJobItem->size, aabb);
}

void DebugVisualisationRenderer::DrawActorCollisionSphere(RasterJob* rasterJob, IActor* actor)
{
	Sphere* sphere = actor->GetWorldCollisionSphere();

	if (sphere->radius > 0.0f)
	{
		DrawLineSphereRasterJobItem* rasterJobItem = &rasterJob->drawLineSphereJobItems.PushAndGet();
		rasterJobItem->position = sphere->position;
		rasterJobItem->radius = sphere->radius;
	}
}

void DebugVisualisationRenderer::DrawSubActorTriggerSphere(RasterJob* rasterJob, ISubActor* subActor)
{
	DrawLineSphereRasterJobItem* rasterJobItem = &rasterJob->drawLineSphereJobItems.PushAndGet();
	rasterJobItem->position = *subActor->GetWorldPosition();
	rasterJobItem->radius = subActor->GetTriggerSphereDetails()->radius;

	this->DrawDebugIcon(rasterJob, DebugIconTrigger, subActor->GetWorldPosition());
}

void DebugVisualisationRenderer::DrawSubActorTarget(RasterJob* rasterJob, ISubActor* subActor)
{
	this->DrawDebugIcon(rasterJob, DebugIconTarget, subActor->GetWorldPosition());
}

void DebugVisualisationRenderer::DrawDebugIcon(RasterJob* rasterJob, DebugIcon icon, Vec3* position)
{
	IEngine* engine = GetEngine();
	IAssetManager* assetManager = engine->GetAssetManager();
	ISpriteSheetAsset* spriteSheetAsset = assetManager->GetSpriteSheetAsset(this->debugIcons.spriteSheetAssetIndex);

	int spriteIndex = this->debugIcons.spriteIndexLookUp[icon];
	Sprite* sprite = spriteSheetAsset->GetSprite(spriteIndex);

	float aspectRatio = sprite->size.x / sprite->size.y;
	float width = 0.05f;
	float height = width / aspectRatio;

	DrawSprite3DRasterJobItem* rasterJobItem = &rasterJob->drawSprite3DJobItems.PushAndGet();
	rasterJobItem->position = *position;
	Vec2::Set(&rasterJobItem->size, width, height);
	rasterJobItem->rotation = 0.0f;
	rasterJobItem->sprite = sprite;
	rasterJobItem->textureAssetIndex = this->debugIcons.textureAssetIndex;
	Vec2::Set(&rasterJobItem->centerOfRotation, rasterJobItem->size.x / 2.0f, rasterJobItem->size.y / 2.0f);
	rasterJobItem->enableZTest = false;
}