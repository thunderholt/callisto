#include "Core/WorldMeshAsset.h"

WorldMeshAsset::WorldMeshAsset()
{
	this->collisionMesh = null;
	this->lightAtlases = null;
	this->numberOfLightAtlases = 0;
	this->lightIslands = null;
	this->numberOfLightIslands = 0;
	this->lights = null;
	this->numberOfLights = 0;
}

WorldMeshAsset::~WorldMeshAsset()
{
	SafeDeleteAndNull(this->collisionMesh);
	SafeDeleteArrayAndNull(this->lightAtlases);
	SafeDeleteArrayAndNull(this->lightIslands);
	SafeDeleteArrayAndNull(this->lights);
}

bool WorldMeshAsset::Load(const char* filePath)
{
	IFactory* factory = GetFactory();
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	bool success = false;
	Buffer fileData = { 0 };
	Vec3* tempPositions = null;
	Vec3* tempNormals = null;
	Vec2* tempMaterialTexCoords = null;
	Vec2* tempLightAtlasTexCoords = null;
	unsigned short* tempIndecies = null;
	WorldMeshChunk* tempChunks = null;
	Vec3 gridOrigin;
	Vec3::Zero(&gridOrigin);
	Vec3i gridDimensions;
	Vec3i::Zero(&gridDimensions);
	float gridCellSize = 0;

	IFile* file = factory->MakeFile();
	if (!file->OpenForReading(filePath))
	{
		logger->Write("Failed to open world mesh file '%s'.", filePath);
	}
	else
	{
		fileData.length = file->GetLength();
		fileData.data = new char[fileData.length];
		int numberOfBytesRead = file->Read(fileData.data, fileData.length);

		if (numberOfBytesRead != fileData.length)
		{
			logger->Write("Unable to read file '%s'.", filePath);
		}
		else
		{
			ITokenFileParser* parser = factory->MakeTokenFileParser(&fileData);

			char token[256];
			int numberOfIndecies = 0;
			int numberOfVerts = 0;
			int numberOfFaces = 0;
			int numberOfChunks = 0;
			int numberOfMaterialAssetRefs = 0;
			DynamicLengthArray<AssetRef> materialAssetRefs;

			while (!parser->GetIsEOF())
			{
				parser->ReadString(token, 256);

				if (strcmp(token, "number-of-verts") == 0)
				{
					numberOfVerts = parser->ReadInt();
				}
				else if (strcmp(token, "number-of-indecies") == 0)
				{
					numberOfIndecies = parser->ReadInt();
					numberOfFaces = numberOfIndecies / 3;
				}
				else if (strcmp(token, "number-of-chunks") == 0)
				{
					numberOfChunks = parser->ReadInt();
				}
				else if (strcmp(token, "number-of-material-asset-refs") == 0)
				{
					numberOfMaterialAssetRefs = parser->ReadInt();
				}
				else if (strcmp(token, "number-of-lights") == 0)
				{
					this->numberOfLights = parser->ReadInt();
				}
				else if (strcmp(token, "number-of-light-atlases") == 0)
				{
					this->numberOfLightAtlases = parser->ReadInt();
				}
				else if (strcmp(token, "number-of-light-islands") == 0)
				{
					this->numberOfLightIslands = parser->ReadInt();
				}
				else if (strcmp(token, "grid-origin") == 0)
				{
					parser->ReadVec3(&gridOrigin);
				}
				else if (strcmp(token, "grid-dimensions") == 0)
				{
					parser->ReadVec3i(&gridDimensions);
				}
				else if (strcmp(token, "grid-cell-size") == 0)
				{
					gridCellSize = parser->ReadFloat();
				}
				else if (strcmp(token, "positions") == 0)
				{
					tempPositions = new Vec3[numberOfVerts];

					for (int i = 0; i < numberOfVerts; i++)
					{
						parser->ReadVec3(&tempPositions[i]);
					}
				}
				else if (strcmp(token, "normals") == 0)
				{
					tempNormals = new Vec3[numberOfVerts];

					for (int i = 0; i < numberOfVerts; i++)
					{
						parser->ReadVec3(&tempNormals[i]);
					}
				}
				else if (strcmp(token, "material-tex-coords") == 0)
				{
					tempMaterialTexCoords = new Vec2[numberOfVerts];

					for (int i = 0; i < numberOfVerts; i++)
					{
						parser->ReadVec2(&tempMaterialTexCoords[i]);
					}
				}
				else if (strcmp(token, "light-atlas-tex-coords") == 0)
				{
					tempLightAtlasTexCoords = new Vec2[numberOfVerts];

					for (int i = 0; i < numberOfVerts; i++)
					{
						parser->ReadVec2(&tempLightAtlasTexCoords[i]);
					}
				}
				else if (strcmp(token, "indecies") == 0)
				{
					tempIndecies = new unsigned short[numberOfIndecies];

					for (int i = 0; i < numberOfIndecies; i++)
					{
						tempIndecies[i] = parser->ReadInt();
					}
				}
				else if (strcmp(token, "chunks") == 0)
				{
					tempChunks = new WorldMeshChunk[numberOfChunks];

					for (int i = 0; i < numberOfChunks; i++)
					{
						WorldMeshChunk* chunk = &tempChunks[i];
						chunk->startIndex = parser->ReadInt();
						chunk->numberOfFaces = parser->ReadInt();
						chunk->materialAssetRefIndex = parser->ReadInt();
						chunk->lightAtlasIndex = parser->ReadInt();
					}
				}
				else if (strcmp(token, "material-asset-refs") == 0)
				{
					for (int i = 0; i < numberOfMaterialAssetRefs; i++)
					{
						AssetRef* assetRef = &materialAssetRefs.PushAndGet();
						parser->ReadAssetRef(assetRef);
					}
				}
				else if (strcmp(token, "light-atlases") == 0)
				{
					this->lightAtlases = new WorldMeshLightAtlas[this->numberOfLightAtlases];

					for (int i = 0; i < this->numberOfLightAtlases; i++)
					{
						WorldMeshLightAtlas* lightAtlas = &this->lightAtlases[i];
						parser->ReadVec2i(&lightAtlas->size);
					}
				}
				else if (strcmp(token, "light-islands") == 0)
				{
					this->lightIslands = new WorldMeshLightIsland[this->numberOfLightIslands];

					for (int i = 0; i < this->numberOfLightIslands; i++)
					{
						WorldMeshLightIsland* lightIsland = &this->lightIslands[i];
						lightIsland->lightAtlasIndex = parser->ReadInt();
						parser->ReadVec2i(&lightIsland->offset);
						parser->ReadVec2i(&lightIsland->size);
						lightIsland->chunkIndex = parser->ReadInt();
					}
				}
				else if (strcmp(token, "lights") == 0)
				{
					this->lights = new WorldMeshLight[this->numberOfLights];

					for (int i = 0; i < this->numberOfLights; i++)
					{
						WorldMeshLight* light = &this->lights[i];
				
						light->type = (LightType)parser->ReadInt();
						parser->ReadVec3(&light->position);
						//parser->ReadVec3(&light->rotation);
						parser->ReadVec3(&light->iAxis);
						parser->ReadVec3(&light->jAxis);
						parser->ReadVec3(&light->kAxis);
						light->distance = parser->ReadFloat();
						parser->ReadRgbFloat(&light->colour);
						parser->ReadVec2(&light->size);
					}
				}
			}

			SafeDeleteAndNull(parser);

			// Load the materials.
			IMaterialAsset** materialAssets = new IMaterialAsset*[numberOfMaterialAssetRefs];
			bool allMaterialsLoadedSuccessfully = true;

			for (int i = 0; i < numberOfMaterialAssetRefs; i++) 
			{
				AssetRef* assetRef = &materialAssetRefs[i];

				char fullMaterialAssetPath[512];
				strcpy(fullMaterialAssetPath, engine->GetAssetsFolderPath());
				strcat(fullMaterialAssetPath, assetRef->filePath);

				IMaterialAsset* materialAsset = factory->MakeMaterialAsset();
				allMaterialsLoadedSuccessfully &= materialAsset->Load(fullMaterialAssetPath);

				materialAssets[i] = materialAsset;
			}

			// Create the collision mesh.
			this->collisionMesh = factory->MakeCollisionMesh();
			this->collisionMesh->AllocateGeometry(numberOfChunks, numberOfFaces);
			this->collisionMesh->AllocateGrid(gridOrigin, gridDimensions, gridCellSize);

			for (int chunkIndex = 0; chunkIndex < numberOfChunks; chunkIndex++)
			{
				WorldMeshChunk* chunk = &tempChunks[chunkIndex];
				IMaterialAsset* materialAsset = materialAssets[chunk->materialAssetRefIndex];

				this->collisionMesh->PushChunk(
					chunk->startIndex, chunk->numberOfFaces, 
					tempPositions, tempNormals, 
					tempMaterialTexCoords, tempLightAtlasTexCoords, tempIndecies,
					chunk->lightAtlasIndex
					/*, materialAsset->GetStaticLightingDetails()*//*, , chunk->lightIslandOffset, chunk->lightIslandSize*/);
			}

			this->collisionMesh->Finish();

			// Clean up the material assets.
			for (int i = 0; i < numberOfMaterialAssetRefs; i++) 
			{
				SafeDeleteAndNull(materialAssets[i]);
			}
			SafeDeleteArrayAndNull(materialAssets);
			
			// We're done!
			if (allMaterialsLoadedSuccessfully)
			{
				success = true;
			}
		}
	}

	SafeDeleteAndNull(file);
	SafeDeleteArrayAndNull(fileData.data);
	SafeDeleteArrayAndNull(tempPositions);
	SafeDeleteArrayAndNull(tempNormals);
	SafeDeleteArrayAndNull(tempMaterialTexCoords);
	SafeDeleteArrayAndNull(tempLightAtlasTexCoords);
	SafeDeleteArrayAndNull(tempIndecies);
	SafeDeleteArrayAndNull(tempChunks);

	return success;
}

