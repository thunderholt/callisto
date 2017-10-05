#include "ActorControllers/FirstPersonPlayerActorController.h"

FirstPersonPlayerActorController::FirstPersonPlayerActorController()
{
	strcpy(this->name, "FirstPersonPlayer");
}

FirstPersonPlayerActorController::~FirstPersonPlayerActorController()
{

}

void FirstPersonPlayerActorController::OneTimeInit(IActor* actor, IJsonValue* actorAssetJsonConfig, IJsonValue* actorInstanceJsonConfig)
{
	IEngine* engine = GetEngine();
	ISceneManager* sceneManager = engine->GetSceneManager();

	actor->SetControllerData(new FirstPersonPlayerActorControllerData());
	FirstPersonPlayerActorControllerData* controllerData = (FirstPersonPlayerActorControllerData*)actor->GetControllerData();

	controllerData->subActorIndexes.camera = actor->FindSubActorIndexByName("Camera");
	controllerData->subActorIndexes.muzzle = actor->FindSubActorIndexByName("Muzzle");
	controllerData->combatStatusFacet.faction = FactionGoodie;
	controllerData->healthFacet.totalHealth = 20;
	controllerData->healthFacet.currentHealth = controllerData->healthFacet.totalHealth;
	//controllerData->debugVisualisationIndexes.target = actor->AddDebugVisualisation();

	if (actorAssetJsonConfig != null)
	{
		this->ApplyJsonConfig(actor, actorAssetJsonConfig);
	}

	if (actorInstanceJsonConfig != null)
	{
		this->ApplyJsonConfig(actor, actorInstanceJsonConfig);
	}
}

void FirstPersonPlayerActorController::Activate(IActor* actor)
{
}

void FirstPersonPlayerActorController::Deactivate(IActor* actor)
{
}

void FirstPersonPlayerActorController::Heartbeat(IActor* actor)
{
	//////////
	//actor->Translate(Vec3::Create(0.01f, 0.0f, 0.0f), ActorSpaceWorld, 0.01f);
	/////////

	IEngine* engine = GetEngine();
	IAggregatedGamePad* aggregatedGamePad = engine->GetAggregatedGamePad();
	IFrameTimer* frameTimer = engine->GetFrameTimer();
	ISceneManager* sceneManager = engine->GetSceneManager();
	ILogger* logger = engine->GetLogger();

	float frameMultiplier = frameTimer->GetMultiplier();
	FirstPersonPlayerActorControllerData* controllerData = (FirstPersonPlayerActorControllerData*)actor->GetControllerData();
	ISubActor* cameraSubActor = actor->GetSubActor(controllerData->subActorIndexes.camera);

	Vec2* leftStickAxes = aggregatedGamePad->GetLeftStickAxes();
	Vec2* rightStickAxes = aggregatedGamePad->GetRightStickAxes();
	float movementSpeed = controllerData->movementSpeed * frameMultiplier;
	float rotationSpeed = 0.05f; // FIXME
	Vec3 rotation;
	ScenePushResult scenePushResult;

	// Check XZ movement.
	if (!Vec2::IsZero(leftStickAxes))
	{
		Vec3 direction;
		Vec3::Set(&direction, leftStickAxes->x, 0.0f, leftStickAxes->y);

		/*////////
		Vec3::Set(&direction, 0.01f, 0.0f, 0.0f);
		////////*/

		float directionLength = Math::Min(Vec3::Length(&direction), 1.0f);
		Vec3::Normalize(&direction, &direction);

		movementSpeed *= directionLength;

		//actor->Translate(direction, ActorSpaceLocal, movementSpeed);
		actor->PushAlongDirection(&scenePushResult, direction, ActorSpaceLocal, movementSpeed, true, -1);
	}

	// Check yaw rotation.
	if (rightStickAxes->x != 0.0f)
	{
		Vec3::Set(&rotation, 0.0f, rightStickAxes->x * rotationSpeed, 0.0f);
		actor->Rotate(rotation);
	}

	// Check pitch rotation.
	if (rightStickAxes->y != 0.0f)
	{
		Vec3::Set(&rotation, rightStickAxes->y * rotationSpeed, 0.0f, 0.0f);
		cameraSubActor->Rotate(rotation);
	}

	// Apply gravity.
	Vec3 gravityDirection;
	Vec3::Set(&gravityDirection, 0.0f, -1.0f, 0.0f);

	actor->PushAlongDirection(&scenePushResult, gravityDirection, ActorSpaceWorld, 0.08f * frameMultiplier, false, -1); // TODO - get gravity constant from somewhere

	// Determine if the player can shoot.
	if (controllerData->weaponIsCoolingDown)
	{
		if (controllerData->weaponCooldownTicker->Tick())
		{
			controllerData->weaponIsCoolingDown = false;
			controllerData->weaponCooldownTicker->Reset();
		}
	}

	// If the player wants to shoot, do so.
	if (!controllerData->weaponIsCoolingDown)
	{
		if (aggregatedGamePad->GetTriggerPressure(GamePadTriggerRight) > 0.1f)
		{
			logger->Write("Player shoot!");
			this->Shoot(actor);
		}
	}
}

