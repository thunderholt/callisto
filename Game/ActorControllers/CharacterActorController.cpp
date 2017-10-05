#include "ActorControllers/CharacterActorController.h"

CharacterActorController::CharacterActorController()
{
	strcpy(this->name, "Character");
}

CharacterActorController::~CharacterActorController()
{
}

void CharacterActorController::OneTimeInit(IActor* actor, IJsonValue* actorAssetJsonConfig, IJsonValue* actorInstanceJsonConfig)
{
	actor->SetControllerData(new CharacterActorControllerData());
	CharacterActorControllerData* controllerData = (CharacterActorControllerData*)actor->GetControllerData();
	
	controllerData->eyeSubActorIndex = actor->FindSubActorIndexByName("Eye");
	controllerData->muzzleSubActorIndex = actor->FindSubActorIndexByName("Muzzle");
	controllerData->combatStatusFacet.faction = FactionNeutral;
	
	if (actorAssetJsonConfig != null)
	{
		this->ApplyJsonConfig(actor, actorAssetJsonConfig);
	}

	if (actorInstanceJsonConfig != null)
	{
		this->ApplyJsonConfig(actor, actorInstanceJsonConfig);
	}
}

void CharacterActorController::Activate(IActor* actor)
{
	CharacterActorControllerData* controllerData = (CharacterActorControllerData*)actor->GetControllerData();

	controllerData->currentActivity = CharacterActorActivityWandering;
	Vec3::Zero(&controllerData->wanderToPoint);
	controllerData->healthFacet.totalHealth = 3;
	controllerData->healthFacet.currentHealth = controllerData->healthFacet.totalHealth;
	controllerData->enemyActorIndex = -1;
	controllerData->weaponIsCoolingDown = false;
}

void CharacterActorController::Deactivate(IActor* actor)
{
}

void CharacterActorController::Heartbeat(IActor* actor)
{
	IEngine* engine = GetEngine();
	IFrameTimer* frameTimer = engine->GetFrameTimer();

	CharacterActorControllerData* controllerData = (CharacterActorControllerData*)actor->GetControllerData();
	float frameMultiplier = frameTimer->GetMultiplier();

	//this->ClearAllDebugVisualisations(actor);

	// Determine if the character can shoot.
	if (controllerData->weaponIsCoolingDown)
	{
		if (controllerData->weaponCooldownTicker->Tick())
		{
			controllerData->weaponIsCoolingDown = false;
			controllerData->weaponCooldownTicker->Reset();
		}
	}

	// Perform the current activity.
	if (controllerData->currentActivity == CharacterActorActivityWandering)
	{
		this->PerformWanderingActivity(actor);
	}
	else if (controllerData->currentActivity == CharacterActorActivityAttacking)
	{
		this->PerformAttackingActivity(actor);
	}

	// Apply gravity.
	Vec3 gravityDirection;
	Vec3::Set(&gravityDirection, 0.0f, -1.0f, 0.0f);

	ScenePushResult scenePushResult;
	actor->PushAlongDirection(&scenePushResult, gravityDirection, ActorSpaceWorld, 0.08f * frameMultiplier, false, -1); // TODO - get gravity constant from somewhere
}

void CharacterActorController::Tell(IActor* actor, int messageId, void* data)
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	CharacterActorControllerData* controllerData = (CharacterActorControllerData*)actor->GetControllerData();

	if (messageId == ActorMessageIdSpawnedYou)
	{
		SpawnedYouMessageData* messageData = (SpawnedYouMessageData*)data;
		controllerData->spawnTriggerActorIndex = messageData->spawnTriggerActorIndex;

		this->JoinEncounter(actor);
	}
	else if (messageId == ActorMessageIdShotYou)
	{
		ShotYouActorMessageData* messageData = (ShotYouActorMessageData*)data;
		controllerData->healthFacet.currentHealth--;

		if (controllerData->healthFacet.currentHealth > 0)
		{
			logger->Write("%s: ouch! New health: %d", actor->GetName(), controllerData->healthFacet);
		}
		else
		{
			this->Die(actor);
		}
	}
}

