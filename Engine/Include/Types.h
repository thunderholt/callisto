#pragma once

#include "Math/Math.h"
#include "Include/FixedLengthArray.h"
#include "Include/FixedLengthSlottedArray.h"
#include "Include/DynamicLengthArray.h"
#include "Include/DynamicLengthSlottedArray.h"
//#include "Include/String.h"

#define null 0
#define AssetManagerNumberOfStores 11
#define AssetMaxFilePathLength 64
#define ActorMaxNameLength 64
#define ActorMaxSubActors 32
#define ActorControllerMaxNameLength 64
//#define ActorMessageDataMaxLength 256
#define SubActorMaxNameLength 64
#define SubActorMaxTriggerActorIndexes 16
#define SubActorTriggerSphereMaxApplicableActorTypes 16
#define SpriteMaxNameLength 32
#define MaterialNumberOfTextureTypes 1
#define ScenePushMaxRecursions 5
#define DebugIconCount 4

typedef void(*ThreadEntryPointFunction)(void*);
typedef unsigned int ProviderId;

struct EngineStartUpArgs
{
	char startUpMapPath[AssetMaxFilePathLength];
};

struct Buffer
{
	char* data;
	int length;
};

enum DebugIcon
{
	DebugIconNone = -1,
	DebugIconWanderToPoint = 0,
	DebugIconTrigger = 1,
	DebugIconEye = 2,
	DebugIconTarget = 3
};

enum LoadingFileState
{
	LoadingFileStateLoading = 0xf001,
	LoadingFileStateLoaded = 0xf002,
	LoadingFileStateFailed = 0xf003
};

enum EngineMapState
{
	EngineMapStateNotAssigned,
	EngineMapStateLoading,
	EngineMapStateLoaded
};

enum AssetType
{
	AssetTypeMap = 0,
	AssetTypeWorldMesh = 1,
	AssetTypeActor = 2,
	AssetTypeTexture = 3,
	AssetTypeSpriteSheet = 4,
	AssetTypeText = 5,
	AssetTypeStaticMesh = 6,
	AssetTypeSkinnedMesh = 7,
	AssetTypeSkinnedMeshAnimation = 8,
	AssetTypeMaterial = 9,
	AssetTypePVS = 10,
};

struct AssetRef
{
	AssetType assetType;
	char filePath[AssetMaxFilePathLength];
	int index;
};

//------ Volumes -----//

enum VolumeType
{
	VolumeTypeNone,
	VolumeTypeCylinder
};

struct CylinderVolumeSpec
{
	unsigned short numberOfSides;
	Vec3 size;
};

struct VolumeSpec
{
	VolumeType type;
	union
	{
		CylinderVolumeSpec cylinderSpec;
	};
};

struct VolumeData
{
	Vec3* positions;
	int numberOfPositions;
	unsigned short* indecies;
	int numberOfIndecies;
	int numberOfFaces;
};

struct VolumeBuffers
{
	ProviderId positionBufferId;
	ProviderId indexBufferId;
};

//--------------------

enum SubActorType
{
	SubActorTypeDummy,
	SubActorTypeStaticMesh,
	SubActorTypeSkinnedMesh,
	SubActorTypeCamera,
	SubActorTypeTriggerSphere,
	SubActorTypeTarget,
	SubActorTypeSprite3D,
	SubActorTypeVolume
};

struct SubActorSpecStaticMeshDetails
{
	int staticMeshAssetRefIndex;
	bool isCollidable;
};

struct SubActorSpecSkinnedMeshDetails
{
	int skinnedMeshAssetRefIndex;
	int skinnedMeshAnimationAssetRefIndex;
	int animationframeIndex;
};

struct SubActorSpecCameraDetails
{
	float fov;
};

struct SubActorSpecTriggerSphereDetails
{
	float radius;
	int applicableActorTypes[SubActorTriggerSphereMaxApplicableActorTypes];
	int numberOfApplicableActorTypes;
};

struct SubActorSpecSprite3DDetails
{
	int spriteSheetAssetRefIndex;
	int textureAssetRefIndex;
	char spriteName[SpriteMaxNameLength];
	Vec2 size;
	float rotation;
};

struct SubActorSpecVolumeDetails
{
	int volumeIndex;
};

