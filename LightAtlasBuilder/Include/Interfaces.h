#pragma once

#include "Include/Types.h"

class ILogger
{
public:
	virtual ~ILogger() {}
	virtual void Write(const char *format, ...) = 0;
};

class IFile
{
public:
	virtual ~IFile() {}
	virtual bool OpenForReading(const char* filePath) = 0;
	virtual bool OpenForWriting(const char* filePath) = 0;
	virtual void Close() = 0;
	virtual int Read(void* outBuffer, int numberOfBytesToRead) = 0;
	virtual int Write(void* buffer, int numberOfBytesToWrite) = 0;
	virtual int WriteString(const char* str) = 0;
	virtual bool Seek(int offset) = 0;
	virtual int GetLength() = 0;
	virtual char* GetPath() = 0;
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
	virtual void ReadVec2i(Vec2i* out) = 0;
	virtual void ReadVec3(Vec3* out) = 0;
	virtual void ReadVec3i(Vec3i* out) = 0;
	virtual void ReadRgbFloat(RgbFloat* out) = 0;
	virtual void ReadAssetRef(AssetRef* out) = 0;
};

class IThreadManager
{
public:
	virtual ~IThreadManager() {}
	virtual void StartThread(ThreadEntryPointFunction entryPoint, void* arg) = 0;
	virtual void Sleep(unsigned long duration) = 0;
};

class ITimestampProvider
{
public:
	virtual ~ITimestampProvider() {}
	virtual double GetTimestampMillis() = 0;
};

class ICollisionMesh
{
public:
	virtual ~ICollisionMesh() {}
	virtual void AllocateGeometry(int numberOfChunks, int numberOfFaces) = 0;
	virtual void AllocateGrid(Vec3 gridOriginOffset, Vec3i gridDimensions, float gridCellSize) = 0;
	virtual void PushChunk(int startIndex, int numberOfFaces, Vec3* positions, Vec3* normals, Vec2* materialUVs, Vec2* lightAtlasUVs, unsigned short* indecies,/*, MaterialStaticLightingDetails* staticLightingDetails*/int lightAtlasIndex) = 0;
	virtual void Finish() = 0;
	virtual bool DetermineIfLineIntersectsMesh(CollisionLine* line, int ignoreChunkIndex1, int ignoreChunkIndex2) = 0;
	virtual bool FindNearestLineIntersectWithMesh(Vec3* outIntersection, MeshChunkFaceIndex* outChunkFaceIndex, CollisionLine* line, int ignoreChunkIndex1, int ignoreChunkIndex2) = 0;
	virtual CollisionMeshChunk* GetChunk(int chunkIndex) = 0;
	virtual int GetNumberOfChunks() = 0;
	virtual CollisionFace* GetFace(int faceIndex) = 0;
	virtual int GetNumberOfFaces() = 0;
};

class IWorldMeshAsset
{
public:
	virtual ~IWorldMeshAsset() {}
	virtual bool Load(const char* filePath) = 0;
	virtual ICollisionMesh* GetCollisionMesh() = 0;
	virtual WorldMeshLightAtlas* GetLightAtlas(int lightAtlasIndex) = 0;
	virtual int GetNumberOfLightAtlases() = 0;
	virtual WorldMeshLightIsland* GetLightIsland(int lightIslandIndex) = 0;
	virtual WorldMeshLightIsland* FindLightIslandForChunk(int chunkIndex) = 0;
	virtual int GetNumberOfLightIslands() = 0;
	virtual WorldMeshLight* GetLight(int lightIndex) = 0;
	virtual int GetNumberOfLights() = 0;
};

class IMaterialAsset
{
public:
	virtual ~IMaterialAsset() {}
	virtual bool Load(const char* filePath) = 0;
	//virtual MaterialStaticLightingDetails* GetStaticLightingDetails() = 0;
};

