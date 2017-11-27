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
	this->startMeshChunkIndex = 0;
	this->meshChunkIndexesStride = 0;
	this->currentLight = null;
}

Worker::~Worker()
{
	
}

void Worker::Init(int startMeshChunkIndex, int meshChunkIndexesStride)
{
	this->startMeshChunkIndex = startMeshChunkIndex;
	this->meshChunkIndexesStride = meshChunkIndexesStride;
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

	/*///////// Temp ////////
	Vec3 lightDir;
	//Vec3::Set(&lightDir, -1.0f, -1.0f, 1.0f);
	Vec3::Set(&lightDir, 0.0f, -1.0f, 0.0f);
	Vec3::Normalize(&lightDir, &lightDir);

	Vec3 invLightDir;
	Vec3::Scale(&invLightDir, &lightDir, -1.0f);
	///////////////////////*/

	/*///////// Temp ////////
	Vec3 lightPosition;
	Vec3::Set(&lightPosition, 7.0f, 0.6f, 2.0f);

	Vec3 lightPositions[200];
	for (int i = 0; i < 200; i++)
	{
		lightPositions[i] = lightPosition;
		lightPositions[i].x += 0.5f * Math::GenerateRandomFloat();
		lightPositions[i].z += 0.5f * Math::GenerateRandomFloat();
	}
	//Vec3::Set(&lightPosition, 9.5f, 0.6f, 8.0f);
	float lightRadius = 8.0f;
	///////////////////////*/

	int logCounter = 0;

	for (int chunkIndex = this->startMeshChunkIndex;
	chunkIndex < /*this->startMeshChunkIndex + this->numberOfMeshChunkIndexes && chunkIndex <*/ collisionMesh->GetNumberOfChunks();
		chunkIndex += this->meshChunkIndexesStride)
	{
		if (logCounter == 10)
		{
			logger->Write("Chunk index: %d/%d", chunkIndex + 1, collisionMesh->GetNumberOfChunks());
			logCounter = 0;
		}

		logCounter++;

		/*if (chunkIndex == 21)
		{
			int h = 1;
		}*/

		/*//////// Temp //////
		RgbFloat faceColours[200];
		for (int i = 0; i < 200; i++)
		{
			RgbFloat* colour = &faceColours[i];
			colour->r = Math::GenerateRandomFloat();
			colour->g = Math::GenerateRandomFloat();
			colour->b = Math::GenerateRandomFloat();
		}
		////////////////////*/

		CollisionMeshChunk* chunk = collisionMesh->GetChunk(chunkIndex);
		if (this->CheckIfChunkIsEffectedByLight(chunk, this->currentLight))
		{
			ILightAtlas* lightAtlas = engine->GetLightAtlas(chunk->lightAtlasIndex);
			Vec2i lightAtlasSize = lightAtlas->GetSize();
			RgbFloat* texels = lightAtlas->GetTexels();

			Vec2i startTexelIndex;
			startTexelIndex.x = (int)roundf(chunk->lightIslandOffset.x * lightAtlasSize.x);
			startTexelIndex.y = (int)roundf(chunk->lightIslandOffset.y * lightAtlasSize.y);

			Vec2i numberOfTexels;
			numberOfTexels.x = (int)roundf(chunk->lightIslandSize.x * lightAtlasSize.x);
			numberOfTexels.y = (int)roundf(chunk->lightIslandSize.y * lightAtlasSize.y);

			/*RgbFloat randomColour;
			randomColour.r = 1.0f; // Math::GenerateRandomFloat();
			randomColour.g = 0.0f; // Math::GenerateRandomFloat();
			randomColour.b = 0.0f; // Math::GenerateRandomFloat();*/

			for (int y = startTexelIndex.y; y < startTexelIndex.y + numberOfTexels.y; y++)
			{
				for (int x = startTexelIndex.x; x < startTexelIndex.x + numberOfTexels.x; x++)
				{
					int texelIndex = y * lightAtlasSize.x + x;

					Vec2 uv;
					uv.x = ((x / (float)lightAtlasSize.x) - chunk->lightIslandOffset.x) / chunk->lightIslandSize.x;
					uv.y = ((y / (float)lightAtlasSize.y) - chunk->lightIslandOffset.y) / chunk->lightIslandSize.y;

					Vec3 worldPosition;
					Vec3 normal;
					bool faceFound = this->FindWorldPositionForLightIslandTexel(&worldPosition, &normal, collisionMesh, chunk, uv);
					if (faceFound)
					{
						//texels[texelIndex] = rayTracer->CalculateColourForChunkAtPosition(&worldPosition, &normal, chunkIndex);
						float directIlluminationIntensity = rayTracer->CalculateDirectIlluminationIntensityForChunkAtPosition(this->currentLight, &worldPosition, &normal, collisionMesh, chunkIndex);

						// Don't do this - store the intensity in the direct illumination intensity cache.
						RgbFloat lightColour;
						RgbFloat::Scale(&lightColour, &this->currentLight->colour, directIlluminationIntensity);

						RgbFloat* texel = &texels[texelIndex];
						RgbFloat::Add(texel, texel, &lightColour);
					}
				}
			}
		}
	}
}

bool Worker::FindWorldPositionForLightIslandTexel(Vec3* outWorldPosition, Vec3* outNormal, ICollisionMesh* collisionMesh, CollisionMeshChunk* chunk, Vec2 uv)
{
	bool faceFound = false;

	for (int i = 0; i < chunk->numberOfFaces; i++)
	{
		CollisionFace* face = collisionMesh->GetFace(chunk->startFaceIndex + i);

		if (Math::CalculateWorldPositionAndNormalFromUV(outWorldPosition, outNormal, face->points, face->normals, face->uvs, &uv))
		{
			faceFound = true;
			break;
		}
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