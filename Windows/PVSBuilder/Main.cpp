#define WIN32_LEAN_AND_MEAN  
#include <windows.h> 
#include <crtdbg.h> 
#include "Include/PVSBuilder.h"

int main(int argc, char *argv[])
{
	CreateFactory();
	CreateEngine();

	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	logger->Write("****** Welcome to Callisto PVS Generator ******");

	if (argc != 4)
	{
		logger->Write("Incorrect number of arguments.");
	}
	else
	{
		char* worldMeshAssetFilePath = argv[1];
		char* assetsFolderPath = argv[2];
		char* outputFilePath = argv[3];

		engine->BuildPVS(worldMeshAssetFilePath, assetsFolderPath, outputFilePath);
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

