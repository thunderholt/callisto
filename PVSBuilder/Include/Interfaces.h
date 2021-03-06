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
	virtual void ReadVec3(Vec3* out) = 0;
	virtual void ReadVec3i(Vec3i* out) = 0;
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
	virtual void PushChunk(int startIndex, int numberOfFaces, float* positions, unsigned short* indecies, bool isVisibilityOccluder) = 0;
	virtual void Finish() = 0;
	virtual bool DetermineIfPointIsInsideIndoorMesh(Vec3* point) = 0;
	virtual bool DetermineIfPointIsPotentiallyInsideOutdoorMesh(Vec3* point) = 0;
	virtual bool DetermineIfLineIntersectsMesh(CollisionLine* line, bool excludeNonOccludingChunks) = 0;
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
	virtual AABB* GetPvsSectorAABB(int index) = 0;
	virtual int GetNumberOfPvsSectorAABBs() = 0;
};

class IMaterialAsset
{
public:
	virtual ~IMaterialAsset() {}
	virtual bool Load(const char* filePath) = 0;
	virtual bool GetIsVisibilityOcculuder() = 0;
};

class ISectorVisibilityLookup
{
public:
	virtual ~ISectorVisibilityLookup() {}
	virtual void Allocate(int numberOfSectors) = 0;
	virtual SectorVisibilityState GetSectorVisibilityState(int sectorIndexA, int sectorIndexB) = 0;
	virtual void SetSectorVisibilityState(int sectorIndexA, int sectorIndexB, SectorVisibilityState sectorVisibilityState) = 0;
};

class ISectorCruncher
{
public:
	virtual ~ISectorCruncher() {}
	virtual void Run(int startSectorIndex, int numberOfSectorsToCrunch) = 0;
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
	virtual void Init(int startSectorIndex, int numberOfSectorsToCrunch) = 0;
	virtual void ComputeSectorInsidePointsAsync() = 0;
	virtual void RunSectorCruncherAsync(SectorCruncherType sectorCruncherType) = 0;
	virtual bool GetHasFinished() = 0;
	virtual void RunThreadEntryPoint() = 0;
};

class IEngine
{
public:
	virtual ~IEngine() {}
	virtual void BuildPVS(const char* worldMeshAssetFilePath, const char* assetsFolderPath, const char* outputFilePath) = 0;
	virtual ILogger* GetLogger() = 0;
	virtual IWorldMeshAsset* GetWorldMeshAsset() = 0;
	virtual ISectorVisibilityLookup* GetSectorVisibilityLookup() = 0;
	//virtual SectorMetrics* GetSectorMetrics() = 0;
	virtual IThreadManager* GetThreadManager() = 0;
	virtual ITimestampProvider* GetTimestampProvider() = 0;
	virtual Sector* GetSector(int index) = 0;
	virtual int GetNumberOfSectors() = 0;
	virtual const char* GetAssetsFolderPath() = 0;
	//virtual Vec3* GetPointCompletelyOutsideOfCollisionMeshExtremities() = 0;
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
	virtual ISectorVisibilityLookup* MakeSectorVisibilityLookup() = 0;
	//virtual ISectorCruncher* MakeOcclusionSectorCruncher() = 0;
	virtual ISectorCruncher* MakeBruteForceSectorCruncher() = 0;
	virtual IWorker* MakeWorker() = 0;
	virtual IThreadManager* MakeThreadManager() = 0;
	virtual ITimestampProvider* MakeTimestampProvider() = 0;
	virtual IJsonValue* MakeJsonValue() = 0;
	virtual IJsonProperty* MakeJsonProperty() = 0;
	virtual IJsonParser* MakeJsonParser() = 0;
};