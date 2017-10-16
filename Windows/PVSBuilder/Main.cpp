#define WIN32_LEAN_AND_MEAN  
#include <windows.h> 
#include <crtdbg.h> 
#include "Include/PVSBuilder.h"

int main()
{
	CreateFactory();
	CreateEngine();

	IEngine* engine = GetEngine();
	SectorMetrics sectorMetrics;
	Vec3::Set(&sectorMetrics.originOffset, -5.0f, 0.0f, 0.0f);
	sectorMetrics.sectorCounts[0] = 10;
	sectorMetrics.sectorCounts[1] = 2;
	sectorMetrics.sectorCounts[2] = 10;
	
	/*sectorMetrics.sectorCounts[0] = 2;
	sectorMetrics.sectorCounts[1] = 2;
	sectorMetrics.sectorCounts[2] = 2;*/
	
	sectorMetrics.numberOfSectors = sectorMetrics.sectorCounts[0] * sectorMetrics.sectorCounts[1] * sectorMetrics.sectorCounts[2];
	sectorMetrics.sectorSize = 1.0f;
	engine->BuildPVS("World-Meshes/demo-1.wmesh", sectorMetrics);

	DestroyEngine();
	DestroyFactory();

	OutputDebugStringA("Dumping memory leaks...");
	_CrtDumpMemoryLeaks();
	OutputDebugStringA("... done.");

    return 0;
}

