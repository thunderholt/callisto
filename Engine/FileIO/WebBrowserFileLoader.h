#pragma once
#ifdef EMSCRIPTEN

#include "Include/Callisto.h"

struct LoadingFile
{
	char filePath[AssetMaxFilePathLength];
	Buffer fileData;
	volatile LoadingFileState state;
};

class WebBrowserFileLoader : public IFileLoader
{
public:
	WebBrowserFileLoader();
	virtual ~WebBrowserFileLoader();
	virtual void Init();
	//virtual void Heartbeat();
	virtual int BeginLoadingFile(const char* filePath);
	virtual LoadingFileState GetFileState(int loadingFileIndex);
	virtual Buffer* GetFileData(int loadingFileIndex);
	virtual void FreeFile(int loadingFileIndex);
	//virtual void CleanUp();

private:
	FixedLengthSlottedArray<LoadingFile, 4> loadingFiles;
};

#endif