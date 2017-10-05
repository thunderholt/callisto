#include "Core/SectorVisibilityLookUp.h"

SectorVisibilityLookup::SectorVisibilityLookup()
{
	this->numberOfSectors = 0;
	this->data = null;
}

SectorVisibilityLookup::~SectorVisibilityLookup()
{
	SafeDeleteArrayAndNull(this->data);
}

void SectorVisibilityLookup::Allocate(int numberOfSectors)
{
	this->numberOfSectors = numberOfSectors;

	int dataSize = numberOfSectors * numberOfSectors;
	this->data = new char[dataSize];
	memset((void*)this->data, SectorVisibilityStateUnknown, dataSize);
}

SectorVisibilityState SectorVisibilityLookup::GetSectorVisibilityState(int sectorIndexA, int sectorIndexB)
{
	int index = this->GetDataIndex(sectorIndexA, sectorIndexB);

	return (SectorVisibilityState)this->data[index];
}

void SectorVisibilityLookup::SetSectorVisibilityState(int sectorIndexA, int sectorIndexB, SectorVisibilityState sectorVisibilityState)
{
	int index = this->GetDataIndex(sectorIndexA, sectorIndexB);

	this->data[index] = sectorVisibilityState;
}

int SectorVisibilityLookup::GetDataIndex(int sectorIndexA, int sectorIndexB)
{
	int lowSectorIndex = -1;
	int highSectorIndex = -1;

	if (sectorIndexA < sectorIndexB)
	{
		lowSectorIndex = sectorIndexA;
		highSectorIndex = sectorIndexB;
	}
	else
	{
		lowSectorIndex = sectorIndexB;
		highSectorIndex = sectorIndexA;
	}

	int index = lowSectorIndex * this->numberOfSectors + highSectorIndex;

	return index;
}