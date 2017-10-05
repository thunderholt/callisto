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

	this->unitTester = factory->MakeUnitTester();
	this->fileLoader = factory->MakeFileLoader();
	this->threadManager = factory->MakeThreadManager();
	this->timestampProvider = factory->MakeTimestampProvider();
	this->rasterizer = factory->MakeRasterizer();
	this->rasterJobManager = factory->MakeRasterJobManager();
	this->assetManager = factory->MakeAssetManager();
	this->sceneManager = factory->MakeSceneManager();
	this->actorControllerManager = factory->MakeActorControllerManager();
	this->volumeManager = factory->MakeVolumeManager();
	this->frameTimer = factory->MakeFrameTimer();
	this->behaviour = factory->MakeEngineBehaviour();
	this->renderer = factory->MakeRenderer();
	this->debugVisualisationRenderer = factory->MakeDebugVisualisationRenderer();
	this->keyboard = factory->MakeKeyboard();
	this->mouse = factory->MakeMouse();
	this->touchScreen = factory->MakeTouchScreen();
	this->keyboardMouseGamePad = factory->MakeKeyboardMouseGamePad();
	this->physicalGamePad = factory->MakePhysicalGamePad();
	this->touchScreenGamePad = factory->MakeTouchScreenGamePad();
	this->aggregatedGamePad = factory->MakeAggregatedGamePad();
	this->logger = factory->MakeLogger();

	this->gameController = null;
	this->hasMapToLoad = false;
	strcpy(this->mapToLoadPath, "");
	this->mustTerminate = false;
	//this->mapIsAssigned = false;
	this->mapState = EngineMapStateNotAssigned;
	this->postCoreAssetLoadInitialisationHasRun = false;
	this->frameId = 1;

	/*IJsonParser* jsonParser = factory->MakeJsonParser();
	IJsonValue* jsonValue = jsonParser->ParseJson("{ \"value1\": \"Hello world\", \"value2\": \"Awesome\", \"value3\": { \"subValue1\": \"Brilliant!\" }, \"value4\": 3.142, \"value5\": [1, 2, 3], \"value6\": {}, \"value7\": [] }");

	this->logger->Write("value1: %s, value2: %s, subValue1: %s, value4: %lf, value5[0]: %lf, value5[1]: %lf, value5[2]: %lf, value6.length: %ld, value7.length: %ld",
		jsonValue->GetObjectProperty("value1")->GetStringValue(),
		jsonValue->GetObjectProperty("value2")->GetStringValue(),
		jsonValue->GetObjectProperty("value3")->GetObjectProperty("subValue1")->GetStringValue(),
		jsonValue->GetObjectProperty("value4")->GetDoubleValue(),
		jsonValue->GetObjectProperty("value5")->GetArrayElement(0)->GetDoubleValue(),
		jsonValue->GetObjectProperty("value5")->GetArrayElement(1)->GetDoubleValue(),
		jsonValue->GetObjectProperty("value5")->GetArrayElement(2)->GetDoubleValue(),
		jsonValue->GetObjectProperty("value6")->GetNumberOfObjectProperties(),
		jsonValue->GetObjectProperty("value7")->GetNumberOfArrayElements());

	delete jsonValue;
	delete jsonParser;*/

}

Engine::~Engine()
{
	this->logger->Write("Performing engine clean-up...");

	SafeDeleteAndNull(this->unitTester);
	SafeDeleteAndNull(this->gameController);
	SafeDeleteAndNull(this->behaviour);
	SafeDeleteAndNull(this->renderer);
	SafeDeleteAndNull(this->debugVisualisationRenderer);
	SafeDeleteAndNull(this->frameTimer);
	SafeDeleteAndNull(this->sceneManager);
	SafeDeleteAndNull(this->actorControllerManager);
	SafeDeleteAndNull(this->assetManager);
	SafeDeleteAndNull(this->volumeManager);
	SafeDeleteAndNull(this->rasterJobManager);
	SafeDeleteAndNull(this->rasterizer);
	SafeDeleteAndNull(this->fileLoader);
	SafeDeleteAndNull(this->threadManager);
	SafeDeleteAndNull(this->timestampProvider);
	SafeDeleteAndNull(this->keyboard);
	SafeDeleteAndNull(this->mouse);
	SafeDeleteAndNull(this->touchScreen);
	SafeDeleteAndNull(this->keyboardMouseGamePad);
	SafeDeleteAndNull(this->physicalGamePad);
	SafeDeleteAndNull(this->touchScreenGamePad);
	SafeDeleteAndNull(this->aggregatedGamePad);

	this->logger->Write("... engine clean-up complete.");

	SafeDeleteAndNull(this->logger);
}

