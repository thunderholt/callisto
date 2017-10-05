#include "Assets/ActorAsset.h"

ActorAsset::ActorAsset(const char* filePath, Buffer* fileData, bool isStayResident)
{
	IFactory* factory = GetFactory();
	IJsonParser* parser = factory->MakeJsonParser();

	strcpy(this->filePath, filePath);
	this->isStayResident = isStayResident;

	strcpy(this->actorSpec.controllerName, "");
	AABB::Zero(&this->actorSpec.localAABB);
	Sphere::Zero(&this->actorSpec.localCollisionSphere);
	this->jsonConfig = null;

	IJsonValue* jsonActorSpec = parser->ParseJson(fileData->data);
	this->LoadActorSpecFromJsonValue(jsonActorSpec);

	delete jsonActorSpec;
	delete parser;

	this->loadedSuccessfully = true;
}

ActorAsset::~ActorAsset()
{
	IEngine* engine = GetEngine();
	IVolumeManager* volumeManager = engine->GetVolumeManager();

	for (int subActorIndex = 0; subActorIndex < this->subActorSpecs.GetLength(); subActorIndex++)
	{
		SubActorSpec* subActorSpec = &this->subActorSpecs[subActorIndex];
		if (subActorSpec->type == SubActorTypeVolume)
		{
			volumeManager->DeleteVolume(subActorSpec->details.volumeDetails.volumeIndex);
		}
	}

	SafeDeleteAndNull(this->jsonConfig);
}

bool ActorAsset::ResolveReferencedAssets()
{
	IEngine* engine = GetEngine();
	IAssetManager* assetManager = engine->GetAssetManager();

	bool success = true;

	// Resolve the static mesh asset refs.
	for (int i = 0; i < this->staticMeshAssetRefs.GetLength(); i++)
	{
		AssetRef* assetRef = &this->staticMeshAssetRefs[i];
		success &= assetManager->ResolveAssetRefIndex(assetRef);
	}

	// Resolve the sprite sheet asset refs.
	for (int i = 0; i < this->spriteSheetAssetRefs.GetLength(); i++)
	{
		AssetRef* assetRef = &this->spriteSheetAssetRefs[i];
		success &= assetManager->ResolveAssetRefIndex(assetRef);
	}

	// Resolve the texture asset refs.
	for (int i = 0; i < this->textureAssetRefs.GetLength(); i++)
	{
		AssetRef* assetRef = &this->textureAssetRefs[i];
		success &= assetManager->ResolveAssetRefIndex(assetRef);
	}

	return success;
}

char* ActorAsset::GetFilePath()
{
	return this->filePath;
}

bool ActorAsset::GetIsStayResident()
{
	return this->isStayResident;
}

bool ActorAsset::GetLoadedSuccessfully()
{
	return this->loadedSuccessfully;
}

bool ActorAsset::GetIsEvictable()
{
	return this->isEvictable;
}

void ActorAsset::SetIsEvictable(bool isEvictable)
{
	this->isEvictable = isEvictable;
}

AssetRef* ActorAsset::GetStaticMeshAssetRef(int index)
{
	return &this->staticMeshAssetRefs[index];
}

int ActorAsset::GetNumberOfStaticMeshAssetRefs()
{
	return this->staticMeshAssetRefs.GetLength();
}

AssetRef* ActorAsset::GetSkinnedMeshAssetRef(int index)
{
	return &this->skinnedMeshAssetRefs[index];
}

int ActorAsset::GetNumberOfSkinnedMeshAssetRefs()
{
	return this->skinnedMeshAssetRefs.GetLength();
}

AssetRef* ActorAsset::GetSkinnedMeshAnimationAssetRef(int index)
{
	return &this->skinnedMeshAnimationAssetRefs[index];
}

int ActorAsset::GetNumberOfSkinnedMeshAnimationAssetRefs()
{
	return this->skinnedMeshAnimationAssetRefs.GetLength();
}

