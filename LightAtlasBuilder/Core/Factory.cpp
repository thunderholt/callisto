#include "Core/Factory.h"

#ifdef WIN32
#include "FileIO/WindowsFile.h"
#include "FileIO/TokenFileParser.h" 
#include "Logging/WindowsLogger.h" 
#include "Core/WorldMeshAsset.h" 
#include "Core/MaterialAsset.h" 
#include "Core/CollisionMesh.h" 
#include "Core/Worker.h"
#include "Core/LightAtlas.h"
#include "Core/RayTracer.h"
#include "Threading/WindowsThreadManager.h"
#include "Time/WindowsTimestampProvider.h"
#include "Json/JsonValue.h" 
#include "Json/JsonProperty.h" 
#include "Json/JsonParser.h" 
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

Factory::~Factory()
{

}

IFile* Factory::MakeFile()
{
	#if defined WIN32
	return new WindowsFile();
	#endif
}

ITokenFileParser* Factory::MakeTokenFileParser(Buffer* fileData)
{
	return new TokenFileParser(fileData);
}

ILogger* Factory::MakeLogger()
{
	#if defined WIN32
	return new WindowsLogger();
	#endif
}

IWorldMeshAsset* Factory::MakeWorldMeshAsset()
{
	return new WorldMeshAsset();
}

IMaterialAsset* Factory::MakeMaterialAsset()
{
	return new MaterialAsset();
}

ICollisionMesh* Factory::MakeCollisionMesh()
{
	return new CollisionMesh();
}

IWorker* Factory::MakeWorker()
{
	return new Worker();
}

ILightAtlas* Factory::MakeLightAtlas()
{
	return new LightAtlas();
}

IRayTracer* Factory::MakeRayTracer()
{
	return new RayTracer();
}

IThreadManager* Factory::MakeThreadManager()
{
	return new WindowsThreadManager();
}

ITimestampProvider* Factory::MakeTimestampProvider()
{
	return new WindowsTimestampProvider();
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