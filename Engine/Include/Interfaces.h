#pragma once

#include "Include/Types.h"

class IUnitTester
{
public:
	virtual ~IUnitTester() {}
	virtual bool RunUnitTests() = 0;
};

class IRunnable
{
public:
	virtual ~IRunnable() {}
	virtual void Run() = 0;
};

class IFileLoader
{
public:
	virtual ~IFileLoader() {}
	virtual void Init() = 0;
	virtual int BeginLoadingFile(const char* filePath) = 0;
	virtual LoadingFileState GetFileState(int loadingFileIndex) = 0;
	virtual Buffer* GetFileData(int loadingFileIndex) = 0;
	virtual void FreeFile(int loadingFileIndex) = 0;
};

class ISemaphore
{
public:
	virtual ~ISemaphore() {}
	virtual void Wait() = 0;
	virtual void Signal() = 0;
};

class IThreadManager
{
public:
	virtual ~IThreadManager() {}
	virtual void StartThread(ThreadEntryPointFunction entryPoint, void* arg) = 0;
	virtual void Sleep(unsigned long duration) = 0;
	virtual bool GetThreadingIsSupported() = 0;
};

class IMutex
{
public:
	virtual ~IMutex() {}
	virtual void WaitForOwnership() = 0;
	virtual void Release() = 0;
};

class IFile
{
public:
	virtual ~IFile() {}
	virtual bool Open(const char* filePath) = 0;
	virtual void Close() = 0;
	virtual int Read(void* outBuffer, int numberOfBytesToRead) = 0;
	virtual bool Seek(int offset) = 0;
	virtual int GetLength() = 0;
	virtual char* GetPath() = 0;
};

class ITimestampProvider
{
public:
	virtual ~ITimestampProvider() {}
	virtual double GetTimestampMillis() = 0;
};

class ILogger
{
public:
	virtual ~ILogger() {}
	virtual void Write(const char *format, ...) = 0;
};

class ICollisionMesh
{
public:
	virtual ~ICollisionMesh() {}
	virtual void Allocate(int numberOfChunks, int numberOfFaces) = 0;
	virtual void PushChunk(int startIndex, int numberOfFaces, int materialAssetIndex, float* positions, unsigned short* indecies) = 0;
	virtual bool FindNearestRayIntersection(CollisionMeshIntersectionResult* out, Ray3* ray) = 0;
	virtual bool FindNearestLineIntersection(CollisionMeshIntersectionResult* out, CollisionLine* line) = 0;
	virtual bool FindNearestSphereCollision(CollisionMeshIntersectionResult* out, Sphere* sphere, Vec3* movementDirection) = 0;
	virtual CollisionMeshChunk* GetChunk(int chunkIndex) = 0;
	virtual int GetNumberOfChunks() = 0;
	virtual CollisionFace* GetFace(int faceIndex) = 0;
	virtual int GetNumberOfFaces() = 0;
};

class IRasterizer
{
public:
	virtual ~IRasterizer() {}
	virtual bool Init() = 0;
	virtual bool PostCoreAssetLoadInit() = 0;
	virtual void RasterizeFrame(RasterJob* rasterJob) = 0;
	virtual ProviderId CreateVertexBuffer(int sizeBytes, const float* data) = 0;
	virtual ProviderId CreateIndexBuffer(int sizeBytes, const unsigned short* data) = 0;
	virtual void DeleteVertexBuffer(ProviderId* id) = 0;
	virtual void DeleteIndexBuffer(ProviderId* id) = 0;
	virtual ProviderId CreateTexture(int width, int height, RgbaUByte* imageData) = 0;
	virtual void DeleteTexture(ProviderId* id) = 0;
	virtual Vec2* GetViewportSize() = 0;
};

class IRasterJobManager
{
public:
	virtual ~IRasterJobManager() {}
	virtual RasterJob* GetCurrentFrameRasterJob() = 0;
	virtual RasterJob* GetNextFrameRasterJob() = 0;
	virtual void SwapJobs() = 0;
};

