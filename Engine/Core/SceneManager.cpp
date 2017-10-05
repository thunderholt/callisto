#include "Core/SceneManager.h"

SceneManager::SceneManager()
{
	this->isInitialised = false;
	this->mainCameraActorIndex = -1;
	this->mainCameraSubActorIndex = -1;
}

SceneManager::~SceneManager()
{
	//this->DeleteAllActorControllers();
	this->DeleteAllActors();
}

void SceneManager::Reset()
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	logger->Write("Resetting scene...");

	this->isInitialised = false;
	this->DeleteAllActors();
	this->mainCameraActorIndex = -1;
	this->mainCameraSubActorIndex = -1;

	logger->Write("... scene reset successfully.");
}

void SceneManager::Init()
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();
	IAssetManager* assetManager = engine->GetAssetManager();
	AssetRef* mapAssetRef = engine->GetMapAssetRef();
	IMapAsset* mapAsset = assetManager->GetMapAsset(mapAssetRef->index);

	logger->Write("Initialising scene...");

	for (int i = 0; i < mapAsset->GetNumberOfActorInstances(); i++)
	{
		IActorInstance* actorInstance = mapAsset->GetActorInstance(i);
		
		this->CreateActor(
			actorInstance->GetActorAssetRef()->index, 
			actorInstance->GetName(),
			*actorInstance->GetWorldPosition(), 
			*actorInstance->GetWorldRotation(),
			actorInstance->GetJsonConfig());
	}

	this->isInitialised = true;

	logger->Write("... scene initialised successfully.");
}

/*void SceneManager::RegisterActorController(IActorController* actorController)
{
	this->actorControllers.AddItem(actorController);
}

int SceneManager::FindActorControllerIndex(const char* actorControllerName)
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
}*/

/*int SceneManager::CreateActor(int actorControllerIndex)
{
	IFactory* factory = GetFactory();

	int actorIndex = this->actors.FindAndOccupyAvailableIndex();
	IActor* actor = factory->MakeActor(actorIndex, actorControllerIndex);
	this->actors[actorIndex] = actor;

	return actor->GetIndex();
}*/

int SceneManager::CreateActor(int actorAssetIndex, const char* name, Vec3 worldPosition, Vec3 worldRotation, IJsonValue* actorInstanceJsonConfig)
{
	IFactory* factory = GetFactory();
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	IActor* actor = null;

	// See if we can re-use a suitable deactivated actor.
	int actorIndex = this->FindDeactivatedActor(actorAssetIndex);
	if (actorIndex == -1)
	{
		// Create the actor.
		actorIndex = this->actors.FindAndOccupyAvailableIndex();
		actor = factory->MakeActor(actorIndex);
		this->actors[actorIndex] = actor;

		// One-time init the actor.
		actor->OneTimeInit(actorAssetIndex, name, &worldPosition, &worldRotation, actorInstanceJsonConfig);

		logger->Write("Created new actor instance.");
	}
	else
	{
		// Grab the existing actor.
		actor = this->actors[actorIndex];

		// Re-init the existing actor.
		actor->ReInit(name, &worldPosition, &worldRotation);

		logger->Write("Re-used existing deactivated actor instance.");
	}

	// Activate the actor.
	actor->Activate();

	return actor->GetIndex();
}

void SceneManager::DeleteActor(int actorIndex)
{
	SafeDeleteAndNull(this->actors[actorIndex]);
	this->actors.UnoccupyIndex(actorIndex);
}

bool SceneManager::GetIsInitialised()
{
	return this->isInitialised;
}

void SceneManager::ExecuteGameLogic()
{
	IEngine* engine = GetEngine();
	IActorControllerManager* actorControllerManager = engine->GetActorControllerManager();

	for (int actorIndex = 0; actorIndex < this->actors.GetNumberOfIndexes(); actorIndex++)
	{
		IActor* actor = this->GetActor(actorIndex);
		if (actor != null)
		{
			int actorControllerIndex = actor->GetControllerIndex();
			if (actorControllerIndex != -1)
			{
				IActorController* actorController = actorControllerManager->GetActorController(actorControllerIndex);
				actorController->Heartbeat(actor);
			}
		}
	}
}

