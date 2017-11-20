#include "Core/Renderer.h"

Renderer::Renderer()
{
	this->rasterJob = null;
	this->drawDebugVisualisations = false;
	this->drawVolumes = false;
}

/*bool Renderer::PostCoreAssetLoadInit()
{
	bool success = false;

	if (this->InitDebugIcons())
	{
		success = true;
	}

	return success;
}*/

void Renderer::RenderScene(RasterJob* rasterJob)
{
	this->rasterJob = rasterJob;

	this->rasterJob->drawWorldMeshChunkJobItems.Clear();
	this->rasterJob->drawStaticMeshJobItems.Clear();
	this->rasterJob->drawSkinnedMeshJobItems.Clear();
	this->rasterJob->drawVolumeRasterJobItems.Clear();
	this->rasterJob->drawSprite2DJobItems.Clear();
	this->rasterJob->drawSprite3DJobItems.Clear();
	this->rasterJob->drawLineSphereJobItems.Clear();
	this->rasterJob->drawLineCubeJobItems.Clear();

	this->ClearScreen();

	ISubActor* cameraSubActor = this->FindMainCamera();
	if (cameraSubActor != null)
	{
		this->SetupCamera(cameraSubActor);
		this->DrawWorldMesh();
		this->DrawActors();

		if (this->drawDebugVisualisations)
		{
			this->DrawDebugVisualisations();
		}
	}

	this->DrawHUD();
	this->DrawStats();
}

void Renderer::ClearScreen()
{
	RgbaFloat::SetBlack(&this->rasterJob->clearColour);
}

ISubActor* Renderer::FindMainCamera()
{
	IEngine* engine = GetEngine();
	ISceneManager* sceneManager = engine->GetSceneManager();

	ISubActor* cameraSubActor = sceneManager->GetMainCameraSubActor();
	return cameraSubActor;
}

void Renderer::SetupCamera(ISubActor* cameraSubActor)
{
	IEngine* engine = GetEngine();
	IRasterizer* rasterizer = engine->GetRasterizer();

	CommonRastorizationParameters* commonRasterizationParameters = &this->rasterJob->commonRasterizationParameters;
	Mat4* viewTransform = &commonRasterizationParameters->viewTransform;
	Mat4* projTransform = &commonRasterizationParameters->projTransform;
	Mat4* viewProjTransform = &commonRasterizationParameters->viewProjTransform;
	Vec2* viewportSize = rasterizer->GetViewportSize();
	float viewportAspectRatio = viewportSize->x / viewportSize->y;

	Mat4::Invert(viewTransform, cameraSubActor->GetWorldTransform());
	Mat4::CreatePerspective(projTransform, cameraSubActor->GetCameraDetails()->fov, viewportAspectRatio, 0.01f, 1000.0f); // FIXME
	Mat4::Multiply(viewProjTransform, &this->rasterJob->commonRasterizationParameters.projTransform, &rasterJob->commonRasterizationParameters.viewTransform);

	commonRasterizationParameters->cameraWorldPosition = *cameraSubActor->GetWorldPosition();

	commonRasterizationParameters->cameraResidentPvsSectorIndex =
		this->FindResidentSectorIndexFromWorldPosition(&commonRasterizationParameters->cameraWorldPosition);
}

void Renderer::DrawWorldMesh()
{
	IEngine* engine = GetEngine();
	IAssetManager* assetManager = engine->GetAssetManager();

	AssetRef* mapAssetRef = engine->GetMapAssetRef();
	IMapAsset* mapAsset = assetManager->GetMapAsset(mapAssetRef->index);
	AssetRef*  worldMeshAssetRef = mapAsset->GetWorldMeshAssetRef();
	IWorldMeshAsset* worldMeshAsset = assetManager->GetWorldMeshAsset(worldMeshAssetRef->index);
	AssetRef* pvsAssetRef = worldMeshAsset->GetPVSAssetRef();

	this->rasterJob->worldMeshAssetIndex = worldMeshAssetRef->index;
	this->stats.numberOfVisibleWorldMeshChunks = 0;

	if (pvsAssetRef->index != -1 && this->rasterJob->commonRasterizationParameters.cameraResidentPvsSectorIndex != -1)
	{
		IPVSAsset* pvsAsset = assetManager->GetPVSAsset(pvsAssetRef->index);
		
		this->DrawWorldMeshChunksWithPvsSectors(worldMeshAsset, pvsAsset);
	}
	else
	{
		this->DrawWorldMeshChunksWithoutPvsSectors(worldMeshAsset);
	}
}

