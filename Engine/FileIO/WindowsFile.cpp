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

bool WindowsFile::Open(const char* filePath)
{
	this->Close();

	strcpy(this->path, filePath);

	// Get the EXE folder path.
	char exeFolderpath[256];
	GetModuleFileName(null, exeFolderpath, 256);
	char* lastOccuranceOfSlash = strrchr(exeFolderpath, '\\');
	if (lastOccuranceOfSlash != null)
	{
		lastOccuranceOfSlash[0] = null;
	}

	// Append the file path to the full assets folder path.
	char fixedUpPath[512];
	strcpy(fixedUpPath, exeFolderpath);
	strcat(fixedUpPath, "\\..\\Assets\\");
	strcat(fixedUpPath, this->path);
	
	_sopen_s(&this->fd, fixedUpPath, _O_RDONLY | _O_BINARY, _SH_DENYRW, _S_IREAD);
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