#include "Core/SubActor.h"

SubActor::SubActor(int ownerActorIndex, int subActorIndex, SubActorType type, int parentSubActorIndex, DebugIcon debugIcon)
{
	this->ownerActorIndex = ownerActorIndex;
	this->index = subActorIndex;
	this->type = type;
	this->parentSubActorIndex = parentSubActorIndex;
	this->debugIcon = debugIcon;

	memset(&this->baseLocationData, 0, sizeof(SubBaseActorLocationData));
	memset(&this->nonHierachicalComputedLocationData, 0, sizeof(SubActorNonHierachicalComputedLocationData));
	memset(&this->hierachicalComputedLocationData, 0, sizeof(SubActorHierachicalComputedLocationData));
	memset(&this->details, 0, sizeof(SubActorDetails));
	memset(&this->state, 0, sizeof(SubActorState));
}

SubActor::~SubActor() 
{

}

int SubActor::GetOwnerActorIndex()
{
	return this->ownerActorIndex;
}

int SubActor::GetIndex()
{
	return this->index;
}

char* SubActor::GetName()
{
	return this->name;
}

SubActorType SubActor::GetType()
{
	return this->type;
}

int SubActor::GetParentSubActorIndex()
{
	return this->parentSubActorIndex;
}

DebugIcon SubActor::GetDebugIcon()
{
	return this->debugIcon;
}

SubActorStaticMeshDetails* SubActor::GetStaticMeshDetails()
{
	return &this->details.staticMeshDetails;
}

SubActorSkinnedMeshDetails* SubActor::GetSkinnedMeshDetails()
{
	return &this->details.skinnedMeshDetails;
}

SubActorCameraDetails* SubActor::GetCameraDetails()
{
	return &this->details.cameraDetails;
}

SubActorTriggerSphereDetails* SubActor::GetTriggerSphereDetails()
{
	return &this->details.triggerSphereDetails;
}

SubActorSprite3DDetails* SubActor::GetSprite3DDetails()
{
	return &this->details.sprite3DDetails;
}

SubActorVolumeDetails* SubActor::GetVolumeDetails()
{
	return &this->details.volumeDetails;
}

SubActorTriggerSphereState* SubActor::GetTriggerSphereState()
{
	IEngine* engine = GetEngine();

	if (this->state.triggerSphereState.validForFrameId != engine->GetFrameId())
	{
		this->ResolveTriggerSphereState();
	}

	return &this->state.triggerSphereState;
}

void SubActor::RecalculateNonHierachicalComputedLocationData()
{
	Vec3::Add(&this->nonHierachicalComputedLocationData.localPosition, &this->baseLocationData.localPositionBase, &this->baseLocationData.localPositionOffset);
	Vec3::Add(&this->nonHierachicalComputedLocationData.localRotation, &this->baseLocationData.localRotationBase, &this->baseLocationData.localRotationOffset);

	Math::CreateTransformFromPostionAndRotation(
		&this->nonHierachicalComputedLocationData.localTransform, &this->nonHierachicalComputedLocationData.localPosition, &this->nonHierachicalComputedLocationData.localRotation);

	this->nonHierachicalComputedLocationData.isValid = true;
}

void SubActor::RecalculateHierachicalComputedLocationData()
{
	IEngine* engine = GetEngine();
	ISceneManager* sceneManager = engine->GetSceneManager();

	if (!this->nonHierachicalComputedLocationData.isValid)
	{
		this->RecalculateNonHierachicalComputedLocationData();
	}

	// Calculate the sub actor's world transform.
	Mat4::Copy(
		&this->hierachicalComputedLocationData.worldTransform, 
		&this->nonHierachicalComputedLocationData.localTransform);

	IActor* ownerActor = sceneManager->GetActor(this->ownerActorIndex);
	if (ownerActor != null)
	{
		int parentSubActorIndex = this->parentSubActorIndex;
		while (parentSubActorIndex != -1)
		{
			ISubActor* parentSubActor = ownerActor->GetSubActor(parentSubActorIndex);
			
			Mat4::Multiply(
				&this->hierachicalComputedLocationData.worldTransform, 
				parentSubActor->GetLocalTransform(), 
				&this->hierachicalComputedLocationData.worldTransform);

			parentSubActorIndex = parentSubActor->GetParentSubActorIndex();
		}

		Mat4::Multiply(&this->hierachicalComputedLocationData.worldTransform, ownerActor->GetWorldTransform(), &this->hierachicalComputedLocationData.worldTransform);
	}

	// Calculate the sub actor's inverse world transform.
	Mat4::Invert(&this->hierachicalComputedLocationData.invWorldTransform, &this->hierachicalComputedLocationData.worldTransform);

	// Calculate the sub actor's world postion.
	Vec3::Zero(&this->hierachicalComputedLocationData.worldPosition);
	Vec3::TransformMat4(
		&this->hierachicalComputedLocationData.worldPosition, 
		&this->hierachicalComputedLocationData.worldPosition, 
		&this->hierachicalComputedLocationData.worldTransform);

	// The hierachical computed location data is now valid.
	this->hierachicalComputedLocationData.isValid = true;
}