void Renderer::DrawWorldMeshChunksWithPvsSectors(IWorldMeshAsset* worldMeshAsset, IPVSAsset* pvsAsset)
{
	IEngine* engine = GetEngine();

	int frameId = engine->GetFrameId();

	PVSSector* residentSector = pvsAsset->GetSector(this->rasterJob->commonRasterizationParameters.cameraResidentPvsSectorIndex);
	int* visibleSectorIndexes = pvsAsset->GetVisibleSectorIndexes();
	int* residentWorldMeshChunkIndexes = pvsAsset->GetResidentWorldMeshChunkIndexes();

	// Loop through all of the visible sectors.
	for (int i = 0; i < residentSector->numberOfVisibleSectors; i++)
	{
		int sectorIndex = visibleSectorIndexes[residentSector->visibleSectorIndexesOffset + i];
		PVSSector* visibleSector = pvsAsset->GetSector(sectorIndex);

		// Loop through all of the resident world mesh chunks in the visible sector.
		for (int j = 0; j < visibleSector->numberOfResidentWorldMeshChunkIndexes; j++)
		{
			int chunkIndex = residentWorldMeshChunkIndexes[visibleSector->residentWorldMeshChunkIndexesOffset + j];
			WorldMeshChunk* chunk = worldMeshAsset->GetChunk(chunkIndex);

			if (chunk != null)
			{
				// TODO - Frustum cull.

				// If the chunk hasn't been rendered this frame yet, render it.
				if (chunk->lastRenderedFrameId != frameId)
				{
					DrawWorldMeshChunkRasterJobItem* drawWorldMeshChunkRasterJobItem = &this->rasterJob->drawWorldMeshChunkJobItems.PushAndGet();
					drawWorldMeshChunkRasterJobItem->chunkIndex = chunkIndex;

					chunk->lastRenderedFrameId = frameId;
					this->stats.numberOfVisibleWorldMeshChunks++;
				}
			}
		}
	}
}

void Renderer::DrawWorldMeshChunksWithoutPvsSectors(IWorldMeshAsset* worldMeshAsset)
{
	for (int chunkIndex = 0; chunkIndex < worldMeshAsset->GetNumberOfChunks(); chunkIndex++)
	{
		DrawWorldMeshChunkRasterJobItem* drawWorldMeshChunkRasterJobItem = &rasterJob->drawWorldMeshChunkJobItems.PushAndGet();
		drawWorldMeshChunkRasterJobItem->chunkIndex = chunkIndex;
		this->stats.numberOfVisibleWorldMeshChunks++;
	}
}

