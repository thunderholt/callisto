#include "FileIO/TokenFileParser.h"

TokenFileParser::TokenFileParser(Buffer* fileData)
{
	this->fileData = fileData;
	this->bufferPosition = 0;

	this->BurnWhiteSpace();
}

TokenFileParser::~TokenFileParser()
{
	// Noop.
}

bool TokenFileParser::GetIsEOF()
{
	return this->bufferPosition == this->fileData->length;
}

void TokenFileParser::ReadString(char* buffer, int bufferLength)
{
	this->ReadString(buffer, bufferLength, true);
}

void TokenFileParser::ReadString(char* buffer, int bufferLength, bool enableComments)
{
	while (true)
	{
		strcpy(buffer, "");

		this->BurnWhiteSpace();

		int outBufferPos = 0;
		for (int i = 0; i < bufferLength - 1 && !this->GetIsEOF(); i++)
		{
			char c = ((char*)this->fileData->data)[this->bufferPosition];
			this->bufferPosition++;

			if (!this->GetCharacterIsWhiteSpace(c))
			{
				buffer[outBufferPos] = c;
				outBufferPos++;
			}
			else
			{
				break;
			}
		}

		buffer[outBufferPos] = null;

		if (buffer[0] == '#' && enableComments)
		{
			this->BurnCurrentLine();
		}
		else
		{
			break;
		}
	}
}

int TokenFileParser::ReadInt()
{
	char buffer[128];
	this->ReadString(buffer, 128);
	int i = (int)atoi(buffer);
	return i;
}

float TokenFileParser::ReadFloat()
{
	char buffer[128];
	this->ReadString(buffer, 128);
	float f = (float)atof(buffer);
	return f;
}

bool TokenFileParser::ReadBool()
{
	char buffer[128];
	this->ReadString(buffer, 128);
	bool b = strcmp(buffer, "true") == 0;
	return b;
}

void TokenFileParser::ReadVec2(Vec2* out)
{
	out->x = this->ReadFloat();
	out->y = this->ReadFloat();
}

void TokenFileParser::ReadVec2i(Vec2i* out)
{
	out->x = this->ReadInt();
	out->y = this->ReadInt();
}

void TokenFileParser::ReadVec3(Vec3* out)
{
	out->x = this->ReadFloat();
	out->y = this->ReadFloat();
	out->z = this->ReadFloat();
}

void TokenFileParser::ReadSphere(Sphere* out)
{
	this->ReadVec3(&out->position);
	out->radius = this->ReadFloat();
}

void TokenFileParser::ReadAABB(AABB* out)
{
	this->ReadVec3(&out->from);
	this->ReadVec3(&out->to);
}

void TokenFileParser::ReadAssetRef(AssetRef* out, AssetType assetType)
{
	this->ReadString(out->filePath, AssetMaxFilePathLength);
	out->assetType = assetType;
	out->index = -1;
}

void TokenFileParser::BurnWhiteSpace()
{
	while (!this->GetIsEOF())
	{
		char c = ((char*)this->fileData->data)[this->bufferPosition];
		if (!this->GetCharacterIsWhiteSpace(c))
		{
			break;
		}
		else
		{
			this->bufferPosition++;
		}
	}
}

void TokenFileParser::BurnCurrentLine()
{
	while (!this->GetIsEOF())
	{
		char c = ((char*)this->fileData->data)[this->bufferPosition];
		this->bufferPosition++;
		if (c == '\n')
		{
			break;
		}
	}
}

bool TokenFileParser::GetCharacterIsWhiteSpace(char c)
{
	return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}