/*void SceneManager::UpdateDebugVisualisations()
{
	// Resolve each actor's misc computed values.
	for (int actorIndex = 0; actorIndex < this->actors.GetNumberOfIndexes(); actorIndex++)
	{
		IActor* actor = this->GetActor(actorIndex);
		if (actor != null)
		{
			actor->UpdateDebugVisualisations();
		}
	}
}*/

IActor* SceneManager::GetActor(int index)
{
	IActor* actor = null;
	
	if (index >= 0 && index < this->actors.GetNumberOfIndexes())
	{
		actor = this->actors[index];
	}
	
	if (actor != null && !actor->GetIsActive())
	{
		actor = null;
	}

	return actor;
}

int SceneManager::GetNumberOfActorIndexes()
{
	return this->actors.GetNumberOfIndexes();
}

IActor* SceneManager::FindActorByName(const char* actorName)
{
	IActor* applicableActor = null;

	for (int actorIndex = 0; actorIndex < this->actors.GetNumberOfIndexes(); actorIndex++)
	{
		IActor* actor = this->GetActor(actorIndex);
		if (actor != null && strcmp(actor->GetName(), actorName) == 0)
		{
			applicableActor = actor;
			break;
		}
	}

	return applicableActor;
}

int SceneManager::FindDeactivatedActor(int requiredSourceActorAssetIndex)
{
	int actorIndex = -1;

	for (int i = 0; i < this->actors.GetNumberOfIndexes(); i++)
	{
		IActor* actor = this->actors[i];
		if (actor != null && !actor->GetIsActive() && actor->GetSourceActorAssetIndex() == requiredSourceActorAssetIndex)
		{
			actorIndex = i;
			break;
		}
	}

	return actorIndex;
}

void SceneManager::DeleteAllActors()
{
	for (int actorIndex = 0; actorIndex < this->actors.GetNumberOfIndexes(); actorIndex++)
	{
		this->DeleteActor(actorIndex);
	}
}

void SceneManager::SetMainCameraSubActor(int actorIndex, int subActorIndex)
{
	this->mainCameraActorIndex = actorIndex;
	this->mainCameraSubActorIndex = subActorIndex;
}

ISubActor* SceneManager::GetMainCameraSubActor()
{
	ISubActor* cameraSubActor = null;

	if (this->mainCameraActorIndex != -1 && this->mainCameraSubActorIndex != -1)
	{
		IActor* actor = this->actors[this->mainCameraActorIndex];
		if (actor != null)
		{
			cameraSubActor = actor->GetSubActor(this->mainCameraSubActorIndex);
		}
	}

	return cameraSubActor;
}

void SceneManager::PushSphereThroughScene(
	ScenePushResult* out, ScenePushParameters* parameters)
{
	SceneCollisionData sceneCollisionData;
	this->BuildSceneCollisionData(&sceneCollisionData, &parameters->intersectionMask, true);

	if (out != null)
	{
		memset(out, 0, sizeof(ScenePushResult));
	}

	// Do the math!
	this->PushSphereThroughSceneInternal(
		out, &parameters->sphere, &sceneCollisionData,
		&parameters->desiredDirection, parameters->desiredDistance, parameters->allowSliding, 0);
}