void Renderer::DrawActors()
{
	IEngine* engine = GetEngine();
	ISceneManager* sceneManager = engine->GetSceneManager();

	// TODO - Only render visible actors.
	for (int actorIndex = 0; actorIndex < sceneManager->GetNumberOfActorIndexes(); actorIndex++)
	{
		IActor* actor = sceneManager->GetActor(actorIndex);
		if (actor != null)
		{
			/*//------ Test code -----
			DrawVolumeRasterJobItem* drawVolumeRasterJobItem = &rasterJob->drawVolumeRasterJobItems.PushAndGet();
			drawVolumeRasterJobItem->volumeIndex = 0;
			drawVolumeRasterJobItem->worldTransform = actor->GetLocationValues()->worldTransform;
			//----------------------*/

			for (int subActorIndex = 0; subActorIndex < actor->GetNumberOfSubActorIndexes(); subActorIndex++)
			{
				ISubActor* subActor = actor->GetSubActor(subActorIndex);
				if (subActor != null)
				{
					if (subActor->GetType() == SubActorTypeStaticMesh)
					{
						DrawStaticMeshRasterJobItem* drawStaticMeshRasterJobItem = &rasterJob->drawStaticMeshJobItems.PushAndGet();
						drawStaticMeshRasterJobItem->staticMeshAssetIndex = subActor->GetStaticMeshDetails()->staticMeshAssetIndex;
						drawStaticMeshRasterJobItem->worldTransform = *subActor->GetWorldTransform();
					}
					else if (subActor->GetType() == SubActorTypeSprite3D)
					{
						SubActorSprite3DDetails* subActorSprite3DDetails = subActor->GetSprite3DDetails();
			
						this->DrawSprite3D(
							subActorSprite3DDetails->spriteSheetAssetIndex,
							subActorSprite3DDetails->textureAssetIndex,
							subActorSprite3DDetails->spriteIndex,
							subActor->GetWorldPosition(),
							subActorSprite3DDetails->size.x, subActorSprite3DDetails->size.y,
							true);
					}
					else if (subActor->GetType() == SubActorTypeVolume && this->drawVolumes)
					{
						SubActorVolumeDetails* subActorVolumeDetails = subActor->GetVolumeDetails();

						DrawVolumeRasterJobItem* drawVolumeRasterJobItem = &rasterJob->drawVolumeRasterJobItems.PushAndGet();
						drawVolumeRasterJobItem->volumeIndex = subActorVolumeDetails->volumeIndex;
						drawVolumeRasterJobItem->worldTransform = *subActor->GetWorldTransform();
					}
				}
			}
		}
	}
}

void Renderer::DrawDebugVisualisations()
{
	IEngine* engine = GetEngine();
	IDebugVisualisationRenderer* debugVisualisationRenderer = engine->GetDebugVisualisationRenderer();

	debugVisualisationRenderer->RenderDebugVisualisations(this->rasterJob);
	/*ISceneManager* sceneManager = engine->GetSceneManager();

	//ISpriteSheetAsset* debugIconsSpriteSheetAsset = engine->GetAssetManager()->GetSpriteSheetAsset(this->debugIcons.spriteSheetAssetIndex);

	for (int actorIndex = 0; actorIndex < sceneManager->GetNumberOfActorIndexes(); actorIndex++)
	{
		IActor* actor = sceneManager->GetActor(actorIndex);
		if (actor != null)
		{
			for (int debugVisualisationIndex = 0; debugVisualisationIndex < actor->GetNumberOfDebugVisualisationIndexes(); debugVisualisationIndex++)
			{
				ActorDebugVisualisation* actorDebugVisualisation = actor->GetDebugVisualisation(debugVisualisationIndex);
				if (actorDebugVisualisation != null && actorDebugVisualisation->enabled)
				{
					if (actorDebugVisualisation->type == ActorDebugVisualisationTypeIcon)
					{
						int spriteIndex = this->debugIcons.spriteIndexLookUp[actorDebugVisualisation->icon];
						//Sprite* sprite = debugIconsSpriteSheetAsset->GetSprite(spriteIndex);
						//float spriteScaleFactor = 0.001f;

						this->DrawSprite3D(
							this->debugIcons.spriteSheetAssetIndex,
							this->debugIcons.textureAssetIndex,
							spriteIndex,
							&actorDebugVisualisation->position,
							//sprite->size.x * spriteScaleFactor, sprite->size.y * spriteScaleFactor,
							0.05f, 0.0f,
							false);
					}
					else if (actorDebugVisualisation->type == ActorDebugVisualisationTypeLineSphere)
					{
						this->DrawLineSphere(
							&actorDebugVisualisation->position, 
							actorDebugVisualisation->radius);
					}
					else if (actorDebugVisualisation->type == ActorDebugVisualisationTypeLineCube)
					{
						this->DrawLineCube(
							&actorDebugVisualisation->position,
							&actorDebugVisualisation->size);
					}
				}
			}
		}
	}*/
}

