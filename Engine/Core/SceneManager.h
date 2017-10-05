#pragma once

#include "Include/Callisto.h"

#define SceneCollisionDataMaxCollisionMeshes 32

enum SceneCollisionDataMeshType
{
	SceneCollisionDataMeshTypeWorldMesh,
	SceneCollisionDataMeshTypeActor
};

struct SceneCollisionDataMesh
{
	SceneCollisionDataMeshType type;
	ICollisionMesh* collisionMesh;
	Mat4* worldTransform;
	Mat4* inverseWorldTransform;
	int actorIndex;
};

struct SceneCollisionData
{
	int numberOfCollisionMeshes;
	SceneCollisionDataMesh meshes[SceneCollisionDataMaxCollisionMeshes];
	Mat4 identity;
};

class SceneManager : public ISceneManager
{
public:
	SceneManager();
	virtual ~SceneManager();
	virtual void Reset();
	virtual void Init();
	//virtual void RegisterActorController(IActorController* actorController);
	//virtual int FindActorControllerIndex(const char* actorControllerName);
	//virtual int CreateActor(int actorControllerIndex);
	virtual int CreateActor(int actorAssetIndex, const char* name, Vec3 worldPosition, Vec3 worldRotation, IJsonValue* jsonConfig);
	virtual void DeleteActor(int actorIndex);
	virtual bool GetIsInitialised();
	virtual void ExecuteGameLogic();
	//virtual void UpdateDebugVisualisations();
	virtual IActor* GetActor(int index);
	virtual int GetNumberOfActorIndexes();
	virtual IActor* FindActorByName(const char* actorName);
	virtual void SetMainCameraSubActor(int actorIndex, int subActorIndex);
	virtual ISubActor* GetMainCameraSubActor();
	virtual void PushSphereThroughScene(ScenePushResult* out, ScenePushParameters* parameters);
	virtual bool FindNearestLineIntersectionWithScene(SceneIntersectionResult* out, CollisionLine* collisionLine, bool includeVolumes);
	virtual bool CheckLineOfSight(Vec3* from, Vec3* to, bool includeVolumes);
	virtual bool AttemptToFindRelativePointWithNoObstruction(Vec3* out, Vec3* from, int maximumAttempts, float maximumDistance);
	virtual int FindActorsWithinRadius(int* out, int maxActorIndexes, int* applicableActorTypes, int numberOfApplicableActorTypes, Vec3* position, float radius);
	virtual bool CheckIfActorIsOfType(int actorIndex, int* applicableActorTypes, int numberOfApplicableActorTypes);

private:
	//void DeleteAllActorControllers();
	int FindDeactivatedActor(int requiredSourceActorAssetIndex);
	void DeleteAllActors();
	void BuildSceneCollisionData(SceneCollisionData* out, SceneIntersectionMask* intersectionMask, bool includeVolumes);
	void PushSphereThroughSceneInternal(
		ScenePushResult* out, Sphere* sphere, SceneCollisionData* sceneCollisionData,
		Vec3* desiredDirection, float desiredDistance, bool allowSliding, int recursionDepth);

	bool isInitialised;
	//DynamicLengthSlottedArray<IActorController*> actorControllers;
	DynamicLengthSlottedArray<IActor*> actors;
	int mainCameraActorIndex;
	int mainCameraSubActorIndex;
};