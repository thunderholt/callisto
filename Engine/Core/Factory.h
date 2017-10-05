#pragma once

#include "Include/Callisto.h"

class Factory : public IFactory
{
public:
	virtual IUnitTester* MakeUnitTester();
	virtual ITicker* MakeTicker(float ticksPerSecond);
	virtual IFileLoader* MakeFileLoader();
	virtual IFile* MakeFile();
	virtual ITokenFileParser* MakeTokenFileParser(Buffer* fileData);
	virtual ISemaphore* MakeSemaphore();
	virtual IThreadManager* MakeThreadManager();
	virtual IMutex* MakeMutex();
	virtual IWorker* MakeWorker(IRunnable* runnable);
	virtual ITimestampProvider* MakeTimestampProvider();
	virtual IRasterizer* MakeRasterizer();
	virtual IRasterJobManager* MakeRasterJobManager();
	virtual ILogger* MakeLogger();
	virtual IJsonValue* MakeJsonValue();
	virtual IJsonProperty* MakeJsonProperty();
	virtual IJsonParser* MakeJsonParser();
	virtual IAssetManager* MakeAssetManager();
	virtual IActorAsset* MakeActorAsset(const char* filePath, Buffer* fileData, bool isStayResident);
	virtual IMapAsset* MakeMapAsset(const char* filePath, Buffer* fileData, bool isStayResident);
	virtual ISpriteSheetAsset* MakeSpriteSheetAsset(const char* filePath, Buffer* fileData, bool isStayResident);
	virtual ITextAsset* MakeTextAsset(const char* filePath, Buffer* fileData, bool isStayResident);
	virtual ITextureAsset* MakeTextureAsset(const char* filePath, Buffer* fileData, bool isStayResident);
	virtual IWorldMeshAsset* MakeWorldMeshAsset(const char* filePath, Buffer* fileData, bool isStayResident);
	virtual IStaticMeshAsset* MakeStaticMeshAsset(const char* filePath, Buffer* fileData, bool isStayResident);
	virtual IMaterialAsset* MakeMaterialAsset(const char* filePath, Buffer* fileData, bool isStayResident);
	virtual IPVSAsset* MakePVSAsset(const char* filePath, Buffer* fileData, bool isStayResident);
	virtual ISceneManager* MakeSceneManager();
	virtual IActorControllerManager* MakeActorControllerManager();
	virtual IFrameTimer* MakeFrameTimer();
	virtual IEngineBehaviour* MakeEngineBehaviour();
	virtual IRenderer* MakeRenderer();
	virtual IDebugVisualisationRenderer* MakeDebugVisualisationRenderer();
	virtual IKeyboard* MakeKeyboard();
	virtual IMouse* MakeMouse();
	virtual ITouchScreen* MakeTouchScreen();
	virtual IKeyboardMouseGamePad* MakeKeyboardMouseGamePad();
	virtual IPhysicalGamePad* MakePhysicalGamePad();
	virtual ITouchScreenGamePad* MakeTouchScreenGamePad();
	virtual IAggregatedGamePad* MakeAggregatedGamePad();
	virtual IActorInstance* MakeActorInstance();
	virtual IActor* MakeActor(int actorIndex);
	virtual ISubActor* MakeSubActor(int ownerActorIndex, int subActorIndex, SubActorType type, int parentSubActorIndex, DebugIcon debugIcon);
	virtual ICollisionMesh* MakeCollisionMesh();
	virtual IVolumeManager* MakeVolumeManager();
	virtual IVolume* MakeVolume();
};