AssetRef* ActorAsset::GetSpriteSheetAssetRef(int index)
{
	return &this->spriteSheetAssetRefs[index];
}

int ActorAsset::GetNumberOfSpriteSheetAssetRefs()
{
	return this->spriteSheetAssetRefs.GetLength();
}

AssetRef* ActorAsset::GetTextureAssetRef(int index)
{
	return &this->textureAssetRefs[index];
}

int ActorAsset::GetNumberOfTextureAssetRefs()
{
	return this->textureAssetRefs.GetLength();
}

ActorSpec* ActorAsset::GetActorSpec()
{
	return &this->actorSpec;
}

SubActorSpec* ActorAsset::GetSubActorSpec(int index)
{
	return &this->subActorSpecs[index];
}

int ActorAsset::GetNumberOfSubActorSpecs()
{
	return this->subActorSpecs.GetLength();
}

IJsonValue* ActorAsset::GetJsonConfig()
{
	return this->jsonConfig;
}

/*AABB* ActorAsset::GetLocalAABB()
{
	return &this->localAABB;
}*/

void ActorAsset::LoadActorSpecFromJsonValue(IJsonValue* jsonActorSpec)
{
	for (int propertyIndex = 0; propertyIndex < jsonActorSpec->GetNumberOfObjectProperties(); propertyIndex++)
	{
		IJsonProperty* jsonProperty = jsonActorSpec->GetObjectProperty(propertyIndex);
		IJsonValue* jsonPropertyValue = jsonProperty->GetValue();

		if (strcmp(jsonProperty->GetName(), "controller") == 0)
		{
			jsonPropertyValue->CopyStringValue(this->actorSpec.controllerName, ActorControllerMaxNameLength);
		}
		else if (strcmp(jsonProperty->GetName(), "local-aabb") == 0)
		{
			jsonPropertyValue->CopyAABBValue(&this->actorSpec.localAABB);
		}
		else if (strcmp(jsonProperty->GetName(), "local-collision-sphere") == 0)
		{
			jsonPropertyValue->CopySphereValue(&this->actorSpec.localCollisionSphere);
		}
		else if (strcmp(jsonProperty->GetName(), "static-mesh-asset-refs") == 0)
		{
			for (int i = 0; i < jsonPropertyValue->GetNumberOfArrayElements(); i++)
			{
				AssetRef* assetRef = &this->staticMeshAssetRefs.PushAndGet();
				jsonPropertyValue->GetArrayElement(i)->CopyAssetRefValue(assetRef, AssetTypeStaticMesh);
			}
		}
		else if (strcmp(jsonProperty->GetName(), "sprite-sheet-asset-refs") == 0)
		{
			for (int i = 0; i < jsonPropertyValue->GetNumberOfArrayElements(); i++)
			{
				AssetRef* assetRef = &this->spriteSheetAssetRefs.PushAndGet();
				jsonPropertyValue->GetArrayElement(i)->CopyAssetRefValue(assetRef, AssetTypeSpriteSheet);
			}
		}
		else if (strcmp(jsonProperty->GetName(), "texture-asset-refs") == 0)
		{
			for (int i = 0; i < jsonPropertyValue->GetNumberOfArrayElements(); i++)
			{
				AssetRef* assetRef = &this->textureAssetRefs.PushAndGet();
				jsonPropertyValue->GetArrayElement(i)->CopyAssetRefValue(assetRef, AssetTypeTexture);
			}
		}
		else if (strcmp(jsonProperty->GetName(), "sub-actors") == 0)
		{
			for (int i = 0; i < jsonPropertyValue->GetNumberOfArrayElements(); i++)
			{
				IJsonValue* jsonSubActorSpec = jsonPropertyValue->GetArrayElement(i);
				this->LoadSubActorSpecFromJsonValue(jsonSubActorSpec);
			}
		}
		else if (strcmp(jsonProperty->GetName(), "config") == 0)
		{
			this->jsonConfig = jsonPropertyValue->Clone();
		}
	}
}