Vec3 SubActor::GetLocalPositionBase()
{
	return this->baseLocationData.localPositionBase;
}

void SubActor::SetLocalPositionBase(const Vec3* localPositionBase)
{
	this->baseLocationData.localPositionBase = *localPositionBase;
	this->nonHierachicalComputedLocationData.isValid = false;
	this->InvalidateHierachicalLocationData();
}

Vec3 SubActor::GetLocalRotationBase()
{
	return this->baseLocationData.localRotationBase;
}

void SubActor::SetLocalRotationBase(const Vec3* localRotationBase)
{
	this->baseLocationData.localRotationBase = *localRotationBase;
	this->nonHierachicalComputedLocationData.isValid = false;
	this->InvalidateHierachicalLocationData();
}

Vec3 SubActor::GetLocalPositionOffset()
{
	return this->baseLocationData.localPositionOffset;
}

void SubActor::SetLocalPositionOffset(const Vec3* localPositionOffset)
{
	this->baseLocationData.localPositionOffset = *localPositionOffset;
	this->nonHierachicalComputedLocationData.isValid = false;
	this->InvalidateHierachicalLocationData();
}

Vec3 SubActor::GetLocalRotationOffset()
{
	return this->baseLocationData.localRotationOffset;
}

void SubActor::SetLocalRotationOffset(const Vec3* localRotationOffset)
{
	this->baseLocationData.localRotationOffset = *localRotationOffset;
	this->nonHierachicalComputedLocationData.isValid = false;
	this->InvalidateHierachicalLocationData();
}

Mat4* SubActor::GetLocalTransform()
{
	if (!this->nonHierachicalComputedLocationData.isValid)
	{
		this->RecalculateNonHierachicalComputedLocationData();
	}

	return &this->nonHierachicalComputedLocationData.localTransform;
}

Mat4* SubActor::GetWorldTransform()
{
	if (!this->hierachicalComputedLocationData.isValid)
	{
		this->RecalculateHierachicalComputedLocationData();
	}

	return &this->hierachicalComputedLocationData.worldTransform;
}

Mat4* SubActor::GetInverseWorldTransform()
{
	if (!this->hierachicalComputedLocationData.isValid)
	{
		this->RecalculateHierachicalComputedLocationData();
	}

	return &this->hierachicalComputedLocationData.invWorldTransform;
}

Vec3* SubActor::GetWorldPosition()
{
	if (!this->hierachicalComputedLocationData.isValid)
	{
		this->RecalculateHierachicalComputedLocationData();
	}

	return &this->hierachicalComputedLocationData.worldPosition;
}

void SubActor::InvalidateHierachicalLocationData()
{
	IEngine* engine = GetEngine();
	ISceneManager* sceneManager = engine->GetSceneManager();

	this->hierachicalComputedLocationData.isValid = false;

	IActor* ownerActor = sceneManager->GetActor(this->ownerActorIndex);
	if (ownerActor != null)
	{
		for (int childSubActorIndex = 0; childSubActorIndex < ownerActor->GetNumberOfSubActorIndexes(); childSubActorIndex++)
		{
			ISubActor* childSubActor = ownerActor->GetSubActor(childSubActorIndex);
			if (childSubActor != null && childSubActor->GetParentSubActorIndex() == this->index)
			{
				childSubActor->InvalidateHierachicalLocationData();
			}
		}
	}
}

void SubActor::Translate(Vec3 direction, SubActorSpace directionSpace, float distance)
{
	this->Translate(direction, directionSpace, distance, false, Vec3::Create(), Vec3::Create());
}

void SubActor::Translate(Vec3 direction, SubActorSpace directionSpace, float distance, bool applyLimits, Vec3 upperLimit, Vec3 lowerLimit)
{
	if (directionSpace == SubActorSpaceLocal)
	{
		Vec3::TransformMat3(&direction, &direction, this->GetWorldTransform());
	}

	Vec3 localPositionOffset = this->GetLocalPositionOffset();
	Vec3::ScaleAndAdd(&localPositionOffset, &localPositionOffset, &direction, distance);

	if (applyLimits)
	{
		Vec3::Max(&localPositionOffset, &localPositionOffset, &lowerLimit);
		Vec3::Min(&localPositionOffset, &localPositionOffset, &upperLimit);
	}

	this->SetLocalPositionOffset(&localPositionOffset);
}

