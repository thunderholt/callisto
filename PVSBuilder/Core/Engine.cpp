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
	
	for (int i = 0; i < this->workers.GetLength(); i++)
	{
		SafeDeleteAndNull(this->workers[i]);
	}

	SafeDeleteArrayAndNull(this->sectors);
}

void Engine::BuildPVS(const char* worldMeshAssetFilePath, SectorMetrics sectorMetrics)
{
	this->sectorMetrics = sectorMetrics;

	if (!this->worldMeshAsset->Load(worldMeshAssetFilePath))
	{
		this->logger->Write("Build PVS failed.");
	}
	else
	{
		this->InitSectors();

		//this->pointCompletelyOutsideOfCollisionMeshExtremities = this->worldMeshAsset->GetCollisionMesh()->FindPointCompletelyOutsideOfExtremities();

		this->sectorVisibilityLookup->Allocate(this->sectorMetrics.numberOfSectors);
		
		this->InitWorkers();

		this->ComputeSectorInsidePointsOnWorkers();

		this->RunSectorCruncherOnWorkers(SectorCruncherTypeBruteForce);

		this->ComputeSectorOutputVariables();

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

Sector* Engine::GetSectors()
{
	return this->sectors;
}

/*Vec3* Engine::GetPointCompletelyOutsideOfCollisionMeshExtremities()
{
	return &this->pointCompletelyOutsideOfCollisionMeshExtremities;
}*/

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
	this->sectors = new Sector[this->sectorMetrics.numberOfSectors];
	memset(this->sectors, 0, sizeof(Sector) * this->sectorMetrics.numberOfSectors);

	int sectorIndex = 0;

	for (int z = 0; z < this->sectorMetrics.sectorCounts[2]; z++) 
	{
		for (int y = 0; y < this->sectorMetrics.sectorCounts[1]; y++)
		{
			for (int x = 0; x < this->sectorMetrics.sectorCounts[0]; x++)
			{
				Sector* sector = &this->sectors[sectorIndex];

				Vec3::Set(
					&sector->origin,
					x * this->sectorMetrics.sectorSize,
					y * this->sectorMetrics.sectorSize,
					z * this->sectorMetrics.sectorSize);

				Vec3::Add(&sector->origin, &sector->origin, &this->sectorMetrics.originOffset);

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

void Engine::ComputeSectorOutputVariables()
{
	int offset = 0;
	this->numberOfVisibleSectorIndexes = 0;

	for (int sectorAIndex = 0; sectorAIndex < this->sectorMetrics.numberOfSectors; sectorAIndex++)
	{
		Sector* sectorA = &this->sectors[sectorAIndex];
		sectorA->visibleSectorIndexesOffset = offset;

		for (int sectorBIndex = 0; sectorBIndex < this->sectorMetrics.numberOfSectors; sectorBIndex++)
		{
			if (this->sectorVisibilityLookup->GetSectorVisibilityState(sectorAIndex, sectorBIndex) == SectorVisibilityStateVisible)
			{
				offset++;
				sectorA->numberOfVisibleSectors++;
				this->numberOfVisibleSectorIndexes++;
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
	sprintf(buffer, "number-of-visible-sector-indexes %d\n", this->numberOfVisibleSectorIndexes);
	outputFile->WriteString(buffer);

	// Write the sector origins.
	outputFile->WriteString("sector-origins\n");

	for (int sectorIndex = 0; sectorIndex < this->sectorMetrics.numberOfSectors; sectorIndex++)
	{
		Sector* sector = &this->sectors[sectorIndex];
		sprintf(buffer, "%f %f %f ", sector->origin.x, sector->origin.y, sector->origin.z);
		outputFile->Write(buffer, strlen(buffer));
	}

	outputFile->WriteString("\n");

	// Write the sector visible sector indexes offsets.
	outputFile->WriteString("sector-visible-sector-indexes-offsets\n");

	for (int sectorIndex = 0; sectorIndex < this->sectorMetrics.numberOfSectors; sectorIndex++)
	{
		Sector* sector = &this->sectors[sectorIndex];
		sprintf(buffer, "%d ", sector->visibleSectorIndexesOffset);
		outputFile->Write(buffer, strlen(buffer));
	}

	outputFile->WriteString("\n");

	// Write the sector visible sector index quantities.
	outputFile->WriteString("sector-visible-sector-index-quantities\n");

	for (int sectorIndex = 0; sectorIndex < this->sectorMetrics.numberOfSectors; sectorIndex++)
	{
		Sector* sector = &this->sectors[sectorIndex];
		sprintf(buffer, "%d ", sector->numberOfVisibleSectors);
		outputFile->Write(buffer, strlen(buffer));
	}

	outputFile->WriteString("\n");

	// Write the visible sector indexes.
	outputFile->WriteString("visible-sector-indexes\n");

	for (int sectorAIndex = 0; sectorAIndex < this->sectorMetrics.numberOfSectors; sectorAIndex++)
	{
		Sector* sectorA = &this->sectors[sectorAIndex];

		for (int sectorBIndex = 0; sectorBIndex < this->sectorMetrics.numberOfSectors; sectorBIndex++)
		{
			if (this->sectorVisibilityLookup->GetSectorVisibilityState(sectorAIndex, sectorBIndex) == SectorVisibilityStateVisible)
			{
				if (sectorAIndex > 0 || sectorBIndex > 0)
				{
					outputFile->WriteString(" ");
				}

				sprintf(buffer, "%d", sectorBIndex);
				outputFile->Write(buffer, strlen(buffer));
			}
		}
	}

	// We're done!
	outputFile->Close();

	delete outputFile;

	this->logger->Write("... done.");
}