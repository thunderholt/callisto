#include "Core/Actor.h"

Actor::Actor(int index)
{
	this->index = index;
	this->actorControllerIndex = -1;
	this->sourceActorAssetIndex = -1;
	this->controllerData = null;
	this->isActive = false;

	memset(&this->baseLocationData, 0, sizeof(ActorBaseLocationData));
	memset(&this->computedLocationData, 0, sizeof(ActorComputedLocationData));

	//this->debugVisualisationIndexes.aabb = this->AddDebugVisualisation();
	//this->debugVisualisationIndexes.collisionSphere = this->AddDebugVisualisation();
	//this->DisableAllDebugVisualisations();
}

Actor::~Actor()
{
	this->RemoveAllSubActors();
	SafeDeleteAndNull(this->controllerData);
}

void Actor::OneTimeInit(int actorAssetIndex, const char* name, Vec3* worldPosition, Vec3* worldRotation, IJsonValue* actorInstanceJsonConfig)
{
	IEngine* engine = GetEngine();
	IActorControllerManager* actorControllerManager = engine->GetActorControllerManager();
	IAssetManager* assetManager = engine->GetAssetManager();
	IVolumeManager* volumeManager = engine->GetVolumeManager();

	// Assign the actor's core values.
	IActorAsset* actorAsset = assetManager->GetActorAsset(actorAssetIndex);
	ActorSpec* actorSpec = actorAsset->GetActorSpec();
	strcpy(this->name, name);
	this->actorControllerIndex = actorControllerManager->FindActorControllerIndex(actorSpec->controllerName);
	this->sourceActorAssetIndex = actorAssetIndex;

	// Assign the actor base location data.
	this->baseLocationData.worldPosition = *worldPosition;
	this->baseLocationData.worldRotation = *worldRotation;
	this->baseLocationData.localAABB = actorSpec->localAABB;
	this->baseLocationData.localCollisionSphere = actorSpec->localCollisionSphere;

	// Create the sub actors.
	for (int subActorSpecIndex = 0; subActorSpecIndex < actorAsset->GetNumberOfSubActorSpecs(); subActorSpecIndex++)
	{
		SubActorSpec* subActorSpec = actorAsset->GetSubActorSpec(subActorSpecIndex);

		// Create the sub actor.
		int subActorIndex = this->AddSubActor(subActorSpec->type, subActorSpec->parentSubActorIndex, subActorSpec->debugIcon);
		ISubActor* subActor = this->GetSubActor(subActorIndex);

		// Assign the sub actor's name.
		strcpy(subActor->GetName(), subActorSpec->name);

		// Assign the sub actor's location values.
		subActor->SetLocalPositionBase(&subActorSpec->localPositionBase);
		subActor->SetLocalRotationBase(&subActorSpec->localRotationBase);

		// Set the sub actor's type-specific values.
		if (subActorSpec->type == SubActorTypeStaticMesh)
		{
			SubActorSpecStaticMeshDetails* subActorSpecStaticMeshDetails = &subActorSpec->details.staticMeshDetails;
			SubActorStaticMeshDetails* subActorStaticMeshDetails = subActor->GetStaticMeshDetails();

			// Assign the static mesh details.
			AssetRef* staticMeshAssetRef = actorAsset->GetStaticMeshAssetRef(subActorSpecStaticMeshDetails->staticMeshAssetRefIndex);
			subActorStaticMeshDetails->staticMeshAssetIndex = staticMeshAssetRef->index;
			subActorStaticMeshDetails->isCollidable = subActorSpecStaticMeshDetails->isCollidable;
		}
		else if (subActorSpec->type == SubActorTypeSkinnedMesh)
		{
			// TODO - load skinned mesh details.
		}
		else if (subActorSpec->type == SubActorTypeCamera)
		{
			SubActorSpecCameraDetails* subActorSpecCameraDetails = &subActorSpec->details.cameraDetails;
			SubActorCameraDetails* subActorCameraDetails = subActor->GetCameraDetails();

			// Assign the camera details.
			subActorCameraDetails->fov = subActorSpecCameraDetails->fov;
		}
		else if (subActorSpec->type == SubActorTypeTriggerSphere)
		{
			SubActorSpecTriggerSphereDetails* subActorSpecTriggerSphereDetails = &subActorSpec->details.triggerSphereDetails;
			SubActorTriggerSphereDetails* subActorTriggerSphereDetails = subActor->GetTriggerSphereDetails();

			// Assign the trigger sphere details.
			subActorTriggerSphereDetails->radius = subActorSpecTriggerSphereDetails->radius;
			memcpy(&subActorTriggerSphereDetails->applicableActorTypes, subActorSpecTriggerSphereDetails->applicableActorTypes, sizeof(int) * SubActorTriggerSphereMaxApplicableActorTypes);
			subActorTriggerSphereDetails->numberOfApplicableActorTypes = subActorSpecTriggerSphereDetails->numberOfApplicableActorTypes;
		}
		else if (subActorSpec->type == SubActorTypeSprite3D)
		{
			SubActorSpecSprite3DDetails* subActorSpecSprite3DDetails = &subActorSpec->details.sprite3DDetails;
			SubActorSprite3DDetails* subActorSprite3DDetails = subActor->GetSprite3DDetails();

			// Assign the sprite 3D details.
			AssetRef* spriteSheetAssetRef = actorAsset->GetSpriteSheetAssetRef(subActorSpecSprite3DDetails->spriteSheetAssetRefIndex);
			AssetRef* textureAssetRef = actorAsset->GetTextureAssetRef(subActorSpecSprite3DDetails->textureAssetRefIndex);
			ISpriteSheetAsset* spriteSheetAsset = assetManager->GetSpriteSheetAsset(spriteSheetAssetRef->index);
			subActorSprite3DDetails->spriteSheetAssetIndex = spriteSheetAssetRef->index;
			subActorSprite3DDetails->textureAssetIndex = textureAssetRef->index;
			subActorSprite3DDetails->spriteIndex = spriteSheetAsset->FindSpriteIndex(subActorSpecSprite3DDetails->spriteName);
			subActorSprite3DDetails->size = subActorSpecSprite3DDetails->size;
			subActorSprite3DDetails->rotation = subActorSpecSprite3DDetails->rotation;
		}
		else if (subActorSpec->type == SubActorTypeVolume)
		{
			SubActorSpecVolumeDetails* subActorSpecVolumeDetails = &subActorSpec->details.volumeDetails;
			SubActorVolumeDetails* subActorVolumeDetails = subActor->GetVolumeDetails();

			// Assign the volume details.
			subActorVolumeDetails->volumeIndex = subActorSpecVolumeDetails->volumeIndex;
		}
	}

	// Tell the actor's controller to initialise the actor.
	if (actorControllerIndex != -1)
	{
		IActorController* actorController = actorControllerManager->GetActorController(this->actorControllerIndex);
		actorController->OneTimeInit(this, actorAsset->GetJsonConfig(), actorInstanceJsonConfig);
	}
}