/*void CharacterActorController::UpdateDebugVisualisations(IActor* actor)
{
	CharacterActorControllerData* controllerData = (CharacterActorControllerData*)actor->GetControllerData();

	if (controllerData->currentActivity == CharacterActorActivityWandering)
	{
		// Enable and update the wander-to-point debug visualisation.
		this->EnableWanderToPointDebugVisualisation(actor);
	}
	else if (controllerData->currentActivity == CharacterActorActivityAttacking)
	{
		
	}

	// Enable and update the eye debug visualisation.
	this->EnableEyeDebugVisualisation(actor);
}*/

char* CharacterActorController::GetControllerName()
{
	return this->name;
}

int CharacterActorController::GetActorType()
{
	return ActorTypeCharacter;
}

void CharacterActorController::ApplyJsonConfig(IActor* actor, IJsonValue* jsonConfig)
{
	CharacterActorControllerData* controllerData = (CharacterActorControllerData*)actor->GetControllerData();

	for (int propertyIndex = 0; propertyIndex < jsonConfig->GetNumberOfObjectProperties(); propertyIndex++)
	{
		IJsonProperty* jsonProperty = jsonConfig->GetObjectProperty(propertyIndex);
		IJsonValue* jsonPropertyValue = jsonProperty->GetValue();

		if (strcmp(jsonProperty->GetName(), "faction") == 0)
		{
			controllerData->combatStatusFacet.faction = Game::ParseFactionString(jsonPropertyValue->GetStringValue());
		}
	}
}

/*void CharacterActorController::PerformWanderingActivity(IActor* actor)
{
	IEngine* engine = GetEngine();
	IFrameTimer* frameTimer = engine->GetFrameTimer();
	ISceneManager* sceneManager = engine->GetSceneManager();
	ILogger* logger = engine->GetLogger();

	CharacterActorControllerData* controllerData = (CharacterActorControllerData*)actor->GetControllerData();
	ActorLocationValues* locationValues = actor->GetLocationValues();
	float frameMultiplier = frameTimer->GetMultiplier();
	float maxMovement = 0.005f * frameMultiplier; // FIXME - Load from config.

	if (controllerData->findNewWanderToPointTicker->Tick())
	{
		sceneManager->AttemptToFindRelativePointWithNoObstruction(
			&controllerData->wanderToPoint, &locationValues->worldPosition, 10, 0.5f);
	}

	actor->PushTowardsDestination(&controllerData->wanderToPoint, maxMovement, true, 0.2f, true); // TODO - max rotation from config.

	// Check for visible enemies.
	if (controllerData->checkForVisibleEnemiesTicker->Tick())
	{
		//this->CheckForVisibleEnemies(actor);
	}
	
	// Enable and update the wander-to-point debug visualisation.
	this->EnableWanderToPointDebugVisualisation(actor);
}*/

void CharacterActorController::PerformWanderingActivity(IActor* actor)
{
	IEngine* engine = GetEngine();
	IFrameTimer* frameTimer = engine->GetFrameTimer();
	ISceneManager* sceneManager = engine->GetSceneManager();
	ILogger* logger = engine->GetLogger();

	CharacterActorControllerData* controllerData = (CharacterActorControllerData*)actor->GetControllerData();
	Vec3 worldPosition = actor->GetWorldPosition();
	float frameMultiplier = frameTimer->GetMultiplier();
	float maxMovement = 0.005f * frameMultiplier; // FIXME - Load from config.

	if (controllerData->findNewWanderToPointTicker->Tick())
	{
		sceneManager->AttemptToFindRelativePointWithNoObstruction(
			&controllerData->wanderToPoint, &worldPosition, 10, 0.5f);
	}

	ScenePushResult scenePushResult;
	actor->PushTowardsDestination(&scenePushResult, controllerData->wanderToPoint, maxMovement, true, 0.2f, true, -1); // TODO - max rotation from config.

	// Check for visible enemies.
	if (controllerData->checkForVisibleEnemiesTicker->Tick())
	{
		this->CheckForVisibleEnemies(actor);
	}
}