class IWorker
{
public:
	virtual ~IWorker() {}
	virtual void Start() = 0;
	virtual void Stop() = 0;
	virtual void DoWork() = 0;
	virtual void WaitForWorkToComplete() = 0;
};

class ITokenFileParser
{
public:
	virtual ~ITokenFileParser() {}
	virtual bool GetIsEOF() = 0;	
	virtual void ReadString(char* buffer, int bufferLength) = 0;
	virtual void ReadString(char* buffer, int bufferLength, bool enableComments) = 0;
	virtual int ReadInt() = 0;
	virtual float ReadFloat() = 0;
	virtual bool ReadBool() = 0;
	virtual void ReadVec2(Vec2* out) = 0;
	virtual void ReadVec3(Vec3* out) = 0;
	virtual void ReadSphere(Sphere* out) = 0;
	virtual void ReadAABB(AABB* out) = 0;
	virtual void ReadAssetRef(AssetRef* out, AssetType assetType) = 0;
};

class ITicker
{
public:
	virtual ~ITicker() {}
	virtual bool Tick() = 0;
	virtual void Reset() = 0;
};

class IJsonProperty;

class IJsonValue
{
public:
	virtual ~IJsonValue() {}
	virtual JsonValueType GetType() = 0;
	virtual void SetType(JsonValueType type) = 0;
	virtual IJsonValue* GetObjectProperty(const char* name) = 0;
	virtual IJsonValue* AddObjectProperty(const char* name) = 0;
	virtual int GetNumberOfObjectProperties() = 0;
	virtual IJsonProperty* GetObjectProperty(int index) = 0;
	virtual IJsonValue* GetArrayElement(int index) = 0;
	virtual IJsonValue* AddArrayElement() = 0;
	virtual int GetNumberOfArrayElements() = 0;
	virtual int GetIntValue() = 0;
	virtual float GetFloatValue() = 0;
	virtual double GetDoubleValue() = 0;
	virtual void SetNumberValue(double value) = 0;
	virtual char* GetStringValue() = 0;
	virtual void SetStringValue(const char* value) = 0;
	virtual bool GetBoolValue() = 0;
	virtual void SetBoolValue(bool value) = 0;
	virtual void CopyAssetRefValue(AssetRef* out, AssetType assetType) = 0;
	virtual void CopyVec3Value(Vec3* out) = 0;
	virtual void CopyVec2Value(Vec2* out) = 0;
	virtual void CopySphereValue(Sphere* out) = 0;
	virtual void CopyAABBValue(AABB* out) = 0;
	virtual void CopyRbgFloatValue(RgbFloat* out) = 0;
	virtual void CopyStringValue(char* out, int bufferSize) = 0;
	virtual IJsonValue* Clone() = 0;
	virtual void CopyFrom(IJsonValue* other) = 0;
};

class IJsonProperty
{
public:
	virtual ~IJsonProperty() {}
	virtual char* GetName() = 0;
	virtual void SetName(const char* name) = 0;
	virtual IJsonValue* GetValue() = 0;
	virtual IJsonProperty* Clone() = 0;
};

class IJsonParser
{
public:
	virtual ~IJsonParser() {}
	virtual IJsonValue* ParseJson(const char* json) = 0;
};

class IActorInstance
{
public:
	virtual ~IActorInstance() {}
	virtual AssetRef* GetActorAssetRef() = 0;
	virtual char* GetName() = 0;
	virtual Vec3* GetWorldPosition() = 0;
	virtual Vec3* GetWorldRotation() = 0;
	virtual IJsonValue* GetJsonConfig() = 0;
	virtual void LoadFromJson(IJsonValue* jsonActorInstance) = 0;
};

