#pragma once

#include "Include/Callisto.h"

enum Faction
{
	FactionNeutral,
	FactionGoodie,
	FactionBaddie
};

enum FactionRelation
{
	FactionRelationNeutral,
	FactionRelationFriend,
	FactionRelationEnemy
};

enum KeyType
{
	KeyTypeUnknown = 0,
	KeyTypeRed = 1,
	KeyTypeGreen = 2,
	KeyTypeBlue = 3
};

#define KeysNumberOfTypes 4

//------ Actor Types -----//

enum ActorType
{
	ActorTypeFirstPersonPlayer = 1,
	ActorTypeCharacter = 2,
	ActorTypeDoor = 3,
	ActorTypeMapTransition = 4,
	ActorTypeProjectile = 5,
	ActorTypeSpawnTrigger = 6,
	ActorTypeSpawnPoint = 7,
	ActorTypeKey = 8
};

//------ Actor Facets -----//

enum ActorFacetId
{
	ActorFacetIdCombatStatus = 1,
	ActorFacetIdHealth = 2,
	ActorFacetIdEncounter = 3,
	ActorFacetIdPlayerInventory = 4
};

struct CombatStatusActorFacet
{
	Faction faction;
};

struct HealthActorFacet
{
	int currentHealth;
	int totalHealth;
};

struct EncounterActorFacet
{
	int numberOfParticipants;
};

struct PlayerInventoryActorFacet
{
	int keyQuantitiesByType[KeysNumberOfTypes];
};

//------ Actor Messages -----//

enum ActorMessageId
{
	ActorMessageIdLaunchedYou = 1,
	ActorMessageIdShotYou = 2,
	ActorMessageIdOrderYouToSpawn = 3,
	ActorMessageIdSpawnedYou = 4,
	ActorMessageIdEncounterComplete = 5,
	ActorMessageIdAddKeyToInventory = 6,
	ActorMessageIdRemoveKeyFromInventory = 7
};

struct LaunchedYouActorMessageData
{
	Vec3 direction;
	int launcherActorIndex;
};

struct ShotYouActorMessageData
{
	// TODO - weapon type and that sort of stuff?
};

struct OrderYouToSpawnMessageData
{
	int spawnTriggerActorIndex;
};

struct SpawnedYouMessageData
{
	int spawnTriggerActorIndex;
};

struct EncounterCompleteMessageData
{
	Vec3 lastParticipantWorldPosition;
};

struct AddKeyToInventoryMessageData
{
	KeyType keyType;
};

struct RemoveKeyFromInventoryMessageData
{
	KeyType keyType;
};

//-----------//

class Game
{
public:
	static Faction ParseFactionString(const char* str);
	static KeyType ParseKeyTypeString(const char* str);
	static FactionRelation GetFactionRelation(Faction faction1, Faction faction2);
	static void GetKeyTypeString(KeyType keyType, char* buffer);
	static void LaunchProjectileAt(int launcherActorIndex, Vec3* from, Vec3* to);
	static void LaunchProjectileAlongDirection(int launcherActorIndex, Vec3* from, Vec3* direction);
};