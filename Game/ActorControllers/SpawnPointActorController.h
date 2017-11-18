#pragma once

#include "Include/Callisto.h"
#include "Game.h"

class SpawnPointActorController : public IActorController
{
public:
	SpawnPointActorController();
	virtual ~SpawnPointActorController();
	virtual void OneTimeInit(IActor* actor, IJsonValue* actorAssetJsonConfig, IJsonValue* actorInstanceJsonConfig);
	virtual void ReInit(IActor* actor, IJsonValue* actorInstanceJsonConfig);
	virtual void Activate(IActor* actor);
	virtual void Deactivate(IActor* actor);
	virtual void Heartbeat(IActor* actor);
	virtual void Tell(IActor* actor, int messageId, void* data);
	virtual char* GetControllerName();
	virtual int GetActorType();

private:
	void ApplyJsonConfig(IActor* actor, IJsonValue* jsonConfig);
	void Spawn(IActor* actor, int spawnTriggerActorIndex);

	char name[ActorControllerMaxNameLength];
};

class SpawnPointActorControllerData : public IActorControllerData
{
public:
	virtual void* GetFacet(int facetId);
	int spawneeActorAssetIndex;
};