class IAsset
{
public:
	virtual ~IAsset() {}
	virtual char* GetFilePath() = 0;
	virtual bool GetIsStayResident() = 0;
	virtual bool GetLoadedSuccessfully() = 0;
	virtual bool GetIsEvictable() = 0;
	virtual void SetIsEvictable(bool isEvictable) = 0;
	virtual bool ResolveReferencedAssets() = 0;
};

class IActorAsset : public IAsset
{
public:
	virtual ~IActorAsset() {}
	virtual AssetRef* GetStaticMeshAssetRef(int index) = 0;
	virtual int GetNumberOfStaticMeshAssetRefs() = 0;
	virtual AssetRef* GetSkinnedMeshAssetRef(int index) = 0;
	virtual int GetNumberOfSkinnedMeshAssetRefs() = 0;
	virtual AssetRef* GetSkinnedMeshAnimationAssetRef(int index) = 0;
	virtual int GetNumberOfSkinnedMeshAnimationAssetRefs() = 0;
	virtual AssetRef* GetSpriteSheetAssetRef(int index) = 0;
	virtual int GetNumberOfSpriteSheetAssetRefs() = 0;
	virtual AssetRef* GetTextureAssetRef(int index) = 0;
	virtual int GetNumberOfTextureAssetRefs() = 0;
	virtual ActorSpec* GetActorSpec() = 0;
	virtual SubActorSpec* GetSubActorSpec(int index) = 0;
	virtual int GetNumberOfSubActorSpecs() = 0;
	virtual IJsonValue* GetJsonConfig() = 0;
};

class IMapAsset : public IAsset
{
public:
	virtual ~IMapAsset() {}
	virtual AssetRef* GetAssetRef(int index) = 0;
	virtual int GetNumberAssetRefs() = 0;
	virtual AssetRef* GetWorldMeshAssetRef() = 0;
	virtual IActorInstance* GetActorInstance(int index) = 0;
	virtual int GetNumberOfActorInstances() = 0;
};

class ISpriteSheetAsset : public IAsset
{
public:
	virtual ~ISpriteSheetAsset() {}
	virtual int FindSpriteIndex(const char* name) = 0;
	virtual Sprite* GetSprite(int index) = 0;
	virtual int GetNumberOfSprites() = 0;
};

class ITextAsset : public IAsset
{
public:
	virtual ~ITextAsset() {}
	virtual char* GetText() = 0;
};

class ITextureAsset : public IAsset
{
public:
	virtual ~ITextureAsset() {}
	virtual Vec2* GetSize() = 0;
	virtual ProviderId GetTextureId() = 0;
};

class IWorldMeshAsset : public IAsset
{
public:
	virtual ~IWorldMeshAsset() {}
	virtual WorldMeshBuffers* GetBuffers() = 0;
	virtual WorldMeshChunk* GetChunk(int index) = 0;
	virtual int GetNumberOfChunks() = 0;
	virtual AssetRef* GetMaterialAssetRef(int index) = 0;
	virtual int GetNumberOfMaterialAssetRefs() = 0;
	virtual AssetRef* GetLightAtlasTextureAssetRef(int index) = 0;
	virtual int GetNumberOfLightAtlasTextureAssetRefs() = 0;
	virtual AssetRef* GetPVSAssetRef() = 0;
	virtual ICollisionMesh* GetCollisionMesh() = 0;
};

class IStaticMeshAsset : public IAsset
{
public:
	virtual ~IStaticMeshAsset() {}
	virtual StaticMeshBuffers* GetBuffers() = 0;
	virtual StaticMeshChunk* GetChunk(int index) = 0;
	virtual int GetNumberOfChunks() = 0;
	virtual AssetRef* GetMaterialAssetRef(int index) = 0;
	virtual int GetNumberOfMaterialAssetRefs() = 0;
	virtual ICollisionMesh* GetCollisionMesh() = 0;
};

class IMaterialAsset : public IAsset
{
public:
	virtual ~IMaterialAsset() {}
	virtual AssetRef* GetTextureAssetRef(MaterialTextureType textureType) = 0;
	//////////////// Test code /////////
	virtual RgbFloat GetEmmissiveColour() = 0;
	////////////////////////////////////
};

