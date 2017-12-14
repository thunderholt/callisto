#include "Json/JsonValue.h"

JsonValue::JsonValue()
{
	this->type = JsonValueTypeNull;
	this->numberValue = 0.0;
	this->stringValue = null;
	this->boolValue = false;
}

JsonValue::~JsonValue()
{
	this->CleanUp();
}

JsonValueType JsonValue::GetType()
{
	return this->type;
}

void JsonValue::SetType(JsonValueType type)
{
	this->type = type;
}

IJsonValue* JsonValue::GetObjectProperty(const char* name)
{
	IJsonValue* value = null;

	if (this->type == JsonValueTypeObject)
	{
		for (int i = 0; i < this->objectProperties.GetLength(); i++)
		{
			IJsonProperty* property = this->objectProperties[i];
			if (strcmp(property->GetName(), name) == 0)
			{
				value = property->GetValue();
				break;
			}
		}
	}

	return value;
}

IJsonValue* JsonValue::AddObjectProperty(const char* name)
{
	IFactory* factory = GetFactory();
	IJsonValue* value = null;

	if (this->type == JsonValueTypeObject)
	{
		IJsonProperty* property = factory->MakeJsonProperty();
		property->SetName(name);
		this->objectProperties.Push(property);

		value = property->GetValue();
	}

	return value;
}

int JsonValue::GetNumberOfObjectProperties()
{
	int numberOfProperties = 0;

	if (this->type == JsonValueTypeObject)
	{
		numberOfProperties = this->objectProperties.GetLength();
	}

	return numberOfProperties;
}

IJsonProperty* JsonValue::GetObjectProperty(int index)
{
	IJsonProperty* property = null;

	if (this->type == JsonValueTypeObject && index < this->objectProperties.GetLength())
	{
		property = this->objectProperties[index];
	}

	return property;
}

IJsonValue* JsonValue::GetArrayElement(int index)
{
	IJsonValue* value = null;

	if (this->type == JsonValueTypeArray && index < this->arrayElements.GetLength())
	{
		value = this->arrayElements[index];
	}

	return value;
}

IJsonValue* JsonValue::AddArrayElement()
{
	IFactory* factory = GetFactory();
	IJsonValue* value = null;

	if (this->type == JsonValueTypeArray)
	{
		value = factory->MakeJsonValue();
		this->arrayElements.Push(value);
	}

	return value;
}

int JsonValue::GetNumberOfArrayElements()
{
	int numberOfElements = 0;

	if (this->type == JsonValueTypeArray)
	{
		numberOfElements = this->arrayElements.GetLength();
	}

	return numberOfElements;
}

int JsonValue::GetIntValue()
{
	int intValue = 0;

	if (this->type == JsonValueTypeNumber)
	{
		intValue = (int)this->numberValue;
	}

	return intValue;
}

float JsonValue::GetFloatValue()
{
	float floatValue = 0.0f;

	if (this->type == JsonValueTypeNumber)
	{
		floatValue = (float)this->numberValue;
	}

	return floatValue;
}

double JsonValue::GetDoubleValue()
{
	double doubleValue = 0.0f;

	if (this->type == JsonValueTypeNumber)
	{
		doubleValue = this->numberValue;
	}

	return doubleValue;
}

void JsonValue::CopyVec3Value(Vec3* out)
{
	if (this->type == JsonValueTypeArray && this->arrayElements.GetLength() == 3)
	{
		out->x = this->arrayElements[0]->GetFloatValue();
		out->y = this->arrayElements[1]->GetFloatValue();
		out->z = this->arrayElements[2]->GetFloatValue();
	}
}

void JsonValue::CopyVec2Value(Vec2* out)
{
	if (this->type == JsonValueTypeArray && this->arrayElements.GetLength() == 2)
	{
		out->x = this->arrayElements[0]->GetFloatValue();
		out->y = this->arrayElements[1]->GetFloatValue();
	}
}

