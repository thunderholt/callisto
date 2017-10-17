#pragma once

#include "Include/PVSBuilder.h"

struct OcclusionSectorCruncherPass
{
	Vec3i direction;
	Vec3i extrpolateSubPass1Directions[2];
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
	bool FindFirstNonVisibleSectorAlongDirection(Vec3i* out, int sectorAIndex, Vec3i* startSectorBIndexParts, bool skipFirstSector, Vec3i* direction, ISectorVisibilityLookup* sectorVisibilityLookup, Sector* sectors, SectorMetrics* sectorMetrics, ICollisionMesh* collisionMesh);
	void MarkSectorsAlongDirectionAsNotVisible(int sectorAIndex, Vec3i* startSectorBIndexParts, Vec3i* direction, ISectorVisibilityLookup* sectorVisibilityLookup, SectorMetrics* sectorMetrics);
};
