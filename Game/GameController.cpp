#include "Game.h"
#include "GameController.h"

GameController::~GameController()
{
}

int GameController::ParseActorType(const char* actorTypeString)
{
	int actorType = 0;

	if (strcmp(actorTypeString, "first-person-player") == 0)
	{
		actorType = ActorTypeFirstPersonPlayer;
	}
	else if (strcmp(actorTypeString, "character") == 0)
	{
		actorType = ActorTypeCharacter;
	}
	else if (strcmp(actorTypeString, "door") == 0)
	{
		actorType = ActorTypeDoor;
	}
	else if (strcmp(actorTypeString, "map-transition") == 0)
	{
		actorType = ActorTypeMapTransition;
	}
	else if (strcmp(actorTypeString, "projectile") == 0)
	{
		actorType = ActorTypeProjectile;
	}
	else if (strcmp(actorTypeString, "spawn-trigger") == 0)
	{
		actorType = ActorTypeSpawnTrigger;
	}
	else if (strcmp(actorTypeString, "spawn-point") == 0)
	{
		actorType = ActorTypeSpawnPoint;
	}

	return actorType;
}