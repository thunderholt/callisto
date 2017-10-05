#include "Core/WorldMeshAsset.h"

WorldMeshAsset::WorldMeshAsset()
{
	this->collisionMesh = null;
}

WorldMeshAsset::~WorldMeshAsset()
{
	SafeDeleteAndNull(this->collisionMesh);
}

bool WorldMeshAsset::Load(const char* filePath)
{
	IFactory* factory = GetFactory();
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	bool success = false;
	Buffer fileData;
	float* tempPositions = null;
	unsigned short* tempIndecies = null;
	WorldMeshChunk* tempChunks = null;

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
				else if (strcmp(token, "positions") == 0)
				{
					int numberOfPositonFloats = numberOfVerts * 3;
					tempPositions = new float[numberOfPositonFloats];

					for (int i = 0; i < numberOfPositonFloats; i++)
					{
						tempPositions[i] = parser->ReadFloat();
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
					}
				}
			}

			SafeDeleteAndNull(parser);

			// Create the collision mesh.
			this->collisionMesh = factory->MakeCollisionMesh();
			this->collisionMesh->Allocate(numberOfChunks, numberOfFaces);

			for (int chunkIndex = 0; chunkIndex < numberOfChunks; chunkIndex++)
			{
				WorldMeshChunk* chunk = &tempChunks[chunkIndex];
				this->collisionMesh->PushChunk(chunk->startIndex, chunk->numberOfFaces, tempPositions, tempIndecies);
			}

			this->collisionMesh->Finish();

			// We're done!
			success = true;
		}
	}

	SafeDeleteAndNull(file);
	SafeDeleteArrayAndNull(fileData.data);
	SafeDeleteArrayAndNull(tempPositions);
	SafeDeleteArrayAndNull(tempIndecies);
	SafeDeleteArrayAndNull(tempChunks);

	return success;
}

ICollisionMesh* WorldMeshAsset::GetCollisionMesh()
{
	return this->collisionMesh;
}