#include <time.h>
#include "Core/Engine.h"

Engine* engine;

void CreateEngine()
{
	engine = new Engine();
}

void DestroyEngine()
{
	delete engine;
}

IEngine* GetEngine()
{
	return engine;
}

Engine::Engine()
{
	IFactory* factory = GetFactory();

	this->logger = factory->MakeLogger();
	this->worldMeshAsset = factory->MakeWorldMeshAsset();
	this->rayTracer = factory->MakeRayTracer();
	this->threadManager = factory->MakeThreadManager();
	this->timestampProvider = factory->MakeTimestampProvider();
	this->currentLightAtlas = factory->MakeLightAtlas();

	this->config.indirectIlluminationHemispehereCircleCount = 16;
	this->config.numberOfIndirectIlluminationSamplesPerBounce = 200;
	this->config.numberOfIndirectIlluminationBounces = 2;

#ifdef _DEBUG
	int numberOfWorkers = 1;
#endif
#ifndef _DEBUG
	int numberOfWorkers = 4;
#endif

	for (int i = 0; i < numberOfWorkers; i++)
	{
		this->workers.Push(factory->MakeWorker());
	}
}

Engine::~Engine()
{
	SafeDeleteAndNull(this->logger);
	SafeDeleteAndNull(this->worldMeshAsset);
	SafeDeleteAndNull(this->rayTracer);
	SafeDeleteAndNull(this->threadManager);
	SafeDeleteAndNull(this->timestampProvider);
	
	for (int i = 0; i < this->workers.GetLength(); i++)
	{
		SafeDeleteAndNull(this->workers[i]);
	}

	SafeDeleteAndNull(this->currentLightAtlas);
}

void Engine::BuildLightAtlases(const char* worldMeshAssetFilePath, const char* assetsFolderPath)
{
	double startTime = this->timestampProvider->GetTimestampMillis();

	srand((int)time(NULL));

	this->assetsFolderPath = assetsFolderPath;

	this->logger->Write("Loading '%s'...", worldMeshAssetFilePath);

	if (!this->worldMeshAsset->Load(worldMeshAssetFilePath))
	{
		this->logger->Write("... failed.");
	}
	else
	{
		this->logger->Write("... loaded world mesh asset.");

		for (int lightAtlasIndex = 0; lightAtlasIndex < this->worldMeshAsset->GetNumberOfLightAtlases(); lightAtlasIndex++)
		{
			this->logger->Write("****** Computing light atlas %d/%d ******", lightAtlasIndex + 1, this->worldMeshAsset->GetNumberOfLightAtlases());

			WorldMeshLightAtlas* worldMeshLightAtlas = this->worldMeshAsset->GetLightAtlas(lightAtlasIndex);

			this->currentLightAtlas->Allocate(worldMeshLightAtlas->size.x, worldMeshLightAtlas->size.y);

			this->ComputeLightIslandsOnWorkers(lightAtlasIndex);

			this->WriteOutputFile(lightAtlasIndex);

			this->logger->Write("Light atlas computed.");
		}
	}

	double endTime = this->timestampProvider->GetTimestampMillis();
	double durationMillis = endTime - startTime;
	double durationSeconds = durationMillis / 1000.0;

	this->logger->Write("Duration: %f seconds.", durationSeconds);
}


ILogger* Engine::GetLogger()
{
	return this->logger;
}

IWorldMeshAsset* Engine::GetWorldMeshAsset()
{
	return this->worldMeshAsset;
}

ILightAtlas* Engine::GetCurrentLightAtlas()
{
	return this->currentLightAtlas;
}

IRayTracer* Engine::GetRayTracer()
{
	return this->rayTracer;
}

IThreadManager* Engine::GetThreadManager()
{
	return this->threadManager;
}

ITimestampProvider* Engine::GetTimestampProvider()
{
	return this->timestampProvider;
}

const char* Engine::GetAssetsFolderPath()
{
	return this->assetsFolderPath;
}

const Config* Engine::GetConfig()
{
	return &this->config;
}