struct SubActorSpecDetails
{
	union
	{
		SubActorSpecStaticMeshDetails staticMeshDetails;
		SubActorSpecSkinnedMeshDetails skinnedMeshDetails;
		SubActorSpecCameraDetails cameraDetails;
		SubActorSpecTriggerSphereDetails triggerSphereDetails;
		SubActorSpecSprite3DDetails sprite3DDetails;
		SubActorSpecVolumeDetails volumeDetails;
	};
};

struct ActorSpec
{
	char controllerName[ActorControllerMaxNameLength];
	AABB localAABB;
	Sphere localCollisionSphere;
};

struct SubActorSpec
{
	char name[SubActorMaxNameLength];
	int parentSubActorIndex;
	SubActorType type;
	Vec3 localPositionBase;
	Vec3 localRotationBase;
	SubActorSpecDetails details;
	DebugIcon debugIcon;
};

enum ActorSpace
{
	ActorSpaceLocal,
	ActorSpaceWorld
};

enum SubActorSpace
{
	SubActorSpaceLocal,
	SubActorSpaceWorld
};

/*enum ActorDebugVisualisationType
{
	ActorDebugVisualisationTypeIcon,
	ActorDebugVisualisationTypeLineSphere,
	ActorDebugVisualisationTypeLineCube,
};

struct ActorDebugVisualisation
{
	ActorDebugVisualisationType type;
	DebugIcon icon;
	bool enabled;
	Vec3 position;
	float radius;
	Vec3 size;
};*/

struct SubActorStaticMeshDetails
{
	int staticMeshAssetIndex;
	bool isCollidable;
};

struct SubActorSkinnedMeshDetails
{
	int skinnedMeshAssetIndex;
	int skinnedMeshAnimationAssetIndex;
	int skinnedMeshAnimationFrameIndex;
};

struct SubActorCameraDetails
{
	float fov;
};

struct SubActorTriggerSphereDetails
{
	float radius;
	int applicableActorTypes[SubActorTriggerSphereMaxApplicableActorTypes];
	int numberOfApplicableActorTypes;
};

struct SubActorSprite3DDetails
{
	int spriteSheetAssetIndex;
	int textureAssetIndex;
	int spriteIndex;
	Vec2 size;
	float rotation;
};

struct SubActorVolumeDetails
{
	int volumeIndex;
};

struct SubActorTriggerSphereState
{
	int validForFrameId;
	int justEnteredActorIndexes[SubActorMaxTriggerActorIndexes];
	int numberOfJustEnteredActors;
	int justExitedActorIndexes[SubActorMaxTriggerActorIndexes];
	int numberOfJustExitedActors;
	int residentActorIndexes[SubActorMaxTriggerActorIndexes];
	int numberOfResidentActors;
};

/*
//------ Actor Physical Effects -----//

enum ActorSpace
{
	ActorSpaceLocal,
	ActorSpaceWorld
};

enum ActorPhysicalEffectType
{
	ActorPhysicalEffectTypeTranslate,
	ActorPhysicalEffectTypeRotate,
	ActorPhysicalEffectTypeRotateToFacePoint,
	ActorPhysicalEffectTypePushAlongDirection,
	ActorPhysicalEffectTypePushTowardsDestination,
};

struct ActorPhysicalEffectTranslationDetails
{
	Vec3 direction;
	ActorSpace directionSpace;
	float distance;
};

struct ActorPhysicalEffectRotationDetails
{
	Vec3 rotation;
};

struct ActorPhysicalEffectRotateToFacePointDetails
{
	Vec3 point;
	float maxRotation;
};

struct ActorPhysicalEffectPushAlongDirectionDetails
{
	Vec3 direction;
	ActorSpace directionSpace;
	float distance;
	bool allowSliding;
};

struct ActorPhysicalEffectPushTowardsDestinationDetails
{
	Vec3 destination;
	float maxDistance;
	bool rotateToFaceDestination;
	float maxRotation;
	bool allowSliding;
};

struct ActorPhysicalEffect
{
	ActorPhysicalEffectType type;
	union
	{
		ActorPhysicalEffectTranslationDetails translationDetails;
		ActorPhysicalEffectRotationDetails rotationDetails;
		ActorPhysicalEffectRotateToFacePointDetails rotateToFacePointDetails;
		ActorPhysicalEffectPushAlongDirectionDetails pushAlongDirectionDetails;
		ActorPhysicalEffectPushTowardsDestinationDetails pushTowardsDestinationDetails;
	};
};
*/
/*
//------ Sub Actor Physical Effects -----//

enum SubActorSpace
{
	SubActorSpaceLocal,
	SubActorSpaceWorld
};

enum SubActorPhysicalEffectType
{
	SubActorPhysicalEffectTypeTranslate,
	SubActorPhysicalEffectTypeRotate,
};

struct SubActorPhysicalEffectTranslationDetails
{
	Vec3 direction;
	SubActorSpace directionSpace;
	float distance;
	bool applyLimits;
	Vec3 upperLimit;
	Vec3 lowerLimit;
};

struct SubActorPhysicalEffectRotationDetails
{
	Vec3 rotation;
};

struct SubActorPhysicalEffect
{
	SubActorPhysicalEffectType type;
	union
	{
		SubActorPhysicalEffectTranslationDetails translationDetails;
		SubActorPhysicalEffectRotationDetails rotationDetails;
	};
};*/