void Actor::ReInit(const char* name, Vec3* worldPosition, Vec3* worldRotation, IJsonValue* actorInstanceJsonConfig)
{
	strcpy(this->name, name);
	this->baseLocationData.worldPosition = *worldPosition;
	this->baseLocationData.worldRotation = *worldRotation;
	this->GetController()->ReInit(this, actorInstanceJsonConfig);
}

void Actor::Activate()
{
	this->GetController()->Activate(this);
	this->isActive = true;
}

void Actor::Deactivate()
{
	this->GetController()->Deactivate(this);
	this->isActive = false;
}

/*void Actor::UpdateDebugVisualisations()
{
	this->DisableAllDebugVisualisations();
	this->ResolveActorDebugVisualisations();

	for (int subActorIndex = 0; subActorIndex < this->subActorContainers.GetNumberOfIndexes(); subActorIndex++)
	{
		ISubActor* subActor = this->subActorContainers[subActorIndex].subActor;
		if (subActor != null)
		{
			this->ResolveSubActorDebugVisualisations(subActor);
		}
	}
}*/

int Actor::GetIndex()
{
	return this->index;
}

int Actor::GetControllerIndex()
{
	return this->actorControllerIndex;
}

int Actor::GetSourceActorAssetIndex()
{
	return this->sourceActorAssetIndex;
}

