#include "Assets/TextAsset.h"

TextAsset::TextAsset(const char* filePath, Buffer* fileData, bool isStayResident)
{
	strcpy(this->filePath, filePath);
	this->isStayResident = isStayResident;

	// Copy the file data.
	this->text = new char[fileData->length + 1];
	memcpy(this->text, fileData->data, fileData->length);

	// Add the null-terminator.
	this->text[fileData->length] = null;

	this->loadedSuccessfully = true;
}

TextAsset::~TextAsset()
{
	SafeDeleteArrayAndNull(this->text);
}

/*void TextAsset::BeginLoadingReferencedAssets()
{
	// Noop.
}*/

bool TextAsset::ResolveReferencedAssets()
{
	// Noop.
	return true;
}

char* TextAsset::GetFilePath()
{
	return this->filePath;
}

bool TextAsset::GetIsStayResident()
{
	return this->isStayResident;
}

bool TextAsset::GetLoadedSuccessfully()
{
	return this->loadedSuccessfully;
}

bool TextAsset::GetIsEvictable()
{
	return this->isEvictable;
}

void TextAsset::SetIsEvictable(bool isEvictable)
{
	this->isEvictable = isEvictable;
}

char* TextAsset::GetText()
{
	return this->text;
}