#pragma once
#include "Include/Callisto.h"

class ActorInstance : public IActorInstance
{
public:
	ActorInstance();
	virtual ~ActorInstance();
	virtual AssetRef* GetActorAssetRef();
	virtual char* GetName();
	virtual Vec3* GetWorldPosition();
	virtual Vec3* GetWorldRotation();
	virtual IJsonValue* GetJsonConfig();
	virtual void LoadFromJson(IJsonValue* jsonActorInstance);

private:
	AssetRef actorAssetRef;
	char name[ActorMaxNameLength];
	Vec3 worldPosition;
	Vec3 worldRotation;
	IJsonValue* jsonConfig;
};