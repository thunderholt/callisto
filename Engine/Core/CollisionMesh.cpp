#include "Core/CollisionMesh.h"

CollisionMesh::CollisionMesh()
{
	this->chunks = null;
	this->numberOfChunks = 0;
	this->nextChunkIndex = 0;
	this->faces = null;
	this->numberOfFaces = 0;
	this->nextFaceIndex = 0;
}

CollisionMesh::~CollisionMesh()
{
	SafeDeleteArrayAndNull(this->chunks);
	SafeDeleteArrayAndNull(this->faces);
}

/*void CollisionMesh::PopulateFromMeshData(float* positions, unsigned short* indecies, MeshChunk* chunks, int numberOfChunks)
{
	SafeDeleteArrayAndNull(this->chunks);
	SafeDeleteArrayAndNull(this->faces);

	this->chunks = new MeshChunk[numberOfChunks];
	memcpy(&this->chunks, chunks, sizeof(MeshChunk) * numberOfChunks);

	for (int chunkIndex = 0; chunkIndex < numberOfChunks; chunkIndex++)
	{
		MeshChunk* chunk = &chunks[chunkIndex];
		this->numberOfFaces += chunk->numberOfFaces;
	}

	this->faces = new CollisionFace[this->numberOfFaces];

	int nextFaceIndex = 0;
	for (int chunkIndex = 0; chunkIndex < numberOfChunks; chunkIndex++) 
	{
		MeshChunk* chunk = &chunks[chunkIndex];

		for (int i = chunk->startIndex; i < chunk->startIndex + chunk->numberOfFaces * 3; i += 3) 
		{
			unsigned short vert0StartIndex = indecies[i] * 3;
			unsigned short vert1StartIndex = indecies[i + 1] * 3;
			unsigned short vert2StartIndex = indecies[i + 2] * 3;

			Vec3 facePoints[3];
			Vec3::Set(&facePoints[0], positions[vert0StartIndex], positions[vert0StartIndex + 1], positions[vert0StartIndex + 2]);
			Vec3::Set(&facePoints[1], positions[vert1StartIndex], positions[vert1StartIndex + 1], positions[vert1StartIndex + 2]);
			Vec3::Set(&facePoints[2], positions[vert2StartIndex], positions[vert2StartIndex + 1], positions[vert2StartIndex + 2]);
			
			CollisionFace* face = &this->faces[nextFaceIndex++];
			CollisionFace::BuildFromPoints(face, facePoints);
		}
	}
}*/

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

void CollisionMesh::PushChunk(int startIndex, int numberOfFaces, int materialAssetIndex, float* positions, unsigned short* indecies)
{
	CollisionMeshChunk* chunk = &this->chunks[this->nextChunkIndex++];
	chunk->startFaceIndex = this->nextFaceIndex;
	chunk->numberOfFaces = numberOfFaces;
	chunk->materialAssetIndex = materialAssetIndex;

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
}

bool CollisionMesh::FindNearestRayIntersection(CollisionMeshIntersectionResult* out, Ray3* ray)
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
}

bool CollisionMesh::FindNearestLineIntersection(CollisionMeshIntersectionResult* out, CollisionLine* line)
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
}

bool CollisionMesh::FindNearestSphereCollision(CollisionMeshIntersectionResult* out, Sphere* sphere, Vec3* movementDirection)
{
	FaceCollisionResult nearestFaceCollisionResult;
	int nearestFaceIntersectionChunkIndex = -1;
	int nearestFaceIntersectionFaceIndex = -1;
	bool collisionFound = false;

	memset(&nearestFaceCollisionResult, 0, sizeof(FaceCollisionResult));

	// TODO - don't check all the faces, use sectors or something!
	for (int chunkIndex = 0; chunkIndex < this->numberOfChunks; chunkIndex++)
	{
		CollisionMeshChunk* chunk = &this->chunks[chunkIndex];

		for (int faceIndex = chunk->startFaceIndex; faceIndex < chunk->startFaceIndex + chunk->numberOfFaces; faceIndex++)
		{
			CollisionFace* face = &this->faces[faceIndex];

			FaceCollisionResult collisionResult;
			bool collides = Sphere::CalculateCollisionWithCollisionFace(
				&collisionResult, sphere, face, movementDirection);

			if (collides)
			{
				if (!collisionFound || collisionResult.distance < nearestFaceCollisionResult.distance)
				{
					collisionFound = true;
					nearestFaceIntersectionChunkIndex = chunkIndex;
					nearestFaceIntersectionFaceIndex = faceIndex;
					nearestFaceCollisionResult = collisionResult;
				}
			}
		}
	}
	
	if (collisionFound && out != null)
	{
		CollisionFace* face = &this->faces[nearestFaceIntersectionFaceIndex];

		out->intersectionPoint = nearestFaceCollisionResult.intersection;
		out->intersectionPlane = nearestFaceCollisionResult.collisionPlane;
		out->chunkIndex = nearestFaceIntersectionChunkIndex;
		out->faceIndex = nearestFaceIntersectionFaceIndex;
		out->distance = nearestFaceCollisionResult.distance;
	}

	return collisionFound;
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