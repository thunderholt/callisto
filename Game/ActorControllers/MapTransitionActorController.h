#pragma once

#include "Include/Callisto.h"
#include "Game.h"

class MapTransitionActorController : public IActorController
{
public:
	MapTransitionActorController();
	virtual ~MapTransitionActorController();
	virtual void OneTimeInit(IActor* actor, IJsonValue* actorAssetJsonConfig, IJsonValue* actorInstanceJsonConfig);
	virtual void Activate(IActor* actor);
	virtual void Deactivate(IActor* actor);
	virtual void Heartbeat(IActor* actor);
	virtual void Tell(IActor* actor, int messageId, void* data);
	//virtual void UpdateDebugVisualisations(IActor* actor);
	virtual char* GetControllerName();
	virtual int GetActorType();

private:
	void ApplyJsonConfig(IActor* actor, IJsonValue* jsonConfig);

	char name[ActorControllerMaxNameLength];
};

class MapTransitionActorControllerData : public IActorControllerData
{
public:
	virtual void* GetFacet(int facetId);

	int triggerSphereSubActorIndex;
	char destinationMapFilePath[AssetMaxFilePathLength];
};
