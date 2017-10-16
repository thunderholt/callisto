#include "Core/CollisionMesh.h"

CollisionMesh::CollisionMesh()
{
	this->chunks = null;
	this->numberOfChunks = 0;
	this->nextChunkIndex = 0;
	this->faces = null;
	this->numberOfFaces = 0;
	this->nextFaceIndex = 0;
	Vec3::Zero(&pointCompletelyOutsideOfExtremities);
}

CollisionMesh::~CollisionMesh()
{
	SafeDeleteArrayAndNull(this->chunks);
	SafeDeleteArrayAndNull(this->faces);
}

void CollisionMesh::Allocate(int numberOfChunks, int numberOfFaces)
{
	SafeDeleteArrayAndNull(this->chunks);
	SafeDeleteArrayAndNull(this->faces);

	this->numberOfChunks = numberOfChunks;
	this->numberOfFaces = numberOfFaces;

	this->chunks = new CollisionMeshChunk[this->numberOfChunks];
	this->nextChunkIndex = 0;

	this->faces = new CollisionFace[this->numberOfFaces];
	this->nextFaceIndex = 0;
}

void CollisionMesh::PushChunk(int startIndex, int numberOfFaces, float* positions, unsigned short* indecies)
{
	CollisionMeshChunk* chunk = &this->chunks[this->nextChunkIndex++];
	chunk->startFaceIndex = this->nextFaceIndex;
	chunk->numberOfFaces = numberOfFaces;

	for (int i = startIndex; i < startIndex + numberOfFaces * 3; i += 3)
	{
		unsigned short vert0StartIndex = indecies[i] * 3;
		unsigned short vert1StartIndex = indecies[i + 1] * 3;
		unsigned short vert2StartIndex = indecies[i + 2] * 3;

		Vec3 facePoints[3];
		Vec3::Set(&facePoints[0], positions[vert0StartIndex], positions[vert0StartIndex + 1], positions[vert0StartIndex + 2]);
		Vec3::Set(&facePoints[1], positions[vert1StartIndex], positions[vert1StartIndex + 1], positions[vert1StartIndex + 2]);
		Vec3::Set(&facePoints[2], positions[vert2StartIndex], positions[vert2StartIndex + 1], positions[vert2StartIndex + 2]);

		CollisionFace* face = &this->faces[this->nextFaceIndex++];
		CollisionFace::BuildFromPoints(face, facePoints);
	}

	AABB::CalculateFromCollisionFaces(&chunk->aabb, &this->faces[chunk->startFaceIndex], chunk->numberOfFaces);
}

void CollisionMesh::Finish()
{
	this->FindPointCompletelyOutsideOfExtremities();
}

bool CollisionMesh::DetermineIfPointIsInsideIndoorMesh(Vec3* point) 
{
	CollisionLine collisionLine;
	collisionLine.from = *point;
	collisionLine.to = this->pointCompletelyOutsideOfExtremities;
	CollisionLine::FromOwnFromAndToPoints(&collisionLine);

	Vec3 faceIntersection;
	float nearestFaceIntersectionDistanceSqr = -1;
	FaceIntersectionType nearestFaceIntersectionType = FaceIntersectionTypeNone;

	for (int chunkIndex = 0; chunkIndex < this->numberOfChunks; chunkIndex++) 
	{
		CollisionMeshChunk* chunk = &this->chunks[chunkIndex];

		for (int faceIndex = chunk->startFaceIndex; faceIndex < chunk->startFaceIndex + chunk->numberOfFaces; faceIndex++)
		{
			CollisionFace* face = &this->faces[faceIndex];

			FaceIntersectionType faceIntersectionType = CollisionLine::CalculateIntersectionWithCollisionFace(
				&faceIntersection, &collisionLine, face);

			if (faceIntersectionType != FaceIntersectionTypeNone)
			{
				float faceIntersectionDistanceSqr = Vec3::DistanceSqr(point, &faceIntersection);
				if (nearestFaceIntersectionDistanceSqr == -1 || faceIntersectionDistanceSqr < nearestFaceIntersectionDistanceSqr) 
				{
					nearestFaceIntersectionType = faceIntersectionType;
					nearestFaceIntersectionDistanceSqr = faceIntersectionDistanceSqr;
				}
			}
		}
	}

	return nearestFaceIntersectionType == FaceIntersectionTypeFrontSide;
}

