#pragma once

#include "Include/Callisto.h"

class PVSAsset : public IPVSAsset
{
public:
	PVSAsset(const char* filePath, Buffer* fileData, bool isStayResident);
	virtual ~PVSAsset();

	virtual PVSSectorMetrics* GetSectorMetrics();
	virtual PVSSector* GetSector(int sectorIndex);
	virtual int* GetVisibleSectorIndexes();

	virtual bool ResolveReferencedAssets();

	virtual char* GetFilePath();
	virtual bool GetIsStayResident();
	virtual bool GetLoadedSuccessfully();
	virtual bool GetIsEvictable();
	virtual void SetIsEvictable(bool isEvictable);

private:
	char filePath[AssetMaxFilePathLength];
	bool isStayResident;
	bool loadedSuccessfully;
	bool isEvictable;

	PVSSectorMetrics sectorMetrics;
	PVSSector* sectors;
	int* visibleSectorIndexes;
};