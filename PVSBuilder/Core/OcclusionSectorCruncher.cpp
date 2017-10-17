#include "Core/OcclusionSectorCruncher.h"

OcclusionSectorCruncher::OcclusionSectorCruncher()
{

}

OcclusionSectorCruncher::~OcclusionSectorCruncher()
{

}

void OcclusionSectorCruncher::Run(int startSectorIndex, int numberOfSectorsToCrunch)
{
	IEngine* engine = GetEngine();
	ISectorVisibilityLookup* sectorVisibilityLookup = engine->GetSectorVisibilityLookup();
	IWorldMeshAsset* worldMeshAsset = engine->GetWorldMeshAsset();
	ICollisionMesh* collisionMesh = worldMeshAsset->GetCollisionMesh();
	SectorMetrics* sectorMetrics = engine->GetSectorMetrics();
	Sector* sectors = engine->GetSectors();

	OcclusionSectorCruncherPass passes[6];
	Vec3i::Set(&passes[0].direction, -1, 0, 0);
	Vec3i::Set(&passes[0].extrpolateSubPass1Directions[0], 0, 0, -1);
	Vec3i::Set(&passes[0].extrpolateSubPass1Directions[1], 0, 0, 1);

	Vec3i::Set(&passes[1].direction, 1, 0, 0);
	Vec3i::Set(&passes[1].extrpolateSubPass1Directions[0], 0, 0, -1);
	Vec3i::Set(&passes[1].extrpolateSubPass1Directions[1], 0, 0, 1);
	
	Vec3i::Set(&passes[2].direction, 0, -1, 0);
	Vec3i::Set(&passes[2].extrpolateSubPass1Directions[0], 0, 0, -1);
	Vec3i::Set(&passes[2].extrpolateSubPass1Directions[1], 0, 0, 1);
	
	Vec3i::Set(&passes[3].direction, 0, 1, 0);
	Vec3i::Set(&passes[3].extrpolateSubPass1Directions[0], 0, 0, -1);
	Vec3i::Set(&passes[3].extrpolateSubPass1Directions[1], 0, 0, 1);
	
	Vec3i::Set(&passes[4].direction, 0, 0, -1);
	Vec3i::Set(&passes[4].extrpolateSubPass1Directions[0], -1, 0, 0);
	Vec3i::Set(&passes[4].extrpolateSubPass1Directions[1], 1, 0, 0);
	
	Vec3i::Set(&passes[5].direction, 0, 0, 1);
	Vec3i::Set(&passes[5].extrpolateSubPass1Directions[0], -1, 0, 0);
	Vec3i::Set(&passes[5].extrpolateSubPass1Directions[1], 1, 0, 0);

	Vec3i sectorIndexParts;
	Vec3i::Zero(&sectorIndexParts);

	for (sectorIndexParts.z = 0; sectorIndexParts.z < sectorMetrics->sectorCounts[2]; sectorIndexParts.z++)
	{
		for (sectorIndexParts.y = 0; sectorIndexParts.y < sectorMetrics->sectorCounts[1]; sectorIndexParts.y++)
		{
			for (sectorIndexParts.x = 0; sectorIndexParts.x < sectorMetrics->sectorCounts[0]; sectorIndexParts.x++)
			{			
				for (int passIndex = 0; passIndex < 6; passIndex++)
				{
					OcclusionSectorCruncherPass* pass = &passes[passIndex];

					this->RunPassOnSector(pass, &sectorIndexParts, sectorVisibilityLookup, sectors, sectorMetrics, collisionMesh);
				}
			}
		}
	}
}

