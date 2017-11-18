#include "Core/EngineBehaviour.h"

EngineBehaviour::EngineBehaviour()
{
	IFactory* factory = GetFactory();

	this->worker = factory->MakeWorker(this);
}

EngineBehaviour::~EngineBehaviour()
{
	this->worker->Stop();

	SafeDeleteAndNull(this->worker);
}

bool EngineBehaviour::PostCoreAssetLoadInit()
{
	this->worker->Start();

	return true;
}

void EngineBehaviour::StartProducingNextFrame(RasterJob* rasterJob)
{
	this->rasterJob = rasterJob;
	this->worker->DoWork();
}

void EngineBehaviour::WaitForProductionOfNextFrameToFinish()
{
	this->worker->WaitForWorkToComplete();
}

void EngineBehaviour::Run()
{
	IEngine* engine = GetEngine();
	IAssetManager* assetManager = engine->GetAssetManager();
	ISceneManager* sceneManager = engine->GetSceneManager();
	IRenderer* renderer = engine->GetRenderer();

	bool renderLoadingScreen = false;

	if (assetManager->GetIsLoading())
	{
		renderLoadingScreen = true;
	}
	else if (engine->GetMapState() == EngineMapStateNotAssigned)
	{
		engine->BeginLoadingMap(engine->GetStartUpArgs()->startUpMapPath);
		renderLoadingScreen = true;
	}
	else
	{
		// Ensure the map asset ref has been resolved.
		AssetRef* mapAssetRef = engine->GetMapAssetRef();
		if (mapAssetRef->index == -1)
		{
			assetManager->ResolveAssetRefIndex(mapAssetRef);
		}

		// Ensure the scene manager is initialised.
		if (!sceneManager->GetIsInitialised())
		{
			sceneManager->Init();
		}

		// Execute the game logic.
		sceneManager->ExecuteGameLogic();

		if (engine->GetMapState() == EngineMapStateLoading)
		{
			renderLoadingScreen = true;
		}
	}

	// Do some rendering.
	if (renderLoadingScreen)
	{
		// FIXME - move to renderer.
		this->rasterJob->drawWorldMeshChunkJobItems.Clear();
		this->rasterJob->drawStaticMeshJobItems.Clear();
		this->rasterJob->drawSkinnedMeshJobItems.Clear();
		this->rasterJob->drawVolumeRasterJobItems.Clear();
		this->rasterJob->drawSprite2DJobItems.Clear();
		this->rasterJob->drawSprite3DJobItems.Clear();
		this->rasterJob->drawLineSphereJobItems.Clear();
		this->rasterJob->drawLineCubeJobItems.Clear();

		RgbaFloat::SetRed(&this->rasterJob->clearColour);
	}
	else
	{
		renderer->RenderScene(this->rasterJob);
	}
}
