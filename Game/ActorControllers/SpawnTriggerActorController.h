#pragma once

#include "Include/Callisto.h"
#include "Game.h"

class SpawnTriggerActorController : public IActorController
{
public:
	SpawnTriggerActorController();
	virtual ~SpawnTriggerActorController();
	virtual void OneTimeInit(IActor* actor, IJsonValue* actorAssetJsonConfig, IJsonValue* actorInstanceJsonConfig);
	virtual void Activate(IActor* actor);
	virtual void Deactivate(IActor* actor);
	virtual void Heartbeat(IActor* actor);
	virtual void Tell(IActor* actor, int messageId, void* data);
	virtual char* GetControllerName();
	virtual int GetActorType();

private:
	void ApplyJsonConfig(IActor* actor, IJsonValue* jsonConfig);
	void HandleCompletedEncounter(IActor* actor, Vec3* lastParticipantWorldPosition);

	char name[ActorControllerMaxNameLength];
};

class SpawnTriggerActorControllerData : public IActorControllerData
{
public:
	SpawnTriggerActorControllerData();
	~SpawnTriggerActorControllerData();
	virtual void* GetFacet(int facetId);
	
	EncounterActorFacet encounterFacet;

	int triggerSphereSubActorIndex;
	DynamicLengthArray<int> spawnPointActorIndexes;
	int rewardDropActorAssetIndex;
	IJsonValue* jsonRewardDropActorConfig;
	bool hasBeenTriggered;
};
