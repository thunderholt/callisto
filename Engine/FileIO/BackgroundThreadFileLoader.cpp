#include "FileIO/BackgroundThreadFileLoader.h"

BackgroundThreadFileLoader::BackgroundThreadFileLoader()
{
	IFactory* factory = GetFactory();

	this->worker = factory->MakeWorker(this);
	this->mutex = factory->MakeMutex();
}

BackgroundThreadFileLoader::~BackgroundThreadFileLoader()
{
	this->worker->Stop();

	for (int i = 0; i < this->loadingFiles.GetNumberOfIndexes(); i++)
	{
		this->FreeFile(i);
	}

	SafeDeleteAndNull(this->worker);
	SafeDeleteAndNull(this->mutex);
}

void BackgroundThreadFileLoader::Init()
{
	this->worker->Start();
}

int BackgroundThreadFileLoader::BeginLoadingFile(const char* filePath)
{
	this->mutex->WaitForOwnership();

	int index = this->loadingFiles.FindAndOccupyAvailableIndex();
	if (index != -1)
	{
		LoadingFile* loadingFile = &this->loadingFiles[index];
		strcpy(loadingFile->filePath, filePath);
		loadingFile->fileData.data = null;
		loadingFile->fileData.length = 0;
		loadingFile->state = LoadingFileStateLoading;
	}

	this->mutex->Release();

	if (index != -1)
	{
		this->worker->DoWork();
	}

	return index;
}

LoadingFileState BackgroundThreadFileLoader::GetFileState(int loadingFileIndex)
{
	LoadingFile* loadingFile = &this->loadingFiles[loadingFileIndex];
	return loadingFile->state;
}

Buffer* BackgroundThreadFileLoader::GetFileData(int loadingFileIndex)
{
	LoadingFile* loadingFile = &this->loadingFiles[loadingFileIndex];
	return &loadingFile->fileData;
}

void BackgroundThreadFileLoader::FreeFile(int loadingFileIndex)
{
	this->mutex->WaitForOwnership();

	LoadingFile* loadingFile = &this->loadingFiles[loadingFileIndex];
	SafeDeleteArrayAndNull(loadingFile->fileData.data);
	this->loadingFiles.UnoccupyIndex(loadingFileIndex);

	this->mutex->Release();
}

void BackgroundThreadFileLoader::Run()
{
	IFactory* factory = GetFactory();

	while (true)
	{
		int loadingFileIndex = this->GetNextUnfinishedLoadingFileIndex();

		if (loadingFileIndex == -1)
		{
			break;
		}
		else
		{
			char filePath[AssetMaxFilePathLength];
			this->GetFilePath(loadingFileIndex, filePath);
			
			IFile* file = factory->MakeFile();

			if (!file->Open(filePath))
			{
				this->SetFileState(loadingFileIndex, LoadingFileStateFailed);
			}
			else
			{
				Buffer fileData;
				fileData.length = file->GetLength();
				fileData.data = new char[fileData.length];
				this->SetFileData(loadingFileIndex, &fileData);

				if (file->Read(fileData.data, fileData.length) != fileData.length)
				{
					this->SetFileState(loadingFileIndex, LoadingFileStateFailed);
				}
				else
				{
					this->SetFileState(loadingFileIndex, LoadingFileStateLoaded);
				}
			}

			delete file;

			
		}
	}
}

int BackgroundThreadFileLoader::GetNextUnfinishedLoadingFileIndex()
{
	int index = -1;

	this->mutex->WaitForOwnership();

	for (int i = 0; i < this->loadingFiles.GetNumberOfIndexes(); i++)
	{
		if (this->loadingFiles.GetIndexIsOccupied(i))
		{
			LoadingFile* loadingFile = &this->loadingFiles[i];
			if (loadingFile->state == LoadingFileStateLoading)
			{
				index = i;
				break;
			}
		}
	}
	this->mutex->Release();

	return index;
}

void BackgroundThreadFileLoader::GetFilePath(int loadingFileIndex, char* filePath)
{
	this->mutex->WaitForOwnership();
	LoadingFile* loadingFile = &this->loadingFiles[loadingFileIndex];
	strcpy(filePath, loadingFile->filePath);
	this->mutex->Release();
}

void BackgroundThreadFileLoader::SetFileData(int loadingFileIndex, Buffer* fileData)
{
	this->mutex->WaitForOwnership();
	LoadingFile* loadingFile = &this->loadingFiles[loadingFileIndex];
	loadingFile->fileData = *fileData;
	this->mutex->Release();
}

void BackgroundThreadFileLoader::SetFileState(int loadingFileIndex, LoadingFileState state)
{
	this->mutex->WaitForOwnership();
	LoadingFile* loadingFile = &this->loadingFiles[loadingFileIndex];
	loadingFile->state = state;
	this->mutex->Release();
}