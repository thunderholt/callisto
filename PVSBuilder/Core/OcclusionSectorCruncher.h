#pragma once

#include "Include/PVSBuilder.h"

struct OcclusionSectorCruncherPass
{
	Vec3i direction;
};

class OcclusionSectorCruncher : public ISectorCruncher
{
public:
	OcclusionSectorCruncher();
	virtual ~OcclusionSectorCruncher();
	virtual void Run(int startSectorIndex, int numberOfSectorsToCrunch);

private:
	void RunPassOnSector(OcclusionSectorCruncherPass* pass, Vec3i* sectorIndexParts, ISectorVisibilityLookup* sectorVisibilityLookup, Sector* sectors, SectorMetrics* sectorMetrics, ICollisionMesh* collisionMesh);
	int OcclusionSectorCruncher::CalculateSectorIndex(Vec3i* sectorIndexParts, SectorMetrics* sectorMetrics);
	bool OcclusionSectorCruncher::CheckSectorIndexIsOutsideOfBounds(Vec3i* sectorIndexParts, SectorMetrics* sectorMetrics);
	bool CheckSectorIsVisible(int sectorAIndex, int sectorBIndex, ISectorVisibilityLookup* sectorVisibilityLookup, Sector* sectors, SectorMetrics* sectorMetrics, ICollisionMesh* collisionMesh);
};
