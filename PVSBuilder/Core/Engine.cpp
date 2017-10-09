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
	this->sectorVisibilityLookup = factory->MakeSectorVisibilityLookup();
	this->threadManager = factory->MakeThreadManager();
	this->timestampProvider = factory->MakeTimestampProvider();

	for (int i = 0; i < 4; i++)
	{
		this->workers.Push(factory->MakeWorker());
	}

	this->sectors = null;
	//Vec3::Zero(&this->pointCompletelyOutsideOfCollisionMeshExtremities);
}

Engine::~Engine()
{
	SafeDeleteAndNull(this->logger);
	SafeDeleteAndNull(this->worldMeshAsset);
	SafeDeleteAndNull(this->sectorVisibilityLookup);
	SafeDeleteAndNull(this->threadManager);
	SafeDeleteAndNull(this->timestampProvider);
	
	for (int i = 0; i < this->workers.GetLength(); i++)
	{
		SafeDeleteAndNull(this->workers[i]);
	}

	SafeDeleteArrayAndNull(this->sectors);
}

void Engine::BuildPVS(const char* worldMeshAssetFilePath, SectorMetrics sectorMetrics)
{
	double startTime = this->timestampProvider->GetTimestampMillis();

	this->sectorMetrics = sectorMetrics;

	if (!this->worldMeshAsset->Load(worldMeshAssetFilePath))
	{
		this->logger->Write("Build PVS failed.");
	}
	else
	{
		this->InitSectorMetrics();

		this->InitSectors();

		//this->pointCompletelyOutsideOfCollisionMeshExtremities = this->worldMeshAsset->GetCollisionMesh()->FindPointCompletelyOutsideOfExtremities();

		this->sectorVisibilityLookup->Allocate(this->sectorMetrics.numberOfSectors);
		
		this->InitWorkers();

		this->ComputeSectorInsidePointsOnWorkers();

		this->RunSectorCruncherOnWorkers(SectorCruncherTypeBruteForce);

		//this->ComputeSectorOutputVariables();
		this->BuildSectorVisibleSectors();

		this->WriteOutputFile();

		/*///////////////////// Test code ///////////
		for (int i = 0; i < this->sectorMetrics.numberOfSectors; i++)
		{
			for (int j = 0; j < this->sectorMetrics.numberOfSectors; j++)
			{
				if (this->sectorVisibilityLookup->GetSectorVisibilityState(i, j) != SectorVisibilityStateVisible)
				{
					this->logger->Write("Missed sector.");
				}
			}

			if (this->sectors[i].numberOfInsidePoints != 100)
			{
				this->logger->Write("Bad number of inside points.");
			}
		}
		///////////////////////////////////////////*/
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

ISectorVisibilityLookup* Engine::GetSectorVisibilityLookup()
{
	return this->sectorVisibilityLookup;
}

SectorMetrics* Engine::GetSectorMetrics()
{
	return &this->sectorMetrics;
}

IThreadManager* Engine::GetThreadManager()
{
	return this->threadManager;
}

ITimestampProvider* Engine::GetTimestampProvider()
{
	return this->timestampProvider;
}

Sector* Engine::GetSectors()
{
	return this->sectors;
}

/*Vec3* Engine::GetPointCompletelyOutsideOfCollisionMeshExtremities()
{
	return &this->pointCompletelyOutsideOfCollisionMeshExtremities;
}*/

void Engine::InitSectorMetrics()
{
	// Build the grid planes.
	Vec3 planeNormals[3];
	Vec3::Set(&planeNormals[0], 1.0f, 0.0f, 0.0f);
	Vec3::Set(&planeNormals[1], 0.0f, 1.0f, 0.0f);
	Vec3::Set(&planeNormals[2], 0.0f, 0.0f, 1.0f);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j <= this->sectorMetrics.sectorCounts[i]; j++)
		{
			Vec3 pointOnPlane;
			Vec3::Scale(&pointOnPlane, &planeNormals[i], j * this->sectorMetrics.sectorSize);

			Plane plane;
			Plane::FromNormalAndPoint(&plane, &planeNormals[i], &pointOnPlane);

			this->sectorMetrics.gridPlanes.Push(plane);
		}
	}
}

void Engine::InitWorkers()
{
	int numberOfSectorsToCrunchPerWorker = (int)ceilf(this->sectorMetrics.numberOfSectors / (float)this->workers.GetLength());

	for (int i = 0; i < this->workers.GetLength(); i++)
	{
		IWorker* worker = this->workers[i];

		int startSectorIndex = numberOfSectorsToCrunchPerWorker * i;

		worker->Init(startSectorIndex, numberOfSectorsToCrunchPerWorker);
	}
}

