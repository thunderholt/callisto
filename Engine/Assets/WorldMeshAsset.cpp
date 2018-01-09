#include "Assets/WorldMeshAsset.h"

WorldMeshAsset::WorldMeshAsset(const char* filePath, Buffer* fileData, bool isStayResident)
{
	IEngine* engine = GetEngine();
	IFactory* factory = GetFactory();
	ITokenFileParser* parser = factory->MakeTokenFileParser(fileData);

	strcpy(this->filePath, filePath);
	this->isStayResident = isStayResident;
	this->collisionMesh = null;

	char token[256];
	//int numberOfIndecies = 0;
	//int numberOfVerts = 0;
	int numberOfChunks = 0;
	int numberOfMaterialAssetRefs = 0;
	//int numberOfLightAtlasTextureAssetRefs = 0;
	int numberOfLightAtlases = 0;

	this->numberOfVerts = 0;
	this->numberOfIndecies = 0;
	this->numberOfFaces = 0;

	this->tempPositions = null;
	this->tempMaterialTexCoords = null;
	this->tempLightAtlasTexCoords = null;
	this->tempIndecies = null;

	this->bufferAreInitialised = false;
	this->collisionMeshIsInitialised = false;
	//memset(&this->lightAtlasTextureAssetRef, 0, sizeof(AssetRef));
	memset(&this->pvsAssetRef, 0, sizeof(AssetRef));
	this->pvsAssetRef.index = -1;

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
		/*else if (strcmp(token, "number-of-light-atlas-texture-asset-refs") == 0)
		{
			numberOfLightAtlasTextureAssetRefs = parser->ReadInt();
		}*/
		else if (strcmp(token, "number-of-light-atlases") == 0)
		{
			numberOfLightAtlases = parser->ReadInt();
		}
		else if (strcmp(token, "pvs-asset-ref") == 0)
		{
			parser->ReadAssetRef(&this->pvsAssetRef, AssetTypePVS);
		}
		else if (strcmp(token, "positions") == 0)
		{
			int numberOfPositonFloats = this->numberOfVerts * 3;
			this->tempPositions = new float[numberOfPositonFloats];

			for (int i = 0; i < numberOfPositonFloats; i++)
			{
				tempPositions[i] = parser->ReadFloat();
			}
		}
		else if (strcmp(token, "material-tex-coords") == 0)
		{
			int numberOfMaterialTexCoordFloats = this->numberOfVerts * 2;
			this->tempMaterialTexCoords = new float[numberOfMaterialTexCoordFloats];

			for (int i = 0; i < numberOfMaterialTexCoordFloats; i++)
			{
				tempMaterialTexCoords[i] = parser->ReadFloat();
			}
		}
		else if (strcmp(token, "light-atlas-tex-coords") == 0)
		{
			int numberOfLightAtlasTexCoordFloats = this->numberOfVerts * 2;
			this->tempLightAtlasTexCoords = new float[numberOfLightAtlasTexCoordFloats];

			for (int i = 0; i < numberOfLightAtlasTexCoordFloats; i++)
			{
				tempLightAtlasTexCoords[i] = parser->ReadFloat();
			}
		}
		else if (strcmp(token, "indecies") == 0)
		{
			this->tempIndecies = new unsigned short[this->numberOfIndecies];

			for (int i = 0; i < numberOfIndecies; i++)
			{
				this->tempIndecies[i] = parser->ReadInt();
			}
		}
		else if (strcmp(token, "chunks") == 0)
		{
			for (int i = 0; i < numberOfChunks; i++)
			{
				WorldMeshChunk* chunk = &this->chunks.PushAndGet();
				chunk->startIndex = parser->ReadInt();
				chunk->numberOfFaces = parser->ReadInt();
				chunk->materialAssetRefIndex = parser->ReadInt();
				chunk->lightAtlasIndex = parser->ReadInt();
				//chunk->lightAtlasTextureAssetRefIndex = 0; // FIXME
				//parser->ReadVec2i(&chunk->lightIslandOffset);
				//parser->ReadVec2i(&chunk->lightIslandSize);
				chunk->lastRenderedFrameId = -1; // FIXME - can this be replaced by a bit field?
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
		/*else if (strcmp(token, "light-atlas-texture-asset-ref") == 0)
		{
			parser->ReadAssetRef(&this->lightAtlasTextureAssetRef, AssetTypeTexture);
		}*/
		else if (strcmp(token, "light-atlas-texture-asset-refs") == 0)
		{
			for (int i = 0; i < numberOfLightAtlases; i++)
			{
				AssetRef* assetRef = &this->lightAtlasTextureAssetRefs.PushAndGet();
				parser->ReadAssetRef(assetRef, AssetTypeTexture);
			}
		}
	}

	SafeDeleteAndNull(parser);

	this->loadedSuccessfully = true;
}

WorldMeshAsset::~WorldMeshAsset()
{
	IEngine* engine = GetEngine();
	IRasterizer* rasterizer = engine->GetRasterizer();

	rasterizer->DeleteVertexBuffer(&this->buffers.positionBufferId);
	rasterizer->DeleteVertexBuffer(&this->buffers.materialTexCoordBufferId);
	rasterizer->DeleteVertexBuffer(&this->buffers.lightAtlasTexCoordBufferId);
	rasterizer->DeleteIndexBuffer(&this->buffers.indexBuffer);

	SafeDeleteAndNull(this->collisionMesh);
	SafeDeleteArrayAndNull(this->tempPositions);
	SafeDeleteArrayAndNull(this->tempMaterialTexCoords);
	SafeDeleteArrayAndNull(this->tempLightAtlasTexCoords);
	SafeDeleteArrayAndNull(this->tempIndecies);
}

bool WorldMeshAsset::ResolveReferencedAssets()
{
	IFactory* factory = GetFactory();
	IEngine* engine = GetEngine();
	IAssetManager* assetManager = engine->GetAssetManager();
	IRasterizer* rasterizer = engine->GetRasterizer();

	bool success = true;

	// Resolve the material asset refs.
	for (int i = 0; i < this->materialAssetRefs.GetLength(); i++)
	{
		AssetRef* assetRef = &this->materialAssetRefs[i];
		success &= assetManager->ResolveAssetRefIndex(assetRef);
	}

	// Resolve the light atlas texture asset refs.
	for (int i = 0; i < this->lightAtlasTextureAssetRefs.GetLength(); i++)
	{
		AssetRef* assetRef = &this->lightAtlasTextureAssetRefs[i];
		success &= assetManager->ResolveAssetRefIndex(assetRef);
	}

	// Resolve the light atlas texture asset ref.
	//success &= assetManager->ResolveAssetRefIndex(&this->lightAtlasTextureAssetRef);

	// Resolve the PVS asset ref.
	if (strlen(this->pvsAssetRef.filePath) > 0)
	{
		success &= assetManager->ResolveAssetRefIndex(&this->pvsAssetRef);
	}

	// Create the buffers.
	if (!this->bufferAreInitialised)
	{
		this->buffers.positionBufferId = rasterizer->CreateVertexBuffer(numberOfVerts * 3 * sizeof(float), tempPositions);
		this->buffers.materialTexCoordBufferId = rasterizer->CreateVertexBuffer(numberOfVerts * 2 * sizeof(float), tempMaterialTexCoords);
		this->buffers.lightAtlasTexCoordBufferId = rasterizer->CreateVertexBuffer(numberOfVerts * 2 * sizeof(float), tempLightAtlasTexCoords);
		this->buffers.indexBuffer = rasterizer->CreateIndexBuffer(numberOfIndecies * sizeof(unsigned short), tempIndecies);

		this->bufferAreInitialised = true;
	}

	// Create the collision mesh.
	if (!this->collisionMeshIsInitialised)
	{
		this->collisionMesh = factory->MakeCollisionMesh();
		this->collisionMesh->Allocate(this->chunks.GetLength(), this->numberOfFaces);

		for (int chunkIndex = 0; chunkIndex < this->chunks.GetLength(); chunkIndex++)
		{
			WorldMeshChunk* chunk = &this->chunks[chunkIndex];
			AssetRef* materialAssetRef = &this->materialAssetRefs[chunk->materialAssetRefIndex];
			this->collisionMesh->PushChunk(chunk->startIndex, chunk->numberOfFaces, materialAssetRef->index, this->tempPositions, this->tempIndecies);
		}

		this->collisionMeshIsInitialised = true;
	}

	SafeDeleteArrayAndNull(this->tempPositions);
	SafeDeleteArrayAndNull(this->tempMaterialTexCoords);
	SafeDeleteArrayAndNull(this->tempLightAtlasTexCoords);
	SafeDeleteArrayAndNull(this->tempIndecies);

	return success;
}

char* WorldMeshAsset::GetFilePath()
{
	return this->filePath;
}

bool WorldMeshAsset::GetIsStayResident()
{
	return this->isStayResident;
}

bool WorldMeshAsset::GetLoadedSuccessfully()
{
	return this->loadedSuccessfully;
}

bool WorldMeshAsset::GetIsEvictable()
{
	return this->isEvictable;
}

void WorldMeshAsset::SetIsEvictable(bool isEvictable)
{
	this->isEvictable = isEvictable;
}

WorldMeshBuffers* WorldMeshAsset::GetBuffers()
{
	return &this->buffers;
}

WorldMeshChunk* WorldMeshAsset::GetChunk(int index)
{
	WorldMeshChunk* chunk = null;

	if (index >= 0 && index < this->chunks.GetLength())
	{
		chunk = &this->chunks[index];
	}

	return chunk;
}

int WorldMeshAsset::GetNumberOfChunks()
{
	return this->chunks.GetLength();
}

AssetRef* WorldMeshAsset::GetMaterialAssetRef(int index)
{
	return &this->materialAssetRefs[index];
}

int WorldMeshAsset::GetNumberOfMaterialAssetRefs()
{
	return this->materialAssetRefs.GetLength();
}

/*AssetRef* WorldMeshAsset::GetLightAtlasTextureAssetRef()
{
	return &this->lightAtlasTextureAssetRef;
}*/

AssetRef* WorldMeshAsset::GetLightAtlasTextureAssetRef(int index)
{
	return &this->lightAtlasTextureAssetRefs[index];
}

int WorldMeshAsset::GetNumberOfLightAtlasTextureAssetRefs()
{
	return this->lightAtlasTextureAssetRefs.GetLength();
}

AssetRef* WorldMeshAsset::GetPVSAssetRef()
{
	return &this->pvsAssetRef;
}

ICollisionMesh* WorldMeshAsset::GetCollisionMesh()
{
	return this->collisionMesh;
}