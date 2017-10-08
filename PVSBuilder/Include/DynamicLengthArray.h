#pragma once

#include <string.h>
#include "Include/Common.h"

template<class TItemType> class DynamicLengthArray
{
public:
	DynamicLengthArray(void)
	{
		this->Init(0, 10);
	}

	DynamicLengthArray(int initialBufferItemCount) : DynamicLengthArray(initialBufferItemCount, 10)
	{
		this->Init(initialBufferItemCount, 10);
	}

	DynamicLengthArray(int initialBufferItemCount, int growAmount)
	{
		this->Init(initialBufferItemCount, growAmount);
	}

	~DynamicLengthArray()
	{
		delete[] this->buffer;
	}

	operator TItemType*() { return this->buffer; }

	void Resize(int newBufferItemCount)
	{
		TItemType* newBuffer = null;

		if (newBufferItemCount > 0)
		{
			newBuffer = new TItemType[newBufferItemCount];

			int copyItemCount = newBufferItemCount < this->currentBufferItemCount ?
				newBufferItemCount : this->currentBufferItemCount;

			memcpy(newBuffer, this->buffer, copyItemCount * sizeof(TItemType));
		}

		if (this->buffer != null)
		{
			delete[] this->buffer;
		}

		this->buffer = newBuffer;
		this->currentBufferItemCount = newBufferItemCount;

		if (this->occupiedItemCount > this->currentBufferItemCount)
		{
			this->occupiedItemCount = this->currentBufferItemCount;
		}
	}

	void AdjustSize(int amount)
	{
		this->Resize(this->currentBufferItemCount + amount);
	}

	int Push(const TItemType& value)
	{
		int index = this->Push();
		this->buffer[index] = value;

		return index;
	}

	int Push()
	{
		int newOccupiedItemCount = this->occupiedItemCount + 1;
		if (newOccupiedItemCount > this->currentBufferItemCount)
		{
			this->AdjustSize(this->growAmount);
		}

		int index = this->occupiedItemCount;
		this->occupiedItemCount++;

		return index;
	}

	TItemType& PushAndGet()
	{
		int index = this->Push();
		return this->buffer[index];
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

private:
	void Init(int initialBufferItemCount, int growAmount)
	{
		this->growAmount = growAmount;
		this->occupiedItemCount = 0;

		this->buffer = null;
		this->currentBufferItemCount = 0;
		this->Resize(initialBufferItemCount);
	}

	TItemType* buffer;
	int currentBufferItemCount;
	int occupiedItemCount;
	int growAmount;
};