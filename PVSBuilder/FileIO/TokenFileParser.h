#pragma once

#include "Include/PVSBuilder.h"

class TokenFileParser : public ITokenFileParser
{
public:
	TokenFileParser(Buffer* fileData);
	virtual ~TokenFileParser();
	virtual bool GetIsEOF();
	virtual void ReadString(char* buffer, int bufferLength);
	virtual void ReadString(char* buffer, int bufferLength, bool enableComments);
	virtual int ReadInt();
	virtual float ReadFloat();
	virtual bool ReadBool();
	virtual void ReadVec2(Vec2* out);
	virtual void ReadVec3(Vec3* out);

private:
	void BurnWhiteSpace();
	void BurnCurrentLine();
	bool GetCharacterIsWhiteSpace(char c);

	Buffer* fileData;
	int bufferPosition;
};