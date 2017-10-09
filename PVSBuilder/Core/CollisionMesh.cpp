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

bool CollisionMesh::DetermineIfLineIntersectsMesh(CollisionLine* line, SectorMetrics* sectorMetrics, Sector* sectors, CollisionMeshLineIntersectionDeterminationWorkingData* workingData)
{
	bool intersectionFound = false;

	workingData->intersectedSectorIndexes.Clear();

	static float epsilon = 0.00001f;

	for (int gridPlaneIndex = 0; gridPlaneIndex < sectorMetrics->gridPlanes.GetLength(); gridPlaneIndex++)
	{
		Plane* gridPlane = &sectorMetrics->gridPlanes[gridPlaneIndex];

		Vec3 intersectionPoint;
		if (Ray3::CalculateIntersectionWithPlane(&intersectionPoint, &line->ray, gridPlane))
		{
			Vec3::Sub(&intersectionPoint, &intersectionPoint, &sectorMetrics->originOffset);

			int sectorIndexParts[3];
			sectorIndexParts[0] = (int)floorf((intersectionPoint.x - epsilon) / sectorMetrics->sectorSize);
			sectorIndexParts[1] = (int)floorf((intersectionPoint.y - epsilon) / sectorMetrics->sectorSize);
			sectorIndexParts[2] = (int)floorf((intersectionPoint.z - epsilon) / sectorMetrics->sectorSize);

			for (int i = 0; i < 3; i++)
			{
				if (sectorIndexParts[i] < 0)
				{
					sectorIndexParts[i] = 0;
				}
				else if (sectorIndexParts[i] >= sectorMetrics->sectorCounts[i])
				{
					sectorIndexParts[i] = sectorMetrics->sectorCounts[i] - 1;
				}
			}

			int sectorIndex = 
				(sectorIndexParts[2] * sectorMetrics->sectorCounts[1] * sectorMetrics->sectorCounts[0]) +
				(sectorIndexParts[1] * sectorMetrics->sectorCounts[0]) +
				sectorIndexParts[0];

			workingData->intersectedSectorIndexes.Push(sectorIndex);
		}
	}

	for (int i = 0; i < workingData->intersectedSectorIndexes.GetLength() && !intersectionFound; i++)
	{
		int sectorIndex = workingData->intersectedSectorIndexes[i];
		Sector* sector = &sectors[sectorIndex];

		for (int j = 0; j < sector->residentWorldMeshChunkIndexes.GetLength() && !intersectionFound; j++)
		{
			int chunkIndex = sector->residentWorldMeshChunkIndexes[j];

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
	}

	/*for (int chunkIndex = 0; 
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