bool SceneManager::FindNearestLineIntersectionWithScene(SceneIntersectionResult* out, CollisionLine* collisionLine, bool includeVolumes)
{
	SceneCollisionData sceneCollisionData;
	this->BuildSceneCollisionData(&sceneCollisionData, null, includeVolumes);

	// Find the nearest intersection in all of the supplied collision meshes.
	bool intersectionFound = false;
	CollisionMeshIntersectionResult nearestIntersectionResult;
	float nearestIntersectionDistanceSqr = -1.0f;
	int nearestIntersectionCollisionMeshIndex = -1;

	for (int collisionMeshIndex = 0; collisionMeshIndex < sceneCollisionData.numberOfCollisionMeshes; collisionMeshIndex++)
	{
		SceneCollisionDataMesh* mesh = &sceneCollisionData.meshes[collisionMeshIndex];
		ICollisionMesh* collisionMesh = mesh->collisionMesh;
		
		// Transform the line into the collision mesh's local space.
		CollisionLine transformedLine;
		CollisionLine::TransformMat4(&transformedLine, collisionLine, mesh->inverseWorldTransform);

		// Attempt to find the nearest intersection with this collision mesh.
		CollisionMeshIntersectionResult nearestIntersectionResultForThisMesh;
		bool intersectionFoundForThisMesh = collisionMesh->FindNearestLineIntersection(
			&nearestIntersectionResultForThisMesh, &transformedLine);

		if (intersectionFoundForThisMesh)
		{
			float intersectionDistanceSqrForThisMesh = Vec3::DistanceSqr(
				&transformedLine.from, &nearestIntersectionResultForThisMesh.intersectionPoint);

			// See if the nearest intersection for this meshes is the nearest of all of them so far.
			if (!intersectionFound || intersectionDistanceSqrForThisMesh < nearestIntersectionDistanceSqr)
			{
				intersectionFound = true;
				nearestIntersectionResult = nearestIntersectionResultForThisMesh;
				nearestIntersectionDistanceSqr = intersectionDistanceSqrForThisMesh;
				nearestIntersectionCollisionMeshIndex = collisionMeshIndex;
			}
		}
	}

	// If we have an intersection, transform it back into world space and populate the result.
	if (intersectionFound && out != null)
	{
		SceneCollisionDataMesh* mesh = &sceneCollisionData.meshes[nearestIntersectionCollisionMeshIndex];
		Vec3::TransformMat4(&out->intersection, &nearestIntersectionResult.intersectionPoint, mesh->worldTransform);

		if (mesh->type == SceneCollisionDataMeshTypeWorldMesh)
		{
			out->type = SceneIntersectionTypeWorldMesh;
		}
		else if (mesh->type == SceneCollisionDataMeshTypeActor)
		{
			out->type = SceneIntersectionTypeActor;
			out->actorIndex = mesh->actorIndex;
		}

		CollisionMeshChunk* chunk = mesh->collisionMesh->GetChunk(nearestIntersectionResult.chunkIndex);
		out->materialAssetIndex = chunk->materialAssetIndex;
	}

	return intersectionFound;
}

bool SceneManager::CheckLineOfSight(Vec3* from, Vec3* to, bool includeVolumes)
{
	CollisionLine collisionLine;
	collisionLine.from = *from;
	collisionLine.to = *to;
	CollisionLine::FromOwnFromAndToPoints(&collisionLine);

	bool obstructionFound = this->FindNearestLineIntersectionWithScene(null, &collisionLine, includeVolumes); // FIXME - find *any* intersection will be faster.

	return !obstructionFound;
}

bool SceneManager::AttemptToFindRelativePointWithNoObstruction(
	Vec3* out, Vec3* from, int maximumAttempts, float maximumOffset) 
{
	bool pointFound = false;

	for (int attempt = 0; attempt < maximumAttempts; attempt++) 
	{
		Vec3 purturbedFrom = *from;
		purturbedFrom.x += Math::GenerateRandomFloat(-maximumOffset, maximumOffset);
		purturbedFrom.z += Math::GenerateRandomFloat(-maximumOffset, maximumOffset);
		*out = purturbedFrom;
		if (this->CheckLineOfSight(from, &purturbedFrom, true)) 
		{
			
			pointFound = true;
			break;
		}
	}

	return pointFound;
}

/*int SceneManager::FindActorsWithinRadius(int* out, int maxActorIndexes, int* applicableActorTypes, int numberOfApplicableActorTypes, Vec3* position, float radius)
{
	IEngine* engine = GetEngine();

	int numberOfActorsFound = 0;
	float radiusSqr = radius * radius;

	for (int actorIndex = 0; actorIndex < this->actors.GetNumberOfIndexes() && numberOfActorsFound < maxActorIndexes; actorIndex++)
	{
		IActor* actor = this->actors[actorIndex];
		if (actor != null)
		{
			bool actorIsOfApplicableType = this->CheckIfActorIsOfType(actorIndex, applicableActorTypes, numberOfApplicableActorTypes);

			if (actorIsOfApplicableType)
			{
				float actorDistanceFromPositionSqr = Vec3::DistanceSqr(position, &actor->GetLocationValues()->worldPosition);
				if (actorDistanceFromPositionSqr <= radiusSqr)
				{
					out[numberOfActorsFound] = actorIndex;
					numberOfActorsFound++;
				}
			}
		}
	}

	return numberOfActorsFound;
}*/

