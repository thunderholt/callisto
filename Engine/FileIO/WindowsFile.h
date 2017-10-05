#pragma once
#ifdef WIN32

#include "Include/Callisto.h"

class WindowsFile : public IFile
{
public:
	WindowsFile();
	~WindowsFile();

	bool Open(const char* filePath);
	void Close();
	int Read(void* outBuffer, int numberOfBytesToRead);
	bool Seek(int offset);
	int GetLength();
	char* GetPath();

private:
	char path[256];
	int fd;
};

#endif