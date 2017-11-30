#include "Core/CollisionMesh.h"

CollisionMesh::CollisionMesh()
{
	this->chunks = null;
	this->numberOfChunks = 0;
	this->nextChunkIndex = 0;
	this->faces = null;
	this->numberOfFaces = 0;
	this->nextFaceIndex = 0;
	Vec3::Zero(&this->gridMetrics.gridOrigin);
	Vec3i::Zero(&this->gridMetrics.gridDimensions);
	this->gridMetrics.numberOfCells = 0;
	this->gridMetrics.gridCellSize = 0.0f;
	this->gridCells = null;
}

CollisionMesh::~CollisionMesh()
{
	SafeDeleteArrayAndNull(this->chunks);
	SafeDeleteArrayAndNull(this->faces);
	SafeDeleteArrayAndNull(this->gridCells);
}

void CollisionMesh::AllocateGeometry(int numberOfChunks, int numberOfFaces)
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

void CollisionMesh::AllocateGrid(Vec3 gridOrigin, Vec3i gridDimensions, float gridCellSize)
{
	SafeDeleteArrayAndNull(this->gridCells);

	this->gridMetrics.gridOrigin = gridOrigin;
	this->gridMetrics.gridDimensions = gridDimensions;
	this->gridMetrics.gridCellSize = gridCellSize;

	// Build the grid planes.
	Vec3 planeNormals[3];
	Vec3::Set(&planeNormals[0], 1.0f, 0.0f, 0.0f);
	Vec3::Set(&planeNormals[1], 0.0f, 1.0f, 0.0f);
	Vec3::Set(&planeNormals[2], 0.0f, 0.0f, 1.0f);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j <= this->gridMetrics.gridDimensions.i[i]; j++)
		{
			Vec3 pointOnPlane = this->gridMetrics.gridOrigin;
			Vec3::ScaleAndAdd(&pointOnPlane, &pointOnPlane, &planeNormals[i], j * this->gridMetrics.gridCellSize);

			Plane plane;
			Plane::FromNormalAndPoint(&plane, &planeNormals[i], &pointOnPlane);

			this->gridMetrics.gridPlanes.Push(plane);
		}
	}

	// Build the grid cells.
	this->gridMetrics.numberOfCells = 
		this->gridMetrics.gridDimensions.x * 
		this->gridMetrics.gridDimensions.y * 
		this->gridMetrics.gridDimensions.z;

	this->gridCells = new CollisionMeshGridCell[this->gridMetrics.numberOfCells];
}

void CollisionMesh::PushChunk(int startIndex, int numberOfFaces, Vec3* positions, Vec3* normals, Vec2* uvs, unsigned short* indecies/*, int lightAtlasIndex, Vec2i lightIslandOffset, Vec2i lightIslandSize*/)
{
	CollisionMeshChunk* chunk = &this->chunks[this->nextChunkIndex++];
	chunk->startFaceIndex = this->nextFaceIndex;
	chunk->numberOfFaces = numberOfFaces;
	/*chunk->lightAtlasIndex = lightAtlasIndex;
	chunk->lightIslandOffset = lightIslandOffset;
	chunk->lightIslandSize = lightIslandSize;*/

	for (int i = startIndex; i < startIndex + numberOfFaces * 3; i += 3)
	{
		/*unsigned short vert0StartIndex = indecies[i];
		unsigned short vert1StartIndex = indecies[i + 1];
		unsigned short vert2StartIndex = indecies[i + 2];*/

		unsigned short vert0Index = indecies[i];
		unsigned short vert1Index = indecies[i + 1];
		unsigned short vert2Index = indecies[i + 2];

		Vec3 facePoints[3];
		facePoints[0] = positions[vert0Index];
		facePoints[1] = positions[vert1Index];
		facePoints[2] = positions[vert2Index];
		/*Vec3::Set(&facePoints[0], positions[vert0StartIndex], positions[vert0StartIndex + 1], positions[vert0StartIndex + 2]);
		Vec3::Set(&facePoints[1], positions[vert1StartIndex], positions[vert1StartIndex + 1], positions[vert1StartIndex + 2]);
		Vec3::Set(&facePoints[2], positions[vert2StartIndex], positions[vert2StartIndex + 1], positions[vert2StartIndex + 2]);*/

		Vec3 faceNormals[3];
		faceNormals[0] = normals[vert0Index];
		faceNormals[1] = normals[vert1Index];
		faceNormals[2] = normals[vert2Index];

		Vec2 faceUVs[3];
		faceUVs[0] = uvs[vert0Index];
		faceUVs[1] = uvs[vert1Index];
		faceUVs[2] = uvs[vert2Index];

		CollisionFace* face = &this->faces[this->nextFaceIndex++];
		CollisionFace::BuildFromPoints(face, facePoints, faceNormals, faceUVs);
	}

	AABB::CalculateFromCollisionFaces(&chunk->aabb, &this->faces[chunk->startFaceIndex], chunk->numberOfFaces);
}