int SceneManager::FindActorsWithinRadius(int* out, int maxActorIndexes, int* applicableActorTypes, int numberOfApplicableActorTypes, Vec3* position, float radius)
{
	IEngine* engine = GetEngine();

	int numberOfActorsFound = 0;
	float radiusSqr = radius * radius;

	for (int actorIndex = 0; actorIndex < this->actors.GetNumberOfIndexes() && numberOfActorsFound < maxActorIndexes; actorIndex++)
	{
		IActor* actor = this->GetActor(actorIndex);
		if (actor != null)
		{
			bool actorIsOfApplicableType = this->CheckIfActorIsOfType(actorIndex, applicableActorTypes, numberOfApplicableActorTypes);

			if (actorIsOfApplicableType)
			{
				Vec3 actorWorldPosition = actor->GetWorldPosition();
				float actorDistanceFromPositionSqr = Vec3::DistanceSqr(position, &actorWorldPosition);
				if (actorDistanceFromPositionSqr <= radiusSqr)
				{
					out[numberOfActorsFound] = actorIndex;
					numberOfActorsFound++;
				}
			}
		}
	}

	return numberOfActorsFound;
}

bool SceneManager::CheckIfActorIsOfType(int actorIndex, int* applicableActorTypes, int numberOfApplicableActorTypes)
{
	IEngine* engine = GetEngine();
	IActorControllerManager* actorControllerManager = engine->GetActorControllerManager();

	bool actorIsOfApplicableType = false;

	if (numberOfApplicableActorTypes == 0)
	{
		actorIsOfApplicableType = true;
	}
	else
	{
		IActor* actor = this->GetActor(actorIndex);
		if (actor != null)
		{
			for (int i = 0; i < numberOfApplicableActorTypes; i++)
			{
				IActorController* actorController = actorControllerManager->GetActorController(actor->GetControllerIndex());

				if (actorController->GetActorType() == applicableActorTypes[i])
				{
					actorIsOfApplicableType = true;
					break;
				}
			}
		}
	}

	return actorIsOfApplicableType;
}

