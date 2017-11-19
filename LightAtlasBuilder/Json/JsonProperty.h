#pragma once

#include "Include/LightAtlasBuilder.h"

class JsonProperty : public IJsonProperty
{
public:
	JsonProperty();
	virtual ~JsonProperty();
	virtual char* GetName();
	virtual void SetName(const char* name);
	virtual IJsonValue* GetValue();
	virtual IJsonProperty* Clone();

private:
	char* name;
	IJsonValue* value;
};
