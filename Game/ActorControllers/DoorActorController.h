#pragma once

#include "Include/Callisto.h"
#include "Game.h"

class DoorActorController : public IActorController
{
public:
	DoorActorController();
	virtual ~DoorActorController();
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
	//bool DetermineIfPlayerIsResidentInTriggerSphere(IActor* actor);

	char name[ActorControllerMaxNameLength];
};

enum DoorActorState
{
	DoorActorStateClosed,
	DoorActorStateOpened,
	DoorActorStateOpening,
	DoorActorStateClosing
};

class DoorActorControllerData : public IActorControllerData
{
public:
	virtual void* GetFacet(int facetId);

	DoorActorState state;
	bool isLocked;
	KeyType requiredKeyType;
	int doorMeshSubActorIndex;
	int triggerSphereSubActorIndex;
};