ICollisionMesh* WorldMeshAsset::GetCollisionMesh()
{
	return this->collisionMesh;
}

WorldMeshLightAtlas* WorldMeshAsset::GetLightAtlas(int lightAtlasIndex)
{
	return &this->lightAtlases[lightAtlasIndex];
}

int WorldMeshAsset::GetNumberOfLightAtlases()
{
	return this->numberOfLightAtlases;
}

WorldMeshLightIsland* WorldMeshAsset::GetLightIsland(int lightIslandIndex)
{
	return &this->lightIslands[lightIslandIndex];
}

WorldMeshLightIsland* WorldMeshAsset::FindLightIslandForChunk(int chunkIndex)
{
	WorldMeshLightIsland* lightIsland = null;

	for (int lightIslandIndex = 0; lightIslandIndex < this->numberOfLightIslands; lightIslandIndex++)
	{
		WorldMeshLightIsland* potentialLightIsland = &this->lightIslands[lightIslandIndex];
		if (potentialLightIsland->chunkIndex == chunkIndex)
		{
			lightIsland = potentialLightIsland;
		}
	}

	return lightIsland;
}

int WorldMeshAsset::GetNumberOfLightIslands()
{
	return this->numberOfLightIslands;
}

WorldMeshLight* WorldMeshAsset::GetLight(int lightIndex)
{
	return &this->lights[lightIndex];
}

int WorldMeshAsset::GetNumberOfLights()
{
	return this->numberOfLights;
}