void OcclusionSectorCruncher::RunPassOnSector(
	OcclusionSectorCruncherPass* pass, Vec3i* sectorIndexParts, 
	ISectorVisibilityLookup* sectorVisibilityLookup, Sector* sectors, SectorMetrics* sectorMetrics, ICollisionMesh* collisionMesh)
{
	int sectorAIndex = this->CalculateSectorIndex(sectorIndexParts, sectorMetrics);

	Vec3i sectorBIndexParts;

	if (this->FindFirstNonVisibleSectorAlongDirection(
		&sectorBIndexParts, sectorAIndex, sectorIndexParts, true, &pass->direction,
		sectorVisibilityLookup, sectors, sectorMetrics, collisionMesh))
	{
		this->MarkSectorsAlongDirectionAsNotVisible(
			sectorAIndex, &sectorBIndexParts, &pass->direction,
			sectorVisibilityLookup, sectorMetrics);

		for (int subPassIndex = 0; subPassIndex < 2; subPassIndex++)
		{
			Vec3i* subPassDirection = &pass->extrpolateSubPass1Directions[subPassIndex];
			Vec3i subPassStartSectorIndexParts = sectorBIndexParts;

			while (true)
			{
				Vec3i::Add(&subPassStartSectorIndexParts, &subPassStartSectorIndexParts, subPassDirection);

				if (this->CheckSectorIndexIsOutsideOfBounds(&subPassStartSectorIndexParts, sectorMetrics))
				{
					break;
				}

				Vec3i subPassSectorBIndexParts;

				if (this->FindFirstNonVisibleSectorAlongDirection(
					&subPassSectorBIndexParts, sectorAIndex, &subPassStartSectorIndexParts, false, &pass->direction,
					sectorVisibilityLookup, sectors, sectorMetrics, collisionMesh))
				{
					this->MarkSectorsAlongDirectionAsNotVisible(
						sectorAIndex, &subPassSectorBIndexParts, &pass->direction,
						sectorVisibilityLookup, sectorMetrics);
				}
			}
		}
	}

	/*Vec3i sectorBIndexParts = *sectorIndexParts;

	while (true)
	{
		Vec3i::Add(&sectorBIndexParts, &sectorBIndexParts, &pass->direction);

		if (this->CheckSectorIndexIsOutsideOfBounds(&sectorBIndexParts, sectorMetrics))
		{
			break;
		}

		int sectorBIndex = this->CalculateSectorIndex(&sectorBIndexParts, sectorMetrics);
		bool sectorBIsVisible = this->CheckSectorIsVisible(sectorAIndex, sectorBIndex, sectorVisibilityLookup, sectors, sectorMetrics, collisionMesh);

		if (!sectorBIsVisible)
		{
			this->MarkSectorsAlongDirectionAsNotVisible(
				sectorAIndex, &sectorBIndexParts, &pass->direction,
				sectorVisibilityLookup, sectorMetrics);

			break;
		}
	}*/
}

int OcclusionSectorCruncher::CalculateSectorIndex(Vec3i* sectorIndexParts, SectorMetrics* sectorMetrics)
{
	int sectorIndex = 
		(sectorIndexParts->z * sectorMetrics->sectorCounts[1] * sectorMetrics->sectorCounts[0]) +
		(sectorIndexParts->y * sectorMetrics->sectorCounts[0]) +
		sectorIndexParts->x;

	return sectorIndex;
}

bool OcclusionSectorCruncher::CheckSectorIndexIsOutsideOfBounds(Vec3i* sectorIndexParts, SectorMetrics* sectorMetrics)
{
	bool outsideOfBounds = false;

	if (sectorIndexParts->x < 0 || sectorIndexParts->x >= sectorMetrics->sectorCounts[0] ||
		sectorIndexParts->y < 0 || sectorIndexParts->y >= sectorMetrics->sectorCounts[1] ||
		sectorIndexParts->z < 0 || sectorIndexParts->z >= sectorMetrics->sectorCounts[2])
	{
		outsideOfBounds = true;
	}

	return outsideOfBounds;
}

