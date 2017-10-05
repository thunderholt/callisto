#include "Include/Callisto.h"
#include "GameController.h"
#include "ActorControllers/FirstPersonPlayerActorController.h"
#include "ActorControllers/DoorActorController.h"
#include "ActorControllers/CharacterActorController.h"
#include "ActorControllers/MapTransitionActorController.h"
#include "ActorControllers/ProjectileActorController.h"
#include "ActorControllers/SpawnTriggerActorController.h"
#include "ActorControllers/SpawnPointActorController.h"
#include "ActorControllers/KeyActorController.h"

void InitGame()
{
	IEngine* engine = GetEngine();
	IActorControllerManager* actorControllerManager = engine->GetActorControllerManager();

	engine->RegisterGameController(new GameController());

	actorControllerManager->RegisterActorController(new FirstPersonPlayerActorController());
	actorControllerManager->RegisterActorController(new DoorActorController());
	actorControllerManager->RegisterActorController(new CharacterActorController());
	actorControllerManager->RegisterActorController(new MapTransitionActorController());
	actorControllerManager->RegisterActorController(new ProjectileActorController());
	actorControllerManager->RegisterActorController(new SpawnTriggerActorController());
	actorControllerManager->RegisterActorController(new SpawnPointActorController());
	actorControllerManager->RegisterActorController(new KeyActorController());
}

Faction Game::ParseFactionString(const char* str)
{
	Faction faction = FactionNeutral;

	if (strcmp(str, "goodie") == 0)
	{
		faction = FactionGoodie;
	}
	else if (strcmp(str, "baddie") == 0)
	{
		faction = FactionBaddie;
	}

	return faction;
}

KeyType Game::ParseKeyTypeString(const char* str)
{
	KeyType keyType = KeyTypeUnknown;

	if (strcmp(str, "red") == 0)
	{
		keyType = KeyTypeRed;
	}
	else if (strcmp(str, "green") == 0)
	{
		keyType = KeyTypeGreen;
	}
	else if (strcmp(str, "blue") == 0)
	{
		keyType = KeyTypeBlue;
	}

	return keyType;
}

FactionRelation Game::GetFactionRelation(Faction faction1, Faction faction2)
{
	FactionRelation relation = FactionRelationNeutral;

	if (faction1 != FactionNeutral && faction2 != FactionNeutral)
	{
		relation = faction1 == faction2 ? FactionRelationFriend : FactionRelationEnemy;
	}

	return relation;
}

void Game::GetKeyTypeString(KeyType keyType, char* buffer)
{
	if (keyType == KeyTypeRed)
	{
		strcpy(buffer, "red");
	}
	else if (keyType == KeyTypeGreen)
	{
		strcpy(buffer, "green");
	}
	else if (keyType == KeyTypeBlue)
	{
		strcpy(buffer, "blue");
	}
}

void Game::LaunchProjectileAt(int launcherActorIndex, Vec3* from, Vec3* to)
{
	Vec3 direction;
	Vec3::Sub(&direction, to, from);
	Vec3::Normalize(&direction, &direction);

	Game::LaunchProjectileAlongDirection(launcherActorIndex, from, &direction);
}

void Game::LaunchProjectileAlongDirection(int launcherActorIndex, Vec3* from, Vec3* direction)
{
	IEngine* engine = GetEngine();
	IAssetManager* assetManager = engine->GetAssetManager();
	ISceneManager* sceneManager = engine->GetSceneManager();

	int plasmaBallActorAssetIndex = assetManager->FindAssetIndex(AssetTypeActor, "actors/plasma-ball.actor"); // TODO - don't load this index every time, keep it in memory.

	Vec3 rotation;
	Vec3::Zero(&rotation);

	int projectileActorIndex = sceneManager->CreateActor(
		plasmaBallActorAssetIndex, "PlasmaBall",
		*from, Vec3::Create(), null);

	IActor* projectileActor = sceneManager->GetActor(projectileActorIndex);
	if (projectileActor != null)
	{
		LaunchedYouActorMessageData messageData;
		messageData.launcherActorIndex = launcherActorIndex;
		messageData.direction = *direction;

		projectileActor->Tell(ActorMessageIdLaunchedYou, &messageData);
	}
}