void ActorAsset::LoadSubActorSpecFromJsonValue(IJsonValue* jsonSubActorSpec)
{
	SubActorSpec* subActorSpec = &this->subActorSpecs.PushAndGet();
	memset(subActorSpec, 0, sizeof(SubActorSpec));
	strcpy(subActorSpec->name, "Untitled");
	subActorSpec->parentSubActorIndex = -1;
	subActorSpec->type = SubActorTypeDummy;
	subActorSpec->debugIcon = DebugIconNone;

	for (int propertyIndex = 0; propertyIndex < jsonSubActorSpec->GetNumberOfObjectProperties(); propertyIndex++)
	{
		IJsonProperty* jsonProperty = jsonSubActorSpec->GetObjectProperty(propertyIndex);
		IJsonValue* jsonPropertyValue = jsonProperty->GetValue();

		if (strcmp(jsonProperty->GetName(), "type") == 0)
		{
			subActorSpec->type = this->GetSubActorTypeFromTypeName(jsonPropertyValue->GetStringValue());
		}
		else if (strcmp(jsonProperty->GetName(), "name") == 0)
		{
			jsonPropertyValue->CopyStringValue(subActorSpec->name, SubActorMaxNameLength);
		}
		else if (strcmp(jsonProperty->GetName(), "parent") == 0)
		{
			subActorSpec->parentSubActorIndex = jsonPropertyValue->GetIntValue();
		}
		else if (strcmp(jsonProperty->GetName(), "position") == 0)
		{
			jsonPropertyValue->CopyVec3Value(&subActorSpec->localPositionBase);
		}
		else if (strcmp(jsonProperty->GetName(), "rotation") == 0)
		{
			jsonPropertyValue->CopyVec3Value(&subActorSpec->localRotationBase);
		}
		else if (strcmp(jsonProperty->GetName(), "trigger-sphere-details") == 0)
		{
			this->LoadSubActorSpecTriggerSphereDetailsFromJsonValue(jsonPropertyValue, subActorSpec);
		}
		else if (strcmp(jsonProperty->GetName(), "camera-details") == 0)
		{
			this->LoadSubActorSpecCameraDetailsFromJsonValue(jsonPropertyValue, subActorSpec);
		}
		else if (strcmp(jsonProperty->GetName(), "static-mesh-details") == 0)
		{
			this->LoadSubActorSpecStaticMeshDetailsFromJsonValue(jsonPropertyValue, subActorSpec);
		}
		else if (strcmp(jsonProperty->GetName(), "sprite-3d-details") == 0)
		{
			this->LoadSubActorSpecSprite3DFromJsonValue(jsonPropertyValue, subActorSpec);
		}
		else if (strcmp(jsonProperty->GetName(), "volume-details") == 0)
		{
			this->LoadSubActorSpecVolumeFromJsonValue(jsonPropertyValue, subActorSpec);
		}
		else if (strcmp(jsonProperty->GetName(), "debug-icon") == 0)
		{
			subActorSpec->debugIcon = this->GetDebugIconFromDebugIconName(jsonPropertyValue->GetStringValue());
		}
	}
}