/*void CharacterActorController::PerformAttackingActivity(IActor* actor)
{
	IEngine* engine = GetEngine();
	ISceneManager* sceneManager = engine->GetSceneManager();
	ILogger* logger = engine->GetLogger();

	CharacterActorControllerData* controllerData = (CharacterActorControllerData*)actor->GetControllerData();
	IActor* enemyActor = sceneManager->GetActor(controllerData->enemyActorIndex);

	if (enemyActor == null)
	{
		controllerData->currentActivity = CharacterActorActivityWandering;
	}
	else
	{
		// Check if we need to shoot.
		if (controllerData->shootTicker->Tick())
		{
			this->ShootAtEnemy(actor);
		}

		// Face thy enemy.
		ActorLocationValues* enemyActorLocationValues = enemyActor->GetLocationValues();
		actor->RotateToFacePoint(&enemyActorLocationValues->worldPosition, 0.2f); // TODO - max rotation from config.
	}
}*/

void CharacterActorController::PerformAttackingActivity(IActor* actor)
{
	IEngine* engine = GetEngine();
	ISceneManager* sceneManager = engine->GetSceneManager();
	ILogger* logger = engine->GetLogger();

	CharacterActorControllerData* controllerData = (CharacterActorControllerData*)actor->GetControllerData();
	IActor* enemyActor = sceneManager->GetActor(controllerData->enemyActorIndex);

	if (enemyActor == null)
	{
		controllerData->currentActivity = CharacterActorActivityWandering;
	}
	else
	{
		// Check if we need to shoot.
		if (!controllerData->weaponIsCoolingDown)
		{
			this->AttemptToShootAtEnemy(actor);
		}

		// Face thy enemy.
		Vec3 enemyWorldPosition = enemyActor->GetWorldPosition();
		actor->RotateToFacePoint(enemyWorldPosition, 0.2f); // TODO - max rotation from config.
	}
}

/*
void CharacterActorController::CheckForVisibleEnemies(IActor* actor)
{
	IEngine* engine = GetEngine();
	ISceneManager* sceneManager = engine->GetSceneManager();
	ILogger* logger = engine->GetLogger();

	//logger->Write("Actor '%s': checking for visible enemies.", actor->GetName());
	CharacterActorControllerData* controllerData = (CharacterActorControllerData*)actor->GetControllerData();

	int actorIndexes[32];
	int applicableActorTypes[] = 
	{
		ActorTypeFirstPersonPlayer,
		ActorTypeCharacter
	};

	int numberOfActorsWithinRadius = sceneManager->FindActorsWithinRadius(
		actorIndexes, 32, applicableActorTypes, 2, 
		&actor->GetLocationValues()->worldPosition, 4.0f); // FIXME - radius from config.
	
	int visibleEnemyActorIndex = -1;

	for (int i = 0; i < numberOfActorsWithinRadius; i++)
	{
		int otherActorIndex = actorIndexes[i];
		if (otherActorIndex != actor->GetIndex())
		{
			IActor* otherActor = sceneManager->GetActor(otherActorIndex);
			if (otherActor != null)
			{
				bool otherActorIsEnemy = this->CheckIfOtherActorIsEnemy(actor, otherActor);
				if (otherActorIsEnemy)
				{
					ISubActor* otherActorTargetSubActor = this->FindVisibleOtherActorTargetSubActor(actor, otherActor);
					if (otherActorTargetSubActor != null)
					{
						logger->Write("Actor '%s': Spotted enemy actor: %s - %s", actor->GetName(), otherActor->GetName(), otherActorTargetSubActor->GetName());
						visibleEnemyActorIndex = otherActor->GetIndex();
						break;
					}
				}
			}
		}
	}

	if (visibleEnemyActorIndex != -1)
	{
		controllerData->enemyActorIndex = visibleEnemyActorIndex;
		controllerData->currentActivity = CharacterActorActivityAttacking;
	}
}
*/

