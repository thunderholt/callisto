#include "Core/MaterialAsset.h"

MaterialAsset::MaterialAsset()
{
	this->isVisibilityOccluder = false;
}

MaterialAsset::~MaterialAsset()
{

}

bool MaterialAsset::Load(const char* filePath)
{
	IFactory* factory = GetFactory();
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	bool success = false;
	Buffer fileData = { 0 };

	IFile* file = factory->MakeFile();
	if (!file->OpenForReading(filePath))
	{
		logger->Write("Failed to open world mesh file '%s'.", filePath);
	}
	else
	{
		fileData.length = file->GetLength();
		fileData.data = new char[fileData.length];
		int numberOfBytesRead = file->Read(fileData.data, fileData.length);

		if (numberOfBytesRead != fileData.length)
		{
			logger->Write("Unable to read file '%s'.", filePath);
		}
		else
		{
			IJsonParser* parser = factory->MakeJsonParser();

			IJsonValue* jsonMaterial = parser->ParseJson(fileData.data);

			for (int propertyIndex = 0; propertyIndex < jsonMaterial->GetNumberOfObjectProperties(); propertyIndex++)
			{
				IJsonProperty* jsonProperty = jsonMaterial->GetObjectProperty(propertyIndex);
				IJsonValue* jsonPropertyValue = jsonProperty->GetValue();

				if (strcmp(jsonProperty->GetName(), "is-visibility-occluder") == 0)
				{
					this->isVisibilityOccluder = jsonPropertyValue->GetBoolValue();
				}
			}

			SafeDeleteAndNull(jsonMaterial);
			SafeDeleteAndNull(parser);

			success = true;
		}
	}

	SafeDeleteAndNull(file);
	SafeDeleteArrayAndNull(fileData.data);

	return success;
}

bool MaterialAsset::GetIsVisibilityOcculuder()
{
	return this->isVisibilityOccluder;
}