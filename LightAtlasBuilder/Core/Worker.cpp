#include "Core/Worker.h"

void WorkerRunSectorCruncherEntryPoint(void* arg)
{
	Worker* worker = (Worker*)arg;
	worker->RunThreadEntryPoint();
}

Worker::Worker()
{
	this->currentTaskType = WorkerTaskTypeNone;
	this->currentTaskHasFinished = false;
	this->startLightIslandIndex = 0;
	this->numberOfLightIslandIndexes = 0;
	this->currentLight = null;
}

Worker::~Worker()
{
	
}

void Worker::Init(int startLightIslandIndex, int numberOfLightIslandIndexes)
{
	this->startLightIslandIndex = startLightIslandIndex;
	this->numberOfLightIslandIndexes = numberOfLightIslandIndexes;
}

void Worker::ComputeDirectIlluminationForLightAsync(Light* light)
{
	IEngine* engine = GetEngine();
	IThreadManager* threadManager = engine->GetThreadManager();

	this->currentTaskType = WorkerTaskTypeComputeDirectIlluminationForLight;
	this->currentLight = light;
	this->currentTaskHasFinished = false;

	threadManager->StartThread(WorkerRunSectorCruncherEntryPoint, this);
}

bool Worker::GetHasFinished()
{
	return this->currentTaskHasFinished;
}

void Worker::RunThreadEntryPoint()
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	if (this->currentTaskType == WorkerTaskTypeComputeDirectIlluminationForLight)
	{
		this->ComputeDirectIlluminationForLightInternal();
	}

	this->currentTaskHasFinished = true;
}

void Worker::ComputeDirectIlluminationForLightInternal()
{
	IEngine* engine = GetEngine();
	IWorldMeshAsset* worldMeshAsset = engine->GetWorldMeshAsset();
	ICollisionMesh* collisionMesh = worldMeshAsset->GetCollisionMesh();
	IRayTracer* rayTracer = engine->GetRayTracer();
	ILogger* logger = engine->GetLogger();

	ILightAtlas* lightAtlas = engine->GetLightAtlas();
	Vec2i lightAtlasSize = lightAtlas->GetSize();
	RgbFloat* texels = lightAtlas->GetTexels();

	Vec2 pixelCentre;
	Vec2::Set(&pixelCentre, 1.0f / lightAtlasSize.x / 2.0f, 1.0f / lightAtlasSize.y / 2.0f);

	int logCounter = 0;

	for (int lightIslandIndex = this->startLightIslandIndex;
		lightIslandIndex < this->startLightIslandIndex + this->numberOfLightIslandIndexes && lightIslandIndex < worldMeshAsset->GetNumberOfLightIslands();
		lightIslandIndex++)
	{
		if (logCounter == 10)
		{
			logger->Write("Light island index: %d/%d", (lightIslandIndex - this->startLightIslandIndex) + 1, numberOfLightIslandIndexes);
			logCounter = 0;
		}

		logCounter++;

		//////// Temp //////
		RgbFloat faceColours[2000];
		for (int i = 0; i < 2000; i++)
		{
			RgbFloat* colour = &faceColours[i];
			colour->r = Math::GenerateRandomFloat();
			colour->g = Math::GenerateRandomFloat();
			colour->b = Math::GenerateRandomFloat();
		}
		////////////////////

		WorldMeshLightIsland* lightIsland = worldMeshAsset->GetLightIsland(lightIslandIndex);

		// TODO - check if the light island chunks are effected by the current light.

		for (int y = lightIsland->offset.y; y < lightIsland->offset.y + lightIsland->size.y; y++)
		{
			for (int x = lightIsland->offset.x; x < lightIsland->offset.x + lightIsland->size.x; x++)
			{
				CollisionMeshChunk* chunk = collisionMesh->GetChunk(lightIsland->chunkFaceIndex.chunkIndex);

				int texelIndex = y * lightAtlasSize.x + x;

				Vec2 uv;
				Vec2::Set(&uv, (float)x, (float)y);
				uv.x /= (float)lightAtlasSize.x;
				uv.y /= (float)lightAtlasSize.y;
				//uv.x = (x - lightIsland->offset.x) / (float)lightIsland->size.x;
				//uv.y = (y - lightIsland->offset.y) / (float)lightIsland->size.y;
				Vec2::Add(&uv, &uv, &pixelCentre);

				Vec3 worldPosition;
				Vec3 normal;
				bool faceFound = this->FindWorldPositionForLightIslandTexel(&worldPosition, &normal, collisionMesh, &lightIsland->chunkFaceIndex, uv);
				if (faceFound)
				{
					RgbFloat* texel = &texels[texelIndex];

					//*texel = faceColours[lightIsland->chunkFaceIndex.faceIndex];

					//texels[texelIndex] = rayTracer->CalculateColourForChunkAtPosition(&worldPosition, &normal, chunkIndex);
					float directIlluminationIntensity = rayTracer->CalculateDirectIlluminationIntensityForChunkAtPosition(this->currentLight, &worldPosition, &normal, collisionMesh, lightIsland->chunkFaceIndex.chunkIndex);

					// Don't do this - store the intensity in the direct illumination intensity cache.
					RgbFloat lightColour;
					RgbFloat::Scale(&lightColour, &this->currentLight->colour, directIlluminationIntensity);


					*texel = lightColour;
					//RgbFloat::Add(texel, texel, &lightColour);*/
				}
			}
		}

		// TODO - don't do this after every light!
		//this->FillLightIslandBorders(lightIsland);
	}
}