bool OcclusionSectorCruncher::CheckSectorIsVisible(
	int sectorAIndex, int sectorBIndex, 
	ISectorVisibilityLookup* sectorVisibilityLookup, Sector* sectors, SectorMetrics* sectorMetrics, ICollisionMesh* collisionMesh)
{
	bool isVisible = false;

	SectorVisibilityState currentVisibilityState = sectorVisibilityLookup->GetSectorVisibilityState(sectorAIndex, sectorBIndex);
	if (currentVisibilityState == SectorVisibilityStateUnknown)
	{
		Sector* sectorA = &sectors[sectorAIndex];
		Sector* sectorB = &sectors[sectorBIndex];

		if (sectorA->numberOfInsidePoints > 0 && sectorB->numberOfInsidePoints > 0)
		{
			for (int i = 0; i < 600; i++)
			{
				CollisionLine line;
				line.from = sectorA->insidePoints[Math::GenerateRandomInt(0, sectorA->numberOfInsidePoints - 1)];
				line.to = sectorB->insidePoints[Math::GenerateRandomInt(0, sectorB->numberOfInsidePoints - 1)];
				CollisionLine::FromOwnFromAndToPoints(&line);

				if (!collisionMesh->DetermineIfLineIntersectsMesh(&line, sectorMetrics, sectors))
				{
					isVisible = true;
					break;
				}
			}
		}

		sectorVisibilityLookup->SetSectorVisibilityState(sectorAIndex, sectorBIndex, isVisible ? SectorVisibilityStateVisible : SectorVisibilityStateNotVisible);
	}
	else if (currentVisibilityState == SectorVisibilityStateVisible)
	{
		isVisible = true;
	}

	return isVisible;
}

bool OcclusionSectorCruncher::FindFirstNonVisibleSectorAlongDirection(
	Vec3i* out, int sectorAIndex, Vec3i* startSectorBIndexParts, bool skipFirstSector, Vec3i* direction, ISectorVisibilityLookup* sectorVisibilityLookup,
	Sector* sectors, SectorMetrics* sectorMetrics, ICollisionMesh* collisionMesh)
{
	bool nonVisibleSectorFound = false;

	//int sectorAIndex = this->CalculateSectorIndex(sectorAIndexParts, sectorMetrics);
	Vec3i sectorBIndexParts = *startSectorBIndexParts;

	if (skipFirstSector)
	{
		Vec3i::Add(&sectorBIndexParts, &sectorBIndexParts, direction);
	}

	while (true)
	{
		if (this->CheckSectorIndexIsOutsideOfBounds(&sectorBIndexParts, sectorMetrics))
		{
			break;
		}

		int sectorBIndex = this->CalculateSectorIndex(&sectorBIndexParts, sectorMetrics);
		bool sectorBIsVisible = this->CheckSectorIsVisible(sectorAIndex, sectorBIndex, sectorVisibilityLookup, sectors, sectorMetrics, collisionMesh);

		if (!sectorBIsVisible)
		{
			nonVisibleSectorFound = true;
			*out = sectorBIndexParts;
			break;
		}

		Vec3i::Add(&sectorBIndexParts, &sectorBIndexParts, direction);
	}

	return nonVisibleSectorFound;
}

void OcclusionSectorCruncher::MarkSectorsAlongDirectionAsNotVisible(
	int sectorAIndex, Vec3i* startSectorBIndexParts, Vec3i* direction,
	ISectorVisibilityLookup* sectorVisibilityLookup, SectorMetrics* sectorMetrics)
{
	Vec3i sectorBIndexParts = *startSectorBIndexParts;

	while (true)
	{
		Vec3i::Add(&sectorBIndexParts, &sectorBIndexParts, direction);

		if (this->CheckSectorIndexIsOutsideOfBounds(&sectorBIndexParts, sectorMetrics))
		{
			break;
		}

		int sectorBIndex = this->CalculateSectorIndex(&sectorBIndexParts, sectorMetrics);

		sectorVisibilityLookup->SetSectorVisibilityState(sectorAIndex, sectorBIndex, SectorVisibilityStateNotVisible);
	}
}