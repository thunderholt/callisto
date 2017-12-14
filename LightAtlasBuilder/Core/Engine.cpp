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

	this->config.indirectIlluminationHemispehereCircleCount = 16;
	//this->config.indirectIlluminationHemispehereSegmentCount = 16;

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

	/*for (int i = 0; i < this->lightAtlases.GetLength(); i++)
	{
		SafeDeleteAndNull(this->lightAtlases[i]);
	}*/
	SafeDeleteAndNull(this->lightAtlas);

	for (int i = 0; i < this->lights.GetLength(); i++)
	{
		SafeDeleteAndNull(this->lights[i]);
	}
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
		this->logger->Write("... done.");

		this->InitWorkers();

		this->InitLightAtlas();

		this->InitLights();

		this->ComputeLightIslandsOnWorkers();

		this->FillBordersOnWorkers();

		this->WriteOutputFiles();
	}

	double endTime = this->timestampProvider->GetTimestampMillis();
	double durationMillis = endTime - startTime;
	double durationSeconds = durationMillis / 1000.0;

	this->logger->Write("Duration: %f seconds.", durationSeconds);
}

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

	/*////////////////// Test code /////////////////
	Light* light = new Light();
	this->lights.Push(light);

	//RgbFloat::Set(&light->colour, 10.0f, 10.0f, 10.0f);
	RgbFloat::Set(&light->colour, 5.0f, 5.0f, 5.0f);
	//light->numberOfEffectedChunks = 0;

	Vec3 lightPosition;
	//Vec3::Set(&lightPosition, 5.5f, 0.6f, 2.0f);
	Vec3::Set(&lightPosition, -3.0f, 2.0f, -1.0f);

	Vec2 lightSize;
	Vec2::Set(&lightSize, 4.0f, 4.0f);

	Vec2i blockCounts;
	Vec2i::Set(&blockCounts, 8, 4);

	Vec2 blockSize;
	Vec2::Set(&blockSize, lightSize.x / blockCounts.x, lightSize.y / blockCounts.y);

	light->numberOfBlocks = blockCounts.x * blockCounts.y;

	int blockIndex = 0;

	for (int y = 0; y < blockCounts.y; y++)
	{
		for (int x = 0; x < blockCounts.x; x++)
		{
			LightBlock* lightBlock = &light->blocks[blockIndex];
			blockIndex++;

			lightBlock->numberOfNodes = 100;

			for (int i = 0; i < lightBlock->numberOfNodes; i++)
			{
				LightNode* lightNode = &lightBlock->nodes[i];

				lightNode->worldPosition = lightPosition;
				lightNode->worldPosition.x += (x * blockSize.x) + ((blockSize.x / 1.0f) * Math::GenerateRandomFloat());
				lightNode->worldPosition.z += (y * blockSize.y) + ((blockSize.y / 1.0f) * Math::GenerateRandomFloat());

				//RgbFloat::Set(&lightNode->colour, 10.0f, 10.0f, 10.0f);

				lightNode->distance = 12.0f;
				lightNode->distanceSqr = lightNode->distance * lightNode->distance;

				//Vec3::Set(&lightNode->direction, 0.0f, -1.0f, 1.0f);
				//Vec3::Set(&lightNode->direction, -1.0f, -1.0f, 0.0f);
				Vec3::Set(&lightNode->direction, 0.0f, -1.0f, 1.0f);
				Vec3::Normalize(&lightNode->direction, &lightNode->direction);

				Vec3::Scale(&lightNode->invDirection, &lightNode->direction, -1.0f);
			}
		}
	}

	

	
	//////////////////////////////////////////////*/

	/*////////////////// Test code /////////////////
	light = new Light();
	this->lights.Push(light);

	RgbFloat::Set(&light->colour, 10.0f, 0.0f, 0.0f);
	//light->numberOfEffectedChunks = 0;

	Vec3::Set(&lightPosition, 5.5f, 0.6f, -2.0f);

	for (int i = 0; i < 200; i++)
	{
		LightNode* lightNode = &light->nodes.PushAndGet();

		lightNode->worldPosition = lightPosition;
		lightNode->worldPosition.x += 3.0f * Math::GenerateRandomFloat();
		lightNode->worldPosition.z += 0.5f * Math::GenerateRandomFloat();

		//RgbFloat::Set(&lightNode->colour, 10.0f, 5.0f, 5.0f);

		lightNode->distance = 4.0f;
		lightNode->distanceSqr = lightNode->distance * lightNode->distance;

		Vec3::Set(&lightNode->direction, 0.0f, -1.0f, 1.0f);
		Vec3::Normalize(&lightNode->direction, &lightNode->direction);

		Vec3::Scale(&lightNode->invDirection, &lightNode->direction, -1.0f);
	}
	//////////////////////////////////////////////*/

	/*for (int chunkIndex = 0; chunkIndex < collisionMesh->GetNumberOfChunks(); chunkIndex++)
	{
		CollisionMeshChunk* chunk = collisionMesh->GetChunk(chunkIndex);

		for (int lightIndex = 0; lightIndex < this->lights.GetLength(); lightIndex++)
		{
			Light* light = this->lights[lightIndex];
			bool isEffectedByLight = false;

			for (int lightNodeIndex = 0; lightNodeIndex < light->nodes.GetLength(); lightNodeIndex++)
			{
				LightNode* lightNode = &light->nodes[lightNodeIndex];
				
				Sphere lightNodeSphere;
				Sphere::Set(&lightNodeSphere, &lightNode->worldPosition, lightNode->distance);

				if (Sphere::CheckIntersectsAABB(&lightNodeSphere, &chunk->aabb))
				{
					isEffectedByLight = true;
					break;
				}
			}

			if (isEffectedByLight)
			{
				chunk->effectiveLightIndexes.Push(lightIndex);
				light->numberOfEffectedChunks++;
			}
		}
	}

	for (int lightIndex = 0; lightIndex < this->lights.GetLength(); lightIndex++)
	{
		Light* light = this->lights[lightIndex];
		this->logger->Write("Light %d: effected chunks: %d", lightIndex, light->numberOfEffectedChunks);
	}*/
}

