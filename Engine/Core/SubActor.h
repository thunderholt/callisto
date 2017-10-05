#pragma once

#include "Include/Callisto.h"

struct SubBaseActorLocationData
{
	Vec3 localPositionBase;
	Vec3 localPositionOffset;
	Vec3 localRotationBase;
	Vec3 localRotationOffset;
};

struct SubActorNonHierachicalComputedLocationData
{
	bool isValid;
	Vec3 localPosition;
	Vec3 localRotation;
	Mat4 localTransform;
};

struct SubActorHierachicalComputedLocationData
{
	bool isValid;
	Mat4 worldTransform;
	Mat4 invWorldTransform;
	Vec3 worldPosition;
};

struct SubActorDetails
{
	union
	{
		SubActorStaticMeshDetails staticMeshDetails;
		SubActorSkinnedMeshDetails skinnedMeshDetails;
		SubActorCameraDetails cameraDetails;
		SubActorTriggerSphereDetails triggerSphereDetails;
		SubActorSprite3DDetails sprite3DDetails;
		SubActorVolumeDetails volumeDetails;
	};
};

struct SubActorState
{
	union
	{
		SubActorTriggerSphereState triggerSphereState;
	};
};

class SubActor : public ISubActor
{
public:
	SubActor(int ownerActorIndex, int subActorIndex, SubActorType type, int parentSubActorIndex, DebugIcon debugIcon);
	virtual ~SubActor();
	virtual int GetOwnerActorIndex();
	virtual int GetIndex();
	virtual char* GetName();
	virtual SubActorType GetType();
	virtual int GetParentSubActorIndex();
	virtual DebugIcon GetDebugIcon();
	//virtual int* GetChildSubActorIndexes();
	//virtual int GetNumberOfChildSubActors();
	//virtual SubActorLocationValues* GetLocationValues();
	virtual SubActorStaticMeshDetails* GetStaticMeshDetails();
	virtual SubActorSkinnedMeshDetails* GetSkinnedMeshDetails();
	virtual SubActorCameraDetails* GetCameraDetails();
	virtual SubActorTriggerSphereDetails* GetTriggerSphereDetails();
	virtual SubActorSprite3DDetails* GetSprite3DDetails();
	virtual SubActorVolumeDetails* GetVolumeDetails();
	virtual SubActorTriggerSphereState* GetTriggerSphereState();
	virtual void RecalculateNonHierachicalComputedLocationData();
	virtual void RecalculateHierachicalComputedLocationData();
	virtual Vec3 GetLocalPositionBase();
	virtual void SetLocalPositionBase(const Vec3* localPositionBase);
	virtual Vec3 GetLocalRotationBase();
	virtual void SetLocalRotationBase(const Vec3* localRotationBase);
	virtual Vec3 GetLocalPositionOffset();
	virtual void SetLocalPositionOffset(const Vec3* localPositionOffset);
	virtual Vec3 GetLocalRotationOffset();
	virtual void SetLocalRotationOffset(const Vec3* localRotationOffset);
	virtual Mat4* GetLocalTransform();
	virtual Mat4* GetWorldTransform();
	virtual Mat4* GetInverseWorldTransform();
	virtual Vec3* GetWorldPosition();
	virtual void InvalidateHierachicalLocationData();
	/*virtual int GetNumberOfPhysicalEffectIndexes();
	virtual SubActorPhysicalEffect* GetPhysicalEffect(int index);
	virtual int AddPhysicalEffect();
	virtual void RemovePhysicalEffect(int index);
	virtual void RemoveAllPhysicalEffects();*/
	//virtual void Translate(const Vec3* direction, SubActorSpace directionSpace, float distance);
	//virtual void Translate(const Vec3* direction, SubActorSpace directionSpace, float distance, bool applyLimits, Vec3* upperLimit, Vec3* lowerLimit);
	virtual void Translate(Vec3 direction, SubActorSpace directionSpace, float distance);
	virtual void Translate(Vec3 direction, SubActorSpace directionSpace, float distance, bool applyLimits, Vec3 upperLimit, Vec3 lowerLimit);
	//virtual void Rotate(const Vec3* rotation);
	virtual void Rotate(Vec3 rotation);

private:
	//void ResolveChildSubActorIndexes();
	void ResolveTriggerSphereState();
	bool CheckIfActorIsApplicableForTriggerSphere(IActor* actor);

	int ownerActorIndex;
	int index;
	char name[SubActorMaxNameLength];
	SubActorType type;
	int parentSubActorIndex;
	DebugIcon debugIcon;
	//int* childSubActorIndexes;
	//int numberOfChildSubActors;
	
	SubBaseActorLocationData baseLocationData;
	SubActorNonHierachicalComputedLocationData nonHierachicalComputedLocationData;
	SubActorHierachicalComputedLocationData hierachicalComputedLocationData;

	//SubActorLocationValues locationValues;
	SubActorDetails details;
	SubActorState state;
	//DynamicLengthSlottedArray<SubActorPhysicalEffect> physicalEffects;
};