void CharacterActorController::CheckForVisibleEnemies(IActor* actor)
{
	IEngine* engine = GetEngine();
	ISceneManager* sceneManager = engine->GetSceneManager();
	ILogger* logger = engine->GetLogger();

	//logger->Write("Actor '%s': checking for visible enemies.", actor->GetName());
	CharacterActorControllerData* controllerData = (CharacterActorControllerData*)actor->GetControllerData();

	int actorIndexes[32];
	int applicableActorTypes[] =
	{
		ActorTypeFirstPersonPlayer,
		ActorTypeCharacter
	};

	Vec3 worldPosition = actor->GetWorldPosition();
	int numberOfActorsWithinRadius = sceneManager->FindActorsWithinRadius(
		actorIndexes, 32, applicableActorTypes, 2,
		&worldPosition, 4.0f); // FIXME - radius from config.

	int visibleEnemyActorIndex = -1;

	for (int i = 0; i < numberOfActorsWithinRadius; i++)
	{
		int otherActorIndex = actorIndexes[i];
		if (otherActorIndex != actor->GetIndex())
		{
			IActor* otherActor = sceneManager->GetActor(otherActorIndex);
			if (otherActor != null)
			{
				bool otherActorIsEnemy = this->CheckIfOtherActorIsEnemy(actor, otherActor);
				if (otherActorIsEnemy)
				{
					ISubActor* otherActorTargetSubActor = this->FindVisibleOtherActorTargetSubActor(actor, otherActor);
					if (otherActorTargetSubActor != null)
					{
						logger->Write("Actor '%s': Spotted enemy actor: %s - %s", actor->GetName(), otherActor->GetName(), otherActorTargetSubActor->GetName());
						visibleEnemyActorIndex = otherActor->GetIndex();
						break;
					}
				}
			}
		}
	}

	if (visibleEnemyActorIndex != -1)
	{
		controllerData->enemyActorIndex = visibleEnemyActorIndex;
		controllerData->currentActivity = CharacterActorActivityAttacking;
	}
}

bool CharacterActorController::CheckIfOtherActorIsEnemy(IActor* actor, IActor* otherActor)
{
	bool isEnemy = false;

	IActorControllerData* otherActorControllerData = otherActor->GetControllerData();
	if (otherActorControllerData != null)
	{
		CombatStatusActorFacet* otherActorCombatStatusFacet = (CombatStatusActorFacet*)otherActorControllerData->GetFacet(ActorFacetIdCombatStatus);
		if (otherActorCombatStatusFacet != null)
		{
			CharacterActorControllerData* controllerData = (CharacterActorControllerData*)actor->GetControllerData();
			FactionRelation factionRelation = Game::GetFactionRelation(controllerData->combatStatusFacet.faction, otherActorCombatStatusFacet->faction);
			isEnemy = factionRelation == FactionRelationEnemy;
		}
	}

	return isEnemy;
}

/*ISubActor* CharacterActorController::FindVisibleOtherActorTargetSubActor(IActor* actor, IActor* otherActor)
{
	IEngine* engine = GetEngine();
	ISceneManager* sceneManager = engine->GetSceneManager();

	ISubActor* otherActorTargetSubActorWithLineOfSight = null;

	CharacterActorControllerData* controllerData = (CharacterActorControllerData*)actor->GetControllerData();
	ISubActor* eyeSubActor = actor->GetSubActor(controllerData->eyeSubActorIndex);

	int otherActorTargetSubActorIndexes[8];
	int numberOfActorTargetSubActorIndexesFound = otherActor->FindSubActorIndexesByType(
		otherActorTargetSubActorIndexes, 8, SubActorTypeTarget);

	for (int i = 0; i < numberOfActorTargetSubActorIndexesFound; i++)
	{
		int otherActorTargetSubActorIndex = otherActorTargetSubActorIndexes[i];
		ISubActor* otherActorTargetSubActor = otherActor->GetSubActor(otherActorTargetSubActorIndex);

		if (otherActorTargetSubActor != null)
		{
			SubActorLocationValues* otherActorTargetSubActorLocationValues = otherActorTargetSubActor->GetLocationValues();

			bool hasLineOfSight = sceneManager->CheckLineOfSight(
				&eyeSubActor->GetLocationValues()->worldPosition,
				&otherActorTargetSubActorLocationValues->worldPosition);

			if (hasLineOfSight)
			{
				bool isWithinFOVCone = this->CheckPointIsWithinFOVCone(
					actor, &otherActorTargetSubActorLocationValues->worldPosition);

				if (isWithinFOVCone)
				{
					otherActorTargetSubActorWithLineOfSight = otherActorTargetSubActor;
					break;
				}
			}
		}
	}

	return otherActorTargetSubActorWithLineOfSight;
}
*/
ISubActor* CharacterActorController::FindVisibleOtherActorTargetSubActor(IActor* actor, IActor* otherActor)
{
	IEngine* engine = GetEngine();
	ISceneManager* sceneManager = engine->GetSceneManager();

	ISubActor* otherActorTargetSubActorWithLineOfSight = null;

	CharacterActorControllerData* controllerData = (CharacterActorControllerData*)actor->GetControllerData();
	ISubActor* eyeSubActor = actor->GetSubActor(controllerData->eyeSubActorIndex);

	int otherActorTargetSubActorIndexes[8];
	int numberOfActorTargetSubActorIndexesFound = otherActor->FindSubActorIndexesByType(
		otherActorTargetSubActorIndexes, 8, SubActorTypeTarget);

	for (int i = 0; i < numberOfActorTargetSubActorIndexesFound; i++)
	{
		int otherActorTargetSubActorIndex = otherActorTargetSubActorIndexes[i];
		ISubActor* otherActorTargetSubActor = otherActor->GetSubActor(otherActorTargetSubActorIndex);

		if (otherActorTargetSubActor != null)
		{
			bool hasLineOfSight = sceneManager->CheckLineOfSight(
				eyeSubActor->GetWorldPosition(),
				otherActorTargetSubActor->GetWorldPosition(), false);

			if (hasLineOfSight)
			{
				bool isWithinFOVCone = this->CheckPointIsWithinFOVCone(
					actor, otherActorTargetSubActor->GetWorldPosition());

				if (isWithinFOVCone)
				{
					otherActorTargetSubActorWithLineOfSight = otherActorTargetSubActor;
					break;
				}
			}
		}
	}

	return otherActorTargetSubActorWithLineOfSight;
}