void FirstPersonPlayerActorController::Tell(IActor* actor, int messageId, void* data)
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	FirstPersonPlayerActorControllerData* controllerData = (FirstPersonPlayerActorControllerData*)actor->GetControllerData();

	if (messageId == ActorMessageIdShotYou)
	{
		ShotYouActorMessageData* messageData = (ShotYouActorMessageData*)data;
		controllerData->healthFacet.currentHealth--;

		if (controllerData->healthFacet.currentHealth > 0)
		{
			logger->Write("%s: ouch! New health: %d", actor->GetName(), controllerData->healthFacet);
		}
		else
		{
			logger->Write("%s: I'm dead!", actor->GetName());
		}
	}
	else if (messageId == ActorMessageIdAddKeyToInventory)
	{
		AddKeyToInventoryMessageData* messageData = (AddKeyToInventoryMessageData*)data;

		PlayerInventoryActorFacet* playerInventoryFacet = 
			(PlayerInventoryActorFacet*)controllerData->GetFacet(ActorFacetIdPlayerInventory);

		playerInventoryFacet->keyQuantitiesByType[messageData->keyType]++;

		char keyTypeString[8];
		Game::GetKeyTypeString(messageData->keyType, keyTypeString);
		logger->Write(
			"Added %s key to player inventory. Player now has %d %s keys.",
			keyTypeString, playerInventoryFacet->keyQuantitiesByType[messageData->keyType], keyTypeString);
	}
	else if (messageId == ActorMessageIdRemoveKeyFromInventory)
	{
		RemoveKeyFromInventoryMessageData* messageData = (RemoveKeyFromInventoryMessageData*)data;

		PlayerInventoryActorFacet* playerInventoryFacet =
			(PlayerInventoryActorFacet*)controllerData->GetFacet(ActorFacetIdPlayerInventory);

		playerInventoryFacet->keyQuantitiesByType[messageData->keyType]--;

		char keyTypeString[8];
		Game::GetKeyTypeString(messageData->keyType, keyTypeString);
		logger->Write(
			"Removed %s key from player inventory. Player now has %d %s keys.",
			keyTypeString, playerInventoryFacet->keyQuantitiesByType[messageData->keyType], keyTypeString);
	}
}

