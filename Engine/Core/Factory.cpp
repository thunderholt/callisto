#include "Core/Factory.h"

#ifdef WIN32
#include "AI/Ticker.h" 
#include "FileIO/BackgroundThreadFileLoader.h"
#include "FileIO/WindowsFile.h"
#include "FileIO/TokenFileParser.h" 
#include "Threading/WindowsSemaphore.h" 
#include "Threading/WindowsThreadManager.h" 
#include "Threading/WindowsMutex.h" 
#include "Threading/Worker.h" 
#include "Time/WindowsTimestampProvider.h" 
#include "Logging/WindowsLogger.h" 
#include "Json/JsonValue.h" 
#include "Json/JsonProperty.h" 
#include "Json/JsonParser.h" 
#include "Rasterization/OpenGL/OpenGLRasterizer.h" 
#include "Rasterization/RasterJobManager.h" 
#include "Core/UnitTester.h" 
#include "Core/ActorInstance.h" 
#include "Core/Actor.h" 
#include "Core/SubActor.h" 
#include "Core/AssetManager.h" 
#include "Core/SceneManager.h"
#include "Core/ActorControllerManager.h"
#include "Core/FrameTimer.h"
#include "Core/EngineBehaviour.h"
#include "Core/Renderer.h"
#include "Core/DebugVisualisationRenderer.h"
#include "Core/CollisionMesh.h" 
#include "Core/VolumeManager.h" 
#include "Core/Volume.h" 
#include "Assets/ActorAsset.h" 
#include "Assets/MapAsset.h" 
#include "Assets/SpriteSheetAsset.h" 
#include "Assets/TextAsset.h" 
#include "Assets/TextureAsset.h" 
#include "Assets/WorldMeshAsset.h" 
#include "Assets/StaticMeshAsset.h" 
#include "Assets/MaterialAsset.h" 
#include "Assets/PVSAsset.h" 
#include "Input/WindowsKeyboard.h" 
#include "Input/WindowsMouse.h" 
#include "Input/NullTouchScreen.h"
#include "Input/KeyboardMouseGamePad.h" 
#include "Input/NullPhysicalGamePad.h"
#include "Input/TouchScreenGamePad.h"
#include "Input/AggregatedGamePad.h" 
#endif

#ifdef ANDROID
#include "AI/Ticker.h" 
#include "FileIO/BackgroundThreadFileLoader.h"
#include "FileIO/AndroidFile.h"
#include "FileIO/TokenFileParser.h" 
#include "Threading/PosixSemaphore.h" 
#include "Threading/PosixThreadManager.h" 
#include "Threading/PosixMutex.h" 
#include "Threading/Worker.h" 
#include "Time/PosixTimestampProvider.h" 
#include "Logging/AndroidLogger.h" 
#include "Json/JsonValue.h" 
#include "Json/JsonProperty.h" 
#include "Json/JsonParser.h" 
#include "Rasterization/OpenGL/OpenGLRasterizer.h" 
#include "Rasterization/RasterJobManager.h" 
#include "Core/UnitTester.h"
#include "Core/ActorInstance.h" 
#include "Core/Actor.h" 
#include "Core/SubActor.h" 
#include "Core/AssetManager.h" 
#include "Core/SceneManager.h"
#include "Core/ActorControllerManager.h"
#include "Core/FrameTimer.h"
#include "Core/EngineBehaviour.h"
#include "Core/Renderer.h"
#include "Core/DebugVisualisationRenderer.h"
#include "Core/CollisionMesh.h" 
#include "Core/VolumeManager.h" 
#include "Core/Volume.h" 
#include "Assets/ActorAsset.h" 
#include "Assets/MapAsset.h" 
#include "Assets/SpriteSheetAsset.h" 
#include "Assets/TextAsset.h" 
#include "Assets/TextureAsset.h" 
#include "Assets/WorldMeshAsset.h" 
#include "Assets/StaticMeshAsset.h" 
#include "Assets/MaterialAsset.h" 
#include "Assets/PVSAsset.h"
#include "Input/NullKeyboard.h" 
#include "Input/NullMouse.h" 
#include "Input/AndroidTouchScreen.h"
#include "Input/KeyboardMouseGamePad.h" 
#include "Input/NullPhysicalGamePad.h"
#include "Input/TouchScreenGamePad.h"
#include "Input/AggregatedGamePad.h" 
#endif

