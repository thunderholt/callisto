#define WIN32_LEAN_AND_MEAN  
#include <windows.h> 
#include <crtdbg.h> 
#include "Include/LightAtlasBuilder.h"

int main(int argc, char *argv[])
{
	CreateFactory();
	CreateEngine();

	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	logger->Write("****** Welcome to Callisto Light Atlas Builder ******");

	if (argc != 3)
	{
		logger->Write("Incorrect number of arguments.");
	}
	else
	{
		char* worldMeshAssetFilePath = argv[1];
		char* assetsFolderPath = argv[2];

		engine->BuildLightAtlases(worldMeshAssetFilePath, assetsFolderPath);
	}

	logger->Write("Press enter to exit.");

	DestroyEngine();
	DestroyFactory();

	OutputDebugStringA("Dumping memory leaks...");
	_CrtDumpMemoryLeaks();
	OutputDebugStringA("... done.");

	getchar();

	return 0;
}