void Engine::RegisterGameController(IGameController* gameController)
{
	this->gameController = gameController;
}

void Engine::Init()
{
	if (!this->unitTester->RunUnitTests())
	{
		this->mustTerminate = true;
	}
	else
	{
		this->logger->Write("Initialising engine...");
		this->fileLoader->Init();
		this->rasterizer->Init();
		this->keyboard->Init();
		this->mouse->Init();
		this->touchScreen->Init();
		this->keyboardMouseGamePad->Init();
		this->physicalGamePad->Init();
		this->touchScreenGamePad->Init();
		this->aggregatedGamePad->Init();
		this->assetManager->BeginLoadingCoreAssets();
		this->logger->Write("...engine initialisation complete.");
	}
}

void Engine::BeginLoadingMap(const char* filePath)
{
	strcpy(this->mapToLoadPath, filePath);
	this->hasMapToLoad = true;
	//this->mapIsAssigned = true;
	this->mapState = EngineMapStateLoading;
}

/*void Engine::CleanUp()
{
	
}*/

void Engine::Heartbeat()
{
	if (!this->mustTerminate)
	{
		if (this->assetManager->GetLoadFailed())
		{
			this->logger->Write("Asset load failed, engine must now terminate.");
			this->mustTerminate = true;
		}
		else
		{
			//this->fileLoader->Heartbeat();

			if (this->assetManager->GetIsLoadingCoreAssets())
			{
				this->assetManager->HeartBeat();
			}
			else if (this->EnsurePostCoreAssetLoadInitHasRun())
			{

				this->frameTimer->StartFrame();

				if (this->mapState == EngineMapStateLoading && !assetManager->GetIsLoading())
				{
					this->mapState = EngineMapStateLoaded;
				}

				this->mouse->Poll();
				this->keyboardMouseGamePad->Poll();
				this->physicalGamePad->Poll();
				this->touchScreenGamePad->Poll();
				this->aggregatedGamePad->Poll();
				this->keyboard->ClearEvents();
				this->touchScreen->ClearEvents();
				//this->mouse->ClearEvents();

				RasterJob* currentFrameRasterJob = this->rasterJobManager->GetCurrentFrameRasterJob();
				RasterJob* nextFrameRasterJob = this->rasterJobManager->GetNextFrameRasterJob();

				if (this->hasMapToLoad)
				{
					this->BeginLoadingMapInternal();
				}

				this->behaviour->StartProducingNextFrame(nextFrameRasterJob);

				this->assetManager->HeartBeat();

				this->rasterizer->RasterizeFrame(currentFrameRasterJob);

				this->behaviour->WaitForProductionOfNextFrameToFinish();

				this->rasterJobManager->SwapJobs();

				/////////////////// Test Code ///////////////
#ifdef WIN32
				if (keyboard->GetKeyState('R') == KeyStateDown)
				{
					this->GenerateTestPathTraceRender();
				}
#endif
				/////////////////////////////////////////////
			}
		}

		this->frameId++;
	}
}

IGameController* Engine::GetGameController()
{
	return this->gameController;
}

IRasterizer* Engine::GetRasterizer()
{
	return this->rasterizer;
}

IRasterJobManager* Engine::GetRasterJobManager()
{
	return this->rasterJobManager;
}

IAssetManager* Engine::GetAssetManager()
{
	return this->assetManager;
}

IEngineBehaviour* Engine::GetBehaviour()
{
	return this->behaviour;
}

IRenderer* Engine::GetRenderer()
{
	return this->renderer;
}