/*bool Worker::FindWorldPositionForLightIslandTexel(Vec3* outWorldPosition, Vec3* outNormal, int* outFaceIndex, ICollisionMesh* collisionMesh, CollisionMeshChunk* chunk, Vec2 uv)
{
	bool faceFound = false;

	for (int faceIndex = chunk->startFaceIndex; faceIndex < chunk->startFaceIndex + chunk->numberOfFaces; faceIndex++)
	{
		CollisionFace* face = collisionMesh->GetFace(faceIndex);

		if (Math::CalculateWorldPositionAndNormalFromUV(outWorldPosition, outNormal, face->points, face->normals, face->uvs, &uv))
		{
			faceFound = true;
			*outFaceIndex = faceIndex;
			break;
		}
	}

	return faceFound;
}*/

bool Worker::FindWorldPositionForLightIslandTexel(Vec3* outWorldPosition, Vec3* outNormal, ICollisionMesh* collisionMesh, MeshChunkFaceIndex* chunkFaceIndex, Vec2 uv)
{
	bool faceFound = false;

	CollisionFace* face = collisionMesh->GetFace(chunkFaceIndex->faceIndex);

	if (Math::CalculateWorldPositionAndNormalFromUV(outWorldPosition, outNormal, face->points, face->normals, face->uvs, &uv))
	{
		faceFound = true;
	}

	return faceFound;
}

bool Worker::CheckIfChunkIsEffectedByLight(CollisionMeshChunk* chunk, Light* light)
{
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

	return isEffectedByLight;
}

void Worker::FillLightIslandBorders(WorldMeshLightIsland* lightIsland)
{
	IEngine* engine = GetEngine();
	ILightAtlas* lightAtlas = engine->GetLightAtlas();

	Vec2i lightAtlasSize = lightAtlas->GetSize();
	RgbFloat* texels = lightAtlas->GetTexels();

	int xFrom = lightIsland->offset.x;
	int xTo = lightIsland->offset.x + lightIsland->size.x - 1;
	int yFrom = lightIsland->offset.y;
	int yTo = lightIsland->offset.y + lightIsland->size.y - 1;

	for (int y = yFrom; y <= yTo; y++)
	{
		for (int x = xFrom; x <= xTo; x++)
		{
			int texelIndex = y * lightAtlasSize.x + x;

			RgbFloat* texel = &texels[texelIndex];
			if (texel->r == -1.0f)
			{
				int numberOfSamples = 0;
				RgbFloat accumulatedSamples;
				RgbFloat::Zero(&accumulatedSamples);

				for (int j = -1; j <= 1; j++)
				{
					for (int i = -1; i <= 1; i++)
					{
						if (i == 0 && j == 0)
						{
							continue;
						}

						if (i == -1 && x == xFrom)
						{
							continue;
						}

						if (i == 1 && x == xTo)
						{
							continue;
						}

						if (j == -1 && y == yFrom)
						{
							continue;
						}

						if (j == 1 && y == xTo)
						{
							continue;
						}

						int sampleTexelIndex = (y + j) * lightAtlasSize.x + (x + i);
						RgbFloat* sample = &texels[sampleTexelIndex];
						if (sample->r >= 0.0f)
						{
							RgbFloat::Add(&accumulatedSamples, &accumulatedSamples, sample);
							numberOfSamples++;
						}
					}
				}

				if (numberOfSamples > 0)
				{
					RgbFloat averageSample;
					RgbFloat::Scale(&averageSample, &accumulatedSamples, -1.0f / (float)numberOfSamples);
					*texel = averageSample;
				}
			}
		}
	}

	for (int y = yFrom; y <= yTo; y++)
	{
		for (int x = xFrom; x <= xTo; x++)
		{
			int texelIndex = y * lightAtlasSize.x + x;

			RgbFloat* texel = &texels[texelIndex];
			if (texel->r < 0)
			{
				RgbFloat::Scale(texel, texel, -1.0f);
			}
		}
	}
}

