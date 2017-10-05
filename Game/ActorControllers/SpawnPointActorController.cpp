#include "ActorControllers/SpawnPointActorController.h"

SpawnPointActorController::SpawnPointActorController()
{
	strcpy(this->name, "SpawnPoint");
}

SpawnPointActorController::~SpawnPointActorController()
{

}

void SpawnPointActorController::OneTimeInit(IActor* actor, IJsonValue* actorAssetJsonConfig, IJsonValue* actorInstanceJsonConfig)
{
	actor->SetControllerData(new SpawnPointActorControllerData());
	SpawnPointActorControllerData* controllerData = (SpawnPointActorControllerData*)actor->GetControllerData();

	if (actorAssetJsonConfig != null)
	{
		this->ApplyJsonConfig(actor, actorAssetJsonConfig);
	}

	if (actorInstanceJsonConfig != null)
	{
		this->ApplyJsonConfig(actor, actorInstanceJsonConfig);
	}
}

void SpawnPointActorController::Activate(IActor* actor)
{
}

void SpawnPointActorController::Deactivate(IActor* actor)
{
}

void SpawnPointActorController::Heartbeat(IActor* actor)
{
	
}

void SpawnPointActorController::Tell(IActor* actor, int messageId, void* data)
{
	IEngine* engine = GetEngine();
	IAssetManager* assetManager = engine->GetAssetManager();
	ISceneManager* sceneManager = engine->GetSceneManager();
	ILogger* logger = engine->GetLogger();

	if (messageId == ActorMessageIdOrderYouToSpawn)
	{
		OrderYouToSpawnMessageData* messageData = (OrderYouToSpawnMessageData*)data;

		logger->Write("%s: I shall now spawn!", actor->GetName());

		this->Spawn(actor, messageData->spawnTriggerActorIndex);
	}
}

char* SpawnPointActorController::GetControllerName()
{
	return this->name;
}

int SpawnPointActorController::GetActorType()
{
	return ActorTypeSpawnPoint;
}

void SpawnPointActorController::ApplyJsonConfig(IActor* actor, IJsonValue* jsonConfig)
{
	IEngine* engine = GetEngine();
	IAssetManager* assetManager = engine->GetAssetManager();

	SpawnPointActorControllerData* controllerData = (SpawnPointActorControllerData*)actor->GetControllerData();

	for (int propertyIndex = 0; propertyIndex < jsonConfig->GetNumberOfObjectProperties(); propertyIndex++)
	{
		IJsonProperty* jsonProperty = jsonConfig->GetObjectProperty(propertyIndex);
		IJsonValue* jsonPropertyValue = jsonProperty->GetValue();

		if (strcmp(jsonProperty->GetName(), "spawnee-actor-asset-ref") == 0)
		{
			controllerData->spawneeActorAssetIndex = 
				assetManager->FindAssetIndex(AssetTypeActor, jsonPropertyValue->GetStringValue());
		}
	}
}

void SpawnPointActorController::Spawn(IActor* actor, int spawnTriggerActorIndex)
{
	IEngine* engine = GetEngine();
	ISceneManager* sceneManager = engine->GetSceneManager();

	SpawnPointActorControllerData* controllerData = (SpawnPointActorControllerData*)actor->GetControllerData();

	if (controllerData->spawneeActorAssetIndex != -1)
	{
		int spawneeActorIndex = sceneManager->CreateActor(
			controllerData->spawneeActorAssetIndex, "SpawnedActor",
			actor->GetWorldPosition(), Vec3::Create(), null);

		IActor* spawneeActor = sceneManager->GetActor(spawneeActorIndex);
		if (spawneeActor != null)
		{
			SpawnedYouMessageData messageData;
			messageData.spawnTriggerActorIndex = spawnTriggerActorIndex;
			spawneeActor->Tell(ActorMessageIdSpawnedYou, &messageData);
		}
	}
}

void* SpawnPointActorControllerData::GetFacet(int facetId)
{
	return null;
}