void ActorAsset::LoadSubActorSpecTriggerSphereDetailsFromJsonValue(IJsonValue* jsonTriggerSphereDetails, SubActorSpec* subActorSpec)
{
	IEngine* engine = GetEngine();
	IGameController* gameController = engine->GetGameController();

	SubActorSpecTriggerSphereDetails* triggerSphereDetails = &subActorSpec->details.triggerSphereDetails;

	for (int propertyIndex = 0; propertyIndex < jsonTriggerSphereDetails->GetNumberOfObjectProperties(); propertyIndex++)
	{
		IJsonProperty* jsonProperty = jsonTriggerSphereDetails->GetObjectProperty(propertyIndex);
		IJsonValue* jsonPropertyValue = jsonProperty->GetValue();

		if (strcmp(jsonProperty->GetName(), "radius") == 0)
		{
			triggerSphereDetails->radius = jsonPropertyValue->GetFloatValue();
		}
		else if (strcmp(jsonProperty->GetName(), "applicableActorTypes") == 0)
		{
			for (int i = 0; i < jsonPropertyValue->GetNumberOfArrayElements() && i < SubActorTriggerSphereMaxApplicableActorTypes; i++)
			{
				IJsonValue* jsonActorType = jsonPropertyValue->GetArrayElement(i);
				triggerSphereDetails->applicableActorTypes[i] = gameController->ParseActorType(jsonActorType->GetStringValue());
				triggerSphereDetails->numberOfApplicableActorTypes++;
			}
		}
	}
}

void ActorAsset::LoadSubActorSpecCameraDetailsFromJsonValue(IJsonValue* jsonCameraDetails, SubActorSpec* subActorSpec)
{
	for (int propertyIndex = 0; propertyIndex < jsonCameraDetails->GetNumberOfObjectProperties(); propertyIndex++)
	{
		IJsonProperty* jsonProperty = jsonCameraDetails->GetObjectProperty(propertyIndex);
		IJsonValue* jsonPropertyValue = jsonProperty->GetValue();

		if (strcmp(jsonProperty->GetName(), "fov") == 0)
		{
			subActorSpec->details.cameraDetails.fov = jsonPropertyValue->GetFloatValue();
		}
	}
}

void ActorAsset::LoadSubActorSpecStaticMeshDetailsFromJsonValue(IJsonValue* jsonStaticMeshDetails, SubActorSpec* subActorSpec)
{
	for (int propertyIndex = 0; propertyIndex < jsonStaticMeshDetails->GetNumberOfObjectProperties(); propertyIndex++)
	{
		IJsonProperty* jsonProperty = jsonStaticMeshDetails->GetObjectProperty(propertyIndex);
		IJsonValue* jsonPropertyValue = jsonProperty->GetValue();

		if (strcmp(jsonProperty->GetName(), "static-mesh-asset-ref-index") == 0)
		{
			subActorSpec->details.staticMeshDetails.staticMeshAssetRefIndex = jsonPropertyValue->GetIntValue();
		}
		else if (strcmp(jsonProperty->GetName(), "is-collidable") == 0)
		{
			subActorSpec->details.staticMeshDetails.isCollidable = jsonPropertyValue->GetBoolValue();
		}
	}
}

void ActorAsset::LoadSubActorSpecSprite3DFromJsonValue(IJsonValue* jsonSprite3DDetails, SubActorSpec* subActorSpec)
{
	for (int propertyIndex = 0; propertyIndex < jsonSprite3DDetails->GetNumberOfObjectProperties(); propertyIndex++)
	{
		IJsonProperty* jsonProperty = jsonSprite3DDetails->GetObjectProperty(propertyIndex);
		IJsonValue* jsonPropertyValue = jsonProperty->GetValue();

		if (strcmp(jsonProperty->GetName(), "sprite-sheet-asset-ref-index") == 0)
		{
			subActorSpec->details.sprite3DDetails.spriteSheetAssetRefIndex = jsonPropertyValue->GetIntValue();
		}
		else if (strcmp(jsonProperty->GetName(), "texture-asset-ref-index") == 0)
		{
			subActorSpec->details.sprite3DDetails.textureAssetRefIndex = jsonPropertyValue->GetIntValue();
		}
		else if (strcmp(jsonProperty->GetName(), "sprite-name") == 0)
		{
			jsonPropertyValue->CopyStringValue(subActorSpec->details.sprite3DDetails.spriteName, SpriteMaxNameLength);
		}
		else if (strcmp(jsonProperty->GetName(), "size") == 0)
		{
			jsonPropertyValue->CopyVec2Value(&subActorSpec->details.sprite3DDetails.size);
		}
		else if (strcmp(jsonProperty->GetName(), "rotation") == 0)
		{
			subActorSpec->details.sprite3DDetails.rotation = jsonPropertyValue->GetFloatValue();
		}
	}
}