void Renderer::DrawHUD()
{
	IEngine* engine = GetEngine();
	ITouchScreenGamePad* touchScreenGamePad = engine->GetTouchScreenGamePad();

	TouchScreenGamePadStick* rightStick = touchScreenGamePad->GetRightStick();
	this->DrawString("F", rightStick->buttonPosition, 0, RgbaFloat::GetWhite());

	/*this->DrawString("L", *touchScreenGamePad->GetLeftStickOrigin(), 0, RgbaFloat::GetWhite());
	this->DrawString("R", *touchScreenGamePad->GetRightStickOrigin(), 0, RgbaFloat::GetWhite());
	this->DrawString("F", *touchScreenGamePad->GetFireStickOrigin(), 0, RgbaFloat::GetWhite());*/
}

void Renderer::DrawStats()
{
	IEngine* engine = GetEngine();
	IFrameTimer* frameTimer = engine->GetFrameTimer();

	char message[128];

	// Display the current FPS.
	sprintf(message, "FPS: %.2f", frameTimer->GetCurrentFPS());
	this->DrawString(message, Vec2::Create(10, 10), 0, RgbaFloat::GetWhite());

	// Display the min FPS.
	sprintf(message, "Min FPS: %.2f", frameTimer->GetMinFPS());
	this->DrawString(message, Vec2::Create(10, 30), 0, RgbaFloat::GetWhite());

	// Display the max FPS.
	sprintf(message, "Max FPS: %.2f", frameTimer->GetMaxFPS());
	this->DrawString(message, Vec2::Create(10, 50), 0, RgbaFloat::GetWhite());

	// Display the average FPS.
	sprintf(message, "Average FPS: %.2f", frameTimer->GetAverageFPS());
	this->DrawString(message, Vec2::Create(10, 70), 0, RgbaFloat::GetWhite());

	// Display the number of FPS hitches.
	sprintf(message, "FPS hitches: %d/%d", frameTimer->GetNumberOfFpsHitches(), frameTimer->GetNumberOfFrames());
	this->DrawString(message, Vec2::Create(10, 90), 0, RgbaFloat::GetWhite());

	// Display the number of FPS hitch rate.
	sprintf(message, "FPS hitch rate: %.4f%%", frameTimer->GetFpsHitchRate());
	this->DrawString(message, Vec2::Create(10, 110), 0, RgbaFloat::GetWhite());

	// Display the number of FPS hitch rate.
	sprintf(message, "Visible World Mesh Chunks: %d", this->stats.numberOfVisibleWorldMeshChunks);
	this->DrawString(message, Vec2::Create(10, 130), 0, RgbaFloat::GetWhite());

	

	/*// Display the mouse movement offset.
	sprintf(message, "Mouse movement offset: %f,%f", mouse->GetMovementOffset()->x, mouse->GetMovementOffset()->y);
	this->DrawString(message, Vec2::Create(10, 130), 0, RgbaFloat::GetWhite());*/
}

void Renderer::DrawString(const char* str, Vec2 postion, int fontSpriteSheetAssetIndex, RgbaFloat tint)
{
	IEngine* engine = GetEngine();
	IAssetManager* assetManager = engine->GetAssetManager();

	ISpriteSheetAsset* fontSpriteSheet = assetManager->GetSpriteSheetAsset(fontSpriteSheetAssetIndex);

	for (int i = 0; ; i++)
	{
		char c = str[i];
		if (c == null)
		{
			break;
		}

		Sprite* sprite = fontSpriteSheet->GetSprite(c - 32);

		DrawSprite2DRasterJobItem& rasterJobItem = this->rasterJob->drawSprite2DJobItems.PushAndGet();
		rasterJobItem.position = postion;
		rasterJobItem.rotation = 0.0f;
		rasterJobItem.size = sprite->size;
		//Vec2::Set(drawSpriteJobItem.size, 400.0f, 100.0f);// sprite->size;
		rasterJobItem.sprite = sprite;
		rasterJobItem.textureAssetIndex = 0; // TODO - get correct index.
		rasterJobItem.tint = tint;

		postion.x += sprite->size.x;
	}
}

