#pragma once

#include "Include/LightAtlasBuilder.h"

class Factory : public IFactory
{
public:
	virtual ~Factory();
	virtual IFile* MakeFile();
	virtual ILogger* MakeLogger();
	virtual ITokenFileParser* MakeTokenFileParser(Buffer* fileData);
	virtual IWorldMeshAsset* MakeWorldMeshAsset();
	virtual IMaterialAsset* MakeMaterialAsset();
	virtual ICollisionMesh* MakeCollisionMesh();
	virtual IWorker* MakeWorker();
	virtual ILightAtlas* MakeLightAtlas();
	virtual IThreadManager* MakeThreadManager();
	virtual ITimestampProvider* MakeTimestampProvider();
	virtual IJsonValue* MakeJsonValue();
	virtual IJsonProperty* MakeJsonProperty();
	virtual IJsonParser* MakeJsonParser();
};