/*void FirstPersonPlayerActorController::UpdateDebugVisualisations(IActor* actor)
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	FirstPersonPlayerActorControllerData* controllerData = (FirstPersonPlayerActorControllerData*)actor->GetControllerData();

	// Setup the target debug visualisation.
	ActorDebugVisualisation* targetVisualisation = actor->GetDebugVisualisation(controllerData->debugVisualisationIndexes.target);

	// Calculate the target.
	Vec3 crossHairTarget;
	bool targetFound = this->CalculateCrossHairTarget(actor, &crossHairTarget);

	if (targetFound)
	{
		targetVisualisation->enabled = true;
		targetVisualisation->type = ActorDebugVisualisationTypeIcon;
		targetVisualisation->icon = DebugIconTrigger;
		targetVisualisation->position = crossHairTarget;
		//Vec3::Add(&targetVisualisation->position, &targetVisualisation->position, 0.0, 0.0, 1.0f);
	}
	else
	{
		targetVisualisation->enabled = false;
		logger->Write("Looking into blank space.");
	}
}*/

/*
void FirstPersonPlayerActorController::Heartbeat(IActor* actor)
{
	IEngine* engine = GetEngine();
	IAggregatedGamePad* aggregatedGamePad = engine->GetAggregatedGamePad();
	IFrameTimer* frameTimer = engine->GetFrameTimer();
	ISceneManager* sceneManager = engine->GetSceneManager();
	ILogger* logger = engine->GetLogger();

	float frameMultiplier = frameTimer->GetMultiplier();
	FirstPersonPlayerActorControllerData* controllerData = (FirstPersonPlayerActorControllerData*)actor->GetControllerData();
	ISubActor* cameraSubActor = actor->GetSubActor(controllerData->subActorIndexes.camera);
	
	Vec2* leftStickAxes = aggregatedGamePad->GetLeftStickAxes();
	Vec2* rightStickAxes = aggregatedGamePad->GetRightStickAxes();
	float movementSpeed = controllerData->movementSpeed * frameMultiplier;
	float rotationSpeed = 0.05f; // FIXME
	Vec3 rotation;

	// Check XZ movement.
	if (!Vec2::IsZero(leftStickAxes))
	{
		Vec3 direction;
		Vec3::Set(&direction, leftStickAxes->x, 0.0f, leftStickAxes->y);
		
		float directionLength = Math::Min(Vec3::Length(&direction), 1.0f);
		Vec3::Normalize(&direction, &direction);

		movementSpeed *= directionLength;

		actor->PushAlongDirection(&direction, ActorSpaceLocal, movementSpeed, true);
	}

	// Check yaw rotation.
	if (rightStickAxes->x != 0.0f)
	{
		Vec3::Set(&rotation, 0.0f, rightStickAxes->x * rotationSpeed, 0.0f);
		actor->Rotate(&rotation);
	}

	// Check pitch rotation.
	if (rightStickAxes->y != 0.0f)
	{
		Vec3::Set(&rotation, rightStickAxes->y * rotationSpeed, 0.0f, 0.0f);
		cameraSubActor->Rotate(&rotation);
	}

	// Apply gravity.
	Vec3 gravityDirection;
	Vec3::Set(&gravityDirection, 0.0f, -1.0f, 0.0f);

	actor->PushAlongDirection(&gravityDirection, ActorSpaceWorld, 0.08f * frameMultiplier, false); // TODO - get gravity constant from somewhere

	// Calculate the target.
	SubActorLocationValues* cameraLocationValues = cameraSubActor->GetLocationValues();

	Vec3 lookAt;
	Vec3::Set(&lookAt, 0.0f, 0.0f, 1.0f);
	Vec3::TransformMat3(&lookAt, &lookAt, &cameraLocationValues->worldTransform);
	
	CollisionLine collisionLine;
	collisionLine.from = cameraLocationValues->worldPosition;
	collisionLine.to = cameraLocationValues->worldPosition;
	Vec3::ScaleAndAdd(&collisionLine.to, &collisionLine.to, &lookAt, 1000.0f);
	CollisionLine::FromOwnFromAndToPoints(&collisionLine);

	SceneIntersectionResult result;
	bool collisionFound = sceneManager->FindNearestLineIntersectionWithScene(&result, &collisionLine);

	// Determine if the player should shoot.
	if (aggregatedGamePad->GetTriggerPressure(GamePadTriggerLeft) > 0.1f)
	{
		//logger->Write("Player shoot!");
		this->Shoot(actor);
	}

	// Setup the target debug visualisation.
	ActorDebugVisualisation* targetVisualisation = actor->GetDebugVisualisation(controllerData->debugVisualisationIndexes.target);

	if (collisionFound)
	{
		targetVisualisation->enabled = true;
		targetVisualisation->type = ActorDebugVisualisationTypeIcon;
		targetVisualisation->icon = DebugIconTrigger;
		targetVisualisation->position = result.intersection;
		//Vec3::Add(&targetVisualisation->position, &targetVisualisation->position, 0.0, 0.0, 1.0f);
	}
	else
	{
		targetVisualisation->enabled = false;
		logger->Write("Looking into blank space.");
	}
}*/

