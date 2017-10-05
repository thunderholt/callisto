#include "ActorControllers/MapTransitionActorController.h"

MapTransitionActorController::MapTransitionActorController()
{
	strcpy(this->name, "MapTransition");
}

MapTransitionActorController::~MapTransitionActorController()
{

}

void MapTransitionActorController::OneTimeInit(IActor* actor, IJsonValue* actorAssetJsonConfig, IJsonValue* actorInstanceJsonConfig)
{
	actor->SetControllerData(new MapTransitionActorControllerData());
	MapTransitionActorControllerData* controllerData = (MapTransitionActorControllerData*)actor->GetControllerData();
	controllerData->triggerSphereSubActorIndex = actor->FindSubActorIndexByName("Trigger");
	strcpy(controllerData->destinationMapFilePath, "");

	if (actorAssetJsonConfig != null)
	{
		this->ApplyJsonConfig(actor, actorAssetJsonConfig);
	}

	if (actorInstanceJsonConfig != null)
	{
		this->ApplyJsonConfig(actor, actorInstanceJsonConfig);
	}
}

void MapTransitionActorController::Activate(IActor* actor)
{
}

void MapTransitionActorController::Deactivate(IActor* actor)
{
}

void MapTransitionActorController::Heartbeat(IActor* actor)
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	MapTransitionActorControllerData* controllerData = (MapTransitionActorControllerData*)actor->GetControllerData();
	ISubActor* triggerSphereSubActor = actor->GetSubActor(controllerData->triggerSphereSubActorIndex);

	if (triggerSphereSubActor->GetTriggerSphereState()->numberOfJustEnteredActors > 0)
	{
		logger->Write("Player entered map transition.");
		if (strcmp(controllerData->destinationMapFilePath, "") != 0)
		{
			engine->BeginLoadingMap(controllerData->destinationMapFilePath);
		}
	}
}

void MapTransitionActorController::Tell(IActor* actor, int messageId, void* data)
{

}

/*void MapTransitionActorController::UpdateDebugVisualisations(IActor* actor)
{

}*/

char* MapTransitionActorController::GetControllerName()
{
	return this->name;
}

int MapTransitionActorController::GetActorType()
{
	return ActorTypeMapTransition;
}

void MapTransitionActorController::ApplyJsonConfig(IActor* actor, IJsonValue* jsonConfig)
{
	MapTransitionActorControllerData* controllerData = (MapTransitionActorControllerData*)actor->GetControllerData();

	for (int propertyIndex = 0; propertyIndex < jsonConfig->GetNumberOfObjectProperties(); propertyIndex++)
	{
		IJsonProperty* jsonProperty = jsonConfig->GetObjectProperty(propertyIndex);
		IJsonValue* jsonPropertyValue = jsonProperty->GetValue();

		if (strcmp(jsonProperty->GetName(), "destination-map-file-path") == 0)
		{
			jsonPropertyValue->CopyStringValue(controllerData->destinationMapFilePath, AssetMaxFilePathLength);
		}
	}
}


void* MapTransitionActorControllerData::GetFacet(int facetId)
{
	return null;
}