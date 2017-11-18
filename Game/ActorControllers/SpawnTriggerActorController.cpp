#include "ActorControllers/SpawnTriggerActorController.h"

SpawnTriggerActorController::SpawnTriggerActorController()
{
	strcpy(this->name, "SpawnTrigger");
}

SpawnTriggerActorController::~SpawnTriggerActorController()
{

}

void SpawnTriggerActorController::OneTimeInit(IActor* actor, IJsonValue* actorAssetJsonConfig, IJsonValue* actorInstanceJsonConfig)
{
	actor->SetControllerData(new SpawnTriggerActorControllerData());
	SpawnTriggerActorControllerData* controllerData = (SpawnTriggerActorControllerData*)actor->GetControllerData();
	controllerData->triggerSphereSubActorIndex = actor->FindSubActorIndexByName("Trigger");

	if (actorAssetJsonConfig != null)
	{
		this->ApplyJsonConfig(actor, actorAssetJsonConfig);
	}

	if (actorInstanceJsonConfig != null)
	{
		this->ApplyJsonConfig(actor, actorInstanceJsonConfig);
	}
}

void SpawnTriggerActorController::Activate(IActor* actor)
{
}

void SpawnTriggerActorController::Deactivate(IActor* actor)
{
}

void SpawnTriggerActorController::Heartbeat(IActor* actor)
{
	IEngine* engine = GetEngine();
	ISceneManager* sceneManager = engine->GetSceneManager();
	ILogger* logger = engine->GetLogger();

	SpawnTriggerActorControllerData* controllerData = (SpawnTriggerActorControllerData*)actor->GetControllerData();
	ISubActor* triggerSphereSubActor = actor->GetSubActor(controllerData->triggerSphereSubActorIndex);

	if (!controllerData->hasBeenTriggered && triggerSphereSubActor->GetTriggerSphereState()->numberOfJustEnteredActors > 0)
	{
		logger->Write("Player entered spawn trigger.");

		//for (int i = 0; i < controllerData->spawnPointActorIndexes.GetLength(); i++)
		for (int i = 0; i < controllerData->spawnPointActorNames.GetLength(); i++)
		{
			//int actorIndex = controllerData->spawnPointActorIndexes[i];
			//IActor* spawnPointActor = sceneManager->GetActor(actorIndex);
			IActor* spawnPointActor = sceneManager->FindActorByName(controllerData->spawnPointActorNames[i]);
			if (spawnPointActor != null)
			{
				logger->Write("Notifying '%s'.", spawnPointActor->GetName());

				OrderYouToSpawnMessageData messageData;
				messageData.spawnTriggerActorIndex = actor->GetIndex();
				spawnPointActor->Tell(ActorMessageIdOrderYouToSpawn, &messageData);
			}
		}

		controllerData->hasBeenTriggered = true;
	}
}

void SpawnTriggerActorController::Tell(IActor* actor, int messageId, void* data)
{
	if (messageId == ActorMessageIdEncounterComplete)
	{
		EncounterCompleteMessageData* messageData = (EncounterCompleteMessageData*)data;
		this->HandleCompletedEncounter(actor, &messageData->lastParticipantWorldPosition);
	}
}

char* SpawnTriggerActorController::GetControllerName()
{
	return this->name;
}

int SpawnTriggerActorController::GetActorType()
{
	return ActorTypeSpawnTrigger;
}

void SpawnTriggerActorController::ApplyJsonConfig(IActor* actor, IJsonValue* jsonConfig)
{
	IEngine* engine = GetEngine();
	IAssetManager* assetManager = engine->GetAssetManager();
	ISceneManager* sceneManager = engine->GetSceneManager();

	SpawnTriggerActorControllerData* controllerData = (SpawnTriggerActorControllerData*)actor->GetControllerData();

	for (int propertyIndex = 0; propertyIndex < jsonConfig->GetNumberOfObjectProperties(); propertyIndex++)
	{
		IJsonProperty* jsonProperty = jsonConfig->GetObjectProperty(propertyIndex);
		IJsonValue* jsonPropertyValue = jsonProperty->GetValue();

		if (strcmp(jsonProperty->GetName(), "spawn-points") == 0)
		{
			for (int i = 0; i < jsonPropertyValue->GetNumberOfArrayElements(); i++)
			{
				jsonPropertyValue->GetArrayElement(i)->CopyStringValue(controllerData->spawnPointActorNames.PushAndGet(), ActorMaxNameLength);

				/*IActor* spawnPointActor = sceneManager->FindActorByName(jsonPropertyValue->GetArrayElement(i)->GetStringValue());
				if (spawnPointActor != null)
				{
					controllerData->spawnPointActorIndexes.Push(spawnPointActor->GetIndex());
				}*/
			}
		}
		else if (strcmp(jsonProperty->GetName(), "reward-drop-actor-asset-ref") == 0)
		{
			controllerData->rewardDropActorAssetIndex = 
				assetManager->FindAssetIndex(AssetTypeActor, jsonPropertyValue->GetStringValue());
		}
		else if (strcmp(jsonProperty->GetName(), "reward-drop-actor-config") == 0)
		{
			controllerData->jsonRewardDropActorConfig = jsonPropertyValue->Clone();
		}
	}
}

void SpawnTriggerActorController::HandleCompletedEncounter(IActor* actor, Vec3* lastParticipantWorldPosition)
{
	IEngine* engine = GetEngine();
	ISceneManager* sceneManager = engine->GetSceneManager();
	ILogger* logger = engine->GetLogger();

	SpawnTriggerActorControllerData* controllerData = (SpawnTriggerActorControllerData*)actor->GetControllerData();

	if (controllerData->rewardDropActorAssetIndex != -1)
	{
		sceneManager->CreateActor(
			controllerData->rewardDropActorAssetIndex, "EncounterRewardDrop",
			*lastParticipantWorldPosition, Vec3::Create(), controllerData->jsonRewardDropActorConfig);
	}

	logger->Write("Encounter complete.");
}

SpawnTriggerActorControllerData::SpawnTriggerActorControllerData()
{
	this->hasBeenTriggered = false;
	memset(&this->encounterFacet, 0, sizeof(EncounterActorFacet));
	this->rewardDropActorAssetIndex = -1;
	this->jsonRewardDropActorConfig = null;
}

SpawnTriggerActorControllerData::~SpawnTriggerActorControllerData()
{
	SafeDeleteAndNull(this->jsonRewardDropActorConfig);
}

void* SpawnTriggerActorControllerData::GetFacet(int facetId)
{
	void* facet = null;

	if (facetId == ActorFacetIdEncounter)
	{
		facet = &this->encounterFacet;
	}

	return facet;
}