void SceneManager::BuildSceneCollisionData(SceneCollisionData* out, SceneIntersectionMask* intersectionMask, bool includeVolumes)
{
	IEngine* engine = GetEngine();
	IAssetManager* assetManager = engine->GetAssetManager();
	IVolumeManager* volumeManager = engine->GetVolumeManager();

	out->numberOfCollisionMeshes = 0;
	Mat4::CreateIdentity(&out->identity);

	// Add the world mesh's collision mesh.
	IMapAsset* mapAsset = assetManager->GetMapAsset(engine->GetMapAssetRef()->index);
	IWorldMeshAsset* worldMeshAsset = assetManager->GetWorldMeshAsset(mapAsset->GetWorldMeshAssetRef()->index);

	SceneCollisionDataMesh* mesh = &out->meshes[0];
	mesh->type = SceneCollisionDataMeshTypeWorldMesh;
	mesh->collisionMesh = worldMeshAsset->GetCollisionMesh();
	mesh->worldTransform = &out->identity;
	mesh->inverseWorldTransform = &out->identity;
	out->numberOfCollisionMeshes++;

	// Add collision meshes for the actor static meshes and volumes.
	// TODO - Check actor AABBs.
	for (int actorIndex = 0; actorIndex < this->actors.GetNumberOfIndexes(); actorIndex++)
	{
		// Determine if we should ignore this actor.
		bool ignoreThisActor = false;

		if (intersectionMask != null)
		{
			if (actorIndex == intersectionMask->primaryActorToIgnoreIndex)
			{
				ignoreThisActor = true;
			}
			else if (intersectionMask->otherActorsToIgnoreIndexes != null && intersectionMask->numberOfOtherActorsToIgnore > 0)
			{
				for (int i = 0; i < intersectionMask->numberOfOtherActorsToIgnore; i++)
				{
					if (actorIndex == intersectionMask->otherActorsToIgnoreIndexes[i])
					{
						ignoreThisActor = true;
						break;
					}
				}
			}
		}

		if (ignoreThisActor)
		{
			continue;
		}

		IActor* actor = this->GetActor(actorIndex);
		if (actor != null)
		{
			for (int subActorIndex = 0; subActorIndex < actor->GetNumberOfSubActorIndexes(); subActorIndex++)
			{
				ISubActor* subActor = actor->GetSubActor(subActorIndex);
				if (subActor != null)
				{
					ICollisionMesh* collisionMesh = null;

					if (subActor->GetType() == SubActorTypeStaticMesh &&
						subActor->GetStaticMeshDetails()->isCollidable)
					{
						IStaticMeshAsset* staticMeshAsset = assetManager->GetStaticMeshAsset(subActor->GetStaticMeshDetails()->staticMeshAssetIndex);
						collisionMesh = staticMeshAsset->GetCollisionMesh();
					}
					else if (subActor->GetType() == SubActorTypeVolume && includeVolumes)
					{
						int volumeIndex = subActor->GetVolumeDetails()->volumeIndex;
						IVolume* volume = volumeManager->GetVolume(volumeIndex);
						collisionMesh = volume->GetCollisionMesh();
					}

					if (collisionMesh != null)
					{
						mesh = &out->meshes[out->numberOfCollisionMeshes];
						mesh->type = SceneCollisionDataMeshTypeActor;
						mesh->collisionMesh = collisionMesh;
						mesh->worldTransform = subActor->GetWorldTransform();
						mesh->inverseWorldTransform = subActor->GetInverseWorldTransform();
						mesh->actorIndex = actorIndex;
						out->numberOfCollisionMeshes++;
					}
				}
			}
		}
	}
}

