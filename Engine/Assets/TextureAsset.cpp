#include "Assets/TextureAsset.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_NO_STDIO
#include "Lib/stb_image.h"

TextureAsset::TextureAsset(const char* filePath, Buffer* fileData, bool isStayResident)
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();
	IRasterizer* rasterizer = engine->GetRasterizer();

	strcpy(this->filePath, filePath);
	this->isStayResident = isStayResident;

	int width, height, imageBytesPerPixel = 0;

	// Attempt to parse the file data.
	RgbaUByte* tempImageData = (RgbaUByte*)stbi_load_from_memory(
		(unsigned char*)fileData->data, fileData->length, &width, &height, &imageBytesPerPixel, 4);

	if (tempImageData == null)
	{
		logger->Write("Image not parsable.");
	}
	else
	{
		this->textureId = rasterizer->CreateTexture(width, height, tempImageData);
		Vec2::Set(&this->size, (float)width, (float)height);

		// All done!
		this->loadedSuccessfully = true;
		
		stbi_image_free(tempImageData);
	}
}

TextureAsset::~TextureAsset()
{
	IEngine* engine = GetEngine();
	IRasterizer* rasterizer = engine->GetRasterizer();

	rasterizer->DeleteTexture(&this->textureId);
}

/*void TextureAsset::BeginLoadingReferencedAssets()
{
	// Noop.
}*/

bool TextureAsset::ResolveReferencedAssets()
{
	// Noop.
	return true;
}

char* TextureAsset::GetFilePath()
{
	return this->filePath;
}

bool TextureAsset::GetIsStayResident()
{
	return this->isStayResident;
}

bool TextureAsset::GetLoadedSuccessfully()
{
	return this->loadedSuccessfully;
}

bool TextureAsset::GetIsEvictable()
{
	return this->isEvictable;
}

void TextureAsset::SetIsEvictable(bool isEvictable)
{
	this->isEvictable = isEvictable;
}

Vec2* TextureAsset::GetSize()
{
	return &this->size;
}

ProviderId TextureAsset::GetTextureId()
{
	return this->textureId;
}