bool CollisionMesh::DetermineIfLineIntersectsMesh(CollisionLine* line, SectorMetrics* sectorMetrics, Sector* sectors/*, CollisionMeshLineIntersectionDeterminationWorkingData* workingData*/)
{
	bool intersectionFound = false;
	
	int sectorIndex = this->GetSectorIndexFromPoint(&line->from, sectorMetrics, true);
	if (sectorIndex != -1)
	{
		intersectionFound = this->DetermineIfLineIntersectsChunksInSector(line, sectorMetrics, &sectors[sectorIndex]);
	}

	if (!intersectionFound)
	{
		sectorIndex = this->GetSectorIndexFromPoint(&line->to, sectorMetrics, true);
		if (sectorIndex != -1)
		{
			intersectionFound = this->DetermineIfLineIntersectsChunksInSector(line, sectorMetrics, &sectors[sectorIndex]);
		}

		if (!intersectionFound)
		{
			int lastSectorIndex = -1;

			for (int gridPlaneIndex = 0; gridPlaneIndex < sectorMetrics->gridPlanes.GetLength() && !intersectionFound; gridPlaneIndex++)
			{
				Plane* gridPlane = &sectorMetrics->gridPlanes[gridPlaneIndex];

				Vec3 intersectionPoint;
				if (CollisionLine::CalculateIntersectionWithPlane(&intersectionPoint, line, gridPlane))
				{
					Vec3 fudgedIntersectionPoint;

					Vec3::ScaleAndAdd(&fudgedIntersectionPoint, &intersectionPoint, &gridPlane->normal, 0.01f);

					sectorIndex = this->GetSectorIndexFromPoint(&fudgedIntersectionPoint, sectorMetrics, true);
					if (sectorIndex != -1 && sectorIndex != lastSectorIndex)
					{
						intersectionFound = this->DetermineIfLineIntersectsChunksInSector(line, sectorMetrics, &sectors[sectorIndex]);
					}

					if (!intersectionFound)
					{
						Vec3::ScaleAndAdd(&fudgedIntersectionPoint, &intersectionPoint, &gridPlane->normal, -0.01f);

						sectorIndex = this->GetSectorIndexFromPoint(&fudgedIntersectionPoint, sectorMetrics, true);
						if (sectorIndex != -1)
						{
							intersectionFound = this->DetermineIfLineIntersectsChunksInSector(line, sectorMetrics, &sectors[sectorIndex]);
							lastSectorIndex = sectorIndex;
						}
					}
				}
			}
		}
	}

	/*bool intersectionFound = false;
	bool linePointsChecked = false;
	int gridPlaneIndex = 0;
	int sectorIndexesToCheck[2] = { -1, -1 };
	
	while ((!linePointsChecked || gridPlaneIndex < sectorMetrics->gridPlanes.GetLength()) && !intersectionFound)
	{
		if (!linePointsChecked)
		{
			sectorIndexesToCheck[0] = this->GetSectorIndexFromPoint(&line->from, sectorMetrics, true);
			sectorIndexesToCheck[1] = this->GetSectorIndexFromPoint(&line->to, sectorMetrics, true);
			linePointsChecked = true;
		}
		else
		{
			Plane* gridPlane = &sectorMetrics->gridPlanes[gridPlaneIndex];

			Vec3 intersectionPoint;
			if (CollisionLine::CalculateIntersectionWithPlane(&intersectionPoint, line, gridPlane))
			{
				Vec3 fudgedIntersectionPoint;

				Vec3::ScaleAndAdd(&fudgedIntersectionPoint, &intersectionPoint, &gridPlane->normal, 0.01f);
				sectorIndexesToCheck[0] = this->GetSectorIndexFromPoint(&fudgedIntersectionPoint, sectorMetrics, true);

				Vec3::ScaleAndAdd(&fudgedIntersectionPoint, &intersectionPoint, &gridPlane->normal, -0.01f);
				sectorIndexesToCheck[1] = this->GetSectorIndexFromPoint(&fudgedIntersectionPoint, sectorMetrics, true);
			}

			gridPlaneIndex++;
		}

		for (int i = 0; i < 2 && !intersectionFound; i++)
		{
			int sectorIndex = sectorIndexesToCheck[i];
			if (sectorIndex != -1)
			{
				Sector* sector = &sectors[sectorIndex];

				intersectionFound = this->DetermineIfLineIntersectsChunksInSector(line, sectorMetrics, sector);
			}
		}
	}*/

	/*bool intersectionFound = false;
	Vec3 epsilon;
	Vec3::Set(&epsilon, 0.0000001f, 0.0000001f, 0.0000001f);

	workingData->intersectedSectorIndexes.Clear();

	int fromSectorIndex = this->GetSectorIndexFromPoint(&line->from, sectorMetrics, true);
	if (fromSectorIndex != -1)
	{
		workingData->intersectedSectorIndexes.Push(fromSectorIndex);
	}

	for (int gridPlaneIndex = 0; gridPlaneIndex < sectorMetrics->gridPlanes.GetLength(); gridPlaneIndex++)
	{
		Plane* gridPlane = &sectorMetrics->gridPlanes[gridPlaneIndex];

		Vec3 intersectionPoint;
		if (CollisionLine::CalculateIntersectionWithPlane(&intersectionPoint, line, gridPlane))
		{
			Vec3 fudgedIntersectionPoint;

			//Vec3::Add(&intersectionPoint, &intersectionPoint, &epsilon);
			Vec3::ScaleAndAdd(&fudgedIntersectionPoint, &intersectionPoint, &gridPlane->normal, 0.01f);

			int sectorIndex = this->GetSectorIndexFromPoint(&fudgedIntersectionPoint, sectorMetrics, true);
			if (sectorIndex != -1)
			{
				workingData->intersectedSectorIndexes.Push(sectorIndex);
			}

			Vec3::ScaleAndAdd(&fudgedIntersectionPoint, &intersectionPoint, &gridPlane->normal, -0.01f);

			sectorIndex = this->GetSectorIndexFromPoint(&fudgedIntersectionPoint, sectorMetrics, true);
			if (sectorIndex != -1)
			{
				workingData->intersectedSectorIndexes.Push(sectorIndex);
			}
		}
	}

	int toSectorIndex = this->GetSectorIndexFromPoint(&line->to, sectorMetrics, true);
	if (toSectorIndex != -1)
	{
		workingData->intersectedSectorIndexes.Push(toSectorIndex);
	}

	for (int i = 0; i < workingData->intersectedSectorIndexes.GetLength() && !intersectionFound; i++)
	{
		int sectorIndex = workingData->intersectedSectorIndexes[i];
		Sector* sector = &sectors[sectorIndex];

		intersectionFound = this->DetermineIfLineIntersectsChunksInSector(line, sectorMetrics, sector);
	}
	*/
	///////////////////////////////////////////////////////////////////////////////////////////

	/*bool intersectionFoundB = false;

	for (int sectorIndex = 0; sectorIndex < sectorMetrics->numberOfSectors && !intersectionFoundB; sectorIndex++)
	{
		Sector* sector = &sectors[sectorIndex];

		for (int i = 0;
			i < sector->residentWorldMeshChunkIndexes.GetLength() && !intersectionFoundB;
			i++)
		{
			int chunkIndex = sector->residentWorldMeshChunkIndexes[i];
			CollisionMeshChunk* chunk = &this->chunks[chunkIndex];

			for (int faceIndex = chunk->startFaceIndex;
				faceIndex < (chunk->startFaceIndex + chunk->numberOfFaces) && !intersectionFoundB;
				faceIndex++)
			{
				CollisionFace* face = &this->faces[faceIndex];

				Vec3 faceIntersectionPoint;
				FaceIntersectionType faceIntersectionType = CollisionLine::CalculateIntersectionWithCollisionFace(
					&faceIntersectionPoint, line, face);

				if (faceIntersectionType != FaceIntersectionTypeNone)
				{
					intersectionFoundB = true;

					bool sectorFound = false;
					for (int j = 0; j < workingData->intersectedSectorIndexes.GetLength(); j++)
					{
						if (workingData->intersectedSectorIndexes[j] == sectorIndex)
						{
							sectorFound = true;
						}
					}

					if (!sectorFound)
					{
						int f = 1;
					}

					if (!intersectionFound)
					{
						int g = 1;
					}
				}
			}
		}
	}*/

	///////////////////////////////////////////////////////////////////////////////////////////

	/*
	bool intersectionFound = false;

	for (int chunkIndex = 0; 
		chunkIndex < this->numberOfChunks && !intersectionFound; 
		chunkIndex++)
	{
		CollisionMeshChunk* chunk = &this->chunks[chunkIndex];

		for (int faceIndex = chunk->startFaceIndex; 
			faceIndex < (chunk->startFaceIndex + chunk->numberOfFaces) && !intersectionFound; 
			faceIndex++)
		{
			CollisionFace* face = &this->faces[faceIndex];

			Vec3 faceIntersectionPoint;
			FaceIntersectionType faceIntersectionType = CollisionLine::CalculateIntersectionWithCollisionFace(
				&faceIntersectionPoint, line, face);

			if (faceIntersectionType != FaceIntersectionTypeNone)
			{
				intersectionFound = true;
			}
		}
	}*/

	return intersectionFound;
}

