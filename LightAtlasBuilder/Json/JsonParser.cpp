#include "Json/JsonParser.h"

enum ParseObjectState
{
	ParseObjectStateSearchingForObjectStart = 1,
	ParseObjectStateSearchingForPropertyName = 2,
	ParseObjectStateParsingPropertyName = 3,
	ParseObjectStateSearchingForColon = 4,
	ParseObjectStateReadingPropertyValue = 5,
	ParseObjectStateSearchingForComma = 6
};

enum ParseArrayState
{
	ParseArrayStateSearchingForArrayStart = 1,
	ParseArrayStateSearchingForElementValue = 2,
	ParseArrayStateReadingElementValue = 3,
	ParseArrayStateSearchingForComma = 4
};

enum ReadStringLitteralState
{
	ReadStringLitteralSeachingForOpenQuote = 1,
	ReadStringLitteralReadingCharacters = 2
};

JsonParser::JsonParser()
{
	this->Reset();
}

JsonParser::~JsonParser()
{

}

IJsonValue* JsonParser::ParseJson(const char* json)
{
	IFactory* factory = GetFactory();

	this->Reset();
	this->json = json;
	this->jsonLength = strlen(json);

	IJsonValue* jsonValue = factory->MakeJsonValue();

	if (!this->ParseValue(jsonValue))
	{
		delete jsonValue;
		jsonValue = null;
	}

	return jsonValue;
}

void JsonParser::Reset()
{
	this->json = null;
	this->jsonLength = 0;
	this->jsonIndex = 0;
	this->jsonEofReached = false;
	this->errorCode = JsonParserErrorCodeNone;
}

bool JsonParser::ParseValue(IJsonValue* jsonValue)
{
	bool success = true;

	this->BurnWhiteSpace();

	if (!this->jsonEofReached)
	{
		char c = this->PeekChar();

		if (c == '{')
		{
			success = this->ParseObjectValue(jsonValue);
		}
		else if (c == '[')
		{
			success = this->ParseArrayValue(jsonValue);
		}
		else if (c == '"')
		{
			success = this->ParseStringValue(jsonValue);
		}
		else if (this->CheckCharIsNumberFragment(c))
		{
			success = this->ParseNumberValue(jsonValue);
		}
		else
		{
			success = this->ParseKeywordValue(jsonValue);
		}
	}
	else
	{
		this->errorCode = JsonParserErrorCodeEmptyFile;
	}

	return success;
}

bool JsonParser::ParseObjectValue(IJsonValue* jsonValue)
{
	bool success = true;
	ParseObjectState state = ParseObjectStateSearchingForObjectStart;
	char propertyName[256];

	jsonValue->SetType(JsonValueTypeObject);

	while (!this->jsonEofReached)
	{
		if (state == ParseObjectStateSearchingForObjectStart)
		{
			char c = this->PopChar();

			if (c == '{')
			{
				state = ParseObjectStateSearchingForPropertyName;
			}
		}
		else if (state == ParseObjectStateSearchingForPropertyName)
		{
			char c = this->PeekChar();

			if (c == '}')
			{
				this->PopChar();
				break;
			}
			else if (!this->CheckCharIsWhiteSpace(c))
			{
				state = ParseObjectStateParsingPropertyName;
			}
			else
			{
				this->PopChar();
			}
		}
		else if (state == ParseObjectStateParsingPropertyName)
		{
			if (!this->ReadNextStringLitteral(propertyName, sizeof(propertyName)))
			{
				this->errorCode = JsonParserErrorCodeInvalidPropertyName;
				success = false;
				break;
			}
			else
			{
				state = ParseObjectStateSearchingForColon;
			}
		}
		else if (state == ParseObjectStateSearchingForColon)
		{
			char c = this->PopChar();

			if (c == ':')
			{
				state = ParseObjectStateReadingPropertyValue;
			}
			else if (this->jsonEofReached)
			{
				this->errorCode = JsonParserErrorCodeMalformedProperty;
				success = false;
				break;
			}
		}
		else if (state == ParseObjectStateReadingPropertyValue)
		{
			IJsonValue* jsonPropertyValue = jsonValue->AddObjectProperty(propertyName);

			if (!this->ParseValue(jsonPropertyValue))
			{
				success = false;
				break;
			}
			else
			{
				state = ParseObjectStateSearchingForComma;
			}
		}
		else if (state == ParseObjectStateSearchingForComma)
		{
			char c = this->PopChar();
			if (c == ',')
			{
				state = ParseObjectStateParsingPropertyName;
			}
			else if (c == '}')
			{
				break;
			}
		}
	}

	return success;
}

bool JsonParser::ParseArrayValue(IJsonValue* jsonValue)
{
	bool success = true;
	ParseArrayState state = ParseArrayStateSearchingForArrayStart;

	jsonValue->SetType(JsonValueTypeArray);

	while (!this->jsonEofReached)
	{
		if (state == ParseArrayStateSearchingForArrayStart)
		{
			char c = this->PopChar();

			if (c == '[')
			{
				state = ParseArrayStateSearchingForElementValue;
			}
		}
		else if (state == ParseArrayStateSearchingForElementValue)
		{
			char c = this->PeekChar();

			if (c == ']')
			{
				this->PopChar();
				break;
			}
			else if (!this->CheckCharIsWhiteSpace(c))
			{
				state = ParseArrayStateReadingElementValue;
			}
			else
			{
				this->PopChar();
			}
		}
		else if (state == ParseArrayStateReadingElementValue)
		{
			IJsonValue* jsonPropertyValue = jsonValue->AddArrayElement();

			if (!this->ParseValue(jsonPropertyValue))
			{
				success = false;
				break;
			}
			else
			{
				state = ParseArrayStateSearchingForComma;
			}
		}
		else if (state == ParseArrayStateSearchingForComma)
		{
			char c = this->PopChar();
			if (c == ',')
			{
				state = ParseArrayStateReadingElementValue;
			}
			else if (c == ']')
			{
				break;
			}
		}
	}

	return success;
}