void CollisionMesh::Finish()
{
	this->BuildGridCells();
}

/*bool CollisionMesh::DetermineIfLineIntersectsMesh(CollisionLine* line, int ignoreChunkIndex)
{
	bool intersectionFound = false;
	
	int gridCellIndex = this->GetGridCellIndexFromPoint(&line->from);
	if (gridCellIndex != -1)
	{
		intersectionFound = this->DetermineIfLineIntersectsChunksInGridCell(line, gridCellIndex, ignoreChunkIndex);
	}

	if (!intersectionFound)
	{
		gridCellIndex = this->GetGridCellIndexFromPoint(&line->to);
		if (gridCellIndex != -1)
		{
			intersectionFound = this->DetermineIfLineIntersectsChunksInGridCell(line, gridCellIndex, ignoreChunkIndex);
		}

		if (!intersectionFound)
		{
			int lastGridCellIndex = -1;

			for (int gridPlaneIndex = 0; gridPlaneIndex < this->gridMetrics.gridPlanes.GetLength() && !intersectionFound; gridPlaneIndex++)
			{
				Plane* gridPlane = &this->gridMetrics.gridPlanes[gridPlaneIndex];

				Vec3 intersectionPoint;
				if (CollisionLine::CalculateIntersectionWithPlane(&intersectionPoint, line, gridPlane))
				{
					Vec3 fudgedIntersectionPoint;

					Vec3::ScaleAndAdd(&fudgedIntersectionPoint, &intersectionPoint, &gridPlane->normal, 0.01f);

					gridCellIndex = this->GetGridCellIndexFromPoint(&fudgedIntersectionPoint);
					if (gridCellIndex != -1 && gridCellIndex != lastGridCellIndex)
					{
						intersectionFound = this->DetermineIfLineIntersectsChunksInGridCell(line, gridCellIndex, ignoreChunkIndex);
					}

					if (!intersectionFound)
					{
						Vec3::ScaleAndAdd(&fudgedIntersectionPoint, &intersectionPoint, &gridPlane->normal, -0.01f);

						gridCellIndex = this->GetGridCellIndexFromPoint(&fudgedIntersectionPoint);
						if (gridCellIndex != -1)
						{
							intersectionFound = this->DetermineIfLineIntersectsChunksInGridCell(line, gridCellIndex, ignoreChunkIndex);
							lastGridCellIndex = gridCellIndex;
						}
					}
				}
			}
		}
	}

	return intersectionFound;
}*/