class IPVSAsset : public IAsset
{
public:
	virtual ~IPVSAsset() {}
	//virtual PVSSectorMetrics* GetSectorMetrics() = 0;
	virtual int GetNumberOfSectors() = 0;
	virtual PVSSector* GetSector(int sectorIndex) = 0;
	virtual int* GetVisibleSectorIndexes() = 0;
	virtual int* GetResidentWorldMeshChunkIndexes() = 0;
};

class IAssetManager
{
public:
	virtual ~IAssetManager() {}
	virtual void BeginLoadingCoreAssets() = 0;
	virtual void HeartBeat() = 0;
	virtual void LoadMap(const char* filePath) = 0;
	virtual int FindAssetIndex(AssetType assetType, const char* filePath) = 0;
	virtual bool ResolveAssetRefIndex(AssetRef* assetRef) = 0;
	virtual IMapAsset* GetMapAsset(int assetIndex) = 0;
	virtual IActorAsset* GetActorAsset(int assetIndex) = 0;
	virtual IWorldMeshAsset* GetWorldMeshAsset(int assetIndex) = 0;
	virtual IStaticMeshAsset* GetStaticMeshAsset(int assetIndex) = 0;
	virtual ITextureAsset* GetTextureAsset(int assetIndex) = 0;
	virtual ISpriteSheetAsset* GetSpriteSheetAsset(int assetIndex) = 0;
	virtual ITextAsset* GetTextAsset(int assetIndex) = 0;
	virtual IMaterialAsset* GetMaterialAsset(int assetIndex) = 0;
	virtual IPVSAsset* GetPVSAsset(int assetIndex) = 0;
	virtual bool GetIsLoadingCoreAssets() = 0;
	virtual bool GetIsLoading() = 0;
	virtual bool GetLoadFailed() = 0;
};

class ISubActor
{
public:
	virtual ~ISubActor() {}
	virtual int GetOwnerActorIndex() = 0;
	virtual int GetIndex() = 0;
	virtual char* GetName() = 0;
	virtual SubActorType GetType() = 0;
	virtual int GetParentSubActorIndex() = 0;
	virtual DebugIcon GetDebugIcon() = 0;
	virtual SubActorStaticMeshDetails* GetStaticMeshDetails() = 0;
	virtual SubActorSkinnedMeshDetails* GetSkinnedMeshDetails() = 0;
	virtual SubActorCameraDetails* GetCameraDetails() = 0;
	virtual SubActorTriggerSphereDetails* GetTriggerSphereDetails() = 0;
	virtual SubActorSprite3DDetails* GetSprite3DDetails() = 0;
	virtual SubActorVolumeDetails* GetVolumeDetails() = 0;
	virtual SubActorTriggerSphereState* GetTriggerSphereState() = 0;
	virtual void RecalculateNonHierachicalComputedLocationData() = 0;
	virtual void RecalculateHierachicalComputedLocationData() = 0;
	virtual Vec3 GetLocalPositionBase() = 0;
	virtual void SetLocalPositionBase(const Vec3* localPositionBase) = 0;
	virtual Vec3 GetLocalRotationBase() = 0;
	virtual void SetLocalRotationBase(const Vec3* localRotationBase) = 0;
	virtual Vec3 GetLocalPositionOffset() = 0;
	virtual void SetLocalPositionOffset(const Vec3* localPositionOffset) = 0;
	virtual Vec3 GetLocalRotationOffset() = 0;
	virtual void SetLocalRotationOffset(const Vec3* localRotationOffset) = 0;
	virtual void InvalidateHierachicalLocationData() = 0;
	virtual Mat4* GetLocalTransform() = 0;
	virtual Mat4* GetWorldTransform() = 0;
	virtual Mat4* GetInverseWorldTransform() = 0;
	virtual Vec3* GetWorldPosition() = 0;
	virtual void Translate(Vec3 direction, SubActorSpace directionSpace, float distance) = 0;
	virtual void Translate(Vec3 direction, SubActorSpace directionSpace, float distance, bool applyLimits, Vec3 upperLimit, Vec3 lowerLimit) = 0;
	virtual void Rotate(Vec3 rotation) = 0;
};