void Engine::ComputeLightIslandsOnWorkers(int lightAtlasIndex)
{
	ICollisionMesh* collisionMesh = this->worldMeshAsset->GetCollisionMesh();

	// Find the light islands that are part of the current light atlas.
	this->logger->Write("Finding light islands that are part of the current light atlas...");

	BitField effectedLightIslandsFieldForCurrentLightAtlas;
	effectedLightIslandsFieldForCurrentLightAtlas.Reset(this->worldMeshAsset->GetNumberOfLightIslands());

	for (int lightIslandIndex = 0; lightIslandIndex < this->worldMeshAsset->GetNumberOfLightIslands(); lightIslandIndex++)
	{
		WorldMeshLightIsland* lightIsland = this->worldMeshAsset->GetLightIsland(lightIslandIndex);
		if (lightIsland->lightAtlasIndex == lightAtlasIndex)
		{
			effectedLightIslandsFieldForCurrentLightAtlas.SetBit(lightIslandIndex);
		}
	}

	this->logger->Write("... done.");

	// Gather the light island indexes that are part of the current light atlas.
	this->logger->Write("Gathering light island indexes that are part of the current light atlas...");

	int* effectedLightIslandIndexes = new int[this->worldMeshAsset->GetNumberOfLightIslands()];
	memset(effectedLightIslandIndexes, 0, sizeof(int) * this->worldMeshAsset->GetNumberOfLightIslands());
	int numberOfEffectedLightIslandIndexes = 0;

	for (int lightIslandIndex = 0; lightIslandIndex < this->worldMeshAsset->GetNumberOfLightIslands(); lightIslandIndex++)
	{
		if (effectedLightIslandsFieldForCurrentLightAtlas.GetBit(lightIslandIndex))
		{
			effectedLightIslandIndexes[numberOfEffectedLightIslandIndexes] = lightIslandIndex;
			numberOfEffectedLightIslandIndexes++;
		}
	}

	this->logger->Write("... done. %d light islands are effected.", numberOfEffectedLightIslandIndexes);

	// Find the effected chunks for the current light atlas.
	this->logger->Write("Finding effected chunks for the current light atlas...");

	BitField effectedChunksFieldForCurrentLightAtlas;
	effectedChunksFieldForCurrentLightAtlas.Reset(collisionMesh->GetNumberOfChunks());

	for (int chunkIndex = 0; chunkIndex < collisionMesh->GetNumberOfChunks(); chunkIndex++)
	{
		CollisionMeshChunk* chunk = collisionMesh->GetChunk(chunkIndex);
		if (chunk->lightAtlasIndex == lightAtlasIndex)
		{
			effectedChunksFieldForCurrentLightAtlas.SetBit(chunkIndex);
		}
	}

	this->logger->Write("... done.");

	// Assign the light island indexes to the workers.
	this->logger->Write("Assigning light island indexes to workers...");

	int numberOfLightIslandsToCrunchPerWorker = (int)ceilf(numberOfEffectedLightIslandIndexes / (float)this->workers.GetLength());

	for (int workerIndex = 0; workerIndex < this->workers.GetLength(); workerIndex++)
	{
		IWorker* worker = this->workers[workerIndex];

		int offset = numberOfLightIslandsToCrunchPerWorker * workerIndex;
		if (offset < numberOfEffectedLightIslandIndexes)
		{
			int count = numberOfLightIslandsToCrunchPerWorker;
			if (offset + count > numberOfEffectedLightIslandIndexes)
			{
				count = numberOfEffectedLightIslandIndexes - offset;
			}

			worker->SetCurrentLightAtlasDetails(&effectedLightIslandIndexes[offset], count, &effectedChunksFieldForCurrentLightAtlas);

			this->logger->Write("Worker %d - Offset: %d, Count: %d.", workerIndex + 1, offset, count);
		}
		else
		{
			worker->SetCurrentLightAtlasDetails(null, 0, null);
		}
	}

	this->logger->Write("... done.");

	// Compute the lumel basic data on each of the workers.
	this->logger->Write("Computing basic lumel data...");

	for (int workerIndex = 0; workerIndex < this->workers.GetLength(); workerIndex++)
	{
		IWorker* worker = this->workers[workerIndex];

		worker->ComputeLumelBasicDataAsync();
	}

	this->WaitForAllWorkersToFinish();

	this->logger->Write("... done.");

	// Compute the lumel bounce targets on each of the workers.
	this->logger->Write("Computing bounce targets...");

	for (int workerIndex = 0; workerIndex < this->workers.GetLength(); workerIndex++)
	{
		IWorker* worker = this->workers[workerIndex];

		worker->ComputeLumelBounceTargetsAsync();
	}

	this->WaitForAllWorkersToFinish();

	this->logger->Write("... done.");

	BitField effectedChunksFieldForCurrentLight;

	for (int lightIndex = 0; lightIndex < this->worldMeshAsset->GetNumberOfLights(); lightIndex++)
	{
		this->logger->Write("****** Computing light %d/%d ******", lightIndex + 1, this->worldMeshAsset->GetNumberOfLights());

		WorldMeshLight* worldMeshLight = this->worldMeshAsset->GetLight(lightIndex);

		// Expand the light.
		this->logger->Write("Expanding light...");

		ExpandedLight expandedLight;
		this->ExpandLight(&expandedLight, worldMeshLight);

		this->logger->Write("... done.");

		// Reset the lumel intensity cache.
		this->logger->Write("Resetting lumel intensity cache...");

		this->currentLightAtlas->ResetLumelIntensityCacheItems();

		this->logger->Write("... done.");

		// Find the chunks effected by the light.
		this->logger->Write("Finding chunks effected by current light...");

		this->BuildEffectedChunksFieldForLight(&effectedChunksFieldForCurrentLight, &expandedLight);

		this->logger->Write("Done. %d chunks are effected.", effectedChunksFieldForCurrentLight.CountSetBits());

		// Set the current light on each of the workers.
		this->logger->Write("Setting current light on workers...");

		for (int workerIndex = 0; workerIndex < this->workers.GetLength(); workerIndex++)
		{
			IWorker* worker = this->workers[workerIndex];

			worker->SetCurrentLight(&expandedLight, &effectedChunksFieldForCurrentLight);
		}

		this->logger->Write("... done.");

		// Compute the direct illumination for the current light.
		this->logger->Write("Computing direct illumination...");

		for (int workerIndex = 0; workerIndex < this->workers.GetLength(); workerIndex++)
		{
			IWorker* worker = this->workers[workerIndex];

			worker->ComputeDirectIlluminationIntensitiesForCurrentLightAsync();
		}

		this->WaitForAllWorkersToFinish();

		this->logger->Write("... done.");
	
		// Compute the indirect illumination for the current light.
		this->logger->Write("Computing indirect illumination...");

		for (int bounceNumber = 1; bounceNumber <= this->config.numberOfIndirectIlluminationBounces; bounceNumber++)
		{
			for (int workerIndex = 0; workerIndex < this->workers.GetLength(); workerIndex++)
			{
				IWorker* worker = this->workers[workerIndex];

				worker->ComputeIndirectIlluminationBounceIntensitiesForCurrentLightAsync(bounceNumber);
			}

			this->WaitForAllWorkersToFinish();
		}

		this->logger->Write("... done.");

		// Accumulate the lumel colours.
		this->logger->Write("Accumulating lumel colours...");

		this->currentLightAtlas->AccumulateLumelColoursFromIntensityCacheItems(&worldMeshLight->colour);

		this->logger->Write("... done.");

		this->logger->Write("Light computed.");
	}

	this->logger->Write("Filling borders...");

	for (int workerIndex = 0; workerIndex < this->workers.GetLength(); workerIndex++)
	{
		IWorker* worker = this->workers[workerIndex];

		worker->FillBordersAsync();
	}

	this->WaitForAllWorkersToFinish();

	this->logger->Write("... done.");

	SafeDeleteArrayAndNull(effectedLightIslandIndexes);
}