void JsonValue::CopyVec2iValue(Vec2i* out)
{
	if (this->type == JsonValueTypeArray && this->arrayElements.GetLength() == 2)
	{
		out->x = this->arrayElements[0]->GetIntValue();
		out->y = this->arrayElements[1]->GetIntValue();
	}
}

void JsonValue::CopyRgbFloatValue(RgbFloat* out)
{
	out->r = this->arrayElements[0]->GetFloatValue();
	out->g = this->arrayElements[1]->GetFloatValue();
	out->b = this->arrayElements[2]->GetFloatValue();
}

void JsonValue::CopyAABBValue(AABB* out)
{
	if (this->type == JsonValueTypeObject)
	{
		IJsonValue* from = this->GetObjectProperty("from");
		if (from != null)
		{
			from->CopyVec3Value(&out->from);
		}

		IJsonValue* to = this->GetObjectProperty("to");
		if (to != null)
		{
			to->CopyVec3Value(&out->to);
		}
	}
}

void JsonValue::CopyStringValue(char* out, int bufferSize)
{
	if (this->type == JsonValueTypeString)
	{
		StringUtil::CopyString(out, this->stringValue, bufferSize);
	}
}

void JsonValue::SetNumberValue(double value)
{
	if (this->type == JsonValueTypeNumber)
	{
		this->numberValue = value;
	}
}

char* JsonValue::GetStringValue()
{
	char* stringValue = null;

	if (this->type == JsonValueTypeString)
	{
		stringValue = this->stringValue;
	}

	return stringValue;
}

void JsonValue::SetStringValue(const char* value)
{
	if (this->type == JsonValueTypeString)
	{
		if (this->stringValue != null)
		{
			delete[] this->stringValue;
		}

		this->stringValue = new char[strlen(value) + 1];
		strcpy(this->stringValue, value);
	}
}

bool JsonValue::GetBoolValue()
{
	bool boolValue = false;

	if (this->type == JsonValueTypeBool)
	{
		boolValue = this->boolValue;
	}

	return boolValue;
}

void JsonValue::SetBoolValue(bool value)
{
	if (this->type == JsonValueTypeBool)
	{
		this->boolValue = value;
	}
}

IJsonValue* JsonValue::Clone()
{
	IFactory* factory = GetFactory();
	IJsonValue* clone = factory->MakeJsonValue();
	clone->CopyFrom(this);

	return clone;
}

void JsonValue::CopyFrom(IJsonValue* other)
{
	this->CleanUp();

	this->type = other->GetType();

	if (this->type == JsonValueTypeNumber)
	{
		this->numberValue = other->GetDoubleValue();
	}
	else if (this->type == JsonValueTypeString)
	{
		if (other->GetStringValue() != null)
		{
			this->stringValue = new char[strlen(other->GetStringValue()) + 1];
			strcpy(this->stringValue, other->GetStringValue());
		}
	}
	else if (this->type == JsonValueTypeArray)
	{
		for (int i = 0; i < other->GetNumberOfArrayElements(); i++)
		{
			IJsonValue* value = other->GetArrayElement(i);
			this->arrayElements.Push(value->Clone());
		}
	}
	else if (this->type == JsonValueTypeObject)
	{
		for (int i = 0; i < other->GetNumberOfObjectProperties(); i++)
		{
			IJsonProperty* property = other->GetObjectProperty(i);
			this->objectProperties.Push(property->Clone());
		}
	}
	else if (this->type == JsonValueTypeBool)
	{
		this->boolValue = other->GetBoolValue();
	}
}

void JsonValue::CleanUp()
{
	SafeDeleteArrayAndNull(this->stringValue);

	for (int i = 0; i < this->objectProperties.GetLength(); i++)
	{
		delete this->objectProperties[i];
	}

	this->objectProperties.Clear();

	for (int i = 0; i < this->arrayElements.GetLength(); i++)
	{
		delete this->arrayElements[i];
	}

	this->arrayElements.Clear();

	this->type = JsonValueTypeNull;
}