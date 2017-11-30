#include "Core/LightAtlas.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Lib/stb_image_write.h"

LightAtlas::LightAtlas()
{
	this->texels = null;
	Vec2i::Zero(&this->size);
}

LightAtlas::~LightAtlas()
{
	SafeDeleteArrayAndNull(this->texels);
}

void LightAtlas::Allocate(int width, int height)
{
	this->size.x = width;
	this->size.y = height;

	SafeDeleteArrayAndNull(this->texels);

	int numberOfTexels = this->size.x * this->size.y;
	this->texels = new RgbFloat[numberOfTexels];

	//memset(this->texels, 0, sizeof(RgbFloat) * numberOfTexels);
	for (int i = 0; i < numberOfTexels; i++)
	{
		RgbFloat* texel = &this->texels[i];
		texel->r = -1.0f;
		texel->g = 0.0f;
		texel->b = 0.0f;
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

	RgbUByte* rgbTexels = new RgbUByte[this->size.x * this->size.y];

	for (int y = 0; y < this->size.y; y++)
	{
		for (int x = 0; x < this->size.x; x++)
		{
			int texelIndex = y * this->size.x + x;
			RgbUByte* rgbTexel = &rgbTexels[texelIndex];
			RgbFloat* texel = &this->texels[texelIndex];

			rgbTexel->r = (unsigned char)(Math::Clamp(texel->r, 0.0f, 1.0f) * 255);
			rgbTexel->g = (unsigned char)(Math::Clamp(texel->g, 0.0f, 1.0f) * 255);
			rgbTexel->b = (unsigned char)(Math::Clamp(texel->b, 0.0f, 1.0f) * 255);
		}
	}

	stbi_write_png(filePath, this->size.x, this->size.y, 3, rgbTexels, this->size.x * sizeof(RgbUByte));

	SafeDeleteArrayAndNull(rgbTexels);
}

RgbFloat* LightAtlas::GetTexels()
{
	return this->texels;
}

Vec2i LightAtlas::GetSize()
{
	return this->size;
}