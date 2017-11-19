#include "Json/JsonProperty.h"

JsonProperty::JsonProperty()
{
	this->name = null;
	this->value = null;
}

JsonProperty::~JsonProperty()
{
	SafeDeleteArrayAndNull(this->name);
	SafeDeleteAndNull(this->value);
}

char* JsonProperty::GetName()
{
	return this->name;
}

void JsonProperty::SetName(const char* name)
{
	if (this->name != null)
	{
		delete[] this->name;
	}

	this->name = new char[strlen(name) + 1];
	strcpy(this->name, name);
}

IJsonValue* JsonProperty::GetValue()
{
	if (this->value == null)
	{
		IFactory* factory = GetFactory();
		this->value = factory->MakeJsonValue();
	}

	return this->value;
}

IJsonProperty* JsonProperty::Clone()
{
	JsonProperty* clone = new JsonProperty();

	clone->name = new char[strlen(this->name) + 1];
	strcpy(clone->name, this->name);

	clone->GetValue()->CopyFrom(this->value);

	return clone;
}