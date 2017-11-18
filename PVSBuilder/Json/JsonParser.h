#pragma once

#include "Include/PVSBuilder.h"

class JsonParser : public IJsonParser
{
public:
	JsonParser();
	virtual ~JsonParser();
	virtual IJsonValue* ParseJson(const char* json);

private:
	void Reset();
	bool ParseValue(IJsonValue* jsonValue);
	bool ParseObjectValue(IJsonValue* jsonValue);
	bool ParseArrayValue(IJsonValue* jsonValue);
	bool ParseStringValue(IJsonValue* jsonValue);
	bool ParseNumberValue(IJsonValue* jsonValue);
	bool ParseKeywordValue(IJsonValue* jsonValue);
	bool ReadNextStringLitteral(char* buffer, int bufferLength);
	bool ReadNextNumberLitteral(double* out);
	bool ReadNextKeyword(char* buffer, int bufferLength);
	void BurnWhiteSpace();
	bool CheckCharIsWhiteSpace(char c);
	bool CheckCharIsNumberFragment(char c);
	bool CheckCharIsLetter(char c);
	char PopChar();
	char PeekChar();

	const char* json;
	int jsonLength;
	int jsonIndex;
	bool jsonEofReached;
	JsonParserErrorCode errorCode;
};