#pragma once

#include "Include/LightAtlasBuilder.h"

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
	virtual void ReadVec2i(Vec2i* out);
	virtual void ReadVec3(Vec3* out);
	virtual void ReadVec3i(Vec3i* out);
	virtual void ReadRgbFloat(RgbFloat* out);
	virtual void ReadAssetRef(AssetRef* out);

private:
	void BurnWhiteSpace();
	void BurnCurrentLine();
	bool GetCharacterIsWhiteSpace(char c);

	Buffer* fileData;
	int bufferPosition;
};