bool Actor::GetIsActive()
{
	return this->isActive;
}

char* Actor::GetName()
{
	return this->name;
}

Vec3 Actor::GetWorldPosition()
{
	return this->baseLocationData.worldPosition;
}

void Actor::SetWorldPosition(const Vec3* worldPostion)
{
	this->baseLocationData.worldPosition = *worldPostion;
	this->InvalidateComputedLocationData();
}

Vec3 Actor::GetWorldRotation()
{
	return this->baseLocationData.worldRotation;
}

void Actor::SetWorldRotation(const Vec3* worldRotation)
{
	this->baseLocationData.worldRotation = *worldRotation;
	this->InvalidateComputedLocationData();
}

AABB* Actor::GetWorldAABB()
{
	if (!this->computedLocationData.isValid)
	{
		this->RecalculateComputedLocationData();
	}

	return &this->computedLocationData.worldAABB;
}

Mat4* Actor::GetWorldTransform()
{
	if (!this->computedLocationData.isValid)
	{
		this->RecalculateComputedLocationData();
	}

	return &this->computedLocationData.worldTransform;
}

Sphere* Actor::GetWorldCollisionSphere()
{
	if (!this->computedLocationData.isValid)
	{
		this->RecalculateComputedLocationData();
	}

	return &this->computedLocationData.worldCollisionSphere;
}

int Actor::GetNumberOfSubActorIndexes()
{
	return this->subActors.GetNumberOfIndexes();
}

int Actor::FindSubActorIndexByName(const char* subActorName)
{
	int index = -1;

	for (int subActorIndex = 0; subActorIndex < this->subActors.GetNumberOfIndexes(); subActorIndex++)
	{
		ISubActor* subActor = this->subActors[subActorIndex];
		if (subActor != null && strcmp(subActor->GetName(), subActorName) == 0)
		{
			index = subActorIndex;
			break;
		}
	}

	return index;
}

int Actor::FindSubActorIndexesByType(int* out, int maxSubActorIndexes, SubActorType subActorType)
{
	int numberOfSubActorsFound = 0;

	for (int subActorIndex = 0; subActorIndex < this->subActors.GetNumberOfIndexes() && numberOfSubActorsFound < maxSubActorIndexes; subActorIndex++)
	{
		ISubActor* subActor = this->subActors[subActorIndex];
		if (subActor != null && subActor->GetType() == subActorType)
		{
			out[numberOfSubActorsFound] = subActorIndex;
			numberOfSubActorsFound++;
		}
	}

	return numberOfSubActorsFound;
}

ISubActor* Actor::GetSubActor(int index)
{
	return this->subActors[index];
}

void Actor::Tell(int messageId, void* data)
{
	IEngine* engine = GetEngine();
	IActorControllerManager* actorControllerManager = engine->GetActorControllerManager();

	IActorController* controller = actorControllerManager->GetActorController(this->actorControllerIndex);
	if (controller != null)
	{
		controller->Tell(this, messageId, data);
	}
}

/*int Actor::GetNumberOfDebugVisualisationIndexes()
{
	return this->debugVisualisations.GetNumberOfIndexes();
}

ActorDebugVisualisation* Actor::GetDebugVisualisation(int index)
{
	return &this->debugVisualisations[index];
}

int Actor::AddDebugVisualisation()
{
	int index = this->debugVisualisations.FindAndOccupyAvailableIndex();
	ActorDebugVisualisation* debugVisualisation = &this->debugVisualisations[index];
	debugVisualisation->enabled = false;
	return index;
}

void Actor::RemoveDebugVisualisation(int index)
{
	this->debugVisualisations.UnoccupyIndex(index);
}*/

