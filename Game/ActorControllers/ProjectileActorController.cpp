#include "ActorControllers/ProjectileActorController.h"

ProjectileActorController::ProjectileActorController()
{
	strcpy(this->name, "Projectile");
}

ProjectileActorController::~ProjectileActorController()
{
}

void ProjectileActorController::OneTimeInit(IActor* actor, IJsonValue* actorAssetJsonConfig, IJsonValue* actorInstanceJsonConfig)
{
	actor->SetControllerData(new ProjectileActorControllerData());
	ProjectileActorControllerData* controllerData = (ProjectileActorControllerData*)actor->GetControllerData();

	Vec3::Zero(&controllerData->direction);
}

void ProjectileActorController::Activate(IActor* actor)
{
}

void ProjectileActorController::Deactivate(IActor* actor)
{
}

void ProjectileActorController::Heartbeat(IActor* actor)
{
	IEngine* engine = GetEngine();
	IFrameTimer* frameTimer = engine->GetFrameTimer();
	ISceneManager* sceneManager = engine->GetSceneManager();
	ILogger* logger = engine->GetLogger();

	ProjectileActorControllerData* controllerData = (ProjectileActorControllerData*)actor->GetControllerData();
	float frameMultiplier = frameTimer->GetMultiplier();

	/*for (int messageIndex = 0; messageIndex < actor->GetNumberOfMessages(); messageIndex++)
	{
		ActorMessage* message = actor->GrabMessage(messageIndex);

		if (message->id == ActorMessageIdShootProjectile)
		{
			ShootProjectileMessageData* messageData = (ShootProjectileMessageData*)message->data;
			controllerData->direction = messageData->direction;
			controllerData->shooterActorIndex = messageData->shooterActorIndex;
		}
	}

	actor->ClearMessages();*/

	//actor->Translate(controllerData->direction, ActorSpaceWorld, 0.4f); // FIXME - load from config.
	float speed = 0.1f;  // FIXME - load from config.

	ScenePushResult scenePushResult;
	actor->PushAlongDirection(&scenePushResult, controllerData->direction, ActorSpaceWorld, speed * frameMultiplier, false, controllerData->launcherActorIndex);

	if (scenePushResult.numberOfSceneIntersectionResults > 0)
	{
		SceneIntersectionResult* sceneIntersectionResult = &scenePushResult.sceneIntersectionResults[0];

		if (sceneIntersectionResult->type == SceneIntersectionTypeWorldMesh)
		{
			logger->Write("I hit the world mesh!");
		}
		else if (sceneIntersectionResult->type == SceneIntersectionTypeActor)
		{
			IActor* otherActor = sceneManager->GetActor(sceneIntersectionResult->actorIndex);
			if (otherActor != null)
			{
				logger->Write("I hit '%s'.", otherActor->GetName());

				ShotYouActorMessageData messageData;
				// TODO - populate message data with weapon type and that sort of thing, maybe.

				otherActor->Tell(ActorMessageIdShotYou, &messageData);
			}
		}
		else
		{
			logger->Write("Unknown hit type.");
		}

		actor->Deactivate();
	}
}

void ProjectileActorController::Tell(IActor* actor, int messageId, void* data)
{
	ProjectileActorControllerData* controllerData = (ProjectileActorControllerData*)actor->GetControllerData();

	if (messageId == ActorMessageIdLaunchedYou)
	{
		LaunchedYouActorMessageData* messageData = (LaunchedYouActorMessageData*)data;
		controllerData->direction = messageData->direction;
		controllerData->launcherActorIndex = messageData->launcherActorIndex;
	}
}

/*void ProjectileActorController::UpdateDebugVisualisations(IActor* actor)
{

}*/

char* ProjectileActorController::GetControllerName()
{
	return this->name;
}

int ProjectileActorController::GetActorType()
{
	return ActorTypeProjectile;
}

void ProjectileActorController::ApplyJsonConfig(IActor* actor, IJsonValue* jsonConfig)
{
}

void* ProjectileActorControllerData::GetFacet(int facetId)
{
	return null;
}