void Renderer::DrawSprite3D(int spriteSheetAssetIndex, int textureAssetIndex, int spriteIndex, Vec3* position, float width, float height, bool enableZTest)
{
	IEngine* engine = GetEngine();
	IAssetManager* assetManager = engine->GetAssetManager();
	ISpriteSheetAsset* spriteSheetAsset = assetManager->GetSpriteSheetAsset(spriteSheetAssetIndex);
	Sprite* sprite = spriteSheetAsset->GetSprite(spriteIndex);
	float aspectRatio = sprite->size.x / sprite->size.y;

	if (width <= 0.0f)
	{
		width = aspectRatio * height;
	}
	else if (height <= 0.0f)
	{
		height = width / aspectRatio;
	}

	DrawSprite3DRasterJobItem* rasterJobItem = &this->rasterJob->drawSprite3DJobItems.PushAndGet();
	rasterJobItem->position = *position;
	Vec2::Set(&rasterJobItem->size, width, height);
	rasterJobItem->rotation = 0.0f;
	rasterJobItem->sprite = sprite;
	rasterJobItem->textureAssetIndex = textureAssetIndex;
	Vec2::Set(&rasterJobItem->centerOfRotation, rasterJobItem->size.x / 2.0f, rasterJobItem->size.y / 2.0f);
	rasterJobItem->enableZTest = enableZTest;
}

void Renderer::DrawLineSphere(Vec3* position, float radius)
{
	DrawLineSphereRasterJobItem* rasterJobItem = &rasterJob->drawLineSphereJobItems.PushAndGet();
	rasterJobItem->position = *position;
	rasterJobItem->radius = radius;
}

void Renderer::DrawLineCube(Vec3* position, Vec3* size)
{
	DrawLineCubeRasterJobItem* rasterJobItem = &rasterJob->drawLineCubeJobItems.PushAndGet();
	rasterJobItem->position = *position;
	rasterJobItem->size = *size;
}

int Renderer::FindResidentSectorIndexFromWorldPosition(Vec3* worldPosition)
{
	IEngine* engine = GetEngine();
	IAssetManager* assetManager = engine->GetAssetManager();
	AssetRef* mapAssetRef = engine->GetMapAssetRef();
	IMapAsset* mapAsset = assetManager->GetMapAsset(mapAssetRef->index);
	AssetRef* worldMeshAssetRef = mapAsset->GetWorldMeshAssetRef();
	IWorldMeshAsset* worldMeshAsset = assetManager->GetWorldMeshAsset(worldMeshAssetRef->index);
	AssetRef* pvsAssetRef = worldMeshAsset->GetPVSAssetRef();

	int residentSectorIndex = -1;

	if (pvsAssetRef->index != -1)
	{
		IPVSAsset* pvsAsset = assetManager->GetPVSAsset(pvsAssetRef->index);
		/*PVSSectorMetrics* sectorMetrics = pvsAsset->GetSectorMetrics();

		int xSectorIndex = (int)floorf((worldPosition->x - sectorMetrics->sectorOriginOffset.x) / sectorMetrics->sectorSize);
		int ySectorIndex = (int)floorf((worldPosition->y - sectorMetrics->sectorOriginOffset.y) / sectorMetrics->sectorSize);
		int zSectorIndex = (int)floorf((worldPosition->z - sectorMetrics->sectorOriginOffset.z) / sectorMetrics->sectorSize);

		if (xSectorIndex >= 0 && xSectorIndex < sectorMetrics->sectorCounts[0] &&
			ySectorIndex >= 0 && ySectorIndex < sectorMetrics->sectorCounts[1] &&
			zSectorIndex >= 0 && zSectorIndex < sectorMetrics->sectorCounts[2])
		{
			residentSectorIndex =
				(zSectorIndex * sectorMetrics->sectorCounts[1] * sectorMetrics->sectorCounts[0]) +
				(ySectorIndex * sectorMetrics->sectorCounts[0]) +
				xSectorIndex;
		}*/
		for (int sectorIndex = 0; sectorIndex < pvsAsset->GetNumberOfSectors(); sectorIndex++)
		{
			PVSSector* sector = pvsAsset->GetSector(sectorIndex);

			if (AABB::CheckContainsPoint(&sector->aabb, worldPosition))
			{
				residentSectorIndex = sectorIndex;
				break;
			}
		}
	}

	return residentSectorIndex;
}

/*bool Renderer::InitDebugIcons()
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
}*/