void Actor::Translate(Vec3 direction, ActorSpace directionSpace, float distance)
{
	if (directionSpace == ActorSpaceLocal)
	{
		Vec3::TransformMat3(&direction, &direction, this->GetWorldTransform());
	}

	Vec3 worldPosition = this->GetWorldPosition();
	Vec3::ScaleAndAdd(&worldPosition, &worldPosition, &direction, distance);

	this->SetWorldPosition(&worldPosition);
}

void Actor::Rotate(Vec3 rotation)
{
	Vec3 worldRotation = this->GetWorldRotation();
	Vec3::Add(&worldRotation, &worldRotation, &rotation);

	this->SetWorldRotation(&worldRotation);
}

void Actor::Rotate(float x, float y, float z)
{
	Vec3 worldRotation = this->GetWorldRotation();
	Vec3::Add(&worldRotation, &worldRotation, x, y, z);

	this->SetWorldRotation(&worldRotation);
}

void Actor::RotateToFacePoint(Vec3 point, float maxRotation)
{
	Vec3 worldPosition = this->GetWorldPosition();
	float yAxisFacingAngle = Vec3::CalculateYAxisFacingAngle(&worldPosition, &point);

	Vec3 worldRotation = this->GetWorldRotation();
	worldRotation.y = Math::RotateTowardsTargetAngle(worldRotation.y, yAxisFacingAngle, maxRotation);

	this->SetWorldRotation(&worldRotation);
}

void Actor::PushAlongDirection(ScenePushResult* out, Vec3 direction, ActorSpace directionSpace, float distance, bool allowSliding, int ignoreActorIndex)
{
	if (directionSpace == ActorSpaceLocal)
	{
		Vec3::TransformMat3(&direction, &direction, this->GetWorldTransform());
	}

	this->Push(out, direction, distance, allowSliding, ignoreActorIndex);
}

void Actor::PushTowardsDestination(ScenePushResult* out, Vec3 destination, float maxDistance, bool rotateToFaceDestination, float maxRotation, bool allowSliding, int ignoreActorIndex)
{
	Vec3 worldPosition = this->GetWorldPosition();

	Vec3 direction;
	Vec3::Sub(&direction, &destination, &worldPosition);
	Vec3::Normalize(&direction, &direction);

	float distanceToDestination = Vec3::Distance(&worldPosition, &destination);
	float distance = Math::Min(distanceToDestination, maxDistance);

	this->Push(out, direction, distance, allowSliding, ignoreActorIndex);

	if (rotateToFaceDestination)
	{
		worldPosition = this->GetWorldPosition();
		Vec3 worldRotation = this->GetWorldRotation();

		float yAxisFacingAngle = Vec3::CalculateYAxisFacingAngle(&worldPosition, &destination);

		worldRotation.y = Math::RotateTowardsTargetAngle(worldRotation.y, yAxisFacingAngle, maxRotation);

		this->SetWorldRotation(&worldRotation);
	}
}

IActorControllerData* Actor::GetControllerData()
{
	return this->controllerData;
}

void Actor::SetControllerData(IActorControllerData* controllerData)
{
	if (this->controllerData != null && this->controllerData != controllerData)
	{
		SafeDeleteAndNull(this->controllerData);
	}

	this->controllerData = controllerData;
}

IActorController* Actor::GetController()
{
	IEngine* engine = GetEngine();
	IActorControllerManager* actorControllerManager = engine->GetActorControllerManager();

	IActorController* controller = actorControllerManager->GetActorController(this->actorControllerIndex);
	return controller;
}

