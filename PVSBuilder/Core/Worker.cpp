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
	this->sectorCruncher = null;
	this->startSectorIndex = 0;
	this->numberOfSectorsToCrunch = 0;
}

Worker::~Worker()
{
	SafeDeleteAndNull(this->sectorCruncher);
}

void Worker::Init(int startSectorIndex, int numberOfSectorsToCrunch)
{
	this->startSectorIndex = startSectorIndex;
	this->numberOfSectorsToCrunch = numberOfSectorsToCrunch;
}

void Worker::ComputeSectorInsidePointsAsync()
{
	IEngine* engine = GetEngine();
	IThreadManager* threadManager = engine->GetThreadManager();

	this->currentTaskType = WorkerTaskTypeComputeSectorInsidePoints;
	this->currentTaskHasFinished = false;

	threadManager->StartThread(WorkerRunSectorCruncherEntryPoint, this);
}

void Worker::RunSectorCruncherAsync(SectorCruncherType sectorCruncherType)
{
	IFactory* factory = GetFactory();
	IEngine* engine = GetEngine();
	IThreadManager* threadManager = engine->GetThreadManager();

	SafeDeleteAndNull(this->sectorCruncher);

	/*if (sectorCruncherType == SectorCruncherTypeOcclusion)
	{
		this->sectorCruncher = factory->MakeOcclusionSectorCruncher();
	}
	else */if (sectorCruncherType == SectorCruncherTypeBruteForce)
	{
		this->sectorCruncher = factory->MakeBruteForceSectorCruncher();
	}

	this->currentTaskType = WorkerTaskTypeRunSectorCruncher;
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

	if (this->currentTaskType == WorkerTaskTypeComputeSectorInsidePoints)
	{
		this->ComputeSectorInsidePointsInternal();
	}
	else if (this->currentTaskType == WorkerTaskTypeRunSectorCruncher)
	{
		this->RunSectorCruncherInternal();
	}

	this->currentTaskHasFinished = true;

	//logger->Write("Worker finished.");
}

void Worker::ComputeSectorInsidePointsInternal()
{
	IEngine* engine = GetEngine();
	IWorldMeshAsset* worldMeshAsset = engine->GetWorldMeshAsset();
	ICollisionMesh* collisionMesh = worldMeshAsset->GetCollisionMesh();
	//SectorMetrics* sectorMetrics = engine->GetSectorMetrics();
	//Sector* sectors = engine->GetSectors();
	int numberOfSectors = engine->GetNumberOfSectors();

	for (int sectorIndex = startSectorIndex;
		sectorIndex < numberOfSectors &&
		sectorIndex < startSectorIndex + numberOfSectorsToCrunch;
		sectorIndex++)
	{
		Sector* sector = engine->GetSector(sectorIndex);

		for (int i = 0; i < 400; i++)
		{
			Vec3 insidePoint;
			this->CreateRandomPointWithinSector(&insidePoint, sector);

			//if (collisionMesh->DetermineIfPointIsInsideIndoorMesh(&insidePoint))
			if (collisionMesh->DetermineIfPointIsPotentiallyInsideOutdoorMesh(&insidePoint))
			{
				sector->insidePoints.Push(insidePoint);
			}
		}
	}
}

void Worker::RunSectorCruncherInternal()
{
	this->sectorCruncher->Run(this->startSectorIndex, this->numberOfSectorsToCrunch);
}

void Worker::CreateRandomPointWithinSector(Vec3* out/*, SectorMetrics* sectorMetrics*/, Sector* sector)
{
	*out = sector->aabb.from;

	Vec3 aabbSize;
	AABB::CalculateSize(&aabbSize, &sector->aabb);

	out->x += Math::GenerateRandomFloat() * aabbSize.x;
	out->y += Math::GenerateRandomFloat() * aabbSize.y;
	out->z += Math::GenerateRandomFloat() * aabbSize.z;
}