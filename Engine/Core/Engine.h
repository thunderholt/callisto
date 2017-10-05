#pragma once

#include "Include/Callisto.h"

class Engine : public IEngine
{
public:
	Engine();
	virtual ~Engine();

	virtual void RegisterGameController(IGameController* gameController);
	virtual void Init();
	//virtual void CleanUp();
	virtual void BeginLoadingMap(const char* filePath);
	virtual void Heartbeat();
	virtual IGameController* GetGameController();
	virtual IRasterizer* GetRasterizer();
	virtual IRasterJobManager* GetRasterJobManager();
	virtual IAssetManager* GetAssetManager();
	virtual IEngineBehaviour* GetBehaviour();
	virtual IRenderer* GetRenderer();
	virtual IDebugVisualisationRenderer* GetDebugVisualisationRenderer();
	virtual IFrameTimer* GetFrameTimer();
	virtual ISceneManager* GetSceneManager();
	virtual IActorControllerManager* GetActorControllerManager();
	virtual IVolumeManager* GetVolumeManager();
	virtual IFileLoader* GetFileLoader();
	virtual IThreadManager* GetThreadManager();
	virtual ITimestampProvider* GetTimestampProvider();
	virtual IKeyboard* GetKeyboard();
	virtual IMouse* GetMouse();
	virtual ITouchScreen* GetTouchScreen();
	virtual IKeyboardMouseGamePad* GetKeyboardMouseGamePad();
	virtual IPhysicalGamePad* GetPhysicalGamePad();
	virtual ITouchScreenGamePad* GetTouchScreenGamePad();
	virtual IAggregatedGamePad* GetAggregatedGamePad();
	virtual ILogger* GetLogger();
	virtual AssetRef* GetMapAssetRef();
	virtual bool GetMustTerminate();
	//virtual bool GetMapIsAssigned();
	virtual EngineMapState GetMapState();
	virtual int GetFrameId();

private:
	bool EnsurePostCoreAssetLoadInitHasRun();
	void BeginLoadingMapInternal();

	/////////////// test code //////////////
	void GenerateTestPathTraceRender();
	////////////////////////////////////////

	IUnitTester* unitTester;
	IGameController* gameController;
	IRasterizer* rasterizer;
	IRasterJobManager* rasterJobManager;
	IAssetManager* assetManager;
	IEngineBehaviour* behaviour;
	IRenderer* renderer;
	IDebugVisualisationRenderer* debugVisualisationRenderer;
	IFrameTimer* frameTimer;
	ISceneManager* sceneManager;
	IActorControllerManager* actorControllerManager;
	IVolumeManager* volumeManager;
	IFileLoader* fileLoader;
	IThreadManager* threadManager;
	ITimestampProvider* timestampProvider;
	IKeyboard* keyboard;
	IMouse* mouse;
	ITouchScreen* touchScreen;
	IKeyboardMouseGamePad* keyboardMouseGamePad;
	IPhysicalGamePad* physicalGamePad;
	ITouchScreenGamePad* touchScreenGamePad;
	IAggregatedGamePad* aggregatedGamePad;
	ILogger* logger;
	AssetRef mapAssetRef;
	char mapToLoadPath[AssetMaxFilePathLength];
	bool hasMapToLoad;
	bool mustTerminate;
	//bool mapIsAssigned;
	EngineMapState mapState;
	bool postCoreAssetLoadInitialisationHasRun;
	int frameId;
};