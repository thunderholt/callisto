#pragma once
#ifdef ANDROID

#include <android/asset_manager_jni.h>
#include "Include/Callisto.h" 

class AndroidFile : public IFile
{
public:
	AndroidFile();
	~AndroidFile();

	bool Open(const char* filePath);
	void Close();
	int Read(void* outBuffer, int numberOfBytesToRead);
	bool Seek(int offset);
	int GetLength();
	char* GetPath();

private:
	char path[256];
	AAsset* asset;
};

#endif