int Actor::AddSubActor(SubActorType type, int parentSubActorIndex, DebugIcon debugIcon)
{
	IFactory* factory = GetFactory();

	// Create the sub actor.
	int subActorIndex = this->subActors.FindAndOccupyAvailableIndex();
	ISubActor* subActor = factory->MakeSubActor(this->index, subActorIndex, type, parentSubActorIndex, debugIcon);
	this->subActors[subActorIndex] = subActor;

	/*// Clear all of the sub actor's debug visualisation indexes.
	for (int i = 0; i < SubActorMaxNumberOfDebugVisualisationIndexes; i++)
	{
		container->debugVisualisationIndexes[i] = -1;
	}

	// Setup the sub actor's debug visualisations.
	if (subActor->GetType() == SubActorTypeTriggerSphere)
	{
		container->debugVisualisationIndexes[SubActorTriggerSphere_DebugVisualisationIndex_TriggerIcon] = this->AddDebugVisualisation();
		container->debugVisualisationIndexes[SubActorTriggerSphere_DebugVisualisationIndex_LineSphere] = this->AddDebugVisualisation();
	}
	else if (subActor->GetType() == SubActorTypeTarget)
	{
		container->debugVisualisationIndexes[SubActorTarget_DebugVisualisationIndex_TargetIcon] = this->AddDebugVisualisation();
	}*/

	return subActorIndex;
}

void Actor::RemoveSubActor(int index)
{
	SafeDeleteAndNull(this->subActors[index]);
	this->subActors.UnoccupyIndex(index);
}

void Actor::RemoveAllSubActors()
{
	for (int subActorIndex = 0; subActorIndex < this->subActors.GetNumberOfIndexes(); subActorIndex++)
	{
		this->RemoveSubActor(subActorIndex);
	}
}

void Actor::InvalidateComputedLocationData()
{
	this->computedLocationData.isValid = false;

	for (int subActorIndex = 0; subActorIndex < this->GetNumberOfSubActorIndexes(); subActorIndex++)
	{
		ISubActor* subActor = this->GetSubActor(subActorIndex);
		if (subActor != null && subActor->GetParentSubActorIndex() == -1)
		{
			subActor->InvalidateHierachicalLocationData();
		}
	}
}

void Actor::RecalculateComputedLocationData()
{
	// Calculate the actor's world AABB.
	AABB::Translate(&this->computedLocationData.worldAABB, &this->baseLocationData.localAABB, &this->baseLocationData.worldPosition);

	// Calculate the actor's world collision sphere.
	Sphere::Translate(&this->computedLocationData.worldCollisionSphere, &this->baseLocationData.localCollisionSphere, &this->baseLocationData.worldPosition);

	// Calculate the actor's world transform.
	Math::CreateTransformFromPostionAndEulerRotationXZY(
		&this->computedLocationData.worldTransform, &this->baseLocationData.worldPosition, &this->baseLocationData.worldRotation);

	// The computed location data is now valid.
	this->computedLocationData.isValid = true;
}

void Actor::Push(ScenePushResult* out, Vec3 direction, float distance, bool allowSliding, int ignoreActorIndex)
{
	IEngine* engine = GetEngine();
	ISceneManager* sceneManager = engine->GetSceneManager();

	Sphere* worldCollisionSphere = this->GetWorldCollisionSphere();

	ScenePushParameters scenePushParameters;
	memset(&scenePushParameters, 0, sizeof(ScenePushParameters));
	scenePushParameters.sphere = *worldCollisionSphere;
	scenePushParameters.desiredDirection = direction;
	scenePushParameters.desiredDistance = distance;
	scenePushParameters.allowSliding = allowSliding;
	scenePushParameters.intersectionMask.primaryActorToIgnoreIndex = this->index;
	scenePushParameters.intersectionMask.otherActorsToIgnoreIndexes = &ignoreActorIndex;
	scenePushParameters.intersectionMask.numberOfOtherActorsToIgnore = 1;
	
	sceneManager->PushSphereThroughScene(out, &scenePushParameters);

	Vec3 translation;
	Vec3::Sub(&translation, &scenePushParameters.sphere.position, &worldCollisionSphere->position);

	Vec3 worldPosition = this->GetWorldPosition();
	Vec3::Add(&worldPosition, &worldPosition, &translation);

	this->SetWorldPosition(&worldPosition);
}

