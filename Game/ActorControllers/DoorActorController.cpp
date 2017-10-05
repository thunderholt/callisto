#include "ActorControllers/DoorActorController.h"

DoorActorController::DoorActorController()
{
	strcpy(this->name, "Door");
}

DoorActorController::~DoorActorController()
{

}

void DoorActorController::OneTimeInit(IActor* actor, IJsonValue* actorAssetJsonConfig, IJsonValue* actorInstanceJsonConfig)
{
	actor->SetControllerData(new DoorActorControllerData());
	DoorActorControllerData* controllerData = (DoorActorControllerData*)actor->GetControllerData();
	controllerData->state = DoorActorStateClosed;
	controllerData->doorMeshSubActorIndex = actor->FindSubActorIndexByName("DoorMesh");
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

void DoorActorController::Activate(IActor* actor)
{
}

void DoorActorController::Deactivate(IActor* actor)
{
}

void DoorActorController::Heartbeat(IActor* actor)
{
	IEngine* engine = GetEngine();
	ISceneManager* sceneManager = engine->GetSceneManager();
	ILogger* logger = engine->GetLogger();

	DoorActorControllerData* controllerData = (DoorActorControllerData*)actor->GetControllerData();
	ISubActor* doorMeshSubActor = actor->GetSubActor(controllerData->doorMeshSubActorIndex);
	ISubActor* triggerSphereSubActor = actor->GetSubActor(controllerData->triggerSphereSubActorIndex);
	SubActorTriggerSphereState* triggerSphereState = triggerSphereSubActor->GetTriggerSphereState();

	bool otherActorIsResidentInTriggerSphere = triggerSphereState->numberOfResidentActors > 0;

	if (otherActorIsResidentInTriggerSphere)
	{
		if (controllerData->isLocked)
		{
			IActor* openerActor = sceneManager->GetActor(triggerSphereState->residentActorIndexes[0]);
			if (openerActor != null)
			{
				PlayerInventoryActorFacet* playerInventoryFacet = 
					(PlayerInventoryActorFacet*)openerActor->GetControllerData()->GetFacet(ActorFacetIdPlayerInventory);

				if (playerInventoryFacet != null &&
					playerInventoryFacet->keyQuantitiesByType[controllerData->requiredKeyType] > 0)
				{
					RemoveKeyFromInventoryMessageData messageData;
					messageData.keyType = controllerData->requiredKeyType;

					openerActor->Tell(ActorMessageIdRemoveKeyFromInventory, &messageData);

					controllerData->isLocked = false;
					logger->Write("Door is now unlocked.");
				}
			}
		}

		if (!controllerData->isLocked && controllerData->state != DoorActorStateOpened)
		{
			controllerData->state = DoorActorStateOpening;
		}
	}
	else 
	{
		if (controllerData->state != DoorActorStateClosed)
		{
			controllerData->state = DoorActorStateClosing;
		}
	}
	
	if (controllerData->state == DoorActorStateOpening || controllerData->state == DoorActorStateClosing)
	{
		Vec3 upperLimit, lowerLimit;
		Vec3::Set(&upperLimit, 0.0f, 1.0f, 0.0f);
		Vec3::Zero(&lowerLimit);

		Vec3 direction;
		bool hasReachedLimit = false;
		Vec3 doorMeshLocalPositionOffset = doorMeshSubActor->GetLocalPositionOffset();

		if (controllerData->state == DoorActorStateOpening)
		{
			Vec3::Set(&direction, 0.0f, 1.0f, 0.0f);
			hasReachedLimit = Vec3::Equals(&doorMeshLocalPositionOffset, &upperLimit);
			if (hasReachedLimit)
			{
				controllerData->state = DoorActorStateOpened;
				//logger->Write("Door is now open.");
			}
		}
		else if (controllerData->state == DoorActorStateClosing)
		{
			Vec3::Set(&direction, 0.0f, -1.0f, 0.0f);
			hasReachedLimit = Vec3::Equals(&doorMeshLocalPositionOffset, &lowerLimit);
			if (hasReachedLimit)
			{
				controllerData->state = DoorActorStateClosed;
				//logger->Write("Door is now closed.");
			}
		}

		if (!hasReachedLimit)
		{
			doorMeshSubActor->Translate(direction, SubActorSpaceLocal, 0.05f, true, upperLimit, lowerLimit);
			//logger->Write("Door is transitioning.");
		}
	}
}

void DoorActorController::Tell(IActor* actor, int messageId, void* data)
{

}

/*void DoorActorController::UpdateDebugVisualisations(IActor* actor)
{

}*/

char* DoorActorController::GetControllerName()
{
	return this->name;
}

int DoorActorController::GetActorType()
{
	return ActorTypeDoor;
}

void DoorActorController::ApplyJsonConfig(IActor* actor, IJsonValue* jsonConfig)
{
	DoorActorControllerData* controllerData = (DoorActorControllerData*)actor->GetControllerData();

	for (int propertyIndex = 0; propertyIndex < jsonConfig->GetNumberOfObjectProperties(); propertyIndex++)
	{
		IJsonProperty* jsonProperty = jsonConfig->GetObjectProperty(propertyIndex);
		IJsonValue* jsonPropertyValue = jsonProperty->GetValue();

		if (strcmp(jsonProperty->GetName(), "is-locked") == 0)
		{
			controllerData->isLocked = jsonPropertyValue->GetBoolValue();
		}
		else if (strcmp(jsonProperty->GetName(), "required-key-type") == 0)
		{
			controllerData->requiredKeyType = Game::ParseKeyTypeString(jsonPropertyValue->GetStringValue());
		}
	}
}

/*bool DoorActorController::DetermineIfPlayerIsResidentInTriggerSphere(IActor* actor)
{
	IEngine* engine = GetEngine();
	ISceneManager* sceneManager = engine->GetSceneManager();
	IActorControllerManager* actorControllerManager = engine->GetActorControllerManager();

	DoorActorControllerData* controllerData = (DoorActorControllerData*)actor->GetControllerData();
	ISubActor* triggerSphereSubActor = actor->GetSubActor(controllerData->triggerSphereSubActorIndex);

	bool playerIsResidentInTriggerSphere = false;
	for (int i = 0; i < triggerSphereSubActor->GetTriggerSphereState()->numberOfResidentActors; i++)
	{
		int otherActorIndex = triggerSphereSubActor->GetTriggerSphereState()->residentActorIndexes[i];
		IActor* otherActor = sceneManager->GetActor(otherActorIndex);
		if (otherActor != null)
		{
			IActorController* otherActorController = actorControllerManager->GetActorController(otherActor->GetControllerIndex());
			if (otherActorController != null && otherActorController->GetActorType() == ActorTypeFirstPersonPlayer)
			{
				playerIsResidentInTriggerSphere = true;
				break;
			}
		}
	}

	return playerIsResidentInTriggerSphere;
}*/

void* DoorActorControllerData::GetFacet(int facetId)
{
	return null;
}