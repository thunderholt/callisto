#pragma once

#include "Include/Common.h"

class BitField 
{
public:
	BitField();
	~BitField();

	bool GetBit(int index);
	void SetBit(int index);
	void UnsetBit(int index);
	void ToggleBit(int index, bool set);
	void Reset(int length);
	int CountSetBits();

private:
	int length;
	int sizeInBytes;
	unsigned char* data;
};