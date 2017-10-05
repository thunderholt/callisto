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
};

class IThreadManager
{
public:
	virtual ~IThreadManager() {}
	virtual void StartThread(ThreadEntryPointFunction entryPoint, void* arg) = 0;
	virtual void Sleep(unsigned long duration) = 0;
};

class ICollisionMesh
{
public:
	virtual ~ICollisionMesh() {}
	virtual void Allocate(int numberOfChunks, int numberOfFaces) = 0;
	virtual void PushChunk(int startIndex, int numberOfFaces, float* positions, unsigned short* indecies) = 0;
	virtual void Finish() = 0;
	virtual bool DetermineIfPointIsInsideIndoorMesh(Vec3* point) = 0;
	virtual bool DetermineIfLineIntersectsMesh(CollisionLine* line) = 0;
	//virtual bool FindNearestRayIntersection(CollisionMeshIntersectionResult* out, Ray3* ray) = 0;
	//virtual bool FindNearestLineIntersection(CollisionMeshIntersectionResult* out, CollisionLine* line) = 0;
	//virtual Vec3 FindPointCompletelyOutsideOfExtremities() = 0;
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
	//virtual bool GetHasFinished() = 0;
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
	virtual void BuildPVS(const char* worldMeshAssetFilePath, SectorMetrics sectorMetrics) = 0;
	virtual ILogger* GetLogger() = 0;
	virtual IWorldMeshAsset* GetWorldMeshAsset() = 0;
	virtual ISectorVisibilityLookup* GetSectorVisibilityLookup() = 0;
	virtual SectorMetrics* GetSectorMetrics() = 0;
	virtual IThreadManager* GetThreadManager() = 0;
	virtual Sector* GetSectors() = 0;
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
	virtual ICollisionMesh* MakeCollisionMesh() = 0;
	virtual ISectorVisibilityLookup* MakeSectorVisibilityLookup() = 0;
	virtual ISectorCruncher* MakeBruteForceSectorCruncher() = 0;
	virtual IWorker* MakeWorker() = 0;
	virtual IThreadManager* MakeThreadManager() = 0;
};