class IActorControllerData
{
public:
	virtual ~IActorControllerData() {}
	virtual void* GetFacet(int facetId) = 0;
};

class IActor
{
public:
	virtual ~IActor() {}
	virtual void OneTimeInit(int actorAssetIndex, const char* name, Vec3* worldPosition, Vec3* worldRotation, IJsonValue* actorInstanceJsonConfig) = 0;
	virtual void ReInit(const char* name, Vec3* worldPosition, Vec3* worldRotation, IJsonValue* actorInstanceJsonConfig) = 0;
	virtual void Activate() = 0;
	virtual void Deactivate() = 0;
	virtual int GetIndex() = 0;
	virtual int GetControllerIndex() = 0;
	virtual int GetSourceActorAssetIndex() = 0;
	virtual bool GetIsActive() = 0;
	virtual char* GetName() = 0;
	virtual Vec3 GetWorldPosition() = 0;
	virtual void SetWorldPosition(const Vec3* worldPostion) = 0;
	virtual Vec3 GetWorldRotation() = 0;
	virtual void SetWorldRotation(const Vec3* worldRotation) = 0;
	virtual AABB* GetWorldAABB() = 0;
	virtual Mat4* GetWorldTransform() = 0;
	virtual Sphere* GetWorldCollisionSphere() = 0;
	virtual int GetNumberOfSubActorIndexes() = 0;
	virtual int FindSubActorIndexByName(const char* subActorName) = 0;
	virtual int FindSubActorIndexesByType(int* out, int maxSubActorIndexes, SubActorType subActorType) = 0;
	virtual ISubActor* GetSubActor(int index) = 0;
	virtual void Tell(int messageId, void* data) = 0;
	virtual void Translate(Vec3 direction, ActorSpace directionSpace, float distance) = 0;
	virtual void Rotate(Vec3 rotation) = 0;
	virtual void Rotate(float x, float y, float z) = 0;
	virtual void RotateToFacePoint(Vec3 point, float maxRotation) = 0;
	virtual void PushAlongDirection(ScenePushResult* out, Vec3 direction, ActorSpace directionSpace, float distance, bool allowSliding, int ignoreActorIndex) = 0;
	virtual void PushTowardsDestination(ScenePushResult* out, Vec3 destination, float maxDistance, bool rotateToFaceDestination, float maxRotation, bool allowSliding, int ignoreActorIndex) = 0;
	virtual IActorControllerData* GetControllerData() = 0;
	virtual void SetControllerData(IActorControllerData* controllerData) = 0;
};

class IActorController
{
public:
	virtual ~IActorController() {}
	virtual void OneTimeInit(IActor* actor, IJsonValue* actorAssetJsonConfig, IJsonValue* actorInstanceJsonConfig) = 0;
	virtual void ReInit(IActor* actor, IJsonValue* actorInstanceJsonConfig) = 0;
	virtual void Activate(IActor* actor) = 0;
	virtual void Deactivate(IActor* actor) = 0;
	virtual void Heartbeat(IActor* actor) = 0;
	virtual void Tell(IActor* actor, int messageId, void* data) = 0;
	virtual char* GetControllerName() = 0;
	virtual int GetActorType() = 0;
};

class IActorControllerManager
{
public:
	virtual ~IActorControllerManager() {}
	virtual void RegisterActorController(IActorController* actorController) = 0;
	virtual int FindActorControllerIndex(const char* actorControllerName) = 0;
	virtual IActorController* GetActorController(int index) = 0;
};

