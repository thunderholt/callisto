#include "Core/BruteForceSectorCruncher.h"

BruteForceSectorCruncher::BruteForceSectorCruncher()
{

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
	ILogger* logger = engine->GetLogger();
	//SectorMetrics* sectorMetrics = engine->GetSectorMetrics();
	//Sector* sectors = engine->GetSectors();
	int numberOfSectors = engine->GetNumberOfSectors();
	//CollisionMeshLineIntersectionDeterminationWorkingData collisionMeshLineIntersectionDeterminationWorkingData;

	this->numberOfSectorVisibilityChecksMade = 0;

	for (int sectorAIndex = startSectorIndex;
		sectorAIndex < numberOfSectors &&
		sectorAIndex < startSectorIndex + numberOfSectorsToCrunch;
		sectorAIndex++)
	{
		Sector* sectorA = engine->GetSector(sectorAIndex);

		for (int sectorBIndex = 0; sectorBIndex < numberOfSectors; sectorBIndex++)
		{
			if (sectorAIndex == sectorBIndex)
			{
				sectorVisibilityLookup->SetSectorVisibilityState(sectorAIndex, sectorBIndex, SectorVisibilityStateVisible);
			}
			else
			{
				Sector* sectorB = engine->GetSector(sectorBIndex);

				SectorVisibilityState currentVisibilityState = sectorVisibilityLookup->GetSectorVisibilityState(sectorAIndex, sectorBIndex);
				if (currentVisibilityState == SectorVisibilityStateUnknown)
				{
					bool isVisible = false;

					if (sectorA->insidePoints.GetLength() > 0 && sectorB->insidePoints.GetLength() > 0)
					{
						for (int i = 0; i < 600; i++)
						{
							CollisionLine line;
							line.from = sectorA->insidePoints[Math::GenerateRandomInt(0, sectorA->insidePoints.GetLength() - 1)];
							line.to = sectorB->insidePoints[Math::GenerateRandomInt(0, sectorB->insidePoints.GetLength() - 1)];
							CollisionLine::FromOwnFromAndToPoints(&line);

							if (!collisionMesh->DetermineIfLineIntersectsMesh(&line, true))
							{
								isVisible = true;
								break;
							}
						}
					}

					sectorVisibilityLookup->SetSectorVisibilityState(sectorAIndex, sectorBIndex, isVisible ? SectorVisibilityStateVisible : SectorVisibilityStateNotVisible);
					this->numberOfSectorVisibilityChecksMade++;
				}
			}
		}
	}

	//logger->Write("Performed %d sector visibility checks.", this->numberOfSectorVisibilityChecksMade);
}