void Engine::WaitForAllWorkersToFinish()
{
	bool allWorkersAreFinished = true;

	do
	{
		allWorkersAreFinished = true;

		for (int i = 0; i < this->workers.GetLength(); i++)
		{
			IWorker* worker = this->workers[i];
			if (!worker->GetHasFinished())
			{
				allWorkersAreFinished = false;
				break;
			}
		}

		if (!allWorkersAreFinished)
		{
			this->threadManager->Sleep(10);
		}

	} while (!allWorkersAreFinished);
}

void Engine::ExpandLight(ExpandedLight* out, WorldMeshLight* worldMeshLight)
{
	out->type = worldMeshLight->type;
	out->distance = worldMeshLight->distance;
	out->distanceSqr = worldMeshLight->distance * worldMeshLight->distance;

	if (worldMeshLight->type == LightTypePoint)
	{
		// Build the intensity points.
		out->intensityPoints[0] = worldMeshLight->position;
		out->numberOfIntensityPoints = 1;

		// Build the shadow points.
		float shadowBlurZoneSize = 0.1f;

		Vec3 shadowBlurZoneStart = worldMeshLight->position;
		Vec3::ScaleAndAdd(&shadowBlurZoneStart, &shadowBlurZoneStart, &worldMeshLight->iAxis, -shadowBlurZoneSize / 2.0f);
		Vec3::ScaleAndAdd(&shadowBlurZoneStart, &shadowBlurZoneStart, &worldMeshLight->jAxis, -shadowBlurZoneSize / 2.0f);
		Vec3::ScaleAndAdd(&shadowBlurZoneStart, &shadowBlurZoneStart, &worldMeshLight->kAxis, -shadowBlurZoneSize / 2.0f);

		float step = shadowBlurZoneSize / 4.0f;

		out->numberOfShadowPoints = 0;
		for (int z = 0; z < 4; z++)
		{
			for (int y = 0; y < 4; y++)
			{
				for (int x = 0; x < 4; x++)
				{
					Vec3* point = &out->shadowPoints[out->numberOfShadowPoints];
					*point = shadowBlurZoneStart;
					Vec3::ScaleAndAdd(point, point, &worldMeshLight->kAxis, z * step);
					Vec3::ScaleAndAdd(point, point, &worldMeshLight->jAxis, y * step);
					Vec3::ScaleAndAdd(point, point, &worldMeshLight->iAxis, x * step);

					out->numberOfShadowPoints++;
				}
			}
		}

		// Fill in all the other values.
		Vec3::Zero(&out->invDirection);

		out->minConeAngle = 0.0f;
	}
	else if (worldMeshLight->type == LightTypeArea)
	{
		Vec3 topLeft = worldMeshLight->position;
		Vec3::ScaleAndAdd(&topLeft, &topLeft, &worldMeshLight->iAxis, -worldMeshLight->size.x / 2.0f);
		Vec3::ScaleAndAdd(&topLeft, &topLeft, &worldMeshLight->jAxis, -worldMeshLight->size.y / 2.0f);

		// Build the intensity points.
		Vec2i numberOfPoints;
		Math::CalculateGridCounts(&numberOfPoints, ExpandedLightMaxIntensityPoints, &worldMeshLight->size, 2);

		Vec2 step;
		step.x = worldMeshLight->size.x / (float)(numberOfPoints.x - 1);
		step.y = worldMeshLight->size.y / (float)(numberOfPoints.y - 1);

		out->numberOfIntensityPoints = 0;
		for (int y = 0; y < numberOfPoints.y; y++)
		{
			for (int x = 0; x < numberOfPoints.x; x++)
			{
				Vec3* point = &out->intensityPoints[out->numberOfIntensityPoints];
				*point = topLeft;
				Vec3::ScaleAndAdd(point, point, &worldMeshLight->jAxis, y * step.y);
				Vec3::ScaleAndAdd(point, point, &worldMeshLight->iAxis, x * step.x);

				out->numberOfIntensityPoints++;
			}
		}

		// Build the shadow points.
		Math::CalculateGridCounts(&numberOfPoints, ExpandedLightMaxShadowPoints, &worldMeshLight->size, 2);

		step.x = worldMeshLight->size.x / (float)(numberOfPoints.x - 1);
		step.y = worldMeshLight->size.y / (float)(numberOfPoints.y - 1);

		out->numberOfShadowPoints = 0;
		for (int y = 0; y < numberOfPoints.y; y++)
		{
			for (int x = 0; x < numberOfPoints.x; x++)
			{
				Vec3* point = &out->shadowPoints[out->numberOfShadowPoints];
				*point = topLeft;
				Vec3::ScaleAndAdd(point, point, &worldMeshLight->jAxis, y * step.y);
				Vec3::ScaleAndAdd(point, point, &worldMeshLight->iAxis, x * step.x);

				out->numberOfShadowPoints++;
			}
		}

		// Fill in all the other values.
		out->invDirection = worldMeshLight->kAxis;
		Vec3::Scale(&out->invDirection, &out->invDirection, -1.0f);

		out->minConeAngle = 0.8f;
	}
}