class ISceneManager
{
public:
	virtual ~ISceneManager() {}
	virtual void Reset() = 0;
	virtual void Init() = 0;
	virtual int CreateActor(int actorAssetIndex, const char* name, Vec3 worldPosition, Vec3 worldRotation, IJsonValue* jsonConfig) = 0;
	virtual void DeleteActor(int actorIndex) = 0;
	virtual bool GetIsInitialised() = 0;
	virtual void ExecuteGameLogic() = 0;
	virtual IActor* GetActor(int index) = 0;
	virtual int GetNumberOfActorIndexes() = 0;
	virtual IActor* FindActorByName(const char* actorName) = 0;
	virtual void SetMainCameraSubActor(int actorIndex, int subActorIndex) = 0;
	virtual ISubActor* GetMainCameraSubActor() = 0;
	virtual void PushSphereThroughScene(ScenePushResult* out, ScenePushParameters* parameters) = 0;
	virtual bool FindNearestLineIntersectionWithScene(SceneIntersectionResult* out, CollisionLine* collisionLine, bool includeVolumes) = 0;
	virtual bool CheckLineOfSight(Vec3* from, Vec3* to, bool includeVolumes) = 0;
	virtual bool AttemptToFindRelativePointWithNoObstruction(Vec3* out, Vec3* from, int maximumAttempts, float maximumDistance) = 0;
	virtual int FindActorsWithinRadius(int* out, int maxActorIndexes, int* applicableActorTypes, int numberOfApplicableActorTypes, Vec3* position, float radius) = 0;
	virtual bool CheckIfActorIsOfType(int actorIndex, int* applicableActorTypes, int numberOfApplicableActorTypes) = 0;
};

class IFrameTimer
{
public:
	virtual ~IFrameTimer() {}
	virtual void StartFrame() = 0;
	virtual int GetNumberOfFrames() = 0;
	virtual int GetNumberOfFpsHitches() = 0;
	virtual float GetFpsHitchRate() = 0;
	virtual float GetCurrentFPS() = 0;
	virtual float GetMinFPS() = 0;
	virtual float GetMaxFPS() = 0;
	virtual float GetAverageFPS() = 0;
	virtual float GetLastFrameDurationMillis() = 0;
	virtual float GetMultiplier() = 0;
};

class IVolume
{
public:
	virtual ~IVolume() {}
	virtual void Populate(VolumeSpec* spec, VolumeData* volumeData) = 0;
	virtual VolumeSpec* GetSpec() = 0;
	virtual VolumeBuffers* GetBuffers() = 0;
	virtual ICollisionMesh* GetCollisionMesh() = 0;
	virtual int GetNumberOfFaces() = 0;
};

class IVolumeManager
{
public:
	virtual ~IVolumeManager() {}
	virtual int CreateVolume(VolumeSpec* spec) = 0;
	virtual IVolume* GetVolume(int volumeIndex) = 0;
	virtual void DeleteVolume(int volumeIndex) = 0;
	//virtual void Reset() = 0;
};

class IEngineBehaviour
{
public:
	virtual ~IEngineBehaviour() {}
	virtual bool PostCoreAssetLoadInit() = 0;
	//virtual void CleanUp() = 0;
	virtual void StartProducingNextFrame(RasterJob* rasterJob) = 0;
	virtual void WaitForProductionOfNextFrameToFinish() = 0;
};

class IRenderer
{
public:
	virtual ~IRenderer() {}
	//virtual bool PostCoreAssetLoadInit() = 0;
	virtual void RenderScene(RasterJob* rasterJob) = 0;
};

class IDebugVisualisationRenderer
{
public:
	virtual ~IDebugVisualisationRenderer() {}
	virtual bool PostCoreAssetLoadInit() = 0;
	virtual void RenderDebugVisualisations(RasterJob* rasterJob) = 0;
};

class IKeyboard
{
public:
	virtual ~IKeyboard() {}
	virtual void Init() = 0;
	virtual bool GetIsActive() = 0;
	virtual KeyState GetKeyState(char key) = 0;
	virtual int GetNumberOfEvents() = 0;
	virtual KeyEvent* GetEvent(int index) = 0;
	virtual void ClearEvents() = 0;
};