#ifdef EMSCRIPTEN
#include "AI/Ticker.h" 
#include "FileIO/WebBrowserFileLoader.h"
#include "FileIO/TokenFileParser.h" 
#include "Threading/WebBrowserThreadManager.h"
#include "Threading/Worker.h" 
#include "Time/WebBrowserTimestampProvider.h"
#include "Logging/WebBrowserLogger.h" 
#include "Json/JsonValue.h" 
#include "Json/JsonProperty.h" 
#include "Json/JsonParser.h" 
#include "Rasterization/OpenGL/OpenGLRasterizer.h" 
#include "Rasterization/RasterJobManager.h" 
#include "Core/UnitTester.h" 
#include "Core/ActorInstance.h" 
#include "Core/Actor.h" 
#include "Core/SubActor.h" 
#include "Core/AssetManager.h" 
#include "Core/SceneManager.h"
#include "Core/ActorControllerManager.h"
#include "Core/FrameTimer.h"
#include "Core/EngineBehaviour.h"
#include "Core/Renderer.h"
#include "Core/DebugVisualisationRenderer.h"
#include "Core/CollisionMesh.h" 
#include "Core/VolumeManager.h" 
#include "Core/Volume.h" 
#include "Assets/ActorAsset.h" 
#include "Assets/MapAsset.h" 
#include "Assets/SpriteSheetAsset.h" 
#include "Assets/TextAsset.h" 
#include "Assets/TextureAsset.h" 
#include "Assets/WorldMeshAsset.h" 
#include "Assets/StaticMeshAsset.h" 
#include "Assets/MaterialAsset.h" 
#include "Assets/PVSAsset.h" 
#include "Input/WebBrowserKeyboard.h" 
#include "Input/WebBrowserMouse.h" 
#include "Input/WebBrowserTouchScreen.h" 
#include "Input/KeyboardMouseGamePad.h" 
#include "Input/NullPhysicalGamePad.h"
#include "Input/TouchScreenGamePad.h"
#include "Input/WebBrowserPhysicalGamePad.h"
#include "Input/AggregatedGamePad.h" 
#endif

Factory* factory = null;

void CreateFactory()
{
	factory = new Factory();
}

void DestroyFactory()
{
	delete factory;
}

IFactory* GetFactory()
{
	return factory;
}

IUnitTester* Factory::MakeUnitTester()
{
	return new UnitTester(); 
}

ITicker* Factory::MakeTicker(float ticksPerSecond)
{
	return new Ticker(ticksPerSecond);
}

IFileLoader* Factory::MakeFileLoader()
{
	#if defined WIN32 || defined ANDROID
	return new BackgroundThreadFileLoader();
	#elif defined EMSCRIPTEN
	return new WebBrowserFileLoader();
	#endif
}

IFile* Factory::MakeFile()
{
	#if defined WIN32
	return new WindowsFile();
	#elif defined ANDROID
	return new AndroidFile();
	#elif defined EMSCRIPTEN
	return null;
	#endif
}

ITokenFileParser* Factory::MakeTokenFileParser(Buffer* fileData)
{
	return new TokenFileParser(fileData);
}

ISemaphore* Factory::MakeSemaphore()
{
	#if defined WIN32
	return new WindowsSemaphore();
	#elif defined ANDROID
	return new PosixSemaphore();
	#elif defined EMSCRIPTEN
	return null;
	#endif
}

IThreadManager* Factory::MakeThreadManager()
{
	#if defined WIN32
	return new WindowsThreadManager();
	#elif defined ANDROID
	return new PosixThreadManager();
	#elif defined EMSCRIPTEN
	return new WebBrowserThreadManager;
	#endif
}

IMutex* Factory::MakeMutex()
{
	#if defined WIN32
	return new WindowsMutex();
	#elif defined ANDROID
	return new PosixMutex();
	#elif defined EMSCRIPTEN
	return null;
	#endif
}

IWorker* Factory::MakeWorker(IRunnable* runnable)
{
	return new Worker(runnable);
}

ITimestampProvider* Factory::MakeTimestampProvider()
{
	#if defined WIN32
	return new WindowsTimestampProvider();
	#elif defined ANDROID
	return new PosixTimestampProvider();
	#elif defined EMSCRIPTEN
	return new WebBrowserTimestampProvider;
	#endif
}

IRasterizer* Factory::MakeRasterizer()
{
	return new OpenGLRasterizer();
}

IRasterJobManager* Factory::MakeRasterJobManager()
{
	return new RasterJobManager();
}

ILogger* Factory::MakeLogger()
{
	#if defined WIN32
	return new WindowsLogger();
	#elif defined ANDROID
	return new AndroidLogger();
	#elif defined EMSCRIPTEN
	return new WebBrowserLogger();
	#endif
}

IJsonValue* Factory::MakeJsonValue()
{
	return new JsonValue();
}

IJsonProperty* Factory::MakeJsonProperty()
{
	return new JsonProperty();
}

IJsonParser* Factory::MakeJsonParser()
{
	return new JsonParser();
}

IAssetManager* Factory::MakeAssetManager()
{
	return new AssetManager();
}

