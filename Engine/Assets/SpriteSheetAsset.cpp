#include "Assets/SpriteSheetAsset.h"

SpriteSheetAsset::SpriteSheetAsset(const char* filePath, Buffer* fileData, bool isStayResident)
{
	IFactory* factory = GetFactory();
	ITokenFileParser* parser = factory->MakeTokenFileParser(fileData);

	strcpy(this->filePath, filePath);
	this->isStayResident = isStayResident;

	char token[256];

	while (!parser->GetIsEOF())
	{
		parser->ReadString(token, 256);

		/*if (strcmp(token, "texture-asset-ref") == 0)
		{
			parser->ReadAssetRef(&this->textureAssetRef, AssetTypeTexture);
		}
		else */if (strcmp(token, "sprite") == 0)
		{
			Sprite* sprite = &this->sprites.PushAndGet();
			parser->ReadString(sprite->name, SpriteMaxNameLength, false);
			parser->ReadVec2(&sprite->position);
			parser->ReadVec2(&sprite->size);
		}
	}

	delete parser;

	this->loadedSuccessfully = true;
}

SpriteSheetAsset::~SpriteSheetAsset()
{
	// Noop.
}

/*void SpriteSheetAsset::BeginLoadingReferencedAssets()
{
	IEngine* engine = GetEngine();
	engine->GetAssetManager()->BeginLoadingAsset(&this->textureAssetRef, this->isStayResident, false);
}*/

bool SpriteSheetAsset::ResolveReferencedAssets()
{
	/*IEngine* engine = GetEngine();

	bool success = true;

	success &= engine->GetAssetManager()->ResolveAssetRefIndex(&this->textureAssetRef);

	return success;*/
	return true;
}

char* SpriteSheetAsset::GetFilePath()
{
	return this->filePath;
}

bool SpriteSheetAsset::GetIsStayResident()
{
	return this->isStayResident;
}

bool SpriteSheetAsset::GetLoadedSuccessfully()
{
	return this->loadedSuccessfully;
}

bool SpriteSheetAsset::GetIsEvictable()
{
	return this->isEvictable;
}

void SpriteSheetAsset::SetIsEvictable(bool isEvictable)
{
	this->isEvictable = isEvictable;
}

/*AssetRef* SpriteSheetAsset::GetTextureAssetRef()
{
	return &this->textureAssetRef;
}*/

int SpriteSheetAsset::FindSpriteIndex(const char* name)
{
	int spriteIndex = -1;

	for (int i = 0; i < this->sprites.GetLength(); i++)
	{
		Sprite* sprite = &this->sprites[i];
		if (strcmp(sprite->name, name) == 0)
		{
			spriteIndex = i;
			break;
		}
	}

	return spriteIndex;
}

Sprite* SpriteSheetAsset::GetSprite(int index)
{
	return &this->sprites[index];
}

int SpriteSheetAsset::GetNumberOfSprites()
{
	return this->sprites.GetLength();
}