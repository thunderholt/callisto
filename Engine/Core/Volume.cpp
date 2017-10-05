#include "Core/Volume.h"

Volume::Volume()
{
	memset(&this->spec, 0, sizeof(VolumeSpec));
	memset(&this->buffers, 0, sizeof(VolumeBuffers));
	this->collisionMesh = null;
	this->numberOfFaces = 0;
}

Volume::~Volume()
{
	IEngine* engine = GetEngine();
	IRasterizer* rasterizer = engine->GetRasterizer();

	rasterizer->DeleteVertexBuffer(&this->buffers.positionBufferId);
	rasterizer->DeleteIndexBuffer(&this->buffers.indexBufferId);

	SafeDeleteAndNull(this->collisionMesh);
}

void Volume::Populate(VolumeSpec* spec, VolumeData* volumeData)
{
	IFactory* factory = GetFactory();
	IEngine* engine = GetEngine();
	IRasterizer* rasterizer = engine->GetRasterizer();

	// Copty the details.
	this->spec = *spec;
	this->numberOfFaces = volumeData->numberOfFaces;

	// Create the buffers.
	this->buffers.positionBufferId = rasterizer->CreateVertexBuffer(volumeData->numberOfPositions * 3 * sizeof(float), (float*)volumeData->positions);
	this->buffers.indexBufferId = rasterizer->CreateIndexBuffer(volumeData->numberOfIndecies * sizeof(unsigned short), volumeData->indecies);

	// Create the collision mesh.
	if (this->collisionMesh == null)
	{
		this->collisionMesh = factory->MakeCollisionMesh();
	}

	// Populate the collision mesh.
	this->collisionMesh->Allocate(1, volumeData->numberOfFaces);
	this->collisionMesh->PushChunk(0, volumeData->numberOfFaces, -1, (float*)volumeData->positions, volumeData->indecies);

	/*MeshChunk meshChunk;
	meshChunk.startIndex = 0;
	meshChunk.numberOfFaces = volumeData->numberOfFaces;
	meshChunk.materialAssetRefIndex = -1;

	this->collisionMesh->PopulateFromMeshData(
		(float*)volumeData->positions,
		volumeData->indecies,
		&meshChunk, 1);*/
}

VolumeSpec* Volume::GetSpec()
{
	return &this->spec;
}

VolumeBuffers* Volume::GetBuffers()
{
	return &this->buffers;
}

ICollisionMesh* Volume::GetCollisionMesh()
{
	return this->collisionMesh;
}

int Volume::GetNumberOfFaces()
{
	return this->numberOfFaces;
}