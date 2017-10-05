#include "Assets/StaticMeshAsset.h"

StaticMeshAsset::StaticMeshAsset(const char* filePath, Buffer* fileData, bool isStayResident)
{
	IEngine* engine = GetEngine();
	IFactory* factory = GetFactory();
	//IRasterizer* rasterizer = engine->GetRasterizer();
	ITokenFileParser* parser = factory->MakeTokenFileParser(fileData);

	strcpy(this->filePath, filePath);
	this->isStayResident = isStayResident;
	this->collisionMesh = null;

	char token[256];
	//int numberOfIndecies = 0;
	//int numberOfVerts = 0;
	int numberOfChunks = 0;
	int numberOfMaterialAssetRefs = 0;
	//float* tempPositions = null;
	//float* tempMaterialTexCoords = null;
	//unsigned short* tempIndecies = null;

	this->numberOfVerts = 0;
	this->numberOfIndecies = 0;
	this->numberOfFaces = 0;

	this->tempPositions = null;
	this->tempMaterialTexCoords = null;
	this->tempIndecies = null;

	this->bufferAreInitialised = false;
	this->collisionMeshIsInitialised = false;

	while (!parser->GetIsEOF())
	{
		parser->ReadString(token, 256);

		if (strcmp(token, "number-of-verts") == 0)
		{
			this->numberOfVerts = parser->ReadInt();
		}
		else if (strcmp(token, "number-of-indecies") == 0)
		{
			this->numberOfIndecies = parser->ReadInt();
			this->numberOfFaces = this->numberOfIndecies / 3;
		}
		else if (strcmp(token, "number-of-chunks") == 0)
		{
			numberOfChunks = parser->ReadInt();
		}
		else if (strcmp(token, "number-of-material-asset-refs") == 0)
		{
			numberOfMaterialAssetRefs = parser->ReadInt();
		}
		else if (strcmp(token, "positions") == 0)
		{
			int numberOfPositonFloats = this->numberOfVerts * 3;
			this->tempPositions = new float[numberOfPositonFloats];

			for (int i = 0; i < numberOfPositonFloats; i++)
			{
				this->tempPositions[i] = parser->ReadFloat();
			}
		}
		else if (strcmp(token, "material-tex-coords") == 0)
		{
			int numberOfMaterialTexCoordFloats = this->numberOfVerts * 2;
			this->tempMaterialTexCoords = new float[numberOfMaterialTexCoordFloats];

			for (int i = 0; i < numberOfMaterialTexCoordFloats; i++)
			{
				this->tempMaterialTexCoords[i] = parser->ReadFloat();
			}
		}
		else if (strcmp(token, "indecies") == 0)
		{
			this->tempIndecies = new unsigned short[this->numberOfIndecies];

			for (int i = 0; i < this->numberOfIndecies; i++)
			{
				this->tempIndecies[i] = parser->ReadInt();
			}
		}
		else if (strcmp(token, "chunks") == 0)
		{
			for (int i = 0; i < numberOfChunks; i++)
			{
				StaticMeshChunk* chunk = &this->chunks.PushAndGet();
				chunk->startIndex = parser->ReadInt();
				chunk->numberOfFaces = parser->ReadInt();
				chunk->materialAssetRefIndex = parser->ReadInt();
			}
		}
		else if (strcmp(token, "material-asset-refs") == 0)
		{
			for (int i = 0; i < numberOfMaterialAssetRefs; i++)
			{
				AssetRef* assetRef = &this->materialAssetRefs.PushAndGet();
				parser->ReadAssetRef(assetRef, AssetTypeMaterial);
			}
		}
	}

	SafeDeleteAndNull(parser);
	//SafeDeleteArrayAndNull(tempPositions);
	//SafeDeleteArrayAndNull(tempMaterialTexCoords);
	//SafeDeleteArrayAndNull(tempIndecies);

	this->loadedSuccessfully = true;
}