bool JsonParser::ParseStringValue(IJsonValue* jsonValue)
{
	bool success = true;
	jsonValue->SetType(JsonValueTypeString);

	char buffer[1024];
	if (!this->ReadNextStringLitteral(buffer, sizeof(buffer)))
	{
		this->errorCode = JsonParserErrorCodeMalformedString;
		success = false;
	}
	else
	{
		jsonValue->SetStringValue(buffer);
	}

	return success;
}

bool JsonParser::ParseNumberValue(IJsonValue* jsonValue)
{
	bool success = true;
	jsonValue->SetType(JsonValueTypeNumber);

	double d = 0;
	if (!this->ReadNextNumberLitteral(&d))
	{
		this->errorCode = JsonParserErrorCodeMalformedNumber;
		success = false;
	}
	else
	{
		jsonValue->SetNumberValue(d);
	}

	return success;
}

bool JsonParser::ParseKeywordValue(IJsonValue* jsonValue)
{
	bool success = true;

	char buffer[32];
	if (!this->ReadNextKeyword(buffer, sizeof(buffer)))
	{
		this->errorCode = JsonParserErrorCodeMalformedKeyword;
		success = false;
	}
	else
	{
		if (strcmp(buffer, "true") == 0)
		{
			jsonValue->SetType(JsonValueTypeBool);
			jsonValue->SetBoolValue(true);
		}
		else if (strcmp(buffer, "false") == 0)
		{
			jsonValue->SetType(JsonValueTypeBool);
			jsonValue->SetBoolValue(false);
		}
		else if (strcmp(buffer, "null") == 0)
		{
			jsonValue->SetType(JsonValueTypeNull);
		}
		else
		{
			this->errorCode = JsonParserErrorUnrecognisedKeyword;
			success = false;
		}
	}

	return success;
}

bool JsonParser::ReadNextStringLitteral(char* buffer, int bufferLength)
{
	bool success = true;
	int bufferPos = 0;
	ReadStringLitteralState state = ReadStringLitteralSeachingForOpenQuote;

	this->BurnWhiteSpace();

	while (!this->jsonEofReached)
	{
		if (state == ReadStringLitteralSeachingForOpenQuote)
		{
			char c = this->PopChar();
			if (c != '"')
			{
				success = false;
				break;
			}
			else
			{
				state = ReadStringLitteralReadingCharacters;
			}
		}
		else if (state == ReadStringLitteralReadingCharacters)
		{
			char c = this->PopChar();
			if (c == '"')
			{
				buffer[bufferPos] = null;
				break;
			}
			else
			{
				buffer[bufferPos] = c;
				bufferPos++;
				if (bufferPos == bufferLength - 2)
				{
					success = false;
					break;
				}
			}
		}
	}

	return success;
}

bool JsonParser::ReadNextNumberLitteral(double* out)
{
	bool success = true;

	char buffer[64];
	int bufferPos = 0;

	while (!this->jsonEofReached)
	{
		char c = this->PeekChar();
		if (this->CheckCharIsNumberFragment(c))
		{
			buffer[bufferPos] = c;
			bufferPos++;
			if (bufferPos >= sizeof(buffer))
			{
				success = false;
				break;
			}

			this->PopChar();
		}
		else
		{
			buffer[bufferPos] = null;
			*out = atof(buffer);
			break;
		}
	}

	return success;
}

bool JsonParser::ReadNextKeyword(char* buffer, int bufferLength)
{
	bool success = true;
	int bufferPos = 0;

	this->BurnWhiteSpace();

	while (!this->jsonEofReached)
	{
		char c = this->PeekChar();
		if (this->CheckCharIsLetter(c))
		{
			buffer[bufferPos] = c;
			bufferPos++;
			if (bufferPos >= bufferLength)
			{
				success = false;
				break;
			}

			this->PopChar();
		}
		else
		{
			buffer[bufferPos] = null;
			break;
		}
	}

	return success;
}

void JsonParser::BurnWhiteSpace()
{
	while (!this->jsonEofReached)
	{
		char c = this->PeekChar();
		if (this->CheckCharIsWhiteSpace(c))
		{
			this->PopChar();
		}
		else
		{
			break;
		}
	}
}

bool JsonParser::CheckCharIsWhiteSpace(char c)
{
	return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

bool JsonParser::CheckCharIsNumberFragment(char c)
{
	return (c >= '0' && c <= '9') || c == '-' || c == '.';
}

bool JsonParser::CheckCharIsLetter(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

char JsonParser::PopChar()
{
	char c = 0;

	if (this->jsonIndex < this->jsonLength)
	{
		c = this->json[this->jsonIndex];
		this->jsonIndex++;

		if (this->jsonIndex >= this->jsonLength)
		{
			this->jsonEofReached = true;
		}
	}

	return c;
}

char JsonParser::PeekChar()
{
	char c = 0;

	if (this->jsonIndex < this->jsonLength)
	{
		c = this->json[this->jsonIndex];
	}

	return c;
}