void ActorAsset::LoadSubActorSpecVolumeFromJsonValue(IJsonValue* jsonVolumeDetails, SubActorSpec* subActorSpec)
{
	IEngine* engine = GetEngine();
	IVolumeManager* volumeManager = engine->GetVolumeManager();

	VolumeSpec volumeSpec;
	memset(&volumeSpec, 0, sizeof(VolumeSpec));

	for (int propertyIndex = 0; propertyIndex < jsonVolumeDetails->GetNumberOfObjectProperties(); propertyIndex++)
	{
		IJsonProperty* jsonProperty = jsonVolumeDetails->GetObjectProperty(propertyIndex);
		IJsonValue* jsonPropertyValue = jsonProperty->GetValue();

		if (strcmp(jsonProperty->GetName(), "type") == 0)
		{
			volumeSpec.type = 
				this->GetVolumeTypeFromTypeName(jsonPropertyValue->GetStringValue());
		}
		else if (strcmp(jsonProperty->GetName(), "number-of-sides") == 0)
		{
			volumeSpec.cylinderSpec.numberOfSides =
				jsonPropertyValue->GetIntValue();
		}
		else if (strcmp(jsonProperty->GetName(), "size") == 0)
		{
			jsonPropertyValue->CopyVec3Value(&volumeSpec.cylinderSpec.size);
		}
	}

	subActorSpec->details.volumeDetails.volumeIndex = volumeManager->CreateVolume(&volumeSpec);
}

SubActorType ActorAsset::GetSubActorTypeFromTypeName(const char* subActorTypeName)
{
	SubActorType subActorType = SubActorTypeDummy;

	if (strcmp(subActorTypeName, "static-mesh") == 0)
	{
		subActorType = SubActorTypeStaticMesh;
	}
	else if (strcmp(subActorTypeName, "skinned-mesh") == 0)
	{
		subActorType = SubActorTypeSkinnedMesh;
	}
	else if (strcmp(subActorTypeName, "camera") == 0)
	{
		subActorType = SubActorTypeCamera;
	}
	else if (strcmp(subActorTypeName, "trigger-sphere") == 0)
	{
		subActorType = SubActorTypeTriggerSphere;
	}
	else if (strcmp(subActorTypeName, "target") == 0)
	{
		subActorType = SubActorTypeTarget;
	}
	else if (strcmp(subActorTypeName, "sprite-3d") == 0)
	{
		subActorType = SubActorTypeSprite3D;
	}
	else if (strcmp(subActorTypeName, "volume") == 0)
	{
		subActorType = SubActorTypeVolume;
	}
	
	return subActorType;
}

VolumeType ActorAsset::GetVolumeTypeFromTypeName(const char* volumeTypeName)
{
	VolumeType volumeType = VolumeTypeNone;

	if (strcmp(volumeTypeName, "cylinder") == 0)
	{
		volumeType = VolumeTypeCylinder;
	}

	return volumeType;
}

DebugIcon ActorAsset::GetDebugIconFromDebugIconName(const char* debugIconName)
{
	DebugIcon debugIcon = DebugIconNone;

	if (strcmp(debugIconName, "wander-to-point") == 0)
	{
		debugIcon = DebugIconWanderToPoint;
	}
	else if (strcmp(debugIconName, "trigger") == 0)
	{
		debugIcon = DebugIconTrigger;
	}
	else if (strcmp(debugIconName, "eye") == 0)
	{
		debugIcon = DebugIconEye;
	}
	else if (strcmp(debugIconName, "target") == 0)
	{
		debugIcon = DebugIconTarget;
	}

	return debugIcon;
}