class ILightAtlas
{
public:
	virtual ~ILightAtlas() {}
	virtual void Allocate(int width, int height) = 0;
	virtual void ResetLumelIntensityCacheItems() = 0;
	//virtual void ResetLumelIntensityCacheItemsForBounceNumber(int bounceNumber) = 0;
	//virtual void AverageLumelIntensityCacheItemsForBounceNumber(int bounceNumber) = 0;
	virtual void AccumulateLumelColoursFromIntensityCacheItems(RgbFloat* lightColour) = 0;
	virtual void WriteToPngFile(const char* filePath) = 0;
	virtual Lumel* GetLumels() = 0;
	virtual Vec2i GetSize() = 0;
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
	virtual void CopyVec3Value(Vec3* out) = 0;
	virtual void CopyVec2Value(Vec2* out) = 0;
	virtual void CopyVec2iValue(Vec2i* out) = 0;
	virtual void CopyRgbFloatValue(RgbFloat* out) = 0;
	virtual void CopyAABBValue(AABB* out) = 0;
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

class IWorker
{
public:
	virtual ~IWorker() {}
	//virtual void Init(int startLightIslandIndex, int numberOfLightIslandIndexes) = 0;
	virtual void SetCurrentLightAtlasDetails(int* lightIslandIndexes, int numberofLightIslandIndexes, BitField* effectedChunksFieldForCurrentLightAtlas) = 0;
	virtual void SetCurrentLight(ExpandedLight* light, BitField* effectedChunksFieldForCurrentLight) = 0;
	virtual void ComputeLumelBasicDataAsync() = 0;
	virtual void ComputeLumelBounceTargetsAsync() = 0;
	virtual void ComputeDirectIlluminationIntensitiesForCurrentLightAsync() = 0;
	virtual void ComputeIndirectIlluminationBounceIntensitiesForCurrentLightAsync(int bounceNumber) = 0;
	//virtual void SetCurrentLight(Light* light) = 0;
	//virtual void SetCurrentLightSlotIndex(int lightSlotIndex) = 0;
	//virtual void SetCurrentLightGroup(LightGroup* lightGroup);
	//virtual void ComputeBasicLumelDataAsync() = 0;
	//virtual void ComputeDirectIlluminationIntensitiesForCurrentLightGroupAsync() = 0;
	//virtual void ComputeIndirectIlluminationBouncesTargetsForCurrentLightAsync(NormalWithinHemisphereCalculationMetrics* normalWithinHemisphereCalculationMetrics, int hemisphereCircleIndex, int hemisphereSegmentIndex) = 0;
	//virtual void AccumulateIndirectIlluminationIntensitiesForCurrentLightAsync() = 0;
	//virtual void CompositeColourForCurrentLightAsync() = 0;
	//virtual void AccumulateIndirectIlluminationIntensitiesForCurrentLightGroupAsync(NormalWithinHemisphereCalculationMetrics* normalWithinHemisphereCalculationMetrics, int hemisphereCircleIndex, int hemisphereSegmentIndex, int bounceNumber) = 0;
	virtual void FillBordersAsync() = 0;
	virtual bool GetHasFinished() = 0;
	virtual void RunThreadEntryPoint() = 0;
	//virtual void DumpStats() = 0;
	//virtual int GetTotalFailures() = 0;
};

class IRayTracer
{
public:
	virtual ~IRayTracer() {}
	//virtual RgbFloat CalculateColourForChunkAtPosition(Vec3* worldPosition, Vec3* normal, int chunkIndex) = 0;
	//virtual void CalculateDirectIlluminationIntensityForLumel(float* outAverageIntensity, float* outAverageDistanceToLightSqr, Light* light, Vec3* lumelWorldPosition, Vec3* lumelNormal/*, float baseDistanceToLightSqr*/, ICollisionMesh* collisionMesh, int chunkIndex) = 0;
	//virtual float CalculateDirectIlluminationIntensityForLumel(Light* light, Vec3* lumelWorldPosition, Vec3* lumelNormal, ICollisionMesh* collisionMesh, int chunkIndex) = 0;
	virtual float CalculateDirectIlluminationIntensityForLumel(ExpandedLight* light, Vec3* lumelWorldPosition, Vec3* lumelNormal, ICollisionMesh* collisionMesh, int chunkIndex) = 0;
};

class IEngine
{
public:
	virtual ~IEngine() {}
	virtual void BuildLightAtlases(const char* worldMeshAssetFilePath, const char* assetsFolderPath) = 0;
	virtual ILogger* GetLogger() = 0;
	virtual IWorldMeshAsset* GetWorldMeshAsset() = 0;
	virtual ILightAtlas* GetCurrentLightAtlas() = 0;
	//virtual Light* GetLight(int index) = 0;
	//virtual int GetNumberOfLights() = 0;
	virtual IRayTracer* GetRayTracer() = 0;
	virtual IThreadManager* GetThreadManager() = 0;
	virtual ITimestampProvider* GetTimestampProvider() = 0;
	virtual const char* GetAssetsFolderPath() = 0;
	virtual const Config* GetConfig() = 0;
};

class IFactory
{
public:
	virtual ~IFactory() {}
	virtual IFile* MakeFile() = 0;
	virtual ILogger* MakeLogger() = 0;
	virtual ITokenFileParser* MakeTokenFileParser(Buffer* fileData) = 0;
	virtual IWorldMeshAsset* MakeWorldMeshAsset() = 0;
	virtual IMaterialAsset* MakeMaterialAsset() = 0;
	virtual ICollisionMesh* MakeCollisionMesh() = 0;
	virtual IWorker* MakeWorker() = 0;
	virtual ILightAtlas* MakeLightAtlas() = 0;
	virtual IRayTracer* MakeRayTracer() = 0;
	virtual IThreadManager* MakeThreadManager() = 0;
	virtual ITimestampProvider* MakeTimestampProvider() = 0;
	virtual IJsonValue* MakeJsonValue() = 0;
	virtual IJsonProperty* MakeJsonProperty() = 0;
	virtual IJsonParser* MakeJsonParser() = 0;
};