bool CollisionMesh::DetermineIfLineIntersectsMesh(CollisionLine* line, int ignoreChunkIndex)
{
	bool intersectionFound = false;

	int lastGridCellIndex = -1;

	for (int gridPlaneIndex = 0; gridPlaneIndex < this->gridMetrics.gridPlanes.GetLength() && !intersectionFound; gridPlaneIndex++)
	{
		Plane* gridPlane = &this->gridMetrics.gridPlanes[gridPlaneIndex];

		Vec3 intersectionPoint;
		if (CollisionLine::CalculateIntersectionWithPlane(&intersectionPoint, line, gridPlane))
		{
			Vec3 fudgedIntersectionPoint;

			Vec3::ScaleAndAdd(&fudgedIntersectionPoint, &intersectionPoint, &gridPlane->normal, 0.01f);

			int gridCellIndex = this->GetGridCellIndexFromPoint(&fudgedIntersectionPoint);
			if (gridCellIndex != -1 && gridCellIndex != lastGridCellIndex)
			{
				intersectionFound = this->DetermineIfLineIntersectsChunksInGridCell(line, gridCellIndex, ignoreChunkIndex);
			}

			if (!intersectionFound)
			{
				Vec3::ScaleAndAdd(&fudgedIntersectionPoint, &intersectionPoint, &gridPlane->normal, -0.01f);

				gridCellIndex = this->GetGridCellIndexFromPoint(&fudgedIntersectionPoint);
				if (gridCellIndex != -1)
				{
					intersectionFound = this->DetermineIfLineIntersectsChunksInGridCell(line, gridCellIndex, ignoreChunkIndex);
					lastGridCellIndex = gridCellIndex;
				}
			}
		}
	}
		
	return intersectionFound;
}

bool CollisionMesh::FindNearestLineIntersectWithMesh(Vec3* outIntersection, MeshChunkFaceIndex* outChunkFaceIndex, CollisionLine* line, int ignoreChunkIndex)
{
	bool intersectionFound = false;
	Vec3 nearestIntersection;
	float nearestIntersectionDistanceSqr = -1;

	int lastGridCellIndex = -1;

	// TODO - find grid cell indexes, order by distance, take intersection from first cell that has an intersection.

	for (int gridPlaneIndex = 0; gridPlaneIndex < this->gridMetrics.gridPlanes.GetLength() && !intersectionFound; gridPlaneIndex++)
	{
		Plane* gridPlane = &this->gridMetrics.gridPlanes[gridPlaneIndex];

		Vec3 intersectionPoint;
		if (CollisionLine::CalculateIntersectionWithPlane(&intersectionPoint, line, gridPlane))
		{
			Vec3 fudgedIntersectionPoint;

			for (int fudgeIndex = 0; fudgeIndex < 2; fudgeIndex++)
			{
				Vec3::ScaleAndAdd(&fudgedIntersectionPoint, &intersectionPoint, &gridPlane->normal, fudgeIndex == 0 ? 0.01f : -0.01f);

				int gridCellIndex = this->GetGridCellIndexFromPoint(&fudgedIntersectionPoint);
				if (gridCellIndex != -1 && gridCellIndex != lastGridCellIndex)
				{
					Vec3 intersection;
					if (this->FindNearestLineIntersectionWithChunksInGridCell(&intersection, outChunkFaceIndex, line, gridCellIndex, ignoreChunkIndex))
					{
						float distanceSqr = Vec3::DistanceSqr(&line->from, &intersection);

						if (nearestIntersectionDistanceSqr == -1 || distanceSqr < nearestIntersectionDistanceSqr)
						{
							nearestIntersectionDistanceSqr = distanceSqr;
							nearestIntersection = intersection;
						}

						intersectionFound = true;
					}

					lastGridCellIndex = gridCellIndex;
				}
			}
		}
	}

	if (intersectionFound)
	{
		*outIntersection = nearestIntersection;
	}

	return intersectionFound;
}

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

void CollisionMesh::BuildGridCells()
{
	// Build the grid cells.
	int gridCellIndex = 0;

	for (int z = 0; z < this->gridMetrics.gridDimensions.z; z++)
	{
		for (int y = 0; y < this->gridMetrics.gridDimensions.y; y++)
		{
			for (int x = 0; x < this->gridMetrics.gridDimensions.x; x++)
			{
				CollisionMeshGridCell* gridCell = &this->gridCells[gridCellIndex];

				// Compute the cell's origin.
				Vec3 cellOrigin;
				Vec3::Set(
					&cellOrigin,
					x * this->gridMetrics.gridCellSize,
					y * this->gridMetrics.gridCellSize,
					z * this->gridMetrics.gridCellSize);

				Vec3::Add(&cellOrigin, &cellOrigin, &this->gridMetrics.gridOrigin);

				// Find the resident chunk indexes for this cell.
				AABB cellAabb;
				cellAabb.from = cellOrigin;
				Vec3::Add(&cellAabb.to, &cellAabb.from, this->gridMetrics.gridCellSize, this->gridMetrics.gridCellSize, this->gridMetrics.gridCellSize);

				for (int chunkIndex = 0; chunkIndex < this->numberOfChunks; chunkIndex++)
				{
					CollisionMeshChunk* chunk = &this->chunks[chunkIndex];

					if (AABB::CheckIntersectsAABB(&cellAabb, &chunk->aabb))
					{
						gridCell->residentChunkIndexes.Push(chunkIndex);
					}
				}

				gridCellIndex++;
			}
		}
	}
}