void Engine::InitSectors()
{
	ICollisionMesh* collisionMesh = this->worldMeshAsset->GetCollisionMesh();

	this->sectors = new Sector[this->sectorMetrics.numberOfSectors];

	int sectorIndex = 0;

	//Vec3 sectorSize;
	//Vec3::Set(&sectorSize, this->sectorMetrics.sectorSize, this->sectorMetrics.sectorSize, this->sectorMetrics.sectorSize);

	for (int z = 0; z < this->sectorMetrics.sectorCounts[2]; z++) 
	{
		for (int y = 0; y < this->sectorMetrics.sectorCounts[1]; y++)
		{
			for (int x = 0; x < this->sectorMetrics.sectorCounts[0]; x++)
			{
				Sector* sector = &this->sectors[sectorIndex];

				// Clear the inside points.
				sector->numberOfInsidePoints = 0;
				memset(sector->insidePoints, 0, sizeof(Vec3) * SectorMaxInsidePoints);

				// Compute and set the sector origin.
				Vec3::Set(
					&sector->origin,
					x * this->sectorMetrics.sectorSize,
					y * this->sectorMetrics.sectorSize,
					z * this->sectorMetrics.sectorSize);

				Vec3::Add(&sector->origin, &sector->origin, &this->sectorMetrics.originOffset);

				/*// Build the AABB.
				sector->aabb.from = sector->origin;
				Vec3::Add(&sector->aabb.to, &sector->aabb.from, &sectorSize);*/

				// Find the resident world mesh chunk indexes for this sector.
				AABB sectorAabb;
				sectorAabb.from = sector->origin;
				Vec3::Add(&sectorAabb.to, &sectorAabb.from, this->sectorMetrics.sectorSize, this->sectorMetrics.sectorSize, this->sectorMetrics.sectorSize);

				for (int chunkIndex = 0; chunkIndex < collisionMesh->GetNumberOfChunks(); chunkIndex++)
				{
					CollisionMeshChunk* chunk = collisionMesh->GetChunk(chunkIndex);

					if (AABB::CheckIntersectsAABB(&sectorAabb, &chunk->aabb))
					{
						sector->residentWorldMeshChunkIndexes.Push(chunkIndex);
					}
				}

				sectorIndex++;
			}
		}
	}
}

void Engine::ComputeSectorInsidePointsOnWorkers()
{
	this->logger->Write("Computing inside points...");

	for (int i = 0; i < this->workers.GetLength(); i++)
	{
		IWorker* worker = this->workers[i];

		worker->ComputeSectorInsidePointsAsync();
	}

	this->WaitForAllWorkersToFinish();

	this->logger->Write("... done.");
}

void Engine::RunSectorCruncherOnWorkers(SectorCruncherType sectorCruncherType)
{
	this->logger->Write("Running sector cruncher...");

	for (int i = 0; i < this->workers.GetLength(); i++)
	{
		IWorker* worker = this->workers[i];
		
		worker->RunSectorCruncherAsync(sectorCruncherType);
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
			this->threadManager->Sleep(100);
		}

	} while (!allWorkersAreFinished);
}

/*void Engine::ComputeSectorOutputVariables()
{
	ICollisionMesh* collisionMesh = this->worldMeshAsset->GetCollisionMesh();
	
	for (int sectorAIndex = 0; sectorAIndex < this->sectorMetrics.numberOfSectors; sectorAIndex++)
	{
		Sector* sectorA = &this->sectors[sectorAIndex];
	
		// Find the visible sector indexes for this sector.
		for (int sectorBIndex = 0; sectorBIndex < this->sectorMetrics.numberOfSectors; sectorBIndex++)
		{
			if (this->sectorVisibilityLookup->GetSectorVisibilityState(sectorAIndex, sectorBIndex) == SectorVisibilityStateVisible)
			{
				sectorA->visibleSectorIndexes.Push(sectorBIndex);
			}
		}

		// Find the resident world mesh chunk indexes for this sector.
		AABB sectorAabb;
		sectorAabb.from = sectorA->origin;
		Vec3::Add(&sectorAabb.to, &sectorAabb.from, this->sectorMetrics.sectorSize, this->sectorMetrics.sectorSize, this->sectorMetrics.sectorSize);

		for (int chunkIndex = 0; chunkIndex < collisionMesh->GetNumberOfChunks(); chunkIndex++)
		{
			CollisionMeshChunk* chunk = collisionMesh->GetChunk(chunkIndex);
			
			if (AABB::CheckIntersectsAABB(&sectorAabb, &chunk->aabb))
			{
				sectorA->residentWorldMeshChunkIndexes.Push(chunkIndex);
			}
		}
	}
}*/

