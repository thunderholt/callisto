#include <string.h>
#include "Util/StringUtil.h"

int StringUtil::CopyString(char* dest, const char* source, int destBufferSize)
{
	int sourceLength = strlen(source);

	int i = 0;
	for (; i < sourceLength && i < destBufferSize - 1; i++)
	{
		dest[i] = source[i];
	}

	dest[i] = null;
	return i;
}