void SceneManager::PushSphereThroughSceneInternal(
	ScenePushResult* out, Sphere* sphere, SceneCollisionData* sceneCollisionData,
	Vec3* desiredDirection, float desiredDistance, bool allowSliding, int recursionDepth) 
{
	// If we don't want to move anywhere, then don't bother.
	if (Vec3::IsZero(desiredDirection))
	{
		return;
	}

	// Find the nearest collision in all of the supplied collision meshes.
	bool potentialCollisionFound = false;
	CollisionMeshIntersectionResult nearestIntersectionResult;
	int nearestIntersectionResultCollisionMeshIndex = -1;

	for (int collisionMeshIndex = 0; collisionMeshIndex < sceneCollisionData->numberOfCollisionMeshes; collisionMeshIndex++)
	{
		SceneCollisionDataMesh* mesh = &sceneCollisionData->meshes[collisionMeshIndex];
		ICollisionMesh* collisionMesh = mesh->collisionMesh;
		Mat4* collisionMeshInverseWorldMatrix = mesh->inverseWorldTransform;

		// Transform the sphere into the collision mesh's local space.
		Sphere transformedSphere;
		transformedSphere.radius = sphere->radius;
		Vec3::TransformMat4(&transformedSphere.position, &sphere->position, collisionMeshInverseWorldMatrix);

		// Transform the desired direction into the collision mesh's local space.
		Vec3 transformedDesiredDirection;
		Vec3::TransformMat3(&transformedDesiredDirection, desiredDirection, collisionMeshInverseWorldMatrix);
		
		// Attempt to find the nearest collision with this collision mesh.
		CollisionMeshIntersectionResult nearestIntersectionResultForThisMesh;
		bool collisionFoundForThisMesh = collisionMesh->FindNearestSphereCollision(
			&nearestIntersectionResultForThisMesh, &transformedSphere, &transformedDesiredDirection);

		if (collisionFoundForThisMesh)
		{
			// See if the nearest collision for this meshes is the nearest of all of them so far.
			if (!potentialCollisionFound || nearestIntersectionResultForThisMesh.distance < nearestIntersectionResult.distance)
			{
				potentialCollisionFound = true;
				nearestIntersectionResult = nearestIntersectionResultForThisMesh;
				nearestIntersectionResultCollisionMeshIndex = collisionMeshIndex;
			}
		}
	}

	// Determine how far we can move along the desired direction without colliding with anything.
	float maximumDesiredDirectionMovementDistance = desiredDistance;
	bool actualCollisionFound = false;

	if (potentialCollisionFound)
	{
		// Fudging - hopefully get rid of this once the algorithm is cleaned up!
		nearestIntersectionResult.distance -= 0.01f;
		if (nearestIntersectionResult.distance < 0.0f) {
			nearestIntersectionResult.distance = 0.0f;
		}

		if (maximumDesiredDirectionMovementDistance > nearestIntersectionResult.distance)
		{
			maximumDesiredDirectionMovementDistance = nearestIntersectionResult.distance;
			actualCollisionFound = true;
		}
	}

	// Move along the desired direction by the maximum distance allowed.
	Vec3::ScaleAndAdd(
		&sphere->position, &sphere->position, 
		desiredDirection, maximumDesiredDirectionMovementDistance);

	// Update the out result if we have one.
	if (actualCollisionFound && out != null)
	{
		SceneIntersectionResult* sceneIntersectionResult = &out->sceneIntersectionResults[out->numberOfSceneIntersectionResults];

		SceneCollisionDataMesh* mesh = &sceneCollisionData->meshes[nearestIntersectionResultCollisionMeshIndex];
		CollisionMeshChunk* chunk = mesh->collisionMesh->GetChunk(nearestIntersectionResult.chunkIndex);
		
		// The intersection point will be in the collision mesh's local space, so we'll need to convert it 
		// back into world space.
		Vec3::TransformMat3(
			&sceneIntersectionResult->intersection, &nearestIntersectionResult.intersectionPoint,
			mesh->worldTransform);

		if (mesh->type == SceneCollisionDataMeshTypeWorldMesh)
		{
			sceneIntersectionResult->type = SceneIntersectionTypeWorldMesh;
		}
		else if (mesh->type == SceneCollisionDataMeshTypeActor)
		{ 
			sceneIntersectionResult->type = SceneIntersectionTypeActor;
			sceneIntersectionResult->actorIndex = mesh->actorIndex;
		}

		sceneIntersectionResult->materialAssetIndex = chunk->materialAssetIndex;

		out->numberOfSceneIntersectionResults++;
	}

	// If we haven't recursed too many times, see if we can slide.
	if (recursionDepth < ScenePushMaxRecursions - 1)
	{
		// Attempt to travel the remaining desired distance.
		float remainingDistance = desiredDistance - maximumDesiredDirectionMovementDistance;

		if (allowSliding && remainingDistance > 0.0f)
		{
			// Remember that the slide reaction will be computed in the collision mesh's local space, so we'll need to convert the desired direction into local
			// space first, then convert the slide reaction back into world space.
			SceneCollisionDataMesh* mesh = &sceneCollisionData->meshes[nearestIntersectionResultCollisionMeshIndex];

			Vec3 transformedDesiredDirection;
			Vec3::TransformMat3(
				&transformedDesiredDirection, desiredDirection,
				mesh->inverseWorldTransform);

			// Calculate the slide reaction.
			PlaneCollisionSlideReaction slideReaction;
			bool canSlideReact = Plane::CalculatePlaneCollisionSlideReaction(
				&slideReaction, &nearestIntersectionResult.intersectionPlane, &nearestIntersectionResult.intersectionPoint,
				&transformedDesiredDirection, remainingDistance);

			// Attempt to move through the scene along the slide reaction vector.
			if (canSlideReact && slideReaction.distance > 0.0f)
			{
				Vec3::TransformMat3(
					&slideReaction.direction, &slideReaction.direction,
					mesh->worldTransform);

				this->PushSphereThroughSceneInternal(
					out, sphere, sceneCollisionData,
					&slideReaction.direction, slideReaction.distance, true, recursionDepth + 1);
			}
		}
	}
}