#ifdef WIN32

#include <stdio.h>
#include <stdarg.h>
#include <windows.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#include "FileIO/WindowsFile.h"

WindowsFile::WindowsFile()
{
	this->fd = 0;
}

WindowsFile::~WindowsFile()
{
	this->Close();
}

bool WindowsFile::OpenForReading(const char* filePath)
{
	this->Close();

	strcpy(this->path, filePath);

	char fixedUpPath[512];
	//strcpy(fixedUpPath, "C:/Users/andym/Documents/GitHub/Callisto/Assets/Compiled/"); // TODO
	strcpy(fixedUpPath, "C:/Work/Research/GitHub/callisto/Assets/Compiled/"); // TODO
	strcat(fixedUpPath, this->path);
	
	_sopen_s(&this->fd, fixedUpPath, _O_RDONLY | _O_BINARY, _SH_DENYRW, _S_IREAD);
	if (this->fd == -1)
	{
		return false;
	}

	return true;
}

bool WindowsFile::OpenForWriting(const char* filePath)
{
	this->Close();

	strcpy(this->path, filePath);

	char fixedUpPath[512];
	//strcpy(fixedUpPath, "C:/Users/andym/Documents/GitHub/Callisto/Assets/Compiled/"); // TODO
	strcpy(fixedUpPath, "C:/Work/Research/GitHub/callisto/Assets/Compiled/"); // TODO
	strcat(fixedUpPath, this->path);

	_sopen_s(&this->fd, fixedUpPath, _O_RDWR | _O_CREAT | _O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	if (this->fd == -1)
	{
		return false;
	}

	return true;
}

void WindowsFile::Close()
{
	if (this->fd > 0)
	{
		_close(this->fd);
		this->fd = 0;
	}

	strcpy(this->path, "");
}

int WindowsFile::Read(void* outBuffer, int numberOfBytesToRead)
{
	return _read(this->fd, outBuffer, numberOfBytesToRead);
}

int WindowsFile::Write(void* buffer, int numberOfBytesToWrite)
{
	return _write(this->fd, buffer, numberOfBytesToWrite);
}

int WindowsFile::WriteString(const char* str)
{
	int numberOfBytesToWrite = strlen(str);

	return _write(this->fd, str, numberOfBytesToWrite);
}

bool WindowsFile::Seek(int offset)
{
	return _lseek(this->fd, offset, SEEK_SET) >= 0;
}

int WindowsFile::GetLength()
{
	struct stat st;
	return fstat(this->fd, &st) == 0 ? st.st_size : 0;
}

char* WindowsFile::GetPath()
{
	return this->path;
}

#endif