int CollisionMesh::GetGridCellIndexFromPoint(Vec3* point)
{
	int gridCellIndex = -1;

	Vec3 transformedPoint;
	Vec3::Sub(&transformedPoint, point, &this->gridMetrics.gridOrigin);

	int gridCellIndexParts[3];
	gridCellIndexParts[0] = (int)(transformedPoint.x / this->gridMetrics.gridCellSize);
	gridCellIndexParts[1] = (int)(transformedPoint.y / this->gridMetrics.gridCellSize);
	gridCellIndexParts[2] = (int)(transformedPoint.z / this->gridMetrics.gridCellSize);

	for (int i = 0; i < 3; i++)
	{
		if (gridCellIndexParts[i] < 0)
		{
			gridCellIndexParts[i] = 0;
		}
		else if (gridCellIndexParts[i] >= this->gridMetrics.gridDimensions.i[i])
		{
			gridCellIndexParts[i] = this->gridMetrics.gridDimensions.i[i] - 1;
		}
	}

	gridCellIndex =
		(gridCellIndexParts[2] * this->gridMetrics.gridDimensions.y * this->gridMetrics.gridDimensions.x) +
		(gridCellIndexParts[1] * this->gridMetrics.gridDimensions.x) +
		gridCellIndexParts[0];

	return gridCellIndex;
}

bool CollisionMesh::DetermineIfLineIntersectsChunksInGridCell(CollisionLine* line, int gridCellIndex, int ignoreChunkIndex)
{
	bool intersectionFound = false;

	CollisionMeshGridCell* gridCell = &this->gridCells[gridCellIndex];

	for (int i = 0; i < gridCell->residentChunkIndexes.GetLength() && !intersectionFound; i++)
	{
		int chunkIndex = gridCell->residentChunkIndexes[i];
		if (chunkIndex != ignoreChunkIndex)
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
					break;
				}
			}
		}
	}

	return intersectionFound;
}

bool CollisionMesh::FindNearestLineIntersectionWithChunksInGridCell(Vec3* outIntersection, MeshChunkFaceIndex* outChunkFaceIndex, CollisionLine* line, int gridCellIndex, int ignoreChunkIndex)
{
	bool intersectionFound = false;
	Vec3 nearestIntersection;
	float nearestIntersectionDistanceSqr = -1;

	CollisionMeshGridCell* gridCell = &this->gridCells[gridCellIndex];

	for (int i = 0; i < gridCell->residentChunkIndexes.GetLength() && !intersectionFound; i++)
	{
		int chunkIndex = gridCell->residentChunkIndexes[i];
		if (chunkIndex != ignoreChunkIndex)
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
					float distanceSqr = Vec3::DistanceSqr(&line->from, &faceIntersectionPoint);

					if (nearestIntersectionDistanceSqr == -1 || distanceSqr < nearestIntersectionDistanceSqr)
					{
						nearestIntersection = faceIntersectionPoint;
						nearestIntersectionDistanceSqr = distanceSqr;
						
						outChunkFaceIndex->chunkIndex = chunkIndex;
						outChunkFaceIndex->faceIndex = faceIndex;
						
						intersectionFound = true;
					}
				}
			}
		}
	}

	if (intersectionFound)
	{
		*outIntersection = nearestIntersection;
	}

	return intersectionFound;
}