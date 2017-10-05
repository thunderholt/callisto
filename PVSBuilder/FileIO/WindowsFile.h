#pragma once
#ifdef WIN32

#include "Include/PVSBuilder.h"

class WindowsFile : public IFile
{
public:
	WindowsFile();
	~WindowsFile();

	virtual bool OpenForReading(const char* filePath);
	virtual bool OpenForWriting(const char* filePath);
	virtual void Close();
	virtual int Read(void* outBuffer, int numberOfBytesToRead);
	virtual int Write(void* buffer, int numberOfBytesToWrite);
	virtual int WriteString(const char* str);
	virtual bool Seek(int offset);
	virtual int GetLength();
	virtual char* GetPath();

private:
	char path[256];
	int fd;
};

#endif