ILogger* Engine::GetLogger()
{
	return this->logger;
}

IWorldMeshAsset* Engine::GetWorldMeshAsset()
{
	return this->worldMeshAsset;
}

ILightAtlas* Engine::GetLightAtlas()
{
	return this->lightAtlas;
}

Light* Engine::GetLight(int index)
{
	return this->lights[index];
}

int Engine::GetNumberOfLights()
{
	return this->lights.GetLength();
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

void Engine::InitWorkers()
{
	int numberOfLightIslandsToCrunchPerWorker = (int)ceilf(this->worldMeshAsset->GetNumberOfLightIslands() / (float)this->workers.GetLength());

	for (int i = 0; i < this->workers.GetLength(); i++)
	{
		IWorker* worker = this->workers[i];

		int startLightIslandIndex = numberOfLightIslandsToCrunchPerWorker * i;

		worker->Init(startLightIslandIndex, numberOfLightIslandsToCrunchPerWorker);
		//worker->Init(i, this->workers.GetLength());
	}
}

void Engine::InitLightAtlas()
{
	IFactory* factory = GetFactory();

	this->lightAtlas = factory->MakeLightAtlas();
	this->lightAtlas->Allocate(4096, 4096);

	//this->lightAtlases.Push(lightAtlas);
}

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

	for (int lightIndex = 0; lightIndex < this->lights.GetLength(); lightIndex++)
	{
		Light* light = this->lights[lightIndex];

		// Set the current light on each of the workers.
		for (int workerIndex = 0; workerIndex < this->workers.GetLength(); workerIndex++)
		{
			IWorker* worker = this->workers[workerIndex];

			worker->SetCurrentLight(light);
		}

		// Compute the direct illumination for the current light.
		this->logger->Write("Computing direct illumination for light %d/%d.", lightIndex + 1, this->lights.GetLength());

		for (int workerIndex = 0; workerIndex < this->workers.GetLength(); workerIndex++)
		{
			IWorker* worker = this->workers[workerIndex];

			worker->ComputeDirectIlluminationIntensitiesForCurrentLightAsync();
		}

		this->WaitForAllWorkersToFinish();

		// Compute the indirect illumination for the current light.
		NormalWithinHemisphereCalculationMetrics normalWithinHemisphereCalculationMetrics;
		Math::BuildNormalWithinHemisphereCalculationMetrics(&normalWithinHemisphereCalculationMetrics, this->config.indirectIlluminationHemispehereCircleCount, PI * 0.45f, 100);

		for (int circleIndex = 0; circleIndex < normalWithinHemisphereCalculationMetrics.numberOfCircles; circleIndex++)
		{
			int numberOfSegmentsForCircle = normalWithinHemisphereCalculationMetrics.segmentCountsByCircleIndex[circleIndex];

			for (int segmentIndex = 0; segmentIndex < numberOfSegmentsForCircle; segmentIndex++)
			{
				this->logger->Write(
					"Computing indirect illumination bounce targets for light %d/%d. Circle %d/%d. Segment: %d/%d", 
					lightIndex + 1, this->lights.GetLength(), 
					circleIndex + 1, normalWithinHemisphereCalculationMetrics.numberOfCircles,
					segmentIndex + 1, numberOfSegmentsForCircle);

				for (int workerIndex = 0; workerIndex < this->workers.GetLength(); workerIndex++)
				{
					IWorker* worker = this->workers[workerIndex];

					worker->ComputeIndirectIlluminationBouncesTargetsForCurrentLightAsync(&normalWithinHemisphereCalculationMetrics, circleIndex, segmentIndex);
				}

				this->WaitForAllWorkersToFinish();

				for (int workerIndex = 0; workerIndex < this->workers.GetLength(); workerIndex++)
				{
					IWorker* worker = this->workers[workerIndex];

					worker->AccumulateIndirectIlluminationIntensitiesForCurrentLightAsync();
				}

				this->WaitForAllWorkersToFinish();
			}
		}

		// Composite the colour for the current light.
		this->logger->Write("Compositing colour for light %d/%d.", lightIndex + 1, this->lights.GetLength());

		for (int workerIndex = 0; workerIndex < this->workers.GetLength(); workerIndex++)
		{
			IWorker* worker = this->workers[workerIndex];

			worker->CompositeColourForCurrentLightAsync();
		}

		this->WaitForAllWorkersToFinish();
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

void Engine::FillBordersOnWorkers()
{
	this->logger->Write("Filling borders...");

	for (int workerIndex = 0; workerIndex < this->workers.GetLength(); workerIndex++)
	{
		IWorker* worker = this->workers[workerIndex];

		worker->FillBordersAsync();
	}

	this->WaitForAllWorkersToFinish();

	this->logger->Write("... done.");
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

void Engine::WriteOutputFiles()
{
	this->logger->Write("Writing output files...");

	/*for (int i = 0; i < this->lightAtlases.GetLength(); i++)
	{
		ILightAtlas* lightAtlas = this->lightAtlases[i];
		lightAtlas->WriteToPngFile("C:/Users/andym/Documents/GitHub/Callisto/Windows/Build/Callisto/Assets/textures/light-atlases/debug/debug-map-3-1.png");
		//lightAtlas->WriteToPngFile("C:/temp/debug-map-1-1.png");
	}*/

	this->lightAtlas->WriteToPngFile("C:/Users/andym/Documents/GitHub/Callisto/Windows/Build/Callisto/Assets/textures/light-atlases/debug/debug-map-3.png");

	this->logger->Write("... done");
}