/*
bool CharacterActorController::CheckPointIsWithinFOVCone(IActor* actor, Vec3* point)
{
	bool pointIsWithinFOVCOne = false;

	CharacterActorControllerData* controllerData = (CharacterActorControllerData*)actor->GetControllerData();
	ISubActor* eyeSubActor = actor->GetSubActor(controllerData->eyeSubActorIndex);

	Vec3 actorFacingNormal;
	Vec3::Set(&actorFacingNormal, 0.0f, 0.0f, 1.0f);
	Vec3::TransformMat3(&actorFacingNormal, &actorFacingNormal, actor->GetWorldTransform());

	Vec3 actorToPointNormal;
	Vec3::Sub(&actorToPointNormal, point, &eyeSubActor->GetLocationValues()->worldPosition);
	Vec3::Normalize(&actorToPointNormal, &actorToPointNormal);

	float delta = Vec3::Dot(&actorFacingNormal, &actorToPointNormal);
	float maxDelta = 0.5f; // TODO - load from config.

	if (delta >= maxDelta)
	{
		pointIsWithinFOVCOne = true;
	}

	return pointIsWithinFOVCOne;
}*/
bool CharacterActorController::CheckPointIsWithinFOVCone(IActor* actor, Vec3* point)
{
	bool pointIsWithinFOVCOne = false;

	CharacterActorControllerData* controllerData = (CharacterActorControllerData*)actor->GetControllerData();
	ISubActor* eyeSubActor = actor->GetSubActor(controllerData->eyeSubActorIndex);

	Vec3 actorFacingNormal;
	Vec3::Set(&actorFacingNormal, 0.0f, 0.0f, 1.0f);
	Vec3::TransformMat3(&actorFacingNormal, &actorFacingNormal, actor->GetWorldTransform());

	Vec3 actorToPointNormal;
	Vec3::Sub(&actorToPointNormal, point, eyeSubActor->GetWorldPosition());
	Vec3::Normalize(&actorToPointNormal, &actorToPointNormal);

	float delta = Vec3::Dot(&actorFacingNormal, &actorToPointNormal);
	float maxDelta = 0.5f; // TODO - load from config.

	if (delta >= maxDelta)
	{
		pointIsWithinFOVCOne = true;
	}

	return pointIsWithinFOVCOne;
}