//------ Input -----//

/*enum GamePadType
{
	GamePadTypeKeyboardMouse = 0,
	GamePadTypePhysical = 1,
	GamePadTypeTouchScreen = 2,
	GamePadTypeAggregated = 3,
};*/

enum KeyState
{
	KeyStateUp,
	KeyStateDown
};

struct KeyEvent
{
	char key;
	KeyState keyState;
};

enum TouchEventType
{
	TouchEventTypeStart,
	TouchEventTypeEnd,
	TouchEventTypeMove
};

struct TouchEvent
{
	TouchEventType type;
	int touchId;
	Vec2 position;
};

enum MouseButtonState
{
	MouseButtonStateUp,
	MouseButtonStateDown
};

enum MouseButton
{
	MouseButtonLeft = 0,
	MouseButtonRight = 1
};

enum GamePadTrigger
{
	GamePadTriggerLeft,
	GamePadTriggerRight
};

/*enum TouchScreenGamePadStickActivationMode
{
	TouchScreenGamePadStickActivationModeProximity,
	TouchScreenGamePadStickActivationModeZone
};*/

enum TouchScreenGamePadStickMovementMode
{
	TouchScreenGamePadStickMovementModeElastic,
	TouchScreenGamePadStickMovementModeDrag
};

struct TouchScreenGamePadStick
{
	// Activation details.
	//TouchScreenGamePadStickActivationMode activationMode;
	//Vec2 activationProximityPoint;
	Rect activationZone;
	int activeTouchId;
	bool isActive;

	// Movement details.
	TouchScreenGamePadStickMovementMode movementMode;
	Vec2 movementAnchorPoint;
	float movementElasticRadius;
	float movementDragMultiplier;

	// Button details.
	bool hasButton;
	GamePadTrigger buttonTrigger;
	Vec2 buttonPosition;
	float buttonRadius;
	bool buttonIsPressed;
};
/**/

/*enum MouseEventType
{
	MouseEventTypeMove,
	MouseEventTypeClick
};



struct MouseEvent
{
	MouseEventType eventType;
	union
	{
		Vec2 movementOffset;
		MouseButton button;
	};
};*/

/*struct InputMovementMultipliers
{
	Vec3 translation;
	Vec3 rotation;
};*/

//------ Json -----//

enum JsonValueType
{
	JsonValueTypeNull,
	JsonValueTypeNumber,
	JsonValueTypeString,
	JsonValueTypeArray,
	JsonValueTypeObject,
	JsonValueTypeBool
};

enum JsonParserErrorCode
{
	JsonParserErrorCodeNone = 0,
	JsonParserErrorCodeEmptyFile = 1,
	JsonParserErrorCodeInvalidPropertyName = 2,
	JsonParserErrorCodeMalformedProperty = 3,
	JsonParserErrorCodeMalformedString = 4,
	JsonParserErrorCodeMalformedNumber = 5,
	JsonParserErrorCodeMalformedKeyword = 6,
	JsonParserErrorUnrecognisedKeyword = 7
};

//------ * -----//

enum SceneIntersectionType
{
	SceneIntersectionTypeWorldMesh = 1,
	SceneIntersectionTypeActor = 2
};

struct SceneIntersectionResult
{
	SceneIntersectionType type;
	Vec3 intersection;
	int actorIndex;
	int materialAssetIndex;
};

struct SceneIntersectionMask
{
	int primaryActorToIgnoreIndex;
	int* otherActorsToIgnoreIndexes;
	int numberOfOtherActorsToIgnore;
};

struct ScenePushParameters
{
	Sphere sphere;
	Vec3 desiredDirection;
	float desiredDistance; 
	bool allowSliding;
	SceneIntersectionMask intersectionMask;
};