IActorAsset* Factory::MakeActorAsset(const char* filePath, Buffer* fileData, bool isStayResident)
{
	return new ActorAsset(filePath, fileData, isStayResident);
}

IMapAsset* Factory::MakeMapAsset(const char* filePath, Buffer* fileData, bool isStayResident)
{
	return new MapAsset(filePath, fileData, isStayResident);
}

ISpriteSheetAsset* Factory::MakeSpriteSheetAsset(const char* filePath, Buffer* fileData, bool isStayResident)
{
	return new SpriteSheetAsset(filePath, fileData, isStayResident);
}

ITextAsset* Factory::MakeTextAsset(const char* filePath, Buffer* fileData, bool isStayResident)
{
	return new TextAsset(filePath, fileData, isStayResident);
}

ITextureAsset* Factory::MakeTextureAsset(const char* filePath, Buffer* fileData, bool isStayResident)
{
	return new TextureAsset(filePath, fileData, isStayResident);
}

IWorldMeshAsset* Factory::MakeWorldMeshAsset(const char* filePath, Buffer* fileData, bool isStayResident)
{
	return new WorldMeshAsset(filePath, fileData, isStayResident);
}

IStaticMeshAsset* Factory::MakeStaticMeshAsset(const char* filePath, Buffer* fileData, bool isStayResident)
{
	return new StaticMeshAsset(filePath, fileData, isStayResident);
}

IMaterialAsset* Factory::MakeMaterialAsset(const char* filePath, Buffer* fileData, bool isStayResident)
{
	return new MaterialAsset(filePath, fileData, isStayResident);
}

IPVSAsset* Factory::MakePVSAsset(const char* filePath, Buffer* fileData, bool isStayResident)
{
	return new PVSAsset(filePath, fileData, isStayResident);
}

ISceneManager* Factory::MakeSceneManager()
{
	return new SceneManager();
}

IActorControllerManager* Factory::MakeActorControllerManager()
{
	return new ActorControllerManager();
}

IFrameTimer* Factory::MakeFrameTimer()
{
	return new FrameTimer();
}

IEngineBehaviour* Factory::MakeEngineBehaviour()
{
	return new EngineBehaviour();
}

IRenderer* Factory::MakeRenderer()
{
	return new Renderer();
}

IDebugVisualisationRenderer* Factory::MakeDebugVisualisationRenderer()
{
	return new DebugVisualisationRenderer();
}

IKeyboard* Factory::MakeKeyboard()
{
	#if defined WIN32
	return new WindowsKeyboard();
	#elif defined ANDROID
	return new NullKeyboard();
	#elif defined EMSCRIPTEN
	return new WebBrowserKeyboard();
	#endif
}

IMouse* Factory::MakeMouse()
{
	#if defined WIN32
	return new WindowsMouse();
	#elif defined ANDROID
	return new NullMouse();
	#elif defined EMSCRIPTEN
	return new WebBrowserMouse();
	#endif
}

ITouchScreen* Factory::MakeTouchScreen()
{
	#if defined WIN32
	return new NullTouchScreen();
	#elif defined ANDROID
	return new AndroidTouchScreen();
	#elif defined EMSCRIPTEN
	return new WebBrowserTouchScreen();
	#endif
}

IKeyboardMouseGamePad* Factory::MakeKeyboardMouseGamePad()
{
	return new KeyboardMouseGamePad();
}

ITouchScreenGamePad* Factory::MakeTouchScreenGamePad()
{
	return new TouchScreenGamePad();
}

IPhysicalGamePad* Factory::MakePhysicalGamePad()
{
	#if defined WIN32
	return new NullPhysicalGamePad();
	#elif defined ANDROID
	return new NullPhysicalGamePad();
	#elif defined EMSCRIPTEN
	return new WebBrowserPhysicalGamePad();
	#endif
}

IAggregatedGamePad* Factory::MakeAggregatedGamePad()
{
	return new AggregatedGamePad();
}

IActorInstance* Factory::MakeActorInstance()
{
	return new ActorInstance();
}

IActor* Factory::MakeActor(int actorIndex)
{
	return new Actor(actorIndex);
}

ISubActor* Factory::MakeSubActor(int ownerActorIndex, int subActorIndex, SubActorType type, int parentSubActorIndex, DebugIcon debugIcon)
{
	return new SubActor(ownerActorIndex, subActorIndex, type, parentSubActorIndex, debugIcon);
}

ICollisionMesh* Factory::MakeCollisionMesh()
{
	return new CollisionMesh();
}

IVolumeManager* Factory::MakeVolumeManager()
{
	return new VolumeManager();
}

IVolume* Factory::MakeVolume()
{
	return new Volume();
}