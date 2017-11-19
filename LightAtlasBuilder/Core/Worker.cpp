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
	this->numberOfMeshChunkIndexes = 0;
}

Worker::~Worker()
{
	
}

void Worker::Init(int startMeshChunkIndex, int numberOfMeshChunkIndexes)
{
	this->startMeshChunkIndex = startMeshChunkIndex;
	this->numberOfMeshChunkIndexes = numberOfMeshChunkIndexes;
}

void Worker::ComputeLightIslandsAsync()
{
	IEngine* engine = GetEngine();
	IThreadManager* threadManager = engine->GetThreadManager();

	this->currentTaskType = WorkerTaskTypeComputeLightIslands;
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

	if (this->currentTaskType == WorkerTaskTypeComputeLightIslands)
	{
		this->ComputeLightIslandsInternal();
	}

	this->currentTaskHasFinished = true;
}

void Worker::ComputeLightIslandsInternal()
{
	IEngine* engine = GetEngine();
	IWorldMeshAsset* worldMeshAsset = engine->GetWorldMeshAsset();
	ICollisionMesh* collisionMesh = worldMeshAsset->GetCollisionMesh();

	for (int chunkIndex = this->startMeshChunkIndex;
		chunkIndex < this->startMeshChunkIndex + this->numberOfMeshChunkIndexes && chunkIndex < collisionMesh->GetNumberOfChunks();
		chunkIndex++)
	{
		CollisionMeshChunk* chunk = collisionMesh->GetChunk(chunkIndex);
		ILightAtlas* lightAtlas = engine->GetLightAtlas(chunk->lightAtlasIndex);
		Vec2i lightAtlasSize = lightAtlas->GetSize();
		RgbFloat* texels = lightAtlas->GetTexels();

		Vec2i startTexelIndex;
		startTexelIndex.x = (int)roundf(chunk->lightIslandOffset.x * lightAtlasSize.x);
		startTexelIndex.y = (int)roundf(chunk->lightIslandOffset.y * lightAtlasSize.y);

		Vec2i numberOfTexels;
		numberOfTexels.x = (int)roundf(chunk->lightIslandSize.x * lightAtlasSize.x);
		numberOfTexels.y = (int)roundf(chunk->lightIslandSize.y * lightAtlasSize.y);

		RgbFloat randomColour;
		randomColour.r = Math::GenerateRandomFloat();
		randomColour.g = Math::GenerateRandomFloat();
		randomColour.b = Math::GenerateRandomFloat();

		for (int y = startTexelIndex.y; y < startTexelIndex.y + numberOfTexels.y; y++)
		{
			for (int x = startTexelIndex.x; x < startTexelIndex.x + numberOfTexels.x; x++)
			{
				int texelIndex = y * lightAtlasSize.x + x;
				texels[texelIndex] = randomColour;
			}
		}
	}
}