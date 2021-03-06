#pragma once

#include "Include/PVSBuilder.h"

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
	virtual ISectorVisibilityLookup* MakeSectorVisibilityLookup();
	//virtual ISectorCruncher* MakeOcclusionSectorCruncher();
	virtual ISectorCruncher* MakeBruteForceSectorCruncher();
	virtual IWorker* MakeWorker();
	virtual IThreadManager* MakeThreadManager();
	virtual ITimestampProvider* MakeTimestampProvider();
	virtual IJsonValue* MakeJsonValue();
	virtual IJsonProperty* MakeJsonProperty();
	virtual IJsonParser* MakeJsonParser();
};