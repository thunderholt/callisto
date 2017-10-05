#ifdef EMSCRIPTEN
#include "FileIO/WebBrowserFileLoader.h"

extern "C" 
{
	void jsBeginLoadingFile(const char* filePath, Buffer* outFileData, volatile LoadingFileState* outLoadingFileState);
}

WebBrowserFileLoader::WebBrowserFileLoader()
{
}

WebBrowserFileLoader::~WebBrowserFileLoader()
{
	for (int i = 0; i < this->loadingFiles.GetNumberOfIndexes(); i++)
	{
		this->FreeFile(i);
	}
}

void WebBrowserFileLoader::Init()
{
}

/*void WebBrowserFileLoader::Heartbeat()
{
	
}*/

int WebBrowserFileLoader::BeginLoadingFile(const char* filePath)
{
	int index = this->loadingFiles.FindAndOccupyAvailableIndex();
	if (index != -1)
	{
		LoadingFile* loadingFile = &this->loadingFiles[index];
		strcpy(loadingFile->filePath, filePath);
		loadingFile->fileData.data = null;
		loadingFile->fileData.length = 0;
		loadingFile->state = LoadingFileStateLoading;

		jsBeginLoadingFile(loadingFile->filePath, &loadingFile->fileData, &loadingFile->state);
	}

	return index;
}

LoadingFileState WebBrowserFileLoader::GetFileState(int loadingFileIndex)
{
	LoadingFile* loadingFile = &this->loadingFiles[loadingFileIndex];
	return loadingFile->state;
}

Buffer* WebBrowserFileLoader::GetFileData(int loadingFileIndex)
{
	LoadingFile* loadingFile = &this->loadingFiles[loadingFileIndex];
	return &loadingFile->fileData;
}

void WebBrowserFileLoader::FreeFile(int loadingFileIndex)
{
	LoadingFile* loadingFile = &this->loadingFiles[loadingFileIndex];
	SafeDeleteArrayAndNull(loadingFile->fileData.data);
	this->loadingFiles.UnoccupyIndex(loadingFileIndex);
}

/*void WebBrowserFileLoader::CleanUp()
{
	for (int i = 0; i < this->loadingFiles.GetNumberOfIndexes(); i++)
	{
		this->FreeFile(i);
	}
}*/

#endif