#ifdef ANDROID

#include <jni.h>
#include <stdio.h>
#include <string.h>
#include "FileIO/AndroidFile.h"

AAssetManager *assetManager = 0;

extern "C"
{
	JNIEXPORT void JNICALL
	Java_com_darkstarinteractive_callisto_NativeLibWrapper_receiveAssetManager(JNIEnv *env, jclass cls,
																			   jobject javaAssetManager) 
	{
		assetManager = AAssetManager_fromJava(env, javaAssetManager);
	}
}

AndroidFile::AndroidFile()
{
	this->asset = 0;
}

AndroidFile::~AndroidFile()
{
	this->Close();
}

bool AndroidFile::Open(const char* filePath)
{
	this->Close();

	strcpy(this->path, filePath);

	this->asset = AAssetManager_open(assetManager, filePath, 0);
	if (asset == 0)
	{
		return false;
	}

	return true;
}

void AndroidFile::Close()
{
	if (this->asset != 0)
	{
		AAsset_close(this->asset);
		this->asset = 0;
	}

	strcpy(this->path, "");
}

int AndroidFile::Read(void* outBuffer, int numberOfBytesToRead)
{
	return AAsset_read(this->asset, outBuffer, numberOfBytesToRead);
}

bool AndroidFile::Seek(int offset)
{
	return AAsset_seek(this->asset, offset, SEEK_SET) != -1;
}

int AndroidFile::GetLength()
{
	return AAsset_getLength(this->asset);
}

char* AndroidFile::GetPath()
{
	return this->path;
}

#endif