void Engine::BuildEffectedChunksFieldForLight(BitField* out, ExpandedLight* light)
{
	ICollisionMesh* collisionMesh = this->worldMeshAsset->GetCollisionMesh();

	out->Reset(this->worldMeshAsset->GetCollisionMesh()->GetNumberOfChunks());

	for (int chunkIndex = 0; chunkIndex < collisionMesh->GetNumberOfChunks(); chunkIndex++)
	{
		CollisionMeshChunk* chunk = collisionMesh->GetChunk(chunkIndex);

		bool chunkIsEffected = false;

		if (light->type == LightTypePoint || light->type == LightTypeArea)
		{
			for (int intensityPointIndex = 0; intensityPointIndex < light->numberOfIntensityPoints; intensityPointIndex++)
			{
				Sphere intensityPointSphere;
				intensityPointSphere.position = light->intensityPoints[intensityPointIndex];
				intensityPointSphere.radius = light->distance;

				if (Sphere::CheckIntersectsAABB(&intensityPointSphere, &chunk->aabb))
				{
					chunkIsEffected = true;
				}
			}
		}
		
		if (chunkIsEffected)
		{
			out->SetBit(chunkIndex);
		}
	}
}

void Engine::WriteOutputFile(int lightAtlasIndex)
{
	this->logger->Write("Writing output file...");

	char filePath[256];
	sprintf(filePath, "C:/Users/andym/Documents/GitHub/Callisto/Windows/Build/Callisto/Assets/textures/light-atlases/debug/debug-map-3-%d.png", lightAtlasIndex + 1);

	this->currentLightAtlas->WriteToPngFile(filePath);

	this->logger->Write("... done");
}


