#include "Core/BruteForceSectorCruncher.h"

BruteForceSectorCruncher::BruteForceSectorCruncher()
{
	//this->hasFinished = false;
}

BruteForceSectorCruncher::~BruteForceSectorCruncher()
{

}

void BruteForceSectorCruncher::Run(int startSectorIndex, int numberOfSectorsToCrunch)
{
	IEngine* engine = GetEngine();
	ISectorVisibilityLookup* sectorVisibilityLookup = engine->GetSectorVisibilityLookup();
	IWorldMeshAsset* worldMeshAsset = engine->GetWorldMeshAsset();
	ICollisionMesh* collisionMesh = worldMeshAsset->GetCollisionMesh();
	SectorMetrics* sectorMetrics = engine->GetSectorMetrics();
	Sector* sectors = engine->GetSectors();

	for (int sectorAIndex = startSectorIndex;
		sectorAIndex < sectorMetrics->numberOfSectors &&
		sectorAIndex < startSectorIndex + numberOfSectorsToCrunch;
		sectorAIndex++)
	{
		Sector* sectorA = &sectors[sectorAIndex];

		for (int sectorBIndex = 0; sectorBIndex < sectorMetrics->numberOfSectors; sectorBIndex++)
		{
			if (sectorAIndex == sectorBIndex)
			{
				sectorVisibilityLookup->SetSectorVisibilityState(sectorAIndex, sectorBIndex, SectorVisibilityStateVisible);
			}
			else
			{
				Sector* sectorB = &sectors[sectorBIndex];

				SectorVisibilityState currentVisibilityState = sectorVisibilityLookup->GetSectorVisibilityState(sectorAIndex, sectorBIndex);
				if (currentVisibilityState == SectorVisibilityStateUnknown)
				{
					bool isVisible = false;

					for (int i = 0; i < 100; i++)
					{
						CollisionLine line;
						line.from = sectorA->insidePoints[Math::GenerateRandomInt(0, sectorA->numberOfInsidePoints - 1)];
						line.to = sectorB->insidePoints[Math::GenerateRandomInt(0, sectorB->numberOfInsidePoints - 1)];
						CollisionLine::FromOwnFromAndToPoints(&line);

						if (!collisionMesh->DetermineIfLineIntersectsMesh(&line))
						{
							isVisible = true;
							break;
						}
					}

					sectorVisibilityLookup->SetSectorVisibilityState(sectorAIndex, sectorBIndex, isVisible ? SectorVisibilityStateVisible : SectorVisibilityStateNotVisible);
				}
			}
		}
	}

	//this->hasFinished = true;
}

/*bool BruteForceSectorCruncher::GetHasFinished()
{
	return this->hasFinished;
}*/