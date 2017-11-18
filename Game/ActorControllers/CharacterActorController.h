#pragma once

#include "Include/Callisto.h"
#include "Game.h"

class CharacterActorController : public IActorController
{
public:
	CharacterActorController();
	virtual ~CharacterActorController();
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
	void ApplyJsonConfig(IActor* actor, IJsonValue* jsonConfig);
	void PerformWanderingActivity(IActor* actor);
	void PerformAttackingActivity(IActor* actor);
	void CheckForVisibleEnemies(IActor* actor);
	bool CheckIfOtherActorIsEnemy(IActor* actor, IActor* otherActor);
	ISubActor* FindVisibleOtherActorTargetSubActor(IActor* actor, IActor* otherActor);
	bool CheckPointIsWithinFOVCone(IActor* actor, Vec3* point);
	void AttemptToShootAtEnemy(IActor* actor);
	void Die(IActor* actor);
	void JoinEncounter(IActor* actor);
	void LeaveEncounter(IActor* actor);
	//void ClearAllDebugVisualisations(IActor* actor);
	//void EnableEyeDebugVisualisation(IActor* actor);
	//void EnableWanderToPointDebugVisualisation(IActor* actor);

	char name[ActorControllerMaxNameLength];
};

enum CharacterActorActivity
{
	CharacterActorActivityNone = 0,
	CharacterActorActivityWandering = 1,
	CharacterActorActivityAttacking = 2
};

/*struct CharacterActorDebugVisualisationIndexes
{
	int wanderToPoint;
	int eye;
};*/

class CharacterActorControllerData : public IActorControllerData
{
public:
	CharacterActorControllerData();
	virtual ~CharacterActorControllerData();

	virtual void* GetFacet(int facetId);

	CombatStatusActorFacet combatStatusFacet;
	HealthActorFacet healthFacet;

	int spawnTriggerActorIndex;
	int eyeSubActorIndex;
	int muzzleSubActorIndex;
	CharacterActorActivity currentActivity;
	Vec3 wanderToPoint;
	int enemyActorIndex;
	bool weaponIsCoolingDown;

	ITicker* findNewWanderToPointTicker;
	ITicker* checkForVisibleEnemiesTicker;
	//ITicker* shootTicker;
	ITicker* weaponCooldownTicker;

	//CharacterActorDebugVisualisationIndexes debugVisualisationIndexes;
};