/*bool CharacterActorController::CheckPointIsWithinFOVCone(IActor* actor, Vec3* point) 
{
	bool pointIsWithinFOVCOne = false;

	CharacterActorControllerData* controllerData = (CharacterActorControllerData*)actor->GetControllerData();
	ISubActor* eyeSubActor = actor->GetSubActor(controllerData->eyeSubActorIndex);

	Vec3 actorFacingNormal;
	Vec3::Set(&actorFacingNormal, 0.0f, 0.0f, 1.0f);
	Vec3::TransformMat3(&actorFacingNormal, &actorFacingNormal, &actor->GetLocationValues()->worldTransform);

	Vec3 actorToPointNormal;
	Vec3::Sub(&actorToPointNormal, point, &eyeSubActor->GetLocationValues()->worldPosition);
	Vec3::Normalize(&actorToPointNormal, &actorToPointNormal);

	float delta = Vec3::Dot(&actorFacingNormal, &actorToPointNormal);
	float maxDelta = 0.5f; // TODO - load from config.

	if (delta >= maxDelta)
	{
		pointIsWithinFOVCOne = true;
	}

	return pointIsWithinFOVCOne;
}*/

void CharacterActorController::AttemptToShootAtEnemy(IActor* actor)
{
	IEngine* engine = GetEngine();
	ISceneManager* sceneManager = engine->GetSceneManager();
	ILogger* logger = engine->GetLogger();

	CharacterActorControllerData* controllerData = (CharacterActorControllerData*)actor->GetControllerData();
	ISubActor* muzzleSubActor = actor->GetSubActor(controllerData->muzzleSubActorIndex);
	IActor* enemyActor = sceneManager->GetActor(controllerData->enemyActorIndex);

	// If we have line of sight to the enemy, shoot them.
	ISubActor* enemyTargetSubActor = this->FindVisibleOtherActorTargetSubActor(actor, enemyActor);
	if (enemyTargetSubActor != null)
	{
		Game::LaunchProjectileAt(
			actor->GetIndex(), 
			muzzleSubActor->GetWorldPosition(), 
			enemyTargetSubActor->GetWorldPosition());

		logger->Write("Shoot!");
	}

	controllerData->weaponIsCoolingDown = true;
}

void CharacterActorController::Die(IActor* actor)
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	logger->Write("%s: I'm dead!", actor->GetName());

	this->LeaveEncounter(actor);
	actor->Deactivate();
}

void CharacterActorController::JoinEncounter(IActor* actor)
{
	IEngine* engine = GetEngine();
	ISceneManager* sceneManager = engine->GetSceneManager();
	ILogger* logger = engine->GetLogger();

	CharacterActorControllerData* controllerData = (CharacterActorControllerData*)actor->GetControllerData();

	IActor* spawnTriggerActor = sceneManager->GetActor(controllerData->spawnTriggerActorIndex);
	if (spawnTriggerActor != null)
	{
		IActorControllerData* spawnTriggerActorControllerData = spawnTriggerActor->GetControllerData();

		EncounterActorFacet* encounterFacet = (EncounterActorFacet*)spawnTriggerActorControllerData->GetFacet(ActorFacetIdEncounter);
		if (encounterFacet != null)
		{
			encounterFacet->numberOfParticipants++;

			logger->Write("Joined encounter for '%s'. Number of participants: %d.", spawnTriggerActor->GetName(), encounterFacet->numberOfParticipants);
		}
	}
}

void CharacterActorController::LeaveEncounter(IActor* actor)
{
	IEngine* engine = GetEngine();
	ISceneManager* sceneManager = engine->GetSceneManager();
	ILogger* logger = engine->GetLogger();

	CharacterActorControllerData* controllerData = (CharacterActorControllerData*)actor->GetControllerData();

	IActor* spawnTriggerActor = sceneManager->GetActor(controllerData->spawnTriggerActorIndex);
	if (spawnTriggerActor != null)
	{
		IActorControllerData* spawnTriggerActorControllerData = spawnTriggerActor->GetControllerData();

		EncounterActorFacet* encounterFacet = (EncounterActorFacet*)spawnTriggerActorControllerData->GetFacet(ActorFacetIdEncounter);
		if (encounterFacet != null)
		{
			encounterFacet->numberOfParticipants--;

			logger->Write("Left encounter for '%s'. Number of participants: %d.", spawnTriggerActor->GetName(), encounterFacet->numberOfParticipants);

			if (encounterFacet->numberOfParticipants == 0)
			{
				EncounterCompleteMessageData messageData;
				messageData.lastParticipantWorldPosition = actor->GetWorldPosition();
				spawnTriggerActor->Tell(ActorMessageIdEncounterComplete, &messageData);
			}
		}
	}
}

