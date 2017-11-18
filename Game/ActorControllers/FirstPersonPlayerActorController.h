#pragma once

#include "Include/Callisto.h"
#include "Game.h"

class FirstPersonPlayerActorController : public IActorController
{
public:
	FirstPersonPlayerActorController();
	virtual ~FirstPersonPlayerActorController();
	virtual void OneTimeInit(IActor* actor, IJsonValue* actorAssetJsonConfig, IJsonValue* actorInstanceJsonConfig);
	virtual void ReInit(IActor* actor, IJsonValue* actorInstanceJsonConfig);
	virtual void Activate(IActor* actor);
	virtual void Deactivate(IActor* actor);
	virtual void Heartbeat(IActor* actor);
	virtual void Tell(IActor* actor, int messageId, void* data);
	//virtual void UpdateDebugVisualisations(IActor* actor);
	virtual char* GetControllerName();
	virtual int GetActorType();

private:
	bool CalculateCrossHairTarget(IActor* actor, Vec3* out);
	void Shoot(IActor* actor);
	void ApplyJsonConfig(IActor* actor, IJsonValue* jsonConfig);

	char name[ActorControllerMaxNameLength];
};

struct FirstPersonPlayerSubActorIndexes
{
	int camera;
	int muzzle;
};

/*struct FirstPersonPlayerDebugVisualisationIndexes
{
	int target;
};*/

class FirstPersonPlayerActorControllerData : public IActorControllerData
{
public:
	FirstPersonPlayerActorControllerData();
	virtual ~FirstPersonPlayerActorControllerData();

	virtual void* GetFacet(int facetId);

	CombatStatusActorFacet combatStatusFacet;
	HealthActorFacet healthFacet;
	PlayerInventoryActorFacet inventoryFacet;

	FirstPersonPlayerSubActorIndexes subActorIndexes;
	float movementSpeed;
	//FirstPersonPlayerDebugVisualisationIndexes debugVisualisationIndexes;

	ITicker* weaponCooldownTicker;
	bool weaponIsCoolingDown;
};