/*
void Engine::InitLights()
{
	ICollisionMesh* collisionMesh = this->worldMeshAsset->GetCollisionMesh();

	for (int chunkIndex = 0; chunkIndex < collisionMesh->GetNumberOfChunks(); chunkIndex++)
	{
		CollisionMeshChunk* chunk = collisionMesh->GetChunk(chunkIndex);

		if (chunk->staticLightingDetails.emitsLight)
		{
			Light* light = new Light();
			this->lights.Push(light);

			light->ownerChunkIndex = chunkIndex;
			light->minConeAngle = chunk->staticLightingDetails.minConeAngle;
			light->distance = chunk->staticLightingDetails.distance;
			light->distanceSqr = light->distance * light->distance;

			light->colour = chunk->staticLightingDetails.colour;
			RgbFloat::Scale(&light->colour, &light->colour, chunk->staticLightingDetails.power);

			Vec2 blockCentre;
			Vec2::Set(&blockCentre, 1.0f / chunk->staticLightingDetails.gridDimensions.x / 2.0f, 1.0f / chunk->staticLightingDetails.gridDimensions.y / 2.0f);

			int blockIndex = 0;
			for (int y = 0; y < chunk->staticLightingDetails.gridDimensions.y; y++)
			{
				for (int x = 0; x < chunk->staticLightingDetails.gridDimensions.x; x++)
				{
					Vec2 uv;
					Vec2::Set(&uv, (float)x, (float)y);
					uv.x /= (float)chunk->staticLightingDetails.gridDimensions.x;
					uv.y /= (float)chunk->staticLightingDetails.gridDimensions.y;
					Vec2::Add(&uv, &uv, &blockCentre);

					CollisionFace* face = null;
					Vec3 worldPosition;
					Vec3 normal;

					for (int faceIndex = chunk->startFaceIndex; faceIndex < chunk->startFaceIndex + chunk->numberOfFaces; faceIndex++)
					{
						CollisionFace* possibleFace = collisionMesh->GetFace(faceIndex);

						if (Math::CalculateWorldPositionAndNormalFromUV(&worldPosition, &normal, possibleFace->points, possibleFace->normals, possibleFace->materialUVs, &uv))
						{
							face = possibleFace;
							break;
						}
					}

					if (face)
					{
						LightBlock* lightBlock = &light->blocks[blockIndex];
						light->numberOfBlocks++;
						blockIndex++;

						lightBlock->numberOfNodes = 4;

						Vec3 purturbationNormals[2];
						purturbationNormals[0] = face->freeNormalisedEdges[0];
						Vec3::Cross(&purturbationNormals[1], &face->facePlane.normal, &face->freeNormalisedEdges[0]);

						for (int nodeIndex = 0; nodeIndex < lightBlock->numberOfNodes; nodeIndex++)
						{
							LightNode* lightNode = &lightBlock->nodes[nodeIndex];

							lightNode->worldPosition = worldPosition;

							float maxPurturbationDistance = 0.2f;
							for (int i = 0; i < 2; i++)
							{ 
								float purturbationDistance = (maxPurturbationDistance * 2 * Math::GenerateRandomFloat()) - maxPurturbationDistance;
								Vec3::ScaleAndAdd(&lightNode->worldPosition, &lightNode->worldPosition, &purturbationNormals[i], purturbationDistance);
							}

							lightNode->direction = normal;
							Vec3::Scale(&lightNode->invDirection, &lightNode->direction, -1.0f);
						}
					}
				}
			}
		}
	}
}
*/