/*bool CollisionMesh::FindNearestRayIntersection(CollisionMeshIntersectionResult* out, Ray3* ray)
{
	bool intersectionFound = false;
	int nearestFaceIntersectionChunkIndex = -1;
	int nearestFaceIntersectionFaceIndex = -1;
	Vec3 nearestFaceIntersectionPoint;
	float nearestFaceIntersectionDistanceSqr = -1;
	// TODO - AABB check

	for (int chunkIndex = 0; chunkIndex < this->numberOfChunks; chunkIndex++)
	{
		CollisionMeshChunk* chunk = &this->chunks[chunkIndex];

		for (int faceIndex = chunk->startFaceIndex; faceIndex < chunk->startFaceIndex + chunk->numberOfFaces; faceIndex++)
		{
			CollisionFace* face = &this->faces[faceIndex];

			Vec3 faceIntersectionPoint;
			FaceIntersectionType faceIntersectionType = Ray3::CalculateIntersectionWithCollisionFace(
				&faceIntersectionPoint, ray, face);

			if (faceIntersectionType == FaceIntersectionTypeFrontSide) // FIXME - make this a paramater, um, maybe.
			{
				intersectionFound = true;

				float faceIntersectionDistanceSqr = Vec3::DistanceSqr(&ray->origin, &faceIntersectionPoint);

				if (nearestFaceIntersectionDistanceSqr == -1 || faceIntersectionDistanceSqr < nearestFaceIntersectionDistanceSqr)
				{
					nearestFaceIntersectionChunkIndex = chunkIndex;
					nearestFaceIntersectionFaceIndex = faceIndex;
					nearestFaceIntersectionDistanceSqr = faceIntersectionDistanceSqr;
					nearestFaceIntersectionPoint = faceIntersectionPoint;
				}
			}
		}
	}

	if (intersectionFound && out != null)
	{
		CollisionFace* face = &this->faces[nearestFaceIntersectionFaceIndex];

		out->intersectionPoint = nearestFaceIntersectionPoint;
		out->intersectionPlane = face->facePlane;
		out->chunkIndex = nearestFaceIntersectionChunkIndex;
		out->faceIndex = nearestFaceIntersectionFaceIndex;
		out->distance = sqrtf(nearestFaceIntersectionDistanceSqr);
	}

	return intersectionFound;
}*/

