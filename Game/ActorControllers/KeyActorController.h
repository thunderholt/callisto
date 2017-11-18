#pragma once

#include "Include/Callisto.h"
#include "Game.h"

class KeyActorController : public IActorController
{
public:
	KeyActorController();
	virtual ~KeyActorController();
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

	char name[ActorControllerMaxNameLength];
};

class KeyActorControllerData : public IActorControllerData
{
public:
	KeyActorControllerData();
	virtual void* GetFacet(int facetId);

	int triggerSphereSubActorIndex;
	KeyType keyType;
};