/*
void Engine::BuildSectorResidentWorldMeshChunkIndexes()
{
	ICollisionMesh* collisionMesh = this->worldMeshAsset->GetCollisionMesh();

	for (int sectorIndex = 0; sectorIndex < this->sectorMetrics.numberOfSectors; sectorIndex++)
	{
		Sector* sector = &this->sectors[sectorIndex];

		// Find the resident world mesh chunk indexes for this sector.
		AABB sectorAabb;
		sectorAabb.from = sector->origin;
		Vec3::Add(&sectorAabb.to, &sectorAabb.from, this->sectorMetrics.sectorSize, this->sectorMetrics.sectorSize, this->sectorMetrics.sectorSize);

		for (int chunkIndex = 0; chunkIndex < collisionMesh->GetNumberOfChunks(); chunkIndex++)
		{
			CollisionMeshChunk* chunk = collisionMesh->GetChunk(chunkIndex);

			if (AABB::CheckIntersectsAABB(&sectorAabb, &chunk->aabb))
			{
				sector->residentWorldMeshChunkIndexes.Push(chunkIndex);
			}
		}
	}
}*/

void Engine::BuildSectorVisibleSectors()
{
	ICollisionMesh* collisionMesh = this->worldMeshAsset->GetCollisionMesh();

	for (int sectorAIndex = 0; sectorAIndex < this->sectorMetrics.numberOfSectors; sectorAIndex++)
	{
		Sector* sectorA = &this->sectors[sectorAIndex];

		// Find the visible sector indexes for this sector.
		for (int sectorBIndex = 0; sectorBIndex < this->sectorMetrics.numberOfSectors; sectorBIndex++)
		{
			if (this->sectorVisibilityLookup->GetSectorVisibilityState(sectorAIndex, sectorBIndex) == SectorVisibilityStateVisible)
			{
				sectorA->visibleSectorIndexes.Push(sectorBIndex);
			}
		}
	}
}