/*bool CollisionMesh::FindNearestLineIntersection(CollisionMeshIntersectionResult* out, CollisionLine* line)
{
	bool intersectionFound = false;
	int nearestFaceIntersectionChunkIndex = -1;
	int nearestFaceIntersectionFaceIndex = -1;
	Vec3 nearestFaceIntersectionPoint;
	float nearestFaceIntersectionDistanceSqr = -1;
	// TODO - AABB check

	for (int chunkIndex = 0; chunkIndex < this->numberOfChunks; chunkIndex++)
	{
		CollisionMeshChunk* chunk = &this->chunks[chunkIndex];

		for (int faceIndex = chunk->startFaceIndex; faceIndex < chunk->startFaceIndex + chunk->numberOfFaces; faceIndex++)
		{
			CollisionFace* face = &this->faces[faceIndex];

			Vec3 faceIntersectionPoint;
			FaceIntersectionType faceIntersectionType = CollisionLine::CalculateIntersectionWithCollisionFace(
				&faceIntersectionPoint, line, face);

			if (faceIntersectionType == FaceIntersectionTypeFrontSide)
			{
				intersectionFound = true;

				float faceIntersectionDistanceSqr = Vec3::DistanceSqr(&line->from, &faceIntersectionPoint);

				if (nearestFaceIntersectionDistanceSqr == -1 || faceIntersectionDistanceSqr < nearestFaceIntersectionDistanceSqr)
				{
					nearestFaceIntersectionChunkIndex = chunkIndex;
					nearestFaceIntersectionFaceIndex = faceIndex;
					nearestFaceIntersectionDistanceSqr = faceIntersectionDistanceSqr;
					nearestFaceIntersectionPoint = faceIntersectionPoint;
					//nearestFaceIndex = faceIndex;
				}
			}
		}
	}

	if (intersectionFound && out != null)
	{
		CollisionFace* face = &this->faces[nearestFaceIntersectionFaceIndex];

		out->intersectionPoint = nearestFaceIntersectionPoint;
		out->intersectionPlane = face->facePlane;
		out->chunkIndex = nearestFaceIntersectionChunkIndex;
		out->faceIndex = nearestFaceIntersectionFaceIndex;
		out->distance = sqrtf(nearestFaceIntersectionDistanceSqr);
	}

	return intersectionFound;
}*/

