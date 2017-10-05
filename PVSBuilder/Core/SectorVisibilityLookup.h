#pragma once

#include "Include/PVSBuilder.h"

class SectorVisibilityLookup : public ISectorVisibilityLookup
{
public:
	SectorVisibilityLookup();
	virtual ~SectorVisibilityLookup();
	virtual void Allocate(int numberOfSectors);
	virtual SectorVisibilityState GetSectorVisibilityState(int sectorIndexA, int sectorIndexB);
	virtual void SetSectorVisibilityState(int sectorIndexA, int sectorIndexB, SectorVisibilityState sectorVisibilityState);

private:
	int GetDataIndex(int sectorIndexA, int sectorIndexB);

	int numberOfSectors;
	volatile char* data;
};
