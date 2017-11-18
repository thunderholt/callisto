#pragma once

#include "Include/Callisto.h"

/*#define SubActorMaxNumberOfDebugVisualisationIndexes 2
#define SubActorTriggerSphere_DebugVisualisationIndex_TriggerIcon 0
#define SubActorTriggerSphere_DebugVisualisationIndex_LineSphere 1
#define SubActorTarget_DebugVisualisationIndex_TargetIcon 0*/

/*struct SubActorContainer
{
	ISubActor* subActor;
	int debugVisualisationIndexes[SubActorMaxNumberOfDebugVisualisationIndexes];
};*/

struct ActorBaseLocationData
{
	Vec3 worldPosition;
	Vec3 worldRotation;
	AABB localAABB;
	Sphere localCollisionSphere;
};

struct ActorComputedLocationData
{
	bool isValid;
	AABB worldAABB;
	Mat4 worldTransform;
	Sphere worldCollisionSphere;
};

struct ActorDebugVisualisationIndexes
{
	int aabb;
	int collisionSphere;
};

class Actor : public IActor
{
public:
	Actor(int actorIndex);
	virtual ~Actor();
	virtual void OneTimeInit(int actorAssetIndex, const char* name, Vec3* worldPosition, Vec3* worldRotation, IJsonValue* actorInstanceJsonConfig);
	virtual void ReInit(const char* name, Vec3* worldPosition, Vec3* worldRotation, IJsonValue* actorInstanceJsonConfig);
	virtual void Activate();
	virtual void Deactivate();
	//virtual void ResolveLocationValues();
	//virtual void UpdateDebugVisualisations();
	virtual int GetIndex();
	virtual int GetControllerIndex();
	virtual int GetSourceActorAssetIndex();
	virtual bool GetIsActive();
	virtual char* GetName();

	virtual Vec3 GetWorldPosition();
	virtual void SetWorldPosition(const Vec3* worldPostion);
	virtual Vec3 GetWorldRotation();
	virtual void SetWorldRotation(const Vec3* worldRotation);
	virtual AABB* GetWorldAABB();
	virtual Mat4* GetWorldTransform();
	virtual Sphere* GetWorldCollisionSphere();

	//virtual ActorLocationValues* GetLocationValues();
	virtual int GetNumberOfSubActorIndexes();
	virtual int FindSubActorIndexByName(const char* subActorName);
	virtual int FindSubActorIndexesByType(int* out, int maxSubActorIndexes, SubActorType subActorType);
	virtual ISubActor* GetSubActor(int index);
	/*virtual int GetNumberOfPhysicalEffectIndexes();
	virtual ActorPhysicalEffect* GetPhysicalEffect(int index);
	virtual int AddPhysicalEffect();
	virtual void RemovePhysicalEffect(int index);
	virtual void RemoveAllPhysicalEffects();*/
	//virtual int AddMessage(int messageId, void* data, int dataLength);
	//virtual int GetNumberOfMessages();
	//virtual ActorMessage* GrabMessage(int index);
	//virtual void ClearMessages();
	virtual void Tell(int messageId, void* data);
	//virtual int GetNumberOfDebugVisualisationIndexes();
	//virtual ActorDebugVisualisation* GetDebugVisualisation(int index);
	//virtual int AddDebugVisualisation();
	//virtual void RemoveDebugVisualisation(int index);
	//virtual void Translate(const Vec3* direction, ActorSpace directionSpace, float distance);
	virtual void Translate(Vec3 direction, ActorSpace directionSpace, float distance);
	//virtual void Rotate(const Vec3* rotation);
	virtual void Rotate(Vec3 rotation);
	virtual void Rotate(float x, float y, float z);
	//virtual void RotateToFacePoint(const Vec3* point, float maxRotation);
	virtual void RotateToFacePoint(Vec3 point, float maxRotation);
	//virtual void PushAlongDirection(const Vec3* direction, ActorSpace directionSpace, float distance, bool allowSliding);
	virtual void PushAlongDirection(ScenePushResult* out, Vec3 direction, ActorSpace directionSpace, float distance, bool allowSliding, int ignoreActorIndex);
	//virtual void PushTowardsDestination(const Vec3* destination, float maxDistance, bool rotateToFaceDestination, float maxRotation, bool allowSliding);
	virtual void PushTowardsDestination(ScenePushResult* out, Vec3 destination, float maxDistance, bool rotateToFaceDestination, float maxRotation, bool allowSliding, int ignoreActorIndex);
	virtual IActorControllerData* GetControllerData();
	virtual void SetControllerData(IActorControllerData* controllerData);

private:
	IActorController* GetController();
	int AddSubActor(SubActorType type, int parentSubActorIndex, DebugIcon debugIcon);
	void RemoveSubActor(int index);
	void RemoveAllSubActors();
	void InvalidateComputedLocationData();
	void RecalculateComputedLocationData();
	void Push(ScenePushResult* out, Vec3 direction, float distance, bool allowSliding, int ignoreActorIndex);
	//void ResolveSubActorTriggerSphereDetails(ISubActor* subActor);
	//void DisableAllDebugVisualisations();
	//void ResolveActorDebugVisualisations();
	//void ResolveSubActorDebugVisualisations(ISubActor* subActor);

	int index;
	int actorControllerIndex;
	int sourceActorAssetIndex;
	bool isActive;
	char name[ActorMaxNameLength];

	ActorBaseLocationData baseLocationData;
	ActorComputedLocationData computedLocationData;

	//---- Deprecated ----
	//ActorLocationValues locationValues;
	//--------------------

	DynamicLengthSlottedArray<ISubActor*> subActors;
	//DynamicLengthSlottedArray<ActorPhysicalEffect> physicalEffects;
	//DynamicLengthArray<ActorMessage> messages;
	//DynamicLengthSlottedArray<ActorDebugVisualisation> debugVisualisations;
	IActorControllerData* controllerData;
	ActorDebugVisualisationIndexes debugVisualisationIndexes;
};