IDebugVisualisationRenderer* Engine::GetDebugVisualisationRenderer()
{
	return this->debugVisualisationRenderer;
}

IFrameTimer* Engine::GetFrameTimer()
{
	return this->frameTimer;
}

ISceneManager* Engine::GetSceneManager()
{
	return this->sceneManager;
}

IActorControllerManager* Engine::GetActorControllerManager()
{
	return this->actorControllerManager;
}

IVolumeManager* Engine::GetVolumeManager()
{
	return this->volumeManager;
}

AssetRef* Engine::GetMapAssetRef()
{
	return &this->mapAssetRef;
}

bool Engine::GetMustTerminate()
{
	return this->mustTerminate;
}

/*bool Engine::GetMapIsAssigned()
{
	return this->mapIsAssigned;
}*/

EngineMapState Engine::GetMapState()
{
	return this->mapState;
}

int Engine::GetFrameId()
{
	return this->frameId;
}

IFileLoader* Engine::GetFileLoader()
{
	return this->fileLoader;
}

IThreadManager* Engine::GetThreadManager()
{
	return this->threadManager;
}

ITimestampProvider* Engine::GetTimestampProvider()
{
	return this->timestampProvider;
}

IKeyboard* Engine::GetKeyboard()
{
	return this->keyboard;
}

IMouse* Engine::GetMouse()
{
	return this->mouse;
}

ITouchScreen* Engine::GetTouchScreen()
{
	return this->touchScreen;
}

IKeyboardMouseGamePad* Engine::GetKeyboardMouseGamePad()
{
	return this->keyboardMouseGamePad;
}

IPhysicalGamePad* Engine::GetPhysicalGamePad()
{
	return this->physicalGamePad;
}

ITouchScreenGamePad* Engine::GetTouchScreenGamePad()
{
	return this->touchScreenGamePad;
}

IAggregatedGamePad* Engine::GetAggregatedGamePad()
{
	return this->aggregatedGamePad;
}

ILogger* Engine::GetLogger()
{
	return this->logger;
}

bool Engine::EnsurePostCoreAssetLoadInitHasRun()
{
	bool success = true;

	if (!this->postCoreAssetLoadInitialisationHasRun)
	{
		if (!this->rasterizer->PostCoreAssetLoadInit())
		{
			success = false;
		}
		else if (!this->behaviour->PostCoreAssetLoadInit())
		{
			success = false;
		}
		else if (!this->debugVisualisationRenderer->PostCoreAssetLoadInit())
		{
			success = false;
		}

		if (!success)
		{
			this->mustTerminate = true;
		}

		this->postCoreAssetLoadInitialisationHasRun = true;
	}

	return success;
}

void Engine::BeginLoadingMapInternal()
{
	this->logger->Write("Loading map '%s'.", this->mapToLoadPath);

	this->mapAssetRef.assetType = AssetTypeMap;
	strcpy(this->mapAssetRef.filePath, this->mapToLoadPath);
	this->mapAssetRef.index = -1;

	this->assetManager->LoadMap(this->mapAssetRef.filePath);
	this->sceneManager->Reset();
	this->hasMapToLoad = false;
	strcpy(this->mapToLoadPath, "");
}

//////////////////////////////// Test code //////////////////////////

#ifdef WIN32

#include <windows.h>

