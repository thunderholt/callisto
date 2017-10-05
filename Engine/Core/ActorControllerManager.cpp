#include "Core/ActorControllerManager.h"

ActorControllerManager::~ActorControllerManager()
{
	this->DeleteAllActorControllers();
}

void ActorControllerManager::RegisterActorController(IActorController* actorController)
{
	this->actorControllers.AddItem(actorController);
}

int ActorControllerManager::FindActorControllerIndex(const char* actorControllerName)
{
	int index = -1;

	for (int actorControllerIndex = 0; actorControllerIndex < this->actorControllers.GetNumberOfIndexes(); actorControllerIndex++)
	{
		IActorController* actorController = this->actorControllers[actorControllerIndex];
		if (actorController != null && strcmp(actorController->GetControllerName(), actorControllerName) == 0)
		{
			index = actorControllerIndex;
			break;
		}
	}

	return index;
}

IActorController* ActorControllerManager::GetActorController(int index)
{
	return this->actorControllers[index];
}

void ActorControllerManager::DeleteAllActorControllers()
{
	for (int actorControllerIndex = 0; actorControllerIndex < this->actorControllers.GetNumberOfIndexes(); actorControllerIndex++)
	{
		SafeDeleteAndNull(this->actorControllers[actorControllerIndex]);
		this->actorControllers.UnoccupyIndex(actorControllerIndex);
	}
}