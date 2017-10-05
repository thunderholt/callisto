#pragma once

#include "Include/Callisto.h"

class JsonValue : public IJsonValue
{
public:
	JsonValue();
	virtual ~JsonValue();
	virtual JsonValueType GetType();
	virtual void SetType(JsonValueType type);
	virtual IJsonValue* GetObjectProperty(const char* name);
	virtual IJsonValue* AddObjectProperty(const char* name);
	virtual int GetNumberOfObjectProperties();
	virtual IJsonProperty* GetObjectProperty(int index);
	virtual IJsonValue* GetArrayElement(int index);
	virtual IJsonValue* AddArrayElement();
	virtual int GetNumberOfArrayElements();
	virtual int GetIntValue();
	virtual float GetFloatValue();
	virtual double GetDoubleValue();
	virtual void SetNumberValue(double value);
	virtual char* GetStringValue();
	virtual void SetStringValue(const char* value);
	virtual bool GetBoolValue();
	virtual void SetBoolValue(bool value);
	virtual void CopyAssetRefValue(AssetRef* out, AssetType assetType);
	virtual void CopyVec3Value(Vec3* out);
	virtual void CopyVec2Value(Vec2* out);
	virtual void CopySphereValue(Sphere* out);
	virtual void CopyAABBValue(AABB* out);
	virtual void CopyRbgFloatValue(RgbFloat* out);
	virtual void CopyStringValue(char* out, int bufferSize);
	virtual IJsonValue* Clone();
	virtual void CopyFrom(IJsonValue* other);

private:
	void CleanUp();

	JsonValueType type;
	double numberValue;
	char* stringValue;
	bool boolValue;
	DynamicLengthArray<IJsonProperty*> objectProperties;
	DynamicLengthArray<IJsonValue*> arrayElements;
};

