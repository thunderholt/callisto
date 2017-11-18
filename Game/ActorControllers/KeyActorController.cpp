#include "ActorControllers/KeyActorController.h"

KeyActorController::KeyActorController()
{
	strcpy(this->name, "Key");
}

KeyActorController::~KeyActorController()
{

}

void KeyActorController::OneTimeInit(IActor* actor, IJsonValue* actorAssetJsonConfig, IJsonValue* actorInstanceJsonConfig)
{
	actor->SetControllerData(new KeyActorControllerData());
	KeyActorControllerData* controllerData = (KeyActorControllerData*)actor->GetControllerData();
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

void KeyActorController::ReInit(IActor* actor, IJsonValue* actorInstanceJsonConfig)
{
	if (actorInstanceJsonConfig != null)
	{
		this->ApplyJsonConfig(actor, actorInstanceJsonConfig);
	}
}

void KeyActorController::Activate(IActor* actor)
{
}

void KeyActorController::Deactivate(IActor* actor)
{
}

void KeyActorController::Heartbeat(IActor* actor)
{
	IEngine* engine = GetEngine();
	IFrameTimer* frameTimer = engine->GetFrameTimer();
	ISceneManager* sceneManager = engine->GetSceneManager();
	ILogger* logger = engine->GetLogger();

	KeyActorControllerData* controllerData = (KeyActorControllerData*)actor->GetControllerData();
	float frameMultiplier = frameTimer->GetMultiplier();
	ISubActor* triggerSphereSubActor = actor->GetSubActor(controllerData->triggerSphereSubActorIndex);

	actor->Rotate(0.0f, 0.02f, 0.0f);

	if (triggerSphereSubActor->GetTriggerSphereState()->numberOfJustEnteredActors > 0)
	{
		logger->Write("Key collected.");

		int collectorActorIndex = triggerSphereSubActor->GetTriggerSphereState()->justEnteredActorIndexes[0];
		IActor* collectorActor = sceneManager->GetActor(collectorActorIndex);
		if (collectorActor != null)
		{
			AddKeyToInventoryMessageData messageData;
			messageData.keyType = controllerData->keyType;

			collectorActor->Tell(ActorMessageIdAddKeyToInventory, &messageData);

			/*PlayerInventoryActorFacet* playerInventoryFacet = 
				(PlayerInventoryActorFacet*)collectorActor->GetControllerData()->GetFacet(ActorFacetIdPlayerInventory);

			if (playerInventoryFacet != null)
			{
				playerInventoryFacet->keyQuantitiesByType[controllerData->keyType]++;

				char keyTypeString[8];
				Game::GetKeyTypeString(controllerData->keyType, keyTypeString);
				logger->Write(
					"Added %s key to player inventory. Player now has %d %s keys.", 
					keyTypeString, playerInventoryFacet->keyQuantitiesByType[controllerData->keyType], keyTypeString);
			}*/
		}

		actor->Deactivate();
	}
}

void KeyActorController::Tell(IActor* actor, int messageId, void* data)
{
	
}

char* KeyActorController::GetControllerName()
{
	return this->name;
}

int KeyActorController::GetActorType()
{
	return ActorTypeKey;
}

void KeyActorController::ApplyJsonConfig(IActor* actor, IJsonValue* jsonConfig)
{
	KeyActorControllerData* controllerData = (KeyActorControllerData*)actor->GetControllerData();

	for (int propertyIndex = 0; propertyIndex < jsonConfig->GetNumberOfObjectProperties(); propertyIndex++)
	{
		IJsonProperty* jsonProperty = jsonConfig->GetObjectProperty(propertyIndex);
		IJsonValue* jsonPropertyValue = jsonProperty->GetValue();

		if (strcmp(jsonProperty->GetName(), "key-type") == 0)
		{
			controllerData->keyType = Game::ParseKeyTypeString(jsonPropertyValue->GetStringValue());
		}
	}
}

KeyActorControllerData::KeyActorControllerData()
{

}

void* KeyActorControllerData::GetFacet(int facetId)
{
	return null;
}