StaticMeshAsset::~StaticMeshAsset()
{
	IEngine* engine = GetEngine();
	IRasterizer* rasterizer = engine->GetRasterizer();

	rasterizer->DeleteVertexBuffer(&this->buffers.positionBufferId);
	rasterizer->DeleteVertexBuffer(&this->buffers.materialTexCoordBufferId);

	SafeDeleteAndNull(this->collisionMesh);
	SafeDeleteArrayAndNull(this->tempPositions);
	SafeDeleteArrayAndNull(this->tempMaterialTexCoords);
	SafeDeleteArrayAndNull(this->tempIndecies);
}

/*void StaticMeshAsset::BeginLoadingReferencedAssets()
{
	IEngine* engine = GetEngine();
	IAssetManager* assetManager = engine->GetAssetManager();

	for (int i = 0; i < this->materialAssetRefs.GetLength(); i++)
	{
		AssetRef* assetRef = &this->materialAssetRefs[i];
		assetManager->BeginLoadingAsset(assetRef, this->isStayResident, false);
	}
}*/

bool StaticMeshAsset::ResolveReferencedAssets()
{
	IFactory* factory = GetFactory();
	IEngine* engine = GetEngine();
	IAssetManager* assetManager = engine->GetAssetManager();
	IRasterizer* rasterizer = engine->GetRasterizer();

	bool success = true;

	for (int i = 0; i < this->materialAssetRefs.GetLength(); i++)
	{
		AssetRef* assetRef = &this->materialAssetRefs[i];
		success &= assetManager->ResolveAssetRefIndex(assetRef);
	}

	// Create the buffers.
	if (!this->bufferAreInitialised)
	{
		this->buffers.positionBufferId = rasterizer->CreateVertexBuffer(this->numberOfVerts * 3 * sizeof(float), this->tempPositions);
		this->buffers.materialTexCoordBufferId = rasterizer->CreateVertexBuffer(numberOfVerts * 2 * sizeof(float), this->tempMaterialTexCoords);
		this->buffers.indexBufferId = rasterizer->CreateIndexBuffer(this->numberOfIndecies * sizeof(unsigned short), this->tempIndecies);

		this->bufferAreInitialised = true;
	}

	// Create the collision mesh.
	if (!this->collisionMeshIsInitialised)
	{
		this->collisionMesh = factory->MakeCollisionMesh();
		this->collisionMesh->Allocate(this->chunks.GetLength(), this->numberOfFaces);

		for (int chunkIndex = 0; chunkIndex < this->chunks.GetLength(); chunkIndex++)
		{
			StaticMeshChunk* chunk = &this->chunks[chunkIndex];
			AssetRef* materialAssetRef = &this->materialAssetRefs[chunk->materialAssetRefIndex];
			this->collisionMesh->PushChunk(chunk->startIndex, chunk->numberOfFaces, materialAssetRef->index, this->tempPositions, this->tempIndecies);
		}

		this->collisionMeshIsInitialised = true;
	}

	SafeDeleteArrayAndNull(this->tempPositions);
	SafeDeleteArrayAndNull(this->tempMaterialTexCoords);
	SafeDeleteArrayAndNull(this->tempIndecies);

	return success;
}

char* StaticMeshAsset::GetFilePath()
{
	return this->filePath;
}

bool StaticMeshAsset::GetIsStayResident()
{
	return this->isStayResident;
}

bool StaticMeshAsset::GetLoadedSuccessfully()
{
	return this->loadedSuccessfully;
}

bool StaticMeshAsset::GetIsEvictable()
{
	return this->isEvictable;
}

void StaticMeshAsset::SetIsEvictable(bool isEvictable)
{
	this->isEvictable = isEvictable;
}

StaticMeshBuffers* StaticMeshAsset::GetBuffers()
{
	return &this->buffers;
}

StaticMeshChunk* StaticMeshAsset::GetChunk(int index)
{
	return &this->chunks[index];
}

int StaticMeshAsset::GetNumberOfChunks()
{
	return this->chunks.GetLength();
}

AssetRef* StaticMeshAsset::GetMaterialAssetRef(int index)
{
	return &this->materialAssetRefs[index];
}

int StaticMeshAsset::GetNumberOfMaterialAssetRefs()
{
	return this->materialAssetRefs.GetLength();
}

ICollisionMesh* StaticMeshAsset::GetCollisionMesh()
{
	return this->collisionMesh;
}