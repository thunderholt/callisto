#include "Core/MaterialAsset.h"

MaterialAsset::MaterialAsset()
{
	//memset(&this->staticLightingDetails, 0, sizeof(MaterialStaticLightingDetails));
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

				/*if (strcmp(jsonProperty->GetName(), "static-lighting-details") == 0)
				{
					this->LoadStaticLightingDetailsFromJsonValue(jsonProperty->GetValue());
				}*/
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

/*MaterialStaticLightingDetails* MaterialAsset::GetStaticLightingDetails()
{
	return &this->staticLightingDetails;
}*/

/*
void MaterialAsset::LoadStaticLightingDetailsFromJsonValue(IJsonValue* jsonStaticLightingDetails)
{
	for (int propertyIndex = 0; propertyIndex < jsonStaticLightingDetails->GetNumberOfObjectProperties(); propertyIndex++)
	{
		IJsonProperty* jsonProperty = jsonStaticLightingDetails->GetObjectProperty(propertyIndex);
		IJsonValue* jsonPropertyValue = jsonProperty->GetValue();

		if (strcmp(jsonProperty->GetName(), "emits-light") == 0)
		{
			this->staticLightingDetails.emitsLight = jsonPropertyValue->GetBoolValue();
		}
		else if (strcmp(jsonProperty->GetName(), "colour") == 0)
		{
			jsonPropertyValue->CopyRgbFloatValue(&this->staticLightingDetails.colour);
		}
		else if (strcmp(jsonProperty->GetName(), "power") == 0)
		{
			this->staticLightingDetails.power = jsonPropertyValue->GetFloatValue();
		}
		else if (strcmp(jsonProperty->GetName(), "grid-dimensions") == 0)
		{
			jsonPropertyValue->CopyVec2iValue(&this->staticLightingDetails.gridDimensions);
		}
		else if (strcmp(jsonProperty->GetName(), "min-cone-angle") == 0)
		{
			this->staticLightingDetails.minConeAngle = jsonPropertyValue->GetFloatValue();
		}
		else if (strcmp(jsonProperty->GetName(), "distance") == 0)
		{
			this->staticLightingDetails.distance = jsonPropertyValue->GetFloatValue();
		}
	}
}*/