void WriteOutputBmp(int width, int height, BgrUByte* outputPixels)
{
	FILE* pFile = fopen("test.bmp", "wb"); // wb -> w: writable b: binary, open as writable and binary

	BITMAPINFOHEADER BMIH;                         // BMP header
	BMIH.biSize = sizeof(BITMAPINFOHEADER);
	BMIH.biSizeImage = width * height * 3;
	// Create the bitmap for this OpenGL context
	BMIH.biSize = sizeof(BITMAPINFOHEADER);
	BMIH.biWidth = width;
	BMIH.biHeight = height;
	BMIH.biPlanes = 1;
	BMIH.biBitCount = 24;
	BMIH.biCompression = BI_RGB;
	BMIH.biSizeImage = width * height * 3;

	BITMAPFILEHEADER bmfh;                         // Other BMP header
	int nBitsOffset = sizeof(BITMAPFILEHEADER) + BMIH.biSize;
	LONG lImageSize = BMIH.biSizeImage;
	LONG lFileSize = nBitsOffset + lImageSize;
	bmfh.bfType = 'B' + ('M' << 8);
	bmfh.bfOffBits = nBitsOffset;
	bmfh.bfSize = lFileSize;
	bmfh.bfReserved1 = bmfh.bfReserved2 = 0;

	// Write the bitmap file header               // Saving the first header to file
	UINT nWrittenFileHeaderSize = fwrite(&bmfh, 1, sizeof(BITMAPFILEHEADER), pFile);

	// And then the bitmap info header            // Saving the second header to file
	UINT nWrittenInfoHeaderSize = fwrite(&BMIH, 1, sizeof(BITMAPINFOHEADER), pFile);

	// Finally, write the image data itself
	//-- the data represents our drawing          // Saving the file content in lpBits to file
	UINT nWrittenDIBDataSize = fwrite(outputPixels, 1, lImageSize, pFile);
	fclose(pFile); // closing the file.
}

void BuildOutputPixels(RgbFloat* inputPixels, BgrUByte* outputPixels, int outputWidth, int outputHeight)
{
	for (int y = 0; y < outputHeight; y++)
	{
		for (int x = 0; x < outputWidth; x++)
		{
			RgbFloat* inputPixel = &inputPixels[(outputHeight - y - 1) * outputWidth + x];
			RgbFloat::Saturate(inputPixel, inputPixel);

			BgrUByte* outputPixel = &outputPixels[y * outputWidth + x];
			outputPixel->r = (int)(255.0f * inputPixel->r);
			outputPixel->g = (int)(255.0f * inputPixel->g);
			outputPixel->b = (int)(255.0f * inputPixel->b);
		}
	}
}

/////////////////////////////// Technique 1 //////////////////////////////

RgbFloat T1_CalculateIntersectionColour(ICollisionMesh* collisionMesh, CollisionMeshIntersectionResult* intersectionResult, int* bounceCounts, int depth, int maxDepth)
{
	CollisionMeshChunk* chunk = collisionMesh->GetChunk(intersectionResult->chunkIndex);
	CollisionFace* face = collisionMesh->GetFace(intersectionResult->faceIndex);
	IMaterialAsset* materialAsset = GetEngine()->GetAssetManager()->GetMaterialAsset(chunk->materialAssetIndex);

	RgbFloat colour = materialAsset->GetEmmissiveColour();

	if (depth < maxDepth)
	{
		RgbFloat averageSubColour;
		RgbFloat::Zero(&averageSubColour);
		
		int bounceCount = bounceCounts[depth];

		for (int i = 0; i < bounceCount; i++)
		{
			Ray3 ray;
			ray.origin = intersectionResult->intersectionPoint;
			ray.normal = face->facePlane.normal;

			float purturbationAmount = 0.9f;
			
			ray.normal.x += Math::Lerp(-(purturbationAmount), (purturbationAmount), Math::GenerateRandomFloat());
			ray.normal.y += Math::Lerp(-(purturbationAmount), (purturbationAmount), Math::GenerateRandomFloat());
			ray.normal.z += Math::Lerp(-(purturbationAmount), (purturbationAmount), Math::GenerateRandomFloat());
			
			Vec3::Normalize(&ray.normal, &ray.normal);

			CollisionMeshIntersectionResult subIntersectionResult;
			if (collisionMesh->FindNearestRayIntersection(&subIntersectionResult, &ray))
			{
				RgbFloat subColour = T1_CalculateIntersectionColour(collisionMesh, &subIntersectionResult, bounceCounts, depth + 1, maxDepth);
				RgbFloat::Add(&averageSubColour, &averageSubColour, &subColour);
			}
		}

		RgbFloat::Scale(&averageSubColour, &averageSubColour, 1.0f / (float)bounceCount);

		RgbFloat::Add(&colour, &colour, &averageSubColour);
	}

	return colour;
}

