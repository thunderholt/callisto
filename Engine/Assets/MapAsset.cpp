#include "Assets/MapAsset.h"

MapAsset::MapAsset(const char* filePath, Buffer* fileData, bool isStayResident)
{
	IFactory* factory = GetFactory();
	IJsonParser* parser = factory->MakeJsonParser();

	strcpy(this->filePath, filePath);
	this->isStayResident = isStayResident;

	IJsonValue* jsonMap = parser->ParseJson(fileData->data);

	// Load the assets from the JSON map.
	this->LoadAssetRefsFromJsonValue(jsonMap->GetObjectProperty("world-mesh-asset-refs"), AssetTypeWorldMesh);
	this->LoadAssetRefsFromJsonValue(jsonMap->GetObjectProperty("pvs-asset-refs"), AssetTypePVS);
	this->LoadAssetRefsFromJsonValue(jsonMap->GetObjectProperty("static-mesh-asset-refs"), AssetTypeStaticMesh);
	this->LoadAssetRefsFromJsonValue(jsonMap->GetObjectProperty("material-asset-refs"), AssetTypeMaterial);
	this->LoadAssetRefsFromJsonValue(jsonMap->GetObjectProperty("texture-asset-refs"), AssetTypeTexture);
	this->LoadAssetRefsFromJsonValue(jsonMap->GetObjectProperty("actor-asset-refs"), AssetTypeActor);
	this->LoadAssetRefsFromJsonValue(jsonMap->GetObjectProperty("sprite-sheet-asset-refs"), AssetTypeSpriteSheet);

	// Load the world mesh asset ref from the JSON map.
	IJsonValue* jsonWorldMeshAssetRef = jsonMap->GetObjectProperty("world-mesh-asset-ref");
	if (jsonWorldMeshAssetRef != null)
	{
		jsonWorldMeshAssetRef->CopyAssetRefValue(&this->worldMeshAssetRef, AssetTypeWorldMesh);
	}

	// Load the actor instances from the JSON map.
	IJsonValue* jsonActorInstances = jsonMap->GetObjectProperty("actor-instances");
	if (jsonActorInstances != null)
	{
		for (int i = 0; i < jsonActorInstances->GetNumberOfArrayElements(); i++)
		{
			IJsonValue* jsonActorInstance = jsonActorInstances->GetArrayElement(i);
			IActorInstance* actorInstance = factory->MakeActorInstance();
			actorInstance->LoadFromJson(jsonActorInstance);
			this->actorInstances.Push(actorInstance);
		}
	}

	delete jsonMap;
	delete parser;

	this->loadedSuccessfully = true;
}

MapAsset::~MapAsset()
{
	for (int i = 0; i < this->actorInstances.GetLength(); i++)
	{
		SafeDeleteAndNull(this->actorInstances[i]);
	}
}

/*void MapAsset::BeginLoadingReferencedAssets()
{
	IEngine* engine = GetEngine();
	IAssetManager* assetManager = engine->GetAssetManager();

	this->LoadAssets(&this->worldMeshAssetRefs);
	this->LoadAssets(&this->staticMeshAssetRefs);
	this->LoadAssets(&this->materialAssetRefs);
	this->LoadAssets(&this->textureAssetRefs);
	this->LoadAssets(&this->actorAssetRefs);*/

	/*for (int i = 0; i < this->worldMeshAssetRefs.GetLength(); i++)
	{
		AssetRef* assetRef = &this->worldMeshAssetRefs[i];
		assetManager->BeginLoadingAsset(assetRef, this->isStayResident, false);
	}

	for (int i = 0; i < this->actorAssetRefs.GetLength(); i++)
	{
		AssetRef* assetRef = &this->actorAssetRefs[i];
		assetManager->BeginLoadingAsset(assetRef, this->isStayResident, false);
	}
}*/

bool MapAsset::ResolveReferencedAssets()
{
	IEngine* engine = GetEngine();
	IAssetManager* assetManager = engine->GetAssetManager();

	bool success = true;

	for (int i = 0; i < this->assetRefs.GetLength(); i++)
	{
		AssetRef* assetRef = &this->assetRefs[i];
		success &= assetManager->ResolveAssetRefIndex(assetRef);
	}

	success &= assetManager->ResolveAssetRefIndex(&this->worldMeshAssetRef);

	for (int i = 0; i < this->actorInstances.GetLength(); i++)
	{
		IActorInstance* actorInstance = this->actorInstances[i];
		success &= assetManager->ResolveAssetRefIndex(actorInstance->GetActorAssetRef());
	}

	/*for (int i = 0; i < this->worldMeshAssetRefs.GetLength(); i++)
	{
		AssetRef* assetRef = &this->worldMeshAssetRefs[i];
		success &= assetManager->ResolveAssetRefIndex(assetRef);
	}

	for (int i = 0; i < this->actorAssetRefs.GetLength(); i++)
	{
		AssetRef* assetRef = &this->actorAssetRefs[i];
		success &= assetManager->ResolveAssetRefIndex(assetRef);
	}*/

	return success;
}