/*void Worker::FillLightIslandBorders(ILightAtlas* lightAtlas, CollisionMeshChunk* chunk)
{
	Vec2i lightAtlasSize = lightAtlas->GetSize();
	RgbFloat* texels = lightAtlas->GetTexels();

	int xFrom = chunk->lightIslandOffset.x;
	int xTo = chunk->lightIslandOffset.x + chunk->lightIslandSize.x - 1;
	int yFrom = chunk->lightIslandOffset.y;
	int yTo = chunk->lightIslandOffset.y + chunk->lightIslandSize.y - 1;

	for (int y = yFrom; y <= yTo; y++)
	{
		for (int x = xFrom; x <= xTo; x++)
		{
			int texelIndex = y * lightAtlasSize.x + x;

			RgbFloat* texel = &texels[texelIndex];
			if (texel->r == -1.0f)
			{
				int numberOfSamples = 0;
				RgbFloat accumulatedSamples;
				RgbFloat::Zero(&accumulatedSamples);

				for (int j = -1; j <= 1; j++)
				{
					for (int i = -1; i <= 1; i++)
					{
						if (i == 0 && j == 0)
						{
							continue;
						}

						if (i == -1 && x == xFrom)
						{
							continue;
						}

						if (i == 1 && x == xTo)
						{
							continue;
						}

						if (j == -1 && y == yFrom)
						{
							continue;
						}

						if (j == 1 && y == xTo)
						{
							continue;
						}

						int sampleTexelIndex = (y + j) * lightAtlasSize.x + (x + i);
						RgbFloat* sample = &texels[sampleTexelIndex];
						if (sample->r >= 0.0f)
						{
							RgbFloat::Add(&accumulatedSamples, &accumulatedSamples, sample);
							numberOfSamples++;
						}
					}
				}

				if (numberOfSamples > 0)
				{
					RgbFloat averageSample;
					RgbFloat::Scale(&averageSample, &accumulatedSamples, -1.0f / (float)numberOfSamples);
					*texel = averageSample;
				}
			}
		}
	}

	for (int y = yFrom; y <= yTo; y++)
	{
		for (int x = xFrom; x <= xTo; x++)
		{
			int texelIndex = y * lightAtlasSize.x + x;

			RgbFloat* texel = &texels[texelIndex];
			if (texel->r < 0)
			{
				RgbFloat::Scale(texel, texel, -1.0f);
			}
		}
	}
}*/

/*int Worker::FindFaceIndexForLightIslandTexel(ICollisionMesh* collisionMesh, CollisionMeshChunk* chunk, Vec2 uv)
{
	int faceIndex = -1;

	for (int i = 0; i < chunk->numberOfFaces; i++)
	{
		CollisionFace* face = collisionMesh->GetFace(chunk->startFaceIndex + i);

		Vec3 worldPosition;
		if (Math::CalculateWorldPositionFromUV(&worldPosition, face->points, face->uvs, &uv))
		{
			faceIndex = i;
			break;
		}
	}

	return faceIndex;
}*/