void SubActor::Rotate(Vec3 rotation)
{
	Vec3 localRotationOffset = this->GetLocalRotationOffset();
	Vec3::Add(&localRotationOffset, &localRotationOffset, &rotation);
	this->SetLocalRotationOffset(&localRotationOffset);
}

void SubActor::ResolveTriggerSphereState()
{
	IEngine* engine = GetEngine();
	ISceneManager* sceneManager = engine->GetSceneManager();
	ILogger* logger = engine->GetLogger();

	SubActorTriggerSphereDetails* triggerSphereDetails = &this->details.triggerSphereDetails;
	SubActorTriggerSphereState* triggerSphereState = &this->state.triggerSphereState;

	Sphere triggerSphere;
	triggerSphere.position = *this->GetWorldPosition();
	triggerSphere.radius = triggerSphereDetails->radius;

	// Find the actors which are currently resident in the sphere.
	int residentActorIndexes[SubActorMaxTriggerActorIndexes];
	int numResidentActors = 0;

	for (int actorIndex = 0; actorIndex < sceneManager->GetNumberOfActorIndexes(); actorIndex++)
	{
		IActor* actor = sceneManager->GetActor(actorIndex);

		if (this->CheckIfActorIsApplicableForTriggerSphere(actor))
		{
			if (Sphere::CheckIntersectsAABB(&triggerSphere, actor->GetWorldAABB()))
			{
				residentActorIndexes[numResidentActors] = actor->GetIndex();
				numResidentActors++;
				if (numResidentActors >= SubActorMaxTriggerActorIndexes)
				{
					break;
				}
			}
		}
	}

	// Find the actors which have just entered the sphere.
	triggerSphereState->numberOfJustEnteredActors = 0;

	for (int i = 0; i < numResidentActors; i++)
	{
		int actorIndex = residentActorIndexes[i];

		bool wasPreviouslyResident = false;
		for (int j = 0; j < triggerSphereState->numberOfResidentActors; j++)
		{
			if (triggerSphereState->residentActorIndexes[j] == actorIndex)
			{
				wasPreviouslyResident = true;
				break;
			}
		}

		if (!wasPreviouslyResident)
		{
			triggerSphereState->justEnteredActorIndexes[triggerSphereState->numberOfJustEnteredActors] = actorIndex;
			triggerSphereState->numberOfJustEnteredActors++;
			if (triggerSphereState->numberOfJustEnteredActors >= SubActorMaxTriggerActorIndexes)
			{
				break;
			}

			//logger->Write("Actor %d has entered the sphere", actorIndex);
		}
	}

	// Find the actors which have just left the sphere.
	triggerSphereState->numberOfJustExitedActors = 0;

	for (int i = 0; i < triggerSphereState->numberOfResidentActors; i++)
	{
		int actorIndex = triggerSphereState->residentActorIndexes[i];

		bool isStillResident = false;
		for (int j = 0; j < numResidentActors; j++)
		{
			if (residentActorIndexes[j] == actorIndex)
			{
				isStillResident = true;
				break;
			}
		}

		if (!isStillResident)
		{
			triggerSphereState->justExitedActorIndexes[triggerSphereState->numberOfJustExitedActors] = actorIndex;
			triggerSphereState->numberOfJustExitedActors++;
			if (triggerSphereState->numberOfJustExitedActors >= SubActorMaxTriggerActorIndexes)
			{
				break;
			}

			//logger->Write("Actor %d has exited the sphere", actorIndex);
		}
	}

	// Copy over the actors indexes which are currently resident in the sphere.
	for (int i = 0; i < numResidentActors; i++)
	{
		triggerSphereState->residentActorIndexes[i] = residentActorIndexes[i];
	}

	triggerSphereState->numberOfResidentActors = numResidentActors;

	// Mark the trigger sphere state as valid for this frame.
	triggerSphereState->validForFrameId = engine->GetFrameId();
}

bool SubActor::CheckIfActorIsApplicableForTriggerSphere(IActor* actor)
{
	IEngine* engine = GetEngine();
	IActorControllerManager* actorControllerManager = engine->GetActorControllerManager();

	bool isApplicable = false;

	if (actor != null && actor->GetIndex() != this->ownerActorIndex)
	{
		if (this->details.triggerSphereDetails.numberOfApplicableActorTypes == 0)
		{
			isApplicable = true;
		}
		else
		{
			IActorController* controller = actorControllerManager->GetActorController(actor->GetControllerIndex());
			int actorType = controller->GetActorType();

			for (int i = 0; i < this->details.triggerSphereDetails.numberOfApplicableActorTypes; i++)
			{
				if (actorType == this->details.triggerSphereDetails.applicableActorTypes[i])
				{
					isApplicable = true;
					break;
				}
			}
		}
	}

	return isApplicable;
}