void T1_Execute(int outputHeight, int outputWidth, Vec3* framePoints, Vec3* frameSteps, Vec3 rayDirectionOrigin, ICollisionMesh* collisionMesh, RgbFloat* inputPixels)
{
	//int bounceCounts[] = { 10, 4, 4 };
	//int bounceCounts[] = { 50, 4, 4 };
	int bounceCounts[] = { 200, 4, 2, 2 };

	int pixelIndex = 0;
	for (int y = 0; y < outputHeight; y++)
	{
		for (int x = 0; x < outputWidth; x++)
		{
			Vec3 pixelPoint;
			Vec3::ScaleAndAdd(&pixelPoint, &framePoints[0], &frameSteps[0], (float)x);
			Vec3::ScaleAndAdd(&pixelPoint, &pixelPoint, &frameSteps[1], (float)y);

			Ray3 ray;
			ray.origin = pixelPoint;
			Vec3::Sub(&ray.normal, &pixelPoint, &rayDirectionOrigin);
			Vec3::Normalize(&ray.normal, &ray.normal);

			CollisionMeshIntersectionResult intersectionResult;
			if (collisionMesh->FindNearestRayIntersection(&intersectionResult, &ray))
			{
				inputPixels[pixelIndex] = T1_CalculateIntersectionColour(collisionMesh, &intersectionResult, bounceCounts, 0, 3);
			}
			//Ray3::Set(&ray, &rayOrigin, )

			pixelIndex++;
		}
	}
}

/////////////////////////////// Technique 2 //////////////////////////////

float T2_CalculateShadowFactorForPoint(Vec3* point, Vec3* lightPos, ICollisionMesh* collisionMesh)
{
	float shadowFactor = 0.0f;

	int numberOfPasses = 30;
	for (int i = 0; i < numberOfPasses; i++)
	{
		CollisionLine line;
		line.from = *point;
		line.to = *lightPos;

		float purturbationAmount = 0.1f;
		line.to.x += Math::Lerp(-(purturbationAmount), (purturbationAmount), Math::GenerateRandomFloat());
		line.to.y += Math::Lerp(-(purturbationAmount), (purturbationAmount), Math::GenerateRandomFloat());
		line.to.z += Math::Lerp(-(purturbationAmount), (purturbationAmount), Math::GenerateRandomFloat());

		CollisionLine::FromOwnFromAndToPoints(&line);
		bool lightIsObscured = collisionMesh->FindNearestLineIntersection(null, &line);

		shadowFactor += lightIsObscured ? 0.0f : 1.0f;
	}

	shadowFactor /= numberOfPasses;

	return shadowFactor;
}

float T2_CalculateLightIntensityForFacePoint(Vec3* point, CollisionFace* face, Vec3* lightPos, float lightRadiusSqr, float baseDistanceToLightSqr, ICollisionMesh* collisionMesh, int depth)
{
	float intensity = 0.0f;

	float shadowFactor = T2_CalculateShadowFactorForPoint(point, lightPos, collisionMesh);

	if (shadowFactor > 0.0f)
	{
		float distanceToLightSqr = baseDistanceToLightSqr + Vec3::DistanceSqr(point, lightPos);
		float attentuation = 1.0f - (distanceToLightSqr / lightRadiusSqr);
		if (attentuation < 0.0f)
		{
			attentuation = 0.0f;
		}

		Vec3 directionToLight;
		Vec3::Sub(&directionToLight, lightPos, point);
		Vec3::Normalize(&directionToLight, &directionToLight);

		float lambert = Vec3::Dot(&face->facePlane.normal, &directionToLight);
		if (lambert < 0.0f)
		{
			lambert = 0.0f;
		}

		intensity = attentuation * lambert * shadowFactor;
	}
	
	if (depth == 0)
	{
		float indirectIntensity = 0.0f;
		int bounces = 10;

		for (int i = 0; i < bounces; i++)
		{
			Ray3 ray;
			ray.origin = *point;
			ray.normal = face->facePlane.normal;

			float purturbationAmount = 0.9f;

			ray.normal.x += Math::Lerp(-(purturbationAmount), (purturbationAmount), Math::GenerateRandomFloat());
			ray.normal.y += Math::Lerp(-(purturbationAmount), (purturbationAmount), Math::GenerateRandomFloat());
			ray.normal.z += Math::Lerp(-(purturbationAmount), (purturbationAmount), Math::GenerateRandomFloat());

			Vec3::Normalize(&ray.normal, &ray.normal);

			CollisionMeshIntersectionResult intersectionResult;
			if (collisionMesh->FindNearestRayIntersection(&intersectionResult, &ray))
			{
				float bounceBaseDistanceToLightSqr = Vec3::DistanceSqr(point, &intersectionResult.intersectionPoint);

				indirectIntensity += T2_CalculateLightIntensityForFacePoint(
					&intersectionResult.intersectionPoint,
					collisionMesh->GetFace(intersectionResult.faceIndex),
					lightPos, lightRadiusSqr, bounceBaseDistanceToLightSqr, collisionMesh, depth + 1);
			}
		}

		indirectIntensity /= (float)bounces;
		intensity += indirectIntensity;
	}

	return intensity;
}

