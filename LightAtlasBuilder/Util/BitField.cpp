#include "Util/BitField.h"
#include <math.h>
#include <string.h>

BitField::BitField()
{
	this->length = 0;
	this->sizeInBytes = 0;
	this->data = null;
}

BitField::~BitField()
{
	SafeDeleteArrayAndNull(this->data);
}

bool BitField::GetBit(int index)
{
	bool bitSet = false;

	if (index < this->length)
	{
		int byteIndex = (int)floorf(index / 8.0f);
		int bitOffset = index % 8;
		unsigned char bitMask = 0x80 >> bitOffset;

		unsigned char maskedByte = this->data[byteIndex] & bitMask;
		bitSet = maskedByte != 0;
	}

	return bitSet;
}

void BitField::SetBit(int index)
{
	if (index < this->length)
	{
		int byteIndex = (int)floorf(index / 8.0f);
		int bitOffset = index % 8;
		unsigned char bitMask = 0x80 >> bitOffset;
		unsigned char invBitMask = ~bitMask;

		unsigned char byte = this->data[byteIndex];
		this->data[byteIndex] = (byte & invBitMask) | bitMask;
	}
}

void BitField::UnsetBit(int index)
{
	if (index < this->length)
	{
		int byteIndex = (int)floorf(index / 8.0f);
		int bitOffset = index % 8;
		unsigned char bitMask = 0x80 >> bitOffset;
		unsigned char invBitMask = ~bitMask;

		unsigned char byte = this->data[byteIndex];
		this->data[byteIndex] = (byte & invBitMask);
	}
}

void BitField::ToggleBit(int index, bool set)
{
	if (set)
	{
		this->SetBit(index);
	}
	else
	{
		this->UnsetBit(index);
	}
}

void BitField::Reset(int length)
{
	if (this->length != length) 
	{
		SafeDeleteArrayAndNull(this->data);

		this->length = length;
		this->sizeInBytes = (int)ceilf(this->length / 8.0f);
		this->data = new unsigned char[this->sizeInBytes];
	}

	memset(this->data, 0, this->sizeInBytes);
}

int BitField::CountSetBits()
{
	int count = 0;
	
	for (int i = 0; i < this->length; i++) 
	{
		if (this->GetBit(i)) 
		{
			count++;
		}
	}

	return count;
}