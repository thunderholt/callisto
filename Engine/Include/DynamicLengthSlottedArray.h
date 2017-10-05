#pragma once

#include "Include/Types.h" 
#include "Include/DynamicLengthArray.h" 

template<class TItemType> struct DynamicLengthSlottedArrayItemContainer
{
	bool isOccupied;
	TItemType item;
};

template<class TItemType> class DynamicLengthSlottedArray
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
			DynamicLengthSlottedArrayItemContainer<TItemType>* container = &this->items[i];
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

		DynamicLengthSlottedArrayItemContainer<TItemType>* container = &this->items[index];
		container->isOccupied = true;

		return index;
	}

	int AddItem(TItemType& item)
	{
		int index = this->FindAndOccupyAvailableIndex();

		DynamicLengthSlottedArrayItemContainer<TItemType>* container = &this->items[index];
		this->items[index].item = item;

		return index;
	}

	void UnoccupyIndex(int index)
	{
		DynamicLengthSlottedArrayItemContainer<TItemType>* container = &this->items[index];
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
		DynamicLengthSlottedArrayItemContainer<TItemType>* container = &this->items[index];
		return container->isOccupied;
	}

	int GetNumberOfIndexes()
	{
		return this->items.GetLength();
	}

private:
	DynamicLengthArray<DynamicLengthSlottedArrayItemContainer<TItemType> > items;
};