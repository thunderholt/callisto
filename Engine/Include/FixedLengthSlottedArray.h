#pragma once

#include "Include/Types.h" 
#include "Include/FixedLengthArray.h" 

template<class TItemType> struct FixedLengthSlottedArrayItemContainer
{
	bool isOccupied;
	TItemType item;
};

template<class TItemType, int TLength> class FixedLengthSlottedArray
{
public:
	TItemType& operator[](int index)
	{
		return this->items[index].item;
	}

	int FindAndOccupyAvailableIndex()
	{
		int index = -1;

		for (int i = 0; i < this->items.GetLength(); i++)
		{
			FixedLengthSlottedArrayItemContainer<TItemType>* container = &this->items[i];
			if (!container->isOccupied)
			{
				index = i;
				break;
			}
		}

		if (index == -1)
		{
			index = this->items.Push();
		}

		if (index != -1)
		{
			FixedLengthSlottedArrayItemContainer<TItemType>* container = &this->items[index];
			container->isOccupied = true;
		}

		return index;
	}

	int AddItem(TItemType item)
	{
		int index = this->FindAndOccupyAvailableIndex();

		if (index != -1)
		{
			FixedLengthSlottedArrayItemContainer<TItemType>* container = &this->items[index];
			this->items[index].item = item;
		}

		return index;
	}

	void UnoccupyIndex(int index)
	{
		FixedLengthSlottedArrayItemContainer<TItemType>* container = &this->items[index];
		container->isOccupied = false;
	}

	void UnoccupyAllIndexes()
	{
		for (int i = 0; i < this->items.GetLength(); i++)
		{
			this->UnoccupyIndex(i);
		}
	}

	bool GetIndexIsOccupied(int index)
	{
		FixedLengthSlottedArrayItemContainer<TItemType>* container = &this->items[index];
		return container->isOccupied;
	}

	int GetNumberOfIndexes()
	{
		return this->items.GetLength();
	}

private:
	FixedLengthArray<FixedLengthSlottedArrayItemContainer<TItemType>, TLength > items;
};