char* FirstPersonPlayerActorController::GetControllerName()
{
	return this->name;
}

int FirstPersonPlayerActorController::GetActorType()
{
	return ActorTypeFirstPersonPlayer;
}

bool FirstPersonPlayerActorController::CalculateCrossHairTarget(IActor* actor, Vec3* out)
{
	IEngine* engine = GetEngine();
	ISceneManager* sceneManager = engine->GetSceneManager();

	FirstPersonPlayerActorControllerData* controllerData = (FirstPersonPlayerActorControllerData*)actor->GetControllerData();
	ISubActor* cameraSubActor = actor->GetSubActor(controllerData->subActorIndexes.camera);

	// Calculate the target.
	Vec3 lookAt;
	Vec3::Set(&lookAt, 0.0f, 0.0f, 1.0f);
	Vec3::TransformMat3(&lookAt, &lookAt, cameraSubActor->GetWorldTransform());

	CollisionLine collisionLine;
	collisionLine.from = *cameraSubActor->GetWorldPosition();
	collisionLine.to = *cameraSubActor->GetWorldPosition();
	Vec3::ScaleAndAdd(&collisionLine.to, &collisionLine.to, &lookAt, 1000.0f);
	CollisionLine::FromOwnFromAndToPoints(&collisionLine);

	SceneIntersectionResult sceneIntersectionResult;
	bool intersectionFound = sceneManager->FindNearestLineIntersectionWithScene(&sceneIntersectionResult, &collisionLine, true);

	if (intersectionFound)
	{
		*out = sceneIntersectionResult.intersection;
	}

	return intersectionFound;
}

