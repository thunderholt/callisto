#pragma once

#include "Include/Common.h"

template<class TItemType, int TLength> class FixedLengthArray
{
public:
	FixedLengthArray()
	{
		this->occupiedItemCount = 0;
	}

	operator TItemType*() { return this->buffer; }

	int Push(const TItemType& value)
	{
		int index = this->PushAlongDirection();
		if (index != -1)
		{
			this->buffer[index] = value;
		}

		return index;
	}

	int Push()
	{
		int index = -1;

		if (this->occupiedItemCount < TLength)
		{
			index = this->occupiedItemCount;
			this->occupiedItemCount++;
		}

		return index;
	}

	void Pop()
	{
		if (this->occupiedItemCount > 0)
		{
			this->occupiedItemCount--;
		}
	}

	void Clear()
	{
		this->occupiedItemCount = 0;
	}

	int GetLength()
	{
		return this->occupiedItemCount;
	}

	int GetUnoccupiedLength()
	{
		return TLength - this->occupiedItemCount;
	}

private:
	TItemType buffer[TLength];
	int occupiedItemCount;
};