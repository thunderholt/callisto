#include "Assets/PVSAsset.h"

PVSAsset::PVSAsset(const char* filePath, Buffer* fileData, bool isStayResident)
{
	IEngine* engine = GetEngine();
	IFactory* factory = GetFactory();
	ITokenFileParser* parser = factory->MakeTokenFileParser(fileData);

	strcpy(this->filePath, filePath);
	this->isStayResident = isStayResident;

	//memset(&this->sectorMetrics, 0, sizeof(PVSSectorMetrics));
	this->sectors = null;
	this->numberOfSectors = 0;
	this->visibleSectorIndexes = null;

	char token[256];
	int numberOfVisibleSectorIndexes = 0;
	int numberOfResidentWorldMeshChunkIndexes = 0;
	
	while (!parser->GetIsEOF())
	{
		parser->ReadString(token, 256);

		/*if (strcmp(token, "sector-size") == 0)
		{
			this->sectorMetrics.sectorSize = parser->ReadFloat();
		}
		else if (strcmp(token, "sector-counts") == 0)
		{
			this->sectorMetrics.sectorCounts[0] = parser->ReadInt();
			this->sectorMetrics.sectorCounts[1] = parser->ReadInt();
			this->sectorMetrics.sectorCounts[2] = parser->ReadInt();
		}
		else if (strcmp(token, "sector-origin-offset") == 0)
		{
			parser->ReadVec3(&this->sectorMetrics.sectorOriginOffset);
		}
		else */if (strcmp(token, "number-of-sectors") == 0)
		{
			this->numberOfSectors = parser->ReadInt();
			this->sectors = new PVSSector[this->numberOfSectors];
		}
		else if (strcmp(token, "number-of-visible-sector-indexes") == 0)
		{
			numberOfVisibleSectorIndexes = parser->ReadInt();
			this->visibleSectorIndexes = new int[numberOfVisibleSectorIndexes];
		}
		else if (strcmp(token, "number-of-resident-world-mesh-chunk-indexes") == 0)
		{
			numberOfResidentWorldMeshChunkIndexes = parser->ReadInt();
			this->residentWorldMeshChunkIndexes = new int[numberOfResidentWorldMeshChunkIndexes];
		}
		/*else if (strcmp(token, "sector-origins") == 0)
		{
			for (int i = 0; i < this->sectorMetrics.numberOfSectors; i++)
			{
				PVSSector* sector = &this->sectors[i];
				parser->ReadVec3(&sector->origin);
			}
		}*/
		else if (strcmp(token, "sector-aabbs") == 0)
		{
			for (int i = 0; i < this->numberOfSectors; i++)
			{
				PVSSector* sector = &this->sectors[i];
				parser->ReadAABB(&sector->aabb);
			}
		}
		else if (strcmp(token, "sector-visible-sector-indexes-offsets") == 0)
		{
			for (int i = 0; i < this->numberOfSectors; i++)
			{
				PVSSector* sector = &this->sectors[i];
				sector->visibleSectorIndexesOffset = parser->ReadInt();
			}
		}
		else if (strcmp(token, "sector-visible-sector-index-quantities") == 0)
		{
			for (int i = 0; i < this->numberOfSectors; i++)
			{
				PVSSector* sector = &this->sectors[i];
				sector->numberOfVisibleSectors = parser->ReadInt();
			}
		}
		else if (strcmp(token, "sector-resident-world-mesh-chunk-indexes-offsets") == 0)
		{
			for (int i = 0; i < this->numberOfSectors; i++)
			{
				PVSSector* sector = &this->sectors[i];
				sector->residentWorldMeshChunkIndexesOffset = parser->ReadInt();
			}
		}
		else if (strcmp(token, "sector-resident-world-mesh-chunk-index-quantities") == 0)
		{
			for (int i = 0; i < this->numberOfSectors; i++)
			{
				PVSSector* sector = &this->sectors[i];
				sector->numberOfResidentWorldMeshChunkIndexes = parser->ReadInt();
			}
		}
		else if (strcmp(token, "visible-sector-indexes") == 0)
		{
			for (int i = 0; i < numberOfVisibleSectorIndexes; i++)
			{
				this->visibleSectorIndexes[i] = parser->ReadInt();
			}
		}
		else if (strcmp(token, "resident-world-mesh-chunk-indexes") == 0)
		{
			for (int i = 0; i < numberOfResidentWorldMeshChunkIndexes; i++)
			{
				this->residentWorldMeshChunkIndexes[i] = parser->ReadInt();
			}
		}
	}

	SafeDeleteAndNull(parser);

	this->loadedSuccessfully = true;
}

PVSAsset::~PVSAsset()
{
	SafeDeleteArrayAndNull(this->sectors);
	SafeDeleteArrayAndNull(this->visibleSectorIndexes);
	SafeDeleteArrayAndNull(this->residentWorldMeshChunkIndexes);
}

bool PVSAsset::ResolveReferencedAssets()
{
	return true;
}

char* PVSAsset::GetFilePath()
{
	return this->filePath;
}

bool PVSAsset::GetIsStayResident()
{
	return this->isStayResident;
}

bool PVSAsset::GetLoadedSuccessfully()
{
	return this->loadedSuccessfully;
}

bool PVSAsset::GetIsEvictable()
{
	return this->isEvictable;
}

void PVSAsset::SetIsEvictable(bool isEvictable)
{
	this->isEvictable = isEvictable;
}

/*PVSSectorMetrics* PVSAsset::GetSectorMetrics()
{
	return &this->sectorMetrics;
}*/

int PVSAsset::GetNumberOfSectors()
{
	return this->numberOfSectors;
}

PVSSector* PVSAsset::GetSector(int sectorIndex)
{
	return &this->sectors[sectorIndex];
}

int* PVSAsset::GetVisibleSectorIndexes()
{
	return this->visibleSectorIndexes;
}

int* PVSAsset::GetResidentWorldMeshChunkIndexes()
{
	return this->residentWorldMeshChunkIndexes;
}