/*Light* Engine::GetLight(int index)
{
return this->lights[index];
}

int Engine::GetNumberOfLights()
{
return this->lights.GetLength();
}*/

/*void Engine::InitWorkers()
{
	int numberOfLightIslandsToCrunchPerWorker = (int)ceilf(this->worldMeshAsset->GetNumberOfLightIslands() / (float)this->workers.GetLength());

	for (int i = 0; i < this->workers.GetLength(); i++)
	{
		IWorker* worker = this->workers[i];

		int startLightIslandIndex = numberOfLightIslandsToCrunchPerWorker * i;

		worker->Init(startLightIslandIndex, numberOfLightIslandsToCrunchPerWorker);
		//worker->Init(i, this->workers.GetLength());
	}
}*/

/*
void Engine::InitLightAtlas()
{
	IFactory* factory = GetFactory();

	this->lightAtlas = factory->MakeLightAtlas();
	this->lightAtlas->Allocate(1024, 1024); // FIXME

	//this->lightAtlases.Push(lightAtlas);
}*/


/*
void Engine::ComputeLightIslandsOnWorkers()
{
this->logger->Write("Computing light islands...");

// Compute the basic lumel data on each of the workers.
for (int workerIndex = 0; workerIndex < this->workers.GetLength(); workerIndex++)
{
IWorker* worker = this->workers[workerIndex];

worker->ComputeBasicLumelDataAsync();
}

this->WaitForAllWorkersToFinish();

for (int baseLightIndex = 0; baseLightIndex < this->lights.GetLength(); baseLightIndex += NumberOfLightsPerLightGroup)
{
// Build the light group.
LightGroup lightGroup;
memset(&lightGroup, 0, sizeof(LightGroup));

for (int i = 0; i < NumberOfLightsPerLightGroup && baseLightIndex + i < this->lights.GetLength(); i++)
{
lightGroup[i] = this->lights[baseLightIndex + i];
}


// Set the current light group on each of the workers.
for (int workerIndex = 0; workerIndex < this->workers.GetLength(); workerIndex++)
{
IWorker* worker = this->workers[workerIndex];

worker->SetCurrentLightGroup(&lightGroup);
}

// Compute the direct illumination for the current light.
this->logger->Write("Computing direct illumination for light %d/%d.", baseLightIndex + lightGroupSlotIndex + 1, this->lights.GetLength());

this->lightAtlas->ResetLumelIntensityCacheItemsForBounceNumber(0);

for (int workerIndex = 0; workerIndex < this->workers.GetLength(); workerIndex++)
{
IWorker* worker = this->workers[workerIndex];

worker->ComputeDirectIlluminationIntensitiesForCurrentLightAsync();
}

this->WaitForAllWorkersToFinish();
}


// Compute the indirect illumination for the current light.
for (int bounceNumber = 1; bounceNumber <= this->config.numberOfIndirectIlluminationBounces; bounceNumber++)
{
this->lightAtlas->ResetLumelIntensityCacheItemsForBounceNumber(bounceNumber);

NormalWithinHemisphereCalculationMetrics normalWithinHemisphereCalculationMetrics;
Math::BuildNormalWithinHemisphereCalculationMetrics(&normalWithinHemisphereCalculationMetrics, this->config.indirectIlluminationHemispehereCircleCount, PI * 0.45f, this->config.numberOfIndirectIlluminationSamplesPerBounce);

for (int circleIndex = 0; circleIndex < normalWithinHemisphereCalculationMetrics.numberOfCircles; circleIndex++)
{
int numberOfSegmentsForCircle = normalWithinHemisphereCalculationMetrics.segmentCountsByCircleIndex[circleIndex];

for (int segmentIndex = 0; segmentIndex < numberOfSegmentsForCircle; segmentIndex++)
{
this->logger->Write(
"Computing indirect illumination bounce targets for light %d/%d. Bounce number: %d/%d. Circle %d/%d. Segment: %d/%d",
lightIndex + 1, this->lights.GetLength(),
bounceNumber, this->config.numberOfIndirectIlluminationBounces,
circleIndex + 1, normalWithinHemisphereCalculationMetrics.numberOfCircles,
segmentIndex + 1, numberOfSegmentsForCircle);

for (int workerIndex = 0; workerIndex < this->workers.GetLength(); workerIndex++)
{
IWorker* worker = this->workers[workerIndex];

worker->AccumulateIndirectIlluminationIntensitiesForCurrentLightAsync(&normalWithinHemisphereCalculationMetrics, circleIndex, segmentIndex, bounceNumber);
}

this->WaitForAllWorkersToFinish();

//for (int workerIndex = 0; workerIndex < this->workers.GetLength(); workerIndex++)
//{
//	IWorker* worker = this->workers[workerIndex];
//
//	worker->AccumulateIndirectIlluminationIntensitiesForCurrentLightAsync();
//}

//this->WaitForAllWorkersToFinish();
}
}

this->lightAtlas->AverageLumelIntensityCacheItemsForBounceNumber(bounceNumber);
}

this->lightAtlas->AccumulateLumelColoursFromIntensityCacheItems(&light->colour);

//// Composite the colour for the current light.
//this->logger->Write("Compositing colour for light %d/%d.", lightIndex + 1, this->lights.GetLength());
//
//for (int workerIndex = 0; workerIndex < this->workers.GetLength(); workerIndex++)
//{
//	IWorker* worker = this->workers[workerIndex];
//
//	worker->CompositeColourForCurrentLightAsync();
//}

//this->WaitForAllWorkersToFinish();
}

this->logger->Write("... done.");

//////////////
int totalFailures = 0;
for (int workerIndex = 0; workerIndex < this->workers.GetLength(); workerIndex++)
{
IWorker* worker = this->workers[workerIndex];

worker->DumpStats();
totalFailures += worker->GetTotalFailures();
}
//////////////

logger->Write("Total failures: %d", totalFailures);
}
*/

/*void Engine::FillBordersOnWorkers()
{
this->logger->Write("Filling borders...");

for (int workerIndex = 0; workerIndex < this->workers.GetLength(); workerIndex++)
{
IWorker* worker = this->workers[workerIndex];

worker->FillBordersAsync();
}

this->WaitForAllWorkersToFinish();

this->logger->Write("... done.");
}*/