struct ScenePushResult
{
	SceneIntersectionResult sceneIntersectionResults[ScenePushMaxRecursions];
	int numberOfSceneIntersectionResults;
};

enum MaterialTextureType
{
	MaterialTextureTypeDiffuse = 0
};

struct Sprite
{
	char name[SpriteMaxNameLength];
	Vec2 position;
	Vec2 size;
};

//------ World Meshes -----//

struct WorldMeshBuffers
{
	ProviderId positionBufferId;
	ProviderId materialTexCoordBufferId;
	ProviderId indexBuffer;
};

struct WorldMeshChunk
{
	int startIndex;
	int numberOfFaces;
	int materialAssetRefIndex;
	Vec2 lightIslandOffset;
	Vec2 lightIslandSize;
	int lastRenderedFrameId;
};

//------ Static Meshes -----//

struct StaticMeshBuffers
{
	ProviderId positionBufferId;
	ProviderId materialTexCoordBufferId;
	ProviderId indexBufferId;
};

struct StaticMeshChunk
{
	int startIndex;
	int numberOfFaces;
	int materialAssetRefIndex;
};

//------ Collision Meshes -----//

struct CollisionMeshChunk
{
	int startFaceIndex;
	int numberOfFaces;
	int materialAssetIndex;
};

struct CollisionMeshIntersectionResult
{
	Vec3 intersectionPoint;
	Plane intersectionPlane;
	int chunkIndex;
	int faceIndex;
	float distance;
};

//------ Potential Visibility Sets -----//

/*struct PVSSectorMetrics
{
	float sectorSize;
	int numberOfSectors;
	int sectorCounts[3];
	Vec3 sectorOriginOffset;
};*/

struct PVSSector
{
	AABB aabb;
	int visibleSectorIndexesOffset;
	int numberOfVisibleSectors;
	int residentWorldMeshChunkIndexesOffset;
	int numberOfResidentWorldMeshChunkIndexes;
};

//------ * -----//

struct DrawWorldMeshChunkRasterJobItem
{
	int chunkIndex;
};

struct DrawStaticMeshRasterJobItem
{
	int staticMeshAssetIndex;
	Mat4 worldTransform;
};

struct DrawSkinnedMeshRasterJobItem
{
	int skinnedMeshAssetIndex;
	Vec3 position;
	Vec3 rotation;
	int animationIndex;
	float animationFrameIndex;
};

struct DrawSprite3DRasterJobItem
{
	Sprite* sprite;
	Vec3 position;
	Vec2 size;
	float rotation;
	Vec2 centerOfRotation;
	int textureAssetIndex;
	bool enableZTest;
};

struct DrawSprite2DRasterJobItem
{
	Sprite* sprite;
	Vec2 position;
	Vec2 size;
	float rotation;
	Vec2 centerOfRotation;
	int textureAssetIndex;
	RgbaFloat tint;
};

struct DrawLineSphereRasterJobItem
{
	Vec3 position;
	float radius;
};

struct DrawLineCubeRasterJobItem
{
	Vec3 position;
	Vec3 size;
	RgbFloat unoccludedColour;
	RgbFloat occludedColour;
};

struct DrawVolumeRasterJobItem
{
	int volumeIndex;
	Mat4 worldTransform;
};

struct CommonRastorizationParameters
{
	Mat4 viewTransform;
	Mat4 projTransform;
	Mat4 viewProjTransform;
	Vec3 cameraWorldPosition;
	int cameraResidentPvsSectorIndex;
};

struct RasterJob
{
	CommonRastorizationParameters commonRasterizationParameters;
	RgbaFloat clearColour;
	int worldMeshAssetIndex;
	DynamicLengthArray<DrawWorldMeshChunkRasterJobItem> drawWorldMeshChunkJobItems;
	DynamicLengthArray<DrawStaticMeshRasterJobItem> drawStaticMeshJobItems;
	DynamicLengthArray<DrawSkinnedMeshRasterJobItem> drawSkinnedMeshJobItems;
	DynamicLengthArray<DrawVolumeRasterJobItem> drawVolumeRasterJobItems;
	DynamicLengthArray<DrawSprite2DRasterJobItem> drawSprite2DJobItems;
	DynamicLengthArray<DrawSprite3DRasterJobItem> drawSprite3DJobItems;
	DynamicLengthArray<DrawLineSphereRasterJobItem> drawLineSphereJobItems;
	DynamicLengthArray<DrawLineCubeRasterJobItem> drawLineCubeJobItems;
};