/*void CharacterActorController::ClearAllDebugVisualisations(IActor* actor)
{
	CharacterActorControllerData* controllerData = (CharacterActorControllerData*)actor->GetControllerData();
	actor->GetDebugVisualisation(controllerData->debugVisualisationIndexes.eye)->enabled = false;
	actor->GetDebugVisualisation(controllerData->debugVisualisationIndexes.wanderToPoint)->enabled = false;
}*/

/*
void CharacterActorController::EnableEyeDebugVisualisation(IActor* actor)
{
	CharacterActorControllerData* controllerData = (CharacterActorControllerData*)actor->GetControllerData();
	ActorDebugVisualisation* eyeDebugVisualisation = actor->GetDebugVisualisation(
		controllerData->debugVisualisationIndexes.eye);

	eyeDebugVisualisation->enabled = true;
	eyeDebugVisualisation->type = ActorDebugVisualisationTypeIcon;
	eyeDebugVisualisation->icon = DebugIconEye;
	eyeDebugVisualisation->position = actor->GetSubActor(controllerData->eyeSubActorIndex)->GetLocationValues()->worldPosition;
}*/
/*void CharacterActorController::EnableEyeDebugVisualisation(IActor* actor)
{
	CharacterActorControllerData* controllerData = (CharacterActorControllerData*)actor->GetControllerData();
	ActorDebugVisualisation* eyeDebugVisualisation = actor->GetDebugVisualisation(
		controllerData->debugVisualisationIndexes.eye);

	eyeDebugVisualisation->enabled = true;
	eyeDebugVisualisation->type = ActorDebugVisualisationTypeIcon;
	eyeDebugVisualisation->icon = DebugIconEye;
	eyeDebugVisualisation->position = *actor->GetSubActor(controllerData->eyeSubActorIndex)->GetWorldPosition();
}*/

/*void CharacterActorController::EnableWanderToPointDebugVisualisation(IActor* actor)
{
	CharacterActorControllerData* controllerData = (CharacterActorControllerData*)actor->GetControllerData();
	ActorDebugVisualisation* wanderToPointDebugVisualisation = actor->GetDebugVisualisation(
		controllerData->debugVisualisationIndexes.wanderToPoint);

	wanderToPointDebugVisualisation->enabled = true;
	wanderToPointDebugVisualisation->type = ActorDebugVisualisationTypeIcon;
	wanderToPointDebugVisualisation->icon = DebugIconWanderToPoint;
	wanderToPointDebugVisualisation->position = controllerData->wanderToPoint;
}*/

CharacterActorControllerData::CharacterActorControllerData()
{
	IFactory* factory = GetFactory();

	this->spawnTriggerActorIndex = -1;
	this->currentActivity = CharacterActorActivityNone;

	this->findNewWanderToPointTicker = factory->MakeTicker(/*0.35f*/0.45f); // TODO - load from config
	this->checkForVisibleEnemiesTicker = factory->MakeTicker(2.0f);  // TODO - load from config
	//this->shootTicker = factory->MakeTicker(1.0f); // TODO - load from config.
	this->weaponCooldownTicker = factory->MakeTicker(4); // TODO - load from config.
}

CharacterActorControllerData::~CharacterActorControllerData()
{
	SafeDeleteAndNull(this->findNewWanderToPointTicker);
	SafeDeleteAndNull(this->checkForVisibleEnemiesTicker);
	//SafeDeleteAndNull(this->shootTicker);
	SafeDeleteAndNull(this->weaponCooldownTicker);
}

void* CharacterActorControllerData::GetFacet(int facetId)
{
	void* facet = null;

	if (facetId == ActorFacetIdCombatStatus)
	{
		facet = &this->combatStatusFacet;
	}
	else if (facetId == ActorFacetIdHealth)
	{
		facet = &this->healthFacet;
	}

	return facet;
}