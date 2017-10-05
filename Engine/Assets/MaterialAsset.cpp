#include "Assets/MaterialAsset.h"

/*MaterialAsset::MaterialAsset(const char* filePath, Buffer* fileData, bool isStayResident)
{
	IFactory* factory = GetFactory();
	ITokenFileParser* parser = factory->MakeTokenFileParser(fileData);

	strcpy(this->filePath, filePath);
	this->isStayResident = isStayResident;

	for (int i = 0; i < MaterialNumberOfTextureTypes; i++)
	{
		this->textureAssetRefs[i].index = -1;
		strcpy(this->textureAssetRefs[i].filePath, "");
	}

	char token[256];

	while (!parser->GetIsEOF())
	{
		parser->ReadString(token, 256);

		if (strcmp(token, "diffuse-texture-asset-ref") == 0)
		{
			parser->ReadAssetRef(&this->textureAssetRefs[MaterialTextureTypeDiffuse], AssetTypeTexture);
		}
	}

	SafeDeleteAndNull(parser);

	this->loadedSuccessfully = true;
}*/

MaterialAsset::MaterialAsset(const char* filePath, Buffer* fileData, bool isStayResident)
{
	IFactory* factory = GetFactory();
	IJsonParser* parser = factory->MakeJsonParser();

	strcpy(this->filePath, filePath);
	this->isStayResident = isStayResident;

	for (int i = 0; i < MaterialNumberOfTextureTypes; i++)
	{
		this->textureAssetRefs[i].index = -1;
		strcpy(this->textureAssetRefs[i].filePath, "");
	}

	IJsonValue* jsonMaterial = parser->ParseJson(fileData->data);

	for (int propertyIndex = 0; propertyIndex < jsonMaterial->GetNumberOfObjectProperties(); propertyIndex++)
	{
		IJsonProperty* jsonProperty = jsonMaterial->GetObjectProperty(propertyIndex);
		IJsonValue* jsonPropertyValue = jsonProperty->GetValue();

		if (strcmp(jsonProperty->GetName(), "diffuse-texture-asset-ref") == 0)
		{
			jsonPropertyValue->CopyAssetRefValue(&this->textureAssetRefs[MaterialTextureTypeDiffuse], AssetTypeTexture);
		}
		/////////////// Test code ///////////////
		else if (strcmp(jsonProperty->GetName(), "emissive-colour") == 0)
		{
			jsonPropertyValue->CopyRbgFloatValue(&this->emissiveColour);
		}
		//////////////////////////////
	}

	SafeDeleteAndNull(jsonMaterial);
	SafeDeleteAndNull(parser);

	this->loadedSuccessfully = true;
}

MaterialAsset::~MaterialAsset()
{
	// Noop.
}

AssetRef* MaterialAsset::GetTextureAssetRef(MaterialTextureType textureType)
{
	return &this->textureAssetRefs[MaterialTextureTypeDiffuse];
}

RgbFloat MaterialAsset::GetEmmissiveColour()
{
	return this->emissiveColour;
}

/*void MaterialAsset::BeginLoadingReferencedAssets()
{
	IEngine* engine = GetEngine();
	IAssetManager* assetManager = engine->GetAssetManager();

	for (int i = 0; i < MaterialNumberOfTextureTypes; i++)
	{
		if (strcmp(this->textureAssetRefs[i].filePath, "") != 0)
		{
			assetManager->BeginLoadingAsset(AssetTypeTexture, this->textureAssetRefs[i].filePath, this->isStayResident, false);
		}
	}
}*/

bool MaterialAsset::ResolveReferencedAssets()
{
	IEngine* engine = GetEngine();
	IAssetManager* assetManager = engine->GetAssetManager();

	bool success = true;

	for (int i = 0; i < MaterialNumberOfTextureTypes; i++)
	{
		if (strcmp(this->textureAssetRefs[i].filePath, "") != 0)
		{
			success &= assetManager->ResolveAssetRefIndex(&this->textureAssetRefs[i]);
		}
	}

	return success;
}

char* MaterialAsset::GetFilePath()
{
	return this->filePath;
}

bool MaterialAsset::GetIsStayResident()
{
	return this->isStayResident;
}

bool MaterialAsset::GetLoadedSuccessfully()
{
	return this->loadedSuccessfully;
}

bool MaterialAsset::GetIsEvictable()
{
	return this->isEvictable;
}

void MaterialAsset::SetIsEvictable(bool isEvictable)
{
	this->isEvictable = isEvictable;
}