void FirstPersonPlayerActorController::Shoot(IActor* actor)
{
	IEngine* engine = GetEngine();
	ISceneManager* sceneManager = engine->GetSceneManager();
	IAssetManager* assetManager = engine->GetAssetManager();

	FirstPersonPlayerActorControllerData* controllerData = (FirstPersonPlayerActorControllerData*)actor->GetControllerData();
	ISubActor* muzzleSubActor = actor->GetSubActor(controllerData->subActorIndexes.muzzle);

	// Fire a projectile - We need to ensure that the projectile heads towards whatever the cross hair is targeting, rather than
	// tracing a ray directly from the muzzle, as this would give unexpected results (although it's actually more
	// realistic, technically).
	Vec3 crossHairTarget;
	bool targetFound = this->CalculateCrossHairTarget(actor, &crossHairTarget);

	if (targetFound)
	{
		Game::LaunchProjectileAt(actor->GetIndex(), muzzleSubActor->GetWorldPosition(), &crossHairTarget);
	}
	else
	{
		Vec3 direction;
		Vec3::Set(&direction, 0.0f, 0.0f, 1.0f);
		Vec3::TransformMat3(&direction, &direction, muzzleSubActor->GetWorldTransform());

		Game::LaunchProjectileAlongDirection(actor->GetIndex(), muzzleSubActor->GetWorldPosition(), &direction);
	}

	/*int plasmaBallActorAssetIndex = assetManager->FindAssetIndex(AssetTypeActor, "actors/plasma-ball.actor");

	Vec3 rotation;
	Vec3::Zero(&rotation);

	int projectileActorIndex = sceneManager->CreateActor(
		plasmaBallActorAssetIndex, "PlasmaBall", 
		muzzleSubActor->GetWorldPosition(), 
		&rotation, null);

	IActor* projectileActor = sceneManager->GetActor(projectileActorIndex);
	if (projectileActor != null)
	{
		LaunchedYouActorMessageData messageData;
		messageData.launcherActorIndex = actor->GetIndex();

		// We need to ensure that the projectile heads towards whatever the cross hair is targeting, rather than
		// tracing a ray directly from the muzzle, as this would give unexpected results (although it's actually more
		// realistic, technically).
		Vec3 crossHairTarget;
		bool targetFound = this->CalculateCrossHairTarget(actor, &crossHairTarget);
		
		if (targetFound)
		{
			Vec3::Sub(&messageData.direction, &crossHairTarget, muzzleSubActor->GetWorldPosition());
			Vec3::Normalize(&messageData.direction, &messageData.direction);
		}
		else
		{
			Vec3::Set(&messageData.direction, 0.0f, 0.0f, 1.0f);
			Vec3::TransformMat3(&messageData.direction, &messageData.direction, muzzleSubActor->GetWorldTransform());
		}

		//projectileActor->AddMessage(ActorMessageIdShootProjectile, &messageData, sizeof(ShootProjectileMessageData));
		projectileActor->Tell(ActorMessageIdLaunchedYou, &messageData);
	}*/

	controllerData->weaponIsCoolingDown = true;
}

void FirstPersonPlayerActorController::ApplyJsonConfig(IActor* actor, IJsonValue* jsonConfig)
{
	IEngine* engine = GetEngine();
	ISceneManager* sceneManager = engine->GetSceneManager();

	FirstPersonPlayerActorControllerData* controllerData = (FirstPersonPlayerActorControllerData*)actor->GetControllerData();

	for (int propertyIndex = 0; propertyIndex < jsonConfig->GetNumberOfObjectProperties(); propertyIndex++)
	{
		IJsonProperty* jsonProperty = jsonConfig->GetObjectProperty(propertyIndex);
		IJsonValue* jsonPropertyValue = jsonProperty->GetValue();

		if (strcmp(jsonProperty->GetName(), "movement-speed") == 0)
		{
			controllerData->movementSpeed = jsonPropertyValue->GetFloatValue();
		}
		else if (strcmp(jsonProperty->GetName(), "auto-set-as-default-camera") == 0)
		{
			if (jsonPropertyValue->GetBoolValue())
			{
				sceneManager->SetMainCameraSubActor(actor->GetIndex(), controllerData->subActorIndexes.camera);
			}
		}
	}
}

FirstPersonPlayerActorControllerData::FirstPersonPlayerActorControllerData()
{
	IFactory* factory = GetFactory();

	this->weaponCooldownTicker = factory->MakeTicker(4);
	this->weaponIsCoolingDown = true;
	memset(&this->combatStatusFacet, 0, sizeof(CombatStatusActorFacet));
	memset(&this->healthFacet, 0, sizeof(HealthActorFacet));
	memset(&this->inventoryFacet, 0, sizeof(PlayerInventoryActorFacet));
}

FirstPersonPlayerActorControllerData::~FirstPersonPlayerActorControllerData()
{
	SafeDeleteAndNull(this->weaponCooldownTicker);
}

void* FirstPersonPlayerActorControllerData::GetFacet(int facetId)
{
	void* facet = null;

	if (facetId == ActorFacetIdCombatStatus)
	{
		facet = &this->combatStatusFacet;
	}
	else if (facetId == ActorFacetIdPlayerInventory)
	{
		facet = &this->inventoryFacet;
	}

	return facet;
}