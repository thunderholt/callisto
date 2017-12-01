#include "Core/LightAtlas.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Lib/stb_image_write.h"

LightAtlas::LightAtlas()
{
	this->lumels = null;
	Vec2i::Zero(&this->size);
}

LightAtlas::~LightAtlas()
{
	SafeDeleteArrayAndNull(this->lumels);
}

void LightAtlas::Allocate(int width, int height)
{
	this->size.x = width;
	this->size.y = height;

	SafeDeleteArrayAndNull(this->lumels);

	int numberOfLumels = this->size.x * this->size.y;
	this->lumels = new Lumel[numberOfLumels];

	memset(this->lumels, 0, sizeof(Lumel) * numberOfLumels);

	for (int i = 0; i < numberOfLumels; i++)
	{
		Lumel* lumel = &this->lumels[i];
		lumel->colour.r = 0.0f;
		lumel->colour.g = 1.0f;
		lumel->colour.b = 0.0f;
	}
}

void LightAtlas::WriteToPngFile(const char* filePath)
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	/*int numberOfEmptyPixels = 0;

	do
	{
		numberOfEmptyPixels = 0;

		for (int y = 0; y < this->size.y; y++)
		{
			for (int x = 0; x < this->size.x; x++)
			{
				int texelIndex = y * this->size.x + x;

				RgbFloat* texel = &this->texels[texelIndex];
				if (texel->r == -1.0f)
				{
					numberOfEmptyPixels++;

					int numberOfSamples = 0;
					RgbFloat accumulatedSamples;
					RgbFloat::Zero(&accumulatedSamples);

					for (int j = -1; j <= 1; j++)
					{
						for (int i = -1; i <= 1; i++)
						{
							if (i == 0 && j == 0)
							{
								continue;
							}

							if (i == -1 && x == 0)
							{
								continue;
							}

							if (i == 1 && x == this->size.x - 1)
							{
								continue;
							}

							if (j == -1 && y == 0)
							{
								continue;
							}

							if (j == 1 && y == this->size.y - 1)
							{
								continue;
							}

							int sampleTexelIndex = (y + j) * this->size.x + (x + i);
							RgbFloat* sample = &this->texels[sampleTexelIndex];
							if (sample->r != -1.0f)
							{
								RgbFloat::Add(&accumulatedSamples, &accumulatedSamples, sample);
								numberOfSamples++;
							}
						}
					}

					if (numberOfSamples > 0)
					{
						RgbFloat averageSample;
						RgbFloat::Scale(&averageSample, &accumulatedSamples, 1.0f / (float)numberOfSamples);
						*texel = averageSample;
					}
				}
			}
		}

		logger->Write("Number of empty pixels: %d", numberOfEmptyPixels);

	} while (numberOfEmptyPixels > 0);*/

	/////////////////////

	RgbUByte* outputColours = new RgbUByte[this->size.x * this->size.y];

	for (int y = 0; y < this->size.y; y++)
	{
		for (int x = 0; x < this->size.x; x++)
		{
			int lumelIndex = y * this->size.x + x;
			Lumel* lumel = &this->lumels[lumelIndex];
			RgbFloat* inputColour = &lumel->colour;
			RgbUByte* outputColour = &outputColours[lumelIndex];

			outputColour->r = (unsigned char)(Math::Clamp(inputColour->r, 0.0f, 1.0f) * 255);
			outputColour->g = (unsigned char)(Math::Clamp(inputColour->g, 0.0f, 1.0f) * 255);
			outputColour->b = (unsigned char)(Math::Clamp(inputColour->b, 0.0f, 1.0f) * 255);
		}
	}

	stbi_write_png(filePath, this->size.x, this->size.y, 3, outputColours, this->size.x * sizeof(RgbUByte));

	SafeDeleteArrayAndNull(outputColours);
}

Lumel* LightAtlas::GetLumels()
{
	return this->lumels;
}

Vec2i LightAtlas::GetSize()
{
	return this->size;
}