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
	this->threadManager = factory->MakeThreadManager();
	this->timestampProvider = factory->MakeTimestampProvider();

	for (int i = 0; i < 4; i++)
	{
		this->workers.Push(factory->MakeWorker());
	}
}

Engine::~Engine()
{
	SafeDeleteAndNull(this->logger);
	SafeDeleteAndNull(this->worldMeshAsset);
	SafeDeleteAndNull(this->threadManager);
	SafeDeleteAndNull(this->timestampProvider);
	
	for (int i = 0; i < this->workers.GetLength(); i++)
	{
		SafeDeleteAndNull(this->workers[i]);
	}

	for (int i = 0; i < this->lightAtlases.GetLength(); i++)
	{
		SafeDeleteAndNull(this->lightAtlases[i]);
	}
}

void Engine::BuildLightAtlases(const char* worldMeshAssetFilePath, const char* assetsFolderPath)
{
	double startTime = this->timestampProvider->GetTimestampMillis();

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

		this->InitLightAtlases();

		this->ComputeLightIslandsOnWorkers();

		this->WriteOutputFiles();
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

ILightAtlas* Engine::GetLightAtlas(int index)
{
	return this->lightAtlases[index];
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

void Engine::InitWorkers()
{
	int numberOfMeshChunksToCrunchPerWorker = (int)ceilf(this->worldMeshAsset->GetCollisionMesh()->GetNumberOfChunks() / (float)this->workers.GetLength());

	for (int i = 0; i < this->workers.GetLength(); i++)
	{
		IWorker* worker = this->workers[i];

		int startMeshChunkIndex = numberOfMeshChunksToCrunchPerWorker * i;

		worker->Init(startMeshChunkIndex, numberOfMeshChunksToCrunchPerWorker);
	}
}

void Engine::InitLightAtlases()
{
	IFactory* factory = GetFactory();

	ILightAtlas* lightAtlas = factory->MakeLightAtlas();
	lightAtlas->Allocate(4096, 4096);

	this->lightAtlases.Push(lightAtlas);
}

void Engine::ComputeLightIslandsOnWorkers()
{
	this->logger->Write("Computing light islands...");

	for (int i = 0; i < this->workers.GetLength(); i++)
	{
		IWorker* worker = this->workers[i];

		worker->ComputeLightIslandsAsync();
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

void Engine::WriteOutputFiles()
{
	this->logger->Write("Writing output files...");

	for (int i = 0; i < this->lightAtlases.GetLength(); i++)
	{
		ILightAtlas* lightAtlas = this->lightAtlases[i];
		lightAtlas->WriteToPngFile("c:/temp/test-light-atlas.png");
	}

	this->logger->Write("... done");
}