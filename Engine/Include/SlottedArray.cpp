#include "Core/SlottedArrayHelper.h"

int SlottedArrayHelper::FindUnoccupiedObjectIndex(void* objects, int objectSize, int count)
{
	int index = -1;
	size_t ptr = (size_t)objects;

	for (int i = 0; i < count; i++)
	{
		SlottedArrayObject* obj = (SlottedArrayObject*)ptr;

		if (!obj->slot.isOccupied)
		{
			index = obj->slot.index;
			break;
		}

		ptr += objectSize;
	}

	return index;
}