char* MapAsset::GetFilePath()
{
	return this->filePath;
}

bool MapAsset::GetIsStayResident()
{
	return this->isStayResident;
}

bool MapAsset::GetLoadedSuccessfully()
{
	return this->loadedSuccessfully;
}

bool MapAsset::GetIsEvictable()
{
	return this->isEvictable;
}

void MapAsset::SetIsEvictable(bool isEvictable)
{
	this->isEvictable = isEvictable;
}

AssetRef* MapAsset::GetAssetRef(int index)
{
	return &this->assetRefs[index];
}

int MapAsset::GetNumberAssetRefs()
{
	return this->assetRefs.GetLength();
}

AssetRef* MapAsset::GetWorldMeshAssetRef()
{
	return &this->worldMeshAssetRef;
}

/*AssetRef* MapAsset::GetWorldMeshAssetRef(int index)
{
	return &this->worldMeshAssetRefs[index];
}

int MapAsset::GetNumberOfWorldMeshAssetRefs()
{
	return this->worldMeshAssetRefs.GetLength();
}

AssetRef* MapAsset::GetStaticMeshAssetRef(int index)
{
	return &this->staticMeshAssetRefs[index];
}

int MapAsset::GetNumberOfStaticMeshAssetRefs()
{
	return this->staticMeshAssetRefs.GetLength();
}

AssetRef* MapAsset::GetMaterialAssetRef(int index)
{
	return &this->materialAssetRefs[index];
}

int MapAsset::GetNumberOfMaterialAssetRefs()
{
	return this->materialAssetRefs.GetLength();
}

AssetRef* MapAsset::GetTextureAssetRef(int index)
{
	return &this->textureAssetRefs[index];
}

int MapAsset::GetNumberOfTextureAssetRefs()
{
	return this->textureAssetRefs.GetLength();
}

AssetRef* MapAsset::GetActorAssetRef(int index)
{
	return &this->actorAssetRefs[index];
}

int MapAsset::GetNumberOfActorAssetRefs()
{
	return this->actorAssetRefs.GetLength();
}*/

IActorInstance* MapAsset::GetActorInstance(int index)
{
	return this->actorInstances[index];
}

int MapAsset::GetNumberOfActorInstances()
{
	return this->actorInstances.GetLength();
}

void MapAsset::LoadAssetRefsFromJsonValue(IJsonValue* jsonAssetRefs, AssetType assetType)
{
	if (jsonAssetRefs != null)
	{
		for (int i = 0; i < jsonAssetRefs->GetNumberOfArrayElements(); i++)
		{
			AssetRef* assetRef = &this->assetRefs.PushAndGet();
			jsonAssetRefs->GetArrayElement(i)->CopyAssetRefValue(assetRef, assetType);
		}
	}
}

/*void MapAsset::LoadActorInstanceFromJsonValue(IJsonValue* jsonActorInstance)
{
	IActorInstance* actorInstance = this->actorInstances.PushAndGet();

	for (int propertyIndex = 0; propertyIndex < jsonActorInstance->GetNumberOfObjectProperties(); propertyIndex++)
	{
		IJsonProperty* jsonProperty = jsonActorInstance->GetObjectProperty(propertyIndex);
		IJsonValue* jsonPropertyValue = jsonProperty->GetValue();

		if (strcmp(jsonProperty->GetName(), "name") == 0)
		{
			jsonPropertyValue->CopyStringValue(actorInstance->name, ActorMaxNameLength);
		}
		else if (strcmp(jsonProperty->GetName(), "asset-ref") == 0)
		{
			jsonPropertyValue->CopyAssetRefValue(&actorInstance->actorAssetRef, AssetTypeActor);
		}
		else if (strcmp(jsonProperty->GetName(), "position") == 0)
		{
			jsonPropertyValue->CopyVec3Value(&actorInstance->worldPosition);
		}
		else if (strcmp(jsonProperty->GetName(), "rotation") == 0)
		{
			jsonPropertyValue->CopyVec3Value(&actorInstance->worldRotation);
		}
	}
}*/

/*void MapAsset::LoadAssets(DynamicLengthArray<AssetRef>* assetRefs)
{
	IEngine* engine = GetEngine();
	IAssetManager* assetManager = engine->GetAssetManager();

	for (int i = 0; i < assetRefs->GetLength(); i++)
	{
		AssetRef* assetRef = &(*assetRefs)[i];
		assetManager->BeginLoadingAsset(assetRef, this->isStayResident, false);
	}
}*/