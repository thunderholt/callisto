#include "Core/Factory.h"

#ifdef WIN32
#include "FileIO/WindowsFile.h"
#include "FileIO/TokenFileParser.h" 
#include "Logging/WindowsLogger.h" 
#include "Core/WorldMeshAsset.h" 
#include "Core/CollisionMesh.h" 
#include "Core/SectorVisibilityLookup.h" 
#include "Core/BruteForceSectorCruncher.h" 
#include "Core/Worker.h"
#include "Threading/WindowsThreadManager.h"
#include "Time/WindowsTimestampProvider.h"
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

ICollisionMesh* Factory::MakeCollisionMesh()
{
	return new CollisionMesh();
}

ISectorVisibilityLookup* Factory::MakeSectorVisibilityLookup()
{
	return new SectorVisibilityLookup();
}

ISectorCruncher* Factory::MakeBruteForceSectorCruncher()
{
	return new BruteForceSectorCruncher();
}

IWorker* Factory::MakeWorker()
{
	return new Worker();
}

IThreadManager* Factory::MakeThreadManager()
{
	return new WindowsThreadManager();
}

ITimestampProvider* Factory::MakeTimestampProvider()
{
	return new WindowsTimestampProvider();
}