/*void Actor::DisableAllDebugVisualisations()
{
	// Disable all of the debug visualisations.
	for (int i = 0; i < this->debugVisualisations.GetNumberOfIndexes(); i++)
	{
		ActorDebugVisualisation* debugVisualisation = &this->debugVisualisations[i];
		if (debugVisualisation != null)
		{
			debugVisualisation->enabled = false;
		}
	}
}

void Actor::ResolveActorDebugVisualisations()
{
	IEngine* engine = GetEngine();
	IActorControllerManager* actorControllerManager = engine->GetActorControllerManager();

	// Setup the AABB debug visualisation.
	int lineCubeIndex = this->debugVisualisationIndexes.aabb;
	ActorDebugVisualisation* lineCube = &this->debugVisualisations[lineCubeIndex];
	lineCube->enabled = true;
	lineCube->type = ActorDebugVisualisationTypeLineCube;
	lineCube->position = this->computedLocationData.worldAABB.from;
	AABB::CalculateSize(&lineCube->size, &this->computedLocationData.worldAABB);

	// Setup the collision sphere debug visualisation.
	if (this->computedLocationData.worldCollisionSphere.radius > 0.0f)
	{
		int lineSphereIndex = this->debugVisualisationIndexes.collisionSphere;
		ActorDebugVisualisation* lineSphere = &this->debugVisualisations[lineSphereIndex];
		lineSphere->enabled = true;
		lineSphere->type = ActorDebugVisualisationTypeLineSphere;
		lineSphere->position = this->computedLocationData.worldCollisionSphere.position;
		lineSphere->radius = this->computedLocationData.worldCollisionSphere.radius;
	}

	// Setup the controller's debug visualisations.
	IActorController* controller = actorControllerManager->GetActorController(this->actorControllerIndex);
	if (controller != null)
	{
		controller->UpdateDebugVisualisations(this);
	}
}*/

/*void Actor::ResolveSubActorDebugVisualisations(ISubActor* subActor)
{
	SubActorContainer* container = &this->subActorContainers[subActor->GetIndex()];

	if (subActor->GetType() == SubActorTypeTriggerSphere)
	{
		// Setup the trigger icon.
		int triggerIconIndex = container->debugVisualisationIndexes[SubActorTriggerSphere_DebugVisualisationIndex_TriggerIcon];
		ActorDebugVisualisation* triggerIcon = &this->debugVisualisations[triggerIconIndex];
		triggerIcon->enabled = true;
		triggerIcon->type = ActorDebugVisualisationTypeIcon;
		triggerIcon->icon = DebugIconTrigger;
		triggerIcon->position = *subActor->GetWorldPosition();

		// Setup the line sphere.
		int lineSphereIndex = container->debugVisualisationIndexes[SubActorTriggerSphere_DebugVisualisationIndex_LineSphere];
		ActorDebugVisualisation* lineSphere = &this->debugVisualisations[lineSphereIndex];
		lineSphere->enabled = true;
		lineSphere->type = ActorDebugVisualisationTypeLineSphere;
		lineSphere->position = *subActor->GetWorldPosition();
		lineSphere->radius = subActor->GetTriggerSphereDetails()->radius;
	}
	else if (subActor->GetType() == SubActorTypeTarget)
	{
		// Setup the target icon.
		int targetIconIndex = container->debugVisualisationIndexes[SubActorTarget_DebugVisualisationIndex_TargetIcon];
		ActorDebugVisualisation* triggerIcon = &this->debugVisualisations[targetIconIndex];
		triggerIcon->enabled = true;
		triggerIcon->type = ActorDebugVisualisationTypeIcon;
		triggerIcon->icon = DebugIconTarget;
		triggerIcon->position = *subActor->GetWorldPosition();
	}
}*/