void T2_Execute(int outputHeight, int outputWidth, Vec3* framePoints, Vec3* frameSteps, Vec3 rayDirectionOrigin, ICollisionMesh* collisionMesh, RgbFloat* inputPixels)
{
	//int bounceCounts[] = { 10, 4, 4 };
	//int bounceCounts[] = { 50, 4, 4 };
	//int bounceCounts[] = { 200, 4, 2, 2 };

	Vec3 lightPos;
	Vec3::Set(&lightPos, 2.0f, 1.0f, -2.0f);

	float lightRadius = 5.0f;
	float lightRadiusSqr = lightRadius * lightRadius;

	int pixelIndex = 0;
	for (int y = 0; y < outputHeight; y++)
	{
		for (int x = 0; x < outputWidth; x++)
		{
			Vec3 pixelPoint;
			Vec3::ScaleAndAdd(&pixelPoint, &framePoints[0], &frameSteps[0], (float)x);
			Vec3::ScaleAndAdd(&pixelPoint, &pixelPoint, &frameSteps[1], (float)y);

			Ray3 ray;
			ray.origin = pixelPoint;
			Vec3::Sub(&ray.normal, &pixelPoint, &rayDirectionOrigin);
			Vec3::Normalize(&ray.normal, &ray.normal);

			CollisionMeshIntersectionResult intersectionResult;
			if (collisionMesh->FindNearestRayIntersection(&intersectionResult, &ray))
			{
				CollisionFace* face = collisionMesh->GetFace(intersectionResult.faceIndex);

				float intensity = T2_CalculateLightIntensityForFacePoint(
					&intersectionResult.intersectionPoint, face, &lightPos, lightRadiusSqr, 0, collisionMesh, 0);

				inputPixels[pixelIndex].r = intensity;
				inputPixels[pixelIndex].g = intensity;
				inputPixels[pixelIndex].b = intensity;

				/*bool pointCanSeeLight = T2_PointCanSeeLight(&intersectionResult.intersection, &lightPos, collisionMesh);

				if (pointCanSeeLight)
				{
					float distanceToLightSqr = Vec3::DistanceSqr(&intersectionResult.intersection, &lightPos);
					float attentuation = 1.0f - (distanceToLightSqr / lightRadiusSqr);
					if (attentuation < 0.0f)
					{
						attentuation = 0.0f;
					}

					Vec3 directionToLight;
					Vec3::Sub(&directionToLight, &lightPos, &intersectionResult.intersection);
					Vec3::Normalize(&directionToLight, &directionToLight);

					float lambert = Vec3::Dot(&face->facePlane.normal, &directionToLight);
					if (lambert < 0.0f)
					{
						lambert = 0.0f;
					}

					float intensity = attentuation * lambert;

					inputPixels[pixelIndex].r = intensity;
					inputPixels[pixelIndex].g = intensity;
					inputPixels[pixelIndex].b = intensity;
				}
				else
				{
					for (int i = 0; i < 3; i++)
					{
						Ray3 ray2;
						ray2.origin = intersectionResult.intersection;
						ray2.normal = face->facePlane.normal;

						float purturbationAmount = 0.9f;

						ray2.normal.x += Math::Lerp(-(purturbationAmount), (purturbationAmount), Math::GenerateRandomFloat());
						ray2.normal.y += Math::Lerp(-(purturbationAmount), (purturbationAmount), Math::GenerateRandomFloat());
						ray2.normal.z += Math::Lerp(-(purturbationAmount), (purturbationAmount), Math::GenerateRandomFloat());

						Vec3::Normalize(&ray2.normal, &ray2.normal);
					}
				}*/
			}
			//Ray3::Set(&ray, &rayOrigin, )

			pixelIndex++;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void Engine::GenerateTestPathTraceRender()
{
	//int outputWidth = 200;
	//int outputHeight = 150;
	int outputWidth = 800;
	int outputHeight = 600;
	float outputAspectRatio = outputWidth / (float)outputHeight;

	ISubActor* cameraSubActor = sceneManager->GetMainCameraSubActor();

	Mat4 viewTransform, projTransform, viewProjTransform, invViewProjTransform;

	Mat4::Invert(&viewTransform, cameraSubActor->GetWorldTransform());
	Mat4::CreatePerspective(&projTransform, cameraSubActor->GetCameraDetails()->fov, outputAspectRatio, 0.1f, 1000.0f);
	Mat4::Multiply(&viewProjTransform, &projTransform, &viewTransform);
	Mat4::Invert(&invViewProjTransform, &viewProjTransform);

	Vec3 cameraPosition = *cameraSubActor->GetWorldPosition();
	
	Vec3 cameraDirection;
	Vec3::Set(&cameraDirection, 0.0f, 0.0f, 1.0f);
	Vec3::TransformMat3(&cameraDirection, &cameraDirection, cameraSubActor->GetWorldTransform());

	Vec3 rayDirectionOrigin;
	Vec3::ScaleAndAdd(&rayDirectionOrigin, &cameraPosition, &cameraDirection, -0.5f);

	Vec3 framePoints[4];
	Vec3::Set(&framePoints[0], -1.0f, 1.0f, -1.0f);
	Vec3::Set(&framePoints[1], 1.0f, 1.0f, -1.0f);
	Vec3::Set(&framePoints[2], -1.0f, -1.0f, -1.0f);
	Vec3::Set(&framePoints[3], 1.0f, -1.0f, -1.0f);

	for (int i = 0; i < 4; i++)
	{
		framePoints[i].x *= 8.0f;
		framePoints[i].y *= 8.0f;
		Vec3::TransformMat4(&framePoints[i], &framePoints[i], &invViewProjTransform);
	}

	Vec3 frameSteps[2];
	Vec3::Sub(&frameSteps[0], &framePoints[1], &framePoints[0]);
	Vec3::Scale(&frameSteps[0], &frameSteps[0], 1.0f / (float)outputWidth);
	Vec3::Sub(&frameSteps[1], &framePoints[2], &framePoints[0]);
	Vec3::Scale(&frameSteps[1], &frameSteps[1], 1.0f / (float)outputHeight);

	RgbFloat* inputPixels = new RgbFloat[outputWidth * outputHeight];
	memset(inputPixels, 0, sizeof(RgbFloat) * outputWidth * outputHeight);

	IMapAsset* mapAsset = assetManager->GetMapAsset(engine->GetMapAssetRef()->index);
	IWorldMeshAsset* worldMeshAsset = assetManager->GetWorldMeshAsset(mapAsset->GetWorldMeshAssetRef()->index);
	ICollisionMesh* collisionMesh = worldMeshAsset->GetCollisionMesh();

	T2_Execute(outputHeight, outputWidth, framePoints, frameSteps, rayDirectionOrigin, collisionMesh, inputPixels);

	BgrUByte* outputPixels = new BgrUByte[outputWidth * outputHeight];
	BuildOutputPixels(inputPixels, outputPixels, outputWidth, outputHeight);

	WriteOutputBmp(outputWidth, outputHeight, outputPixels);

	delete[] inputPixels;
	delete[] outputPixels;

	this->logger->Write("Render saved.");
}

#endif