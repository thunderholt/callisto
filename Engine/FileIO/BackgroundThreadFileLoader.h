#pragma once

#include "Include/Callisto.h"

struct LoadingFile
{
	char filePath[AssetMaxFilePathLength];
	Buffer fileData;
	volatile LoadingFileState state;
};

class BackgroundThreadFileLoader : public IFileLoader, public IRunnable
{
public:
	BackgroundThreadFileLoader();
	virtual ~BackgroundThreadFileLoader();
	virtual void Init();
	//virtual void Heartbeat();
	virtual int BeginLoadingFile(const char* filePath);
	virtual LoadingFileState GetFileState(int loadingFileIndex);
	virtual Buffer* GetFileData(int loadingFileIndex);
	virtual void FreeFile(int loadingFileIndex);
	//virtual void CleanUp();
	virtual void Run();

private:
	int GetNextUnfinishedLoadingFileIndex();
	void GetFilePath(int loadingFileIndex, char* filePath);
	void SetFileData(int loadingFileIndex, Buffer* fileData);
	void SetFileState(int loadingFileIndex, LoadingFileState state);

	FixedLengthSlottedArray<LoadingFile, 4> loadingFiles;
	IWorker* worker;
	IMutex* mutex;
};
