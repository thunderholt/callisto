#include "Core/ActorInstance.h"

ActorInstance::ActorInstance()
{
	memset(&this->actorAssetRef, 0, sizeof(AssetRef));
	strcpy(this->name, "");
	Vec3::Zero(&this->worldPosition);
	Vec3::Zero(&worldRotation);
	this->jsonConfig = null;
}

ActorInstance::~ActorInstance()
{
	SafeDeleteAndNull(this->jsonConfig);
}

AssetRef* ActorInstance::GetActorAssetRef()
{
	return &this->actorAssetRef;
}

char* ActorInstance::GetName()
{
	return this->name;
}

Vec3* ActorInstance::GetWorldPosition()
{
	return &this->worldPosition;
}

Vec3* ActorInstance::GetWorldRotation()
{
	return &this->worldRotation;
}

IJsonValue* ActorInstance::GetJsonConfig()
{
	return this->jsonConfig;
}

void ActorInstance::LoadFromJson(IJsonValue* jsonActorInstance)
{
	for (int propertyIndex = 0; propertyIndex < jsonActorInstance->GetNumberOfObjectProperties(); propertyIndex++)
	{
		IJsonProperty* jsonProperty = jsonActorInstance->GetObjectProperty(propertyIndex);
		IJsonValue* jsonPropertyValue = jsonProperty->GetValue();

		if (strcmp(jsonProperty->GetName(), "name") == 0)
		{
			jsonPropertyValue->CopyStringValue(this->name, ActorMaxNameLength);
		}
		else if (strcmp(jsonProperty->GetName(), "asset-ref") == 0)
		{
			jsonPropertyValue->CopyAssetRefValue(&this->actorAssetRef, AssetTypeActor);
		}
		else if (strcmp(jsonProperty->GetName(), "position") == 0)
		{
			jsonPropertyValue->CopyVec3Value(&this->worldPosition);
		}
		else if (strcmp(jsonProperty->GetName(), "rotation") == 0)
		{
			jsonPropertyValue->CopyVec3Value(&this->worldRotation);
		}
		else if (strcmp(jsonProperty->GetName(), "config") == 0)
		{
			this->jsonConfig = jsonPropertyValue->Clone();
		}
	}
}