CollisionMeshChunk* CollisionMesh::GetChunk(int chunkIndex)
{
	return &this->chunks[chunkIndex];
}

int CollisionMesh::GetNumberOfChunks()
{
	return this->numberOfChunks;
}

CollisionFace* CollisionMesh::GetFace(int faceIndex)
{
	return &this->faces[faceIndex];
}

int CollisionMesh::GetNumberOfFaces()
{
	return this->numberOfFaces;
}

void CollisionMesh::FindPointCompletelyOutsideOfExtremities()
{
	Vec3 max;
	Vec3::Zero(&max);
	bool maxSet = false;

	for (int faceIndex = 0; faceIndex < this->numberOfFaces; faceIndex++)
	{
		CollisionFace* face = &this->faces[faceIndex];

		for (int pointIndex = 0; pointIndex < 3; pointIndex++)
		{
			Vec3* point = &face->points[pointIndex];

			if (!maxSet)
			{
				max = *point;
				maxSet = true;
			}
			else if (point->x > max.x)
			{
				max.x = point->x;
			}
			else if (point->y > max.y)
			{
				max.y = point->y;
			}
			else if (point->z > max.z)
			{
				max.z = point->z;
			}
		}
	}

	Vec3::Add(&max, &max, 10.0f, 10.0f, 10.0f);

	this->pointCompletelyOutsideOfExtremities = max;
}

int CollisionMesh::GetSectorIndexFromPoint(Vec3* point, SectorMetrics* sectorMetrics, bool clamp)
{
	int sectorIndex = -1;

	Vec3 transformedPoint;
	Vec3::Sub(&transformedPoint, point, &sectorMetrics->originOffset);

	int sectorIndexParts[3];
	/*sectorIndexParts[0] = (int)floorf(transformedPoint.x / sectorMetrics->sectorSize);
	sectorIndexParts[1] = (int)floorf(transformedPoint.y / sectorMetrics->sectorSize);
	sectorIndexParts[2] = (int)floorf(transformedPoint.z / sectorMetrics->sectorSize);*/
	sectorIndexParts[0] = (int)(transformedPoint.x / sectorMetrics->sectorSize);
	sectorIndexParts[1] = (int)(transformedPoint.y / sectorMetrics->sectorSize);
	sectorIndexParts[2] = (int)(transformedPoint.z / sectorMetrics->sectorSize);

	bool pointIsWithinBounds = true;

	for (int i = 0; i < 3; i++)
	{
		if (sectorIndexParts[i] < 0)
		{
			if (clamp)
			{
				sectorIndexParts[i] = 0;
			}
			else
			{
				pointIsWithinBounds = false;
			}
		}
		else if (sectorIndexParts[i] >= sectorMetrics->sectorCounts[i])
		{
			if (clamp)
			{
				sectorIndexParts[i] = sectorMetrics->sectorCounts[i] - 1;
			}
			else
			{
				pointIsWithinBounds = false;
			}
		}
	}

	if (pointIsWithinBounds)
	{
		sectorIndex =
			(sectorIndexParts[2] * sectorMetrics->sectorCounts[1] * sectorMetrics->sectorCounts[0]) +
			(sectorIndexParts[1] * sectorMetrics->sectorCounts[0]) +
			sectorIndexParts[0];
	}
	else
	{
		int t = 1;
	}

	return sectorIndex;
}

bool CollisionMesh::DetermineIfLineIntersectsChunksInSector(CollisionLine* line, SectorMetrics* sectorMetrics, Sector* sector)
{
	bool intersectionFound = false;

	for (int i = 0; i < sector->residentWorldMeshChunkIndexes.GetLength() && !intersectionFound; i++)
	{
		int chunkIndex = sector->residentWorldMeshChunkIndexes[i];

		CollisionMeshChunk* chunk = &this->chunks[chunkIndex];

		for (int faceIndex = chunk->startFaceIndex;
			faceIndex < (chunk->startFaceIndex + chunk->numberOfFaces) && !intersectionFound;
			faceIndex++)
		{
			CollisionFace* face = &this->faces[faceIndex];

			Vec3 faceIntersectionPoint;
			FaceIntersectionType faceIntersectionType = CollisionLine::CalculateIntersectionWithCollisionFace(
				&faceIntersectionPoint, line, face);

			if (faceIntersectionType != FaceIntersectionTypeNone)
			{
				intersectionFound = true;
			}
		}
	}

	return intersectionFound;
}