void Engine::WriteOutputFile()
{
	IFactory* factory = GetFactory();
	IFile* outputFile = factory->MakeFile();

	this->logger->Write("Writing output file...");

	outputFile->OpenForWriting("test.txt");

	char buffer[128];

	// Write the sector size.
	sprintf(buffer, "sector-size %f\n", this->sectorMetrics.sectorSize);
	outputFile->WriteString(buffer);

	// Write the sector counts.
	sprintf(buffer, "sector-counts %d %d %d\n", this->sectorMetrics.sectorCounts[0], this->sectorMetrics.sectorCounts[1], this->sectorMetrics.sectorCounts[2]);
	outputFile->WriteString(buffer);

	// Write the sector origin offset.
	sprintf(buffer, "sector-origin-offset %f %f %f\n", this->sectorMetrics.originOffset.x, this->sectorMetrics.originOffset.y, this->sectorMetrics.originOffset.z);
	outputFile->WriteString(buffer);

	// Write the number of sectors.
	sprintf(buffer, "number-of-sectors %d\n", this->sectorMetrics.numberOfSectors);
	outputFile->WriteString(buffer);

	// Write the number of visible sector indexes.
	int numberOfVisibleSectorIndexes = 0;
	for (int sectorIndex = 0; sectorIndex < this->sectorMetrics.numberOfSectors; sectorIndex++)
	{
		Sector* sector = &this->sectors[sectorIndex];
		numberOfVisibleSectorIndexes += sector->visibleSectorIndexes.GetLength();
	}

	sprintf(buffer, "number-of-visible-sector-indexes %d\n", numberOfVisibleSectorIndexes);
	outputFile->WriteString(buffer);

	// Write the number of resident world mesh chunk indexes.
	int numberOfResidentWorldMeshChunkIndexes = 0;
	for (int sectorIndex = 0; sectorIndex < this->sectorMetrics.numberOfSectors; sectorIndex++)
	{
		Sector* sector = &this->sectors[sectorIndex];
		numberOfResidentWorldMeshChunkIndexes += sector->residentWorldMeshChunkIndexes.GetLength();
	}

	sprintf(buffer, "number-of-resident-world-mesh-chunk-indexes %d\n", numberOfResidentWorldMeshChunkIndexes);
	outputFile->WriteString(buffer);

	// Write the sector origins.
	outputFile->WriteString("sector-origins\n");

	for (int sectorIndex = 0; sectorIndex < this->sectorMetrics.numberOfSectors; sectorIndex++)
	{
		Sector* sector = &this->sectors[sectorIndex];
		sprintf(buffer, "%f %f %f ", sector->origin.x, sector->origin.y, sector->origin.z);
		outputFile->WriteString(buffer);
	}

	outputFile->WriteString("\n");

	// Write the sector visible sector indexes offsets.
	outputFile->WriteString("sector-visible-sector-indexes-offsets\n");

	int offset = 0;
	for (int sectorIndex = 0; sectorIndex < this->sectorMetrics.numberOfSectors; sectorIndex++)
	{
		Sector* sector = &this->sectors[sectorIndex];
		sprintf(buffer, "%d ", offset);
		outputFile->WriteString(buffer);
		offset += sector->visibleSectorIndexes.GetLength();
	}

	outputFile->WriteString("\n");

	// Write the sector visible sector index quantities.
	outputFile->WriteString("sector-visible-sector-index-quantities\n");

	for (int sectorIndex = 0; sectorIndex < this->sectorMetrics.numberOfSectors; sectorIndex++)
	{
		Sector* sector = &this->sectors[sectorIndex];
		sprintf(buffer, "%d ", sector->visibleSectorIndexes.GetLength());
		outputFile->WriteString(buffer);
	}

	outputFile->WriteString("\n");

	// Write the sector resident world mesh chunk indexes offsets.
	outputFile->WriteString("sector-resident-world-mesh-chunk-indexes-offsets\n");

	offset = 0;
	for (int sectorIndex = 0; sectorIndex < this->sectorMetrics.numberOfSectors; sectorIndex++)
	{
		Sector* sector = &this->sectors[sectorIndex];
		sprintf(buffer, "%d ", offset);
		outputFile->WriteString(buffer);
		offset += sector->residentWorldMeshChunkIndexes.GetLength();
	}

	outputFile->WriteString("\n");

	// Write the sector resident world mesh chunk index quantities.
	outputFile->WriteString("sector-resident-world-mesh-chunk-index-quantities\n");

	for (int sectorIndex = 0; sectorIndex < this->sectorMetrics.numberOfSectors; sectorIndex++)
	{
		Sector* sector = &this->sectors[sectorIndex];
		sprintf(buffer, "%d ", sector->residentWorldMeshChunkIndexes.GetLength());
		outputFile->WriteString(buffer);
	}

	outputFile->WriteString("\n");

	// Write the visible sector indexes.
	outputFile->WriteString("visible-sector-indexes\n");

	for (int sectorAIndex = 0; sectorAIndex < this->sectorMetrics.numberOfSectors; sectorAIndex++)
	{
		Sector* sector = &this->sectors[sectorAIndex];
		for (int i = 0; i < sector->visibleSectorIndexes.GetLength(); i++)
		{
			int sectorBIndex = sector->visibleSectorIndexes[i];

			if (sectorAIndex > 0 || sectorBIndex > 0)
			{
				outputFile->WriteString(" ");
			}

			sprintf(buffer, "%d", sectorBIndex);
			outputFile->WriteString(buffer);
		}
	}

	outputFile->WriteString("\n");

	// Write the resident world mesh chunk indexes.
	outputFile->WriteString("resident-world-mesh-chunk-indexes\n");

	for (int sectorIndex = 0; sectorIndex < this->sectorMetrics.numberOfSectors; sectorIndex++)
	{
		Sector* sector = &this->sectors[sectorIndex];
		for (int i = 0; i < sector->residentWorldMeshChunkIndexes.GetLength(); i++)
		{
			int worldMeshChunkIndex = sector->residentWorldMeshChunkIndexes[i];

			if (sectorIndex > 0 || i > 0)
			{
				outputFile->WriteString(" ");
			}

			sprintf(buffer, "%d", worldMeshChunkIndex);
			outputFile->WriteString(buffer);
		}
	}

	// We're done!
	outputFile->WriteString("\n-- end --");
	outputFile->Close();

	delete outputFile;

	this->logger->Write("... done.");
}