class IMouse
{
public:
	virtual ~IMouse() {}
	virtual void Init() = 0;
	virtual bool GetIsActive() = 0;
	virtual void Poll() = 0;
	virtual Vec2* GetMovementOffset() = 0;
	virtual MouseButtonState GetMouseButtonState(MouseButton button) = 0;
};

class ITouchScreen
{
public:
	virtual ~ITouchScreen() {}
	virtual void Init() = 0;
	virtual bool GetIsActive() = 0;
	virtual int GetNumberOfEvents() = 0;
	virtual TouchEvent* GetEvent(int index) = 0;
	virtual void ClearEvents() = 0;
};

class IGamePad
{
public:
	virtual ~IGamePad() {}
	virtual bool GetIsActive() = 0;
	virtual void Init() = 0;
	virtual void Poll() = 0;
	virtual Vec2* GetLeftStickAxes() = 0;
	virtual Vec2* GetRightStickAxes() = 0;
	virtual float GetTriggerPressure(GamePadTrigger trigger) = 0;
	//virtual ButtonState GetButtonState(int buttonIndex) = 0;
};

class IKeyboardMouseGamePad : public IGamePad
{

};

class IPhysicalGamePad : public IGamePad
{

};

class ITouchScreenGamePad : public IGamePad
{
public:
	//virtual Vec2* GetLeftStickOrigin() = 0;
	//virtual Vec2* GetRightStickOrigin() = 0;
	//virtual Vec2* GetFireStickOrigin() = 0;
	virtual TouchScreenGamePadStick* GetLeftStick() = 0;
	virtual TouchScreenGamePadStick* GetRightStick() = 0;
};

class IAggregatedGamePad : public IGamePad
{

};

class IGameController
{
public:
	virtual ~IGameController() {}
	virtual int ParseActorType(const char* actorTypeString) = 0;
};

class IEngine
{
public:
	virtual ~IEngine() {}
	virtual void RegisterGameController(IGameController* gameController) = 0;
	virtual void Init(EngineStartUpArgs* startUpArgs) = 0;
	virtual void BeginLoadingMap(const char* filePath) = 0;
	virtual void Heartbeat() = 0;
	virtual IGameController* GetGameController() = 0;
	virtual IRasterizer* GetRasterizer() = 0;
	virtual IRasterJobManager* GetRasterJobManager() = 0;
	virtual IAssetManager* GetAssetManager() = 0;
	virtual IEngineBehaviour* GetBehaviour() = 0;
	virtual IRenderer* GetRenderer() = 0;
	virtual IDebugVisualisationRenderer* GetDebugVisualisationRenderer() = 0;
	virtual IFrameTimer* GetFrameTimer() = 0;
	virtual ISceneManager* GetSceneManager() = 0;
	virtual IActorControllerManager* GetActorControllerManager() = 0;
	virtual IVolumeManager* GetVolumeManager() = 0;
	virtual IFileLoader* GetFileLoader() = 0;
	virtual IThreadManager* GetThreadManager() = 0;
	virtual ITimestampProvider* GetTimestampProvider() = 0;
	virtual IKeyboard* GetKeyboard() = 0;
	virtual IMouse* GetMouse() = 0;
	virtual ITouchScreen* GetTouchScreen() = 0;
	virtual IKeyboardMouseGamePad* GetKeyboardMouseGamePad() = 0;
	virtual IPhysicalGamePad* GetPhysicalGamePad() = 0;
	virtual ITouchScreenGamePad* GetTouchScreenGamePad() = 0;
	virtual IAggregatedGamePad* GetAggregatedGamePad() = 0;
	virtual ILogger* GetLogger() = 0;
	virtual EngineStartUpArgs* GetStartUpArgs() = 0;
	virtual AssetRef* GetMapAssetRef() = 0;
	virtual bool GetMustTerminate() = 0;
	virtual EngineMapState GetMapState() = 0;
	virtual int GetFrameId() = 0;
};

class IFactory
{
public:
	virtual IUnitTester* MakeUnitTester() = 0;
	virtual ITicker* MakeTicker(float ticksPerSecond) = 0;
	virtual IFileLoader* MakeFileLoader() = 0;
	virtual IFile* MakeFile() = 0;
	virtual ITokenFileParser* MakeTokenFileParser(Buffer* fileData) = 0;
	virtual ISemaphore* MakeSemaphore() = 0;
	virtual IThreadManager* MakeThreadManager() = 0;
	virtual IMutex* MakeMutex() = 0;
	virtual IWorker* MakeWorker(IRunnable* runnable) = 0;
	virtual ITimestampProvider* MakeTimestampProvider() = 0;
	virtual IRasterizer* MakeRasterizer() = 0;
	virtual IRasterJobManager* MakeRasterJobManager() = 0;
	virtual ILogger* MakeLogger() = 0;
	virtual IJsonValue* MakeJsonValue() = 0;
	virtual IJsonProperty* MakeJsonProperty() = 0;
	virtual IJsonParser* MakeJsonParser() = 0;
	virtual IAssetManager* MakeAssetManager() = 0;
	virtual IActorAsset* MakeActorAsset(const char* filePath, Buffer* fileData, bool isStayResident) = 0;
	virtual IMapAsset* MakeMapAsset(const char* filePath, Buffer* fileData, bool isStayResident) = 0;
	virtual ISpriteSheetAsset* MakeSpriteSheetAsset(const char* filePath, Buffer* fileData, bool isStayResident) = 0;
	virtual ITextAsset* MakeTextAsset(const char* filePath, Buffer* fileData, bool isStayResident) = 0;
	virtual ITextureAsset* MakeTextureAsset(const char* filePath, Buffer* fileData, bool isStayResident) = 0;
	virtual IWorldMeshAsset* MakeWorldMeshAsset(const char* filePath, Buffer* fileData, bool isStayResident) = 0;
	virtual IStaticMeshAsset* MakeStaticMeshAsset(const char* filePath, Buffer* fileData, bool isStayResident) = 0;
	virtual IMaterialAsset* MakeMaterialAsset(const char* filePath, Buffer* fileData, bool isStayResident) = 0;
	virtual IPVSAsset* MakePVSAsset(const char* filePath, Buffer* fileData, bool isStayResident) = 0;
	virtual ISceneManager* MakeSceneManager() = 0;
	virtual IActorControllerManager* MakeActorControllerManager() = 0;
	virtual IFrameTimer* MakeFrameTimer() = 0;
	virtual IEngineBehaviour* MakeEngineBehaviour() = 0;
	virtual IRenderer* MakeRenderer() = 0;
	virtual IDebugVisualisationRenderer* MakeDebugVisualisationRenderer() = 0;
	virtual IKeyboard* MakeKeyboard() = 0;
	virtual IMouse* MakeMouse() = 0;
	virtual ITouchScreen* MakeTouchScreen() = 0;
	virtual IKeyboardMouseGamePad* MakeKeyboardMouseGamePad() = 0;
	virtual ITouchScreenGamePad* MakeTouchScreenGamePad() = 0;
	virtual IPhysicalGamePad* MakePhysicalGamePad() = 0;
	virtual IAggregatedGamePad* MakeAggregatedGamePad() = 0;
	virtual IActorInstance* MakeActorInstance() = 0;
	virtual IActor* MakeActor(int actorIndex) = 0;
	virtual ISubActor* MakeSubActor(int ownerActorIndex, int subActorIndex, SubActorType type, int parentSubActorIndex, DebugIcon debugIcon) = 0;
	virtual ICollisionMesh* MakeCollisionMesh() = 0;
	virtual IVolumeManager* MakeVolumeManager() = 0;
	virtual IVolume* MakeVolume() = 0;
};