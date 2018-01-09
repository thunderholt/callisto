#include "Rasterization/OpenGL/OpenGLRasterizer.h"
#include "Rasterization/OpenGL/OpenGLHelper.h"

OpenGLRasterizer::OpenGLRasterizer()
{
	memset(&this->programs, 0, sizeof(RasterizerPrograms));
	memset(&this->sprite2DBuffers, 0, sizeof(Sprite2DBuffers));
	memset(&this->lineCubeBuffers, 0, sizeof(LineCubeBuffers));
	memset(&this->lineSphereBuffers, 0, sizeof(LineSphereBuffers));
}

OpenGLRasterizer::~OpenGLRasterizer()
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	logger->Write("Performing OpenGL rasterizer clean-up...");

	OpenGLHelper::SafeDeleteProgram(&this->programs.worldMeshChunk.programId);
	OpenGLHelper::SafeDeleteProgram(&this->programs.staticMeshChunk.programId);
	OpenGLHelper::SafeDeleteProgram(&this->programs.sprite2D.programId);
	OpenGLHelper::SafeDeleteProgram(&this->programs.line.programId);
	OpenGLHelper::SafeDeleteBuffer(&this->sprite2DBuffers.sizeMultiplier);
	OpenGLHelper::SafeDeleteBuffer(&this->sprite3DBuffers.sizeMultiplier);
	OpenGLHelper::SafeDeleteBuffer(&this->lineCubeBuffers.position);
	OpenGLHelper::SafeDeleteBuffer(&this->lineSphereBuffers.position);

	logger->Write("... OpenGL rasterizer clean-up complete.");
}

bool OpenGLRasterizer::Init()
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	bool success = false;

	logger->Write("Pre initialising OpenGL rasterizer...");

	if (this->LoadViewportSize())
	{
		success = true;
	}

	logger->Write(success ?
		"... OpenGL rasterizer pre initialised successfully." :
		"... OpenGL rasterizer pre initialisation failed.");

	return success;
}

bool OpenGLRasterizer::PostCoreAssetLoadInit()
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	bool success = false;

	logger->Write("Post initialising OpenGL rasterizer...");

	if (this->LoadSystemTextureAssetIndexes())
	{
		if (this->LoadPrograms())
		{
			if (this->CreateSprite2DBuffers())
			{
				if (this->CreateSprite3DBuffers())
				{
					if (this->CreateLineCubeBuffers())
					{
						if (this->CreateLineSphereBuffers())
						{
							success = true;
						}
					}
				}
			}
		}
	}

	logger->Write(success ?
		"... OpenGL rasterizer post initialised successfully." :
		"... OpenGL rasterizer post initialisation failed.");

	return success;
}

void OpenGLRasterizer::RasterizeFrame(RasterJob* rasterJob)
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	glDepthMask(GL_TRUE);
	glClearColor(rasterJob->clearColour.r, rasterJob->clearColour.g, rasterJob->clearColour.b, rasterJob->clearColour.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (rasterJob->drawWorldMeshChunkJobItems.GetLength() > 0)
	{
		this->DrawWorldMeshChunks(rasterJob);
	}

	if (rasterJob->drawStaticMeshJobItems.GetLength() > 0)
	{
		this->DrawStaticMeshes(rasterJob);
	}

	if (rasterJob->drawVolumeRasterJobItems.GetLength() > 0)
	{
		this->DrawVolumes(rasterJob);
	}

	if (rasterJob->drawLineSphereJobItems.GetLength() > 0)
	{
		this->DrawLineSpheres(rasterJob);
	}

	if (rasterJob->drawLineCubeJobItems.GetLength() > 0)
	{
		this->DrawLineCubes(rasterJob);
	}

	if (rasterJob->drawSprite3DJobItems.GetLength() > 0)
	{
		this->DrawSprites3D(rasterJob);
	}

	if (rasterJob->drawSprite2DJobItems.GetLength() > 0)
	{
		this->DrawSprites2D(rasterJob);
	}

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		logger->Write("OpenGL error: %u", err);
	}
}

ProviderId OpenGLRasterizer::CreateVertexBuffer(int sizeBytes, const float* data)
{
	GLuint bufferId;

	// TODO - error check.
	OpenGLHelper::CreateVertexBufferWithData(&bufferId, sizeBytes, data);

	return bufferId;
}

ProviderId OpenGLRasterizer::CreateIndexBuffer(int sizeBytes, const unsigned short* data)
{
	GLuint bufferId;

	// TODO - error check.
	OpenGLHelper::CreateIndexBufferWithData(&bufferId, sizeBytes, data);

	return bufferId;
}

void OpenGLRasterizer::DeleteVertexBuffer(ProviderId* id)
{
	OpenGLHelper::SafeDeleteBuffer(id);
}

void OpenGLRasterizer::DeleteIndexBuffer(ProviderId* id)
{
	OpenGLHelper::SafeDeleteBuffer(id);
}

ProviderId OpenGLRasterizer::CreateTexture(int width, int height, RgbaUByte* imageData)
{
	GLuint textureId;

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureId;
}

void OpenGLRasterizer::DeleteTexture(ProviderId* id)
{
	OpenGLHelper::SafeDeleteTexture(id);
}

Vec2* OpenGLRasterizer::GetViewportSize()
{
	return &this->viewportSize;
}

void OpenGLRasterizer::DrawWorldMeshChunks(RasterJob* rasterJob)
{
	IEngine* engine = GetEngine();
	IAssetManager* assetManager = engine->GetAssetManager();

	// Use the world-mesh-chunk program.
	glUseProgram(this->programs.worldMeshChunk.programId);
	WorldMeshChunkProgramAttributeLocations* attributeLocations = &this->programs.worldMeshChunk.attributeLocations;
	WorldMeshChunkProgramUniformLocations* uniformLocations = &this->programs.worldMeshChunk.uniformLocations;

	// Set the general uniforms.
	glUniformMatrix4fv(uniformLocations->viewProjTransform, 1, GL_FALSE, rasterJob->commonRasterizationParameters.viewProjTransform.m);
	glUniform1i(uniformLocations->diffuseSampler, 0);
	glUniform1i(uniformLocations->lightAtlasSampler, 1);

	// Enable depth test.
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Enable writing to depth buffer.
	glDepthMask(GL_TRUE);

	// Enable back-face culling.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	// Disable alpha blending.
	glDisable(GL_BLEND);

	// Grab the world mesh asset, so that we can activate its buffers.
	IWorldMeshAsset* worldMeshAsset = assetManager->GetWorldMeshAsset(rasterJob->worldMeshAssetIndex);
	WorldMeshBuffers* worldMeshBuffers = worldMeshAsset->GetBuffers();

	// Activate the position buffer.
	glBindBuffer(GL_ARRAY_BUFFER, worldMeshBuffers->positionBufferId);
	glEnableVertexAttribArray(attributeLocations->position);
	glVertexAttribPointer(attributeLocations->position, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Activate the material tex-coord buffer.
	glBindBuffer(GL_ARRAY_BUFFER, worldMeshBuffers->materialTexCoordBufferId);
	glEnableVertexAttribArray(attributeLocations->materialTexCoord);
	glVertexAttribPointer(attributeLocations->materialTexCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// Activate the light-atlas tex-coord buffer.
	glBindBuffer(GL_ARRAY_BUFFER, worldMeshBuffers->lightAtlasTexCoordBufferId);
	glEnableVertexAttribArray(attributeLocations->lightAtlasTexCoord);
	glVertexAttribPointer(attributeLocations->lightAtlasTexCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// Active the index buffer.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, worldMeshBuffers->indexBuffer);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Draw each of the chunks.
	for (int jobItemIndex = 0; jobItemIndex < rasterJob->drawWorldMeshChunkJobItems.GetLength(); jobItemIndex++)
	{
		DrawWorldMeshChunkRasterJobItem* jobItem = &rasterJob->drawWorldMeshChunkJobItems[jobItemIndex];
		WorldMeshChunk* chunk = worldMeshAsset->GetChunk(jobItem->chunkIndex);
		AssetRef* materialAssetRef = worldMeshAsset->GetMaterialAssetRef(chunk->materialAssetRefIndex);
		//AssetRef* lightAtlasTextureAssetRef = worldMeshAsset->GetLightAtlasTextureAssetRef(chunk->lightAtlasTextureAssetRefIndex);
		
		IMaterialAsset* materialAsset = assetManager->GetMaterialAsset(materialAssetRef->index);
		ITextureAsset* diffuseTextureAsset = assetManager->GetTextureAsset(materialAsset->GetTextureAssetRef(MaterialTextureTypeDiffuse)->index);
		
		ITextureAsset* lightAtlasTextureAsset = null;
		if (chunk->lightAtlasIndex != -1)
		{
			AssetRef* lightAtlasTextureAssetRef = worldMeshAsset->GetLightAtlasTextureAssetRef(chunk->lightAtlasIndex);
			lightAtlasTextureAsset = assetManager->GetTextureAsset(lightAtlasTextureAssetRef->index);
		}
		else
		{
			lightAtlasTextureAsset = assetManager->GetTextureAsset(this->systemTextureAssetIndexes.blankLightAtlas);
		}

		/*if (jobItem->chunkIndex != 21)
		{
			continue;
		}*/

		/*// Set the light island uniforms.
		glUniform2iv(uniformLocations->lightIslandOffset, 1, (int*)&chunk->lightIslandOffset);
		glUniform2iv(uniformLocations->lightIslandSize, 1, (int*)&chunk->lightIslandSize);*/

		// Bind the diffuse texture.
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseTextureAsset->GetTextureId());

		// Bind the light atlas texture.
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, lightAtlasTextureAsset->GetTextureId());

		// Draw the world mesh chunk.
		glDrawElements(
			GL_TRIANGLES, chunk->numberOfFaces * 3, 
			GL_UNSIGNED_SHORT, (void*)(chunk->startIndex * sizeof(unsigned short)));
	}

	// Disable the vertex attributes.
	glDisableVertexAttribArray(attributeLocations->position);
	glDisableVertexAttribArray(attributeLocations->materialTexCoord);
}

void OpenGLRasterizer::DrawStaticMeshes(RasterJob* rasterJob)
{
	IEngine* engine = GetEngine();
	IAssetManager* assetManager = engine->GetAssetManager();

	// Use the static-mesh-chunk program.
	glUseProgram(this->programs.staticMeshChunk.programId);
	StaticMeshChunkProgramAttributeLocations* attributeLocations = &this->programs.staticMeshChunk.attributeLocations;
	StaticMeshChunkProgramUniformLocations* uniformLocations = &this->programs.staticMeshChunk.uniformLocations;

	// Set the general uniforms.
	glUniformMatrix4fv(uniformLocations->viewProjTransform, 1, GL_FALSE, rasterJob->commonRasterizationParameters.viewProjTransform.m);
	glUniform1i(uniformLocations->diffuseSampler, 0);

	// Enable depth test.
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Enable writing to depth buffer.
	glDepthMask(GL_TRUE);

	// Enable back-face culling.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	// Disable alpha blending.
	glDisable(GL_BLEND);

	// Loop through each static mesh.
	for (int jobItemIndex = 0; jobItemIndex < rasterJob->drawStaticMeshJobItems.GetLength(); jobItemIndex++)
	{
		DrawStaticMeshRasterJobItem* jobItem = &rasterJob->drawStaticMeshJobItems[jobItemIndex];

		// Grab the static mesh asset, so that we can activate its buffers.
		IStaticMeshAsset* staticMeshAsset = assetManager->GetStaticMeshAsset(jobItem->staticMeshAssetIndex);
		StaticMeshBuffers* staticMeshBuffers = staticMeshAsset->GetBuffers();

		// Activate the position buffer.
		glBindBuffer(GL_ARRAY_BUFFER, staticMeshBuffers->positionBufferId);
		glEnableVertexAttribArray(attributeLocations->position);
		glVertexAttribPointer(attributeLocations->position, 3, GL_FLOAT, GL_FALSE, 0, 0);

		// Activate the material tex-coord buffer.
		glBindBuffer(GL_ARRAY_BUFFER, staticMeshBuffers->materialTexCoordBufferId);
		glEnableVertexAttribArray(attributeLocations->materialTexCoord);
		glVertexAttribPointer(attributeLocations->materialTexCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
		
		// Active the index buffer.
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, staticMeshBuffers->indexBufferId);

		// Set the specific uniforms.
		glUniformMatrix4fv(uniformLocations->worldTransform, 1, GL_FALSE, jobItem->worldTransform.m);

		// Draw each of the chunks.
		for (int chunkIndex = 0; chunkIndex < staticMeshAsset->GetNumberOfChunks(); chunkIndex++)
		{
			StaticMeshChunk* chunk = staticMeshAsset->GetChunk(chunkIndex);
			AssetRef* materialAssetRef = staticMeshAsset->GetMaterialAssetRef(chunk->materialAssetRefIndex);
			IMaterialAsset* materialAsset = assetManager->GetMaterialAsset(materialAssetRef->index);
			ITextureAsset* diffuseTextureAsset = assetManager->GetTextureAsset(materialAsset->GetTextureAssetRef(MaterialTextureTypeDiffuse)->index);

			// Bind the diffuse texture.
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseTextureAsset->GetTextureId());

			// Draw the static mesh chunk.
			glDrawElements(
				GL_TRIANGLES, chunk->numberOfFaces * 3,
				GL_UNSIGNED_SHORT, (void*)(chunk->startIndex * sizeof(unsigned short)));
		}
	}

	// Disable the vertex attributes.
	glDisableVertexAttribArray(attributeLocations->position);
	glDisableVertexAttribArray(attributeLocations->materialTexCoord);
}

void OpenGLRasterizer::DrawVolumes(RasterJob* rasterJob)
{
	IEngine* engine = GetEngine();
	IVolumeManager* volumeManager = engine->GetVolumeManager();

	// Use the volumek program.
	glUseProgram(this->programs.volume.programId);
	VolumeProgramAttributeLocations* attributeLocations = &this->programs.volume.attributeLocations;
	VolumeProgramUniformLocations* uniformLocations = &this->programs.volume.uniformLocations;

	// Set the general uniforms.
	glUniformMatrix4fv(uniformLocations->viewProjTransform, 1, GL_FALSE, rasterJob->commonRasterizationParameters.viewProjTransform.m);

	// Enable depth test.
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Enable writing to depth buffer.
	glDepthMask(GL_TRUE);

	// Enable back-face culling.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	// Disable alpha blending.
	glDisable(GL_BLEND);

	// Loop through each volume.
	for (int jobItemIndex = 0; jobItemIndex < rasterJob->drawVolumeRasterJobItems.GetLength(); jobItemIndex++)
	{
		DrawVolumeRasterJobItem* jobItem = &rasterJob->drawVolumeRasterJobItems[jobItemIndex];

		// Grab the volume, so that we can activate its buffers.
		IVolume* volume = volumeManager->GetVolume(jobItem->volumeIndex);
		VolumeBuffers* volumeBuffers = volume->GetBuffers();

		// Activate the position buffer.
		glBindBuffer(GL_ARRAY_BUFFER, volumeBuffers->positionBufferId);
		glEnableVertexAttribArray(attributeLocations->position);
		glVertexAttribPointer(attributeLocations->position, 3, GL_FLOAT, GL_FALSE, 0, 0);

		// Active the index buffer.
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, volumeBuffers->indexBufferId);

		// Set the specific uniforms.
		glUniformMatrix4fv(uniformLocations->worldTransform, 1, GL_FALSE, jobItem->worldTransform.m);

		// Draw the faces.
		glDrawElements(
			GL_TRIANGLES, volume->GetNumberOfFaces() * 3,
			GL_UNSIGNED_SHORT, 0);
		//glDrawArrays(GL_LINES, 0, volume->GetNumberOfFaces());
	}

	// Disable the vertex attributes.
	glDisableVertexAttribArray(attributeLocations->position);
}

void OpenGLRasterizer::DrawSprites3D(RasterJob* rasterJob)
{
	IEngine* engine = GetEngine();
	IAssetManager* assetManager = engine->GetAssetManager();

	// Use the sprite 3D program.
	glUseProgram(this->programs.sprite3D.programId);
	Sprite3DProgramAttributeLocations* attributeLocations = &this->programs.sprite3D.attributeLocations;
	Sprite3DProgramUniformLocations* uniformLocations = &this->programs.sprite3D.uniformLocations;

	// Set the general uniforms.
	glUniform1i(uniformLocations->spriteSheetSampler, 0);
	glUniformMatrix4fv(uniformLocations->viewTransform, 1, GL_FALSE, rasterJob->commonRasterizationParameters.viewTransform.m);
	glUniformMatrix4fv(uniformLocations->projTransform, 1, GL_FALSE, rasterJob->commonRasterizationParameters.projTransform.m);

	// Activate the size multiplier buffer.
	glBindBuffer(GL_ARRAY_BUFFER, this->sprite3DBuffers.sizeMultiplier);
	glEnableVertexAttribArray(attributeLocations->sizeMultiplier);
	glVertexAttribPointer(attributeLocations->sizeMultiplier, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// Enable depth test.
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Disable writing to depth buffer.
	glDepthMask(GL_FALSE);

	// Enable back-face culling.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	// Enable alpha blending.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Loop through each job item.
	for (int jobItemIndex = 0; jobItemIndex < rasterJob->drawSprite3DJobItems.GetLength(); jobItemIndex++)
	{
		DrawSprite3DRasterJobItem* jobItem = &rasterJob->drawSprite3DJobItems[jobItemIndex];
		Sprite* sprite = jobItem->sprite;
		ITextureAsset* spriteSheetTextureAsset = assetManager->GetTextureAsset(jobItem->textureAssetIndex);
		Vec2* spriteSheetSize = spriteSheetTextureAsset->GetSize();

		// Enable / disable depth test.
		if (jobItem->enableZTest)
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}

		// Calculate the sprite's axes, accounting for its rotation.
		Mat2 rotationTransform;
		Mat2::CreateRotation(&rotationTransform, -jobItem->rotation); // Note - the rotation is negated as we are looking at the front of the sprite.

		// Set the per-sprite uniforms.
		glUniform3fv(uniformLocations->position, 1, (GLfloat*)&jobItem->position);
		glUniform2fv(uniformLocations->size, 1, (GLfloat*)&jobItem->size);
		glUniform2fv(uniformLocations->centerOfRotation, 1, (GLfloat*)&jobItem->centerOfRotation);
		glUniformMatrix2fv(uniformLocations->rotationTransform, 1, GL_FALSE, (GLfloat*)&rotationTransform.m);
		glUniform2fv(uniformLocations->spritePosition, 1, (GLfloat*)&sprite->position);
		glUniform2fv(uniformLocations->spriteSize, 1, (GLfloat*)&sprite->size);
		glUniform2fv(uniformLocations->spriteSheetSize, 1, (GLfloat*)spriteSheetSize);

		// Bind the sprite sheet texture.
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, spriteSheetTextureAsset->GetTextureId());

		// Draw the billboard.
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	// Disable the vertex attributes.
	glDisableVertexAttribArray(attributeLocations->sizeMultiplier);
}

void OpenGLRasterizer::DrawSprites2D(RasterJob* rasterJob)
{
	IEngine* engine = GetEngine();
	IAssetManager* assetManager = engine->GetAssetManager();

	// Get the viewport size.
	GLint viewportRect[4];
	glGetIntegerv(GL_VIEWPORT, viewportRect);
	Vec2 viewportSize;
	Vec2::Set(&viewportSize, (float)viewportRect[2], (float)viewportRect[3]);

	// Use the sprite program.
	glUseProgram(this->programs.sprite2D.programId);
	Sprite2DProgramAttributeLocations* attributeLocations = &this->programs.sprite2D.attributeLocations;
	Sprite2DProgramUniformLocations* uniformLocations = &this->programs.sprite2D.uniformLocations;

	// Activate the size multipliers buffer.
	glBindBuffer(GL_ARRAY_BUFFER, this->sprite2DBuffers.sizeMultiplier);
	glEnableVertexAttribArray(attributeLocations->sizeMultiplier);
	glVertexAttribPointer(attributeLocations->sizeMultiplier, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// Set the general uniforms.
	glUniform2fv(uniformLocations->viewportSize, 1, (GLfloat*)&viewportSize);
	glUniform1i(uniformLocations->spriteSheetSampler, 0);

	// Disable depth test.
	glDisable(GL_DEPTH_TEST);

	// Disable writing to depth buffer.
	glDepthMask(GL_FALSE);

	// Enable back-face culling.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	// Enable alpha blending.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Loop through each job item.
	for (int jobItemIndex = 0; jobItemIndex < rasterJob->drawSprite2DJobItems.GetLength(); jobItemIndex++)
	{
		DrawSprite2DRasterJobItem* jobItem = &rasterJob->drawSprite2DJobItems[jobItemIndex];
		Sprite* sprite = jobItem->sprite;
		ITextureAsset* spriteSheetTextureAsset = assetManager->GetTextureAsset(jobItem->textureAssetIndex);
		Vec2* spriteSheetSize = spriteSheetTextureAsset->GetSize();

		// Set the per-sprite uniforms.
		glUniform2fv(uniformLocations->position, 1, (GLfloat*)&jobItem->position);
		glUniform2fv(uniformLocations->size, 1, (GLfloat*)&jobItem->size);
		glUniform2fv(uniformLocations->spritePosition, 1, (GLfloat*)&sprite->position);
		glUniform2fv(uniformLocations->spriteSize, 1, (GLfloat*)&sprite->size);
		glUniform2fv(uniformLocations->spriteSheetSize, 1, (GLfloat*)spriteSheetSize);
		glUniform4fv(uniformLocations->tint, 1, (GLfloat*)&jobItem->tint);

		// Bind the sprite sheet texture.
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, spriteSheetTextureAsset->GetTextureId());

		// Draw the sprite.
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	// Disable the vertex attributes.
	glDisableVertexAttribArray(attributeLocations->sizeMultiplier);
}

void OpenGLRasterizer::DrawLineSpheres(RasterJob* rasterJob)
{
	RgbFloat unoccludedColor, occludedColour;
	RgbFloat::Set(&unoccludedColor, 1.0f, 1.0f, 1.0f);
	RgbFloat::Set(&occludedColour, 1.0f, 0.0f, 0.0f);

	for (int jobItemIndex = 0; jobItemIndex < rasterJob->drawLineSphereJobItems.GetLength(); jobItemIndex++)
	{
		DrawLineSphereRasterJobItem* jobItem = &rasterJob->drawLineSphereJobItems[jobItemIndex];

		this->DrawLineSphere(
			&rasterJob->commonRasterizationParameters,
			&jobItem->position,
			jobItem->radius, 
			&unoccludedColor,
			&occludedColour);
	}
}

void OpenGLRasterizer::DrawLineCubes(RasterJob* rasterJob)
{
	/*RgbFloat unoccludedColor, occludedColour;
	RgbFloat::Set(&unoccludedColor, 1.0f, 1.0f, 1.0f);
	RgbFloat::Set(&occludedColour, 1.0f, 0.0f, 0.0f);*/

	for (int jobItemIndex = 0; jobItemIndex < rasterJob->drawLineCubeJobItems.GetLength(); jobItemIndex++)
	{
		DrawLineCubeRasterJobItem* jobItem = &rasterJob->drawLineCubeJobItems[jobItemIndex];

		this->DrawLineCube(
			&rasterJob->commonRasterizationParameters,
			&jobItem->position,
			&jobItem->size,
			&jobItem->unoccludedColour,
			&jobItem->occludedColour);
	}
}

void OpenGLRasterizer::DrawLineCube(CommonRastorizationParameters* commonRasterizationParameters, Vec3* position, Vec3* size, RgbFloat* unoccludedColour, RgbFloat* occludedColour)
{
	// Use the line program.
	glUseProgram(this->programs.line.programId);
	LineProgramAttributeLocations* attributeLocations = &this->programs.line.attributeLocations;
	LineProgramUniformLocations* uniformLocations = &this->programs.line.uniformLocations;

	// Activate the positions buffer.
	glBindBuffer(GL_ARRAY_BUFFER, this->lineCubeBuffers.position);
	glEnableVertexAttribArray(attributeLocations->position);
	glVertexAttribPointer(attributeLocations->position, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Set the general uniforms.
	glUniformMatrix4fv(uniformLocations->viewProjTransform, 1, GL_FALSE, commonRasterizationParameters->viewProjTransform.m);
	glUniform3fv(uniformLocations->translation, 1, (GLfloat*)position);
	glUniform3fv(uniformLocations->scale, 1, (GLfloat*)size);

	// Disable back-face culling.
	glDisable(GL_CULL_FACE);

	// Disable writing to depth buffer.
	glDepthMask(GL_FALSE);

	// Diable alpha blending.
	glDisable(GL_BLEND);

	if (occludedColour != null)
	{
		// Use the occluded colour.
		glUniform3fv(uniformLocations->colour, 1, (GLfloat*)occludedColour);

		// Disable depth test.
		glDisable(GL_DEPTH_TEST);

		// Draw the line cube
		glDrawArrays(GL_LINES, 0, 24);
	}

	if (unoccludedColour != null)
	{
		// Use the unoccluded colour.
		glUniform3fv(uniformLocations->colour, 1, (GLfloat*)unoccludedColour);

		// Enable depth test.
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		// Draw the line cube
		glDrawArrays(GL_LINES, 0, 24);
	}

	// Disable the vertex attributes.
	glDisableVertexAttribArray(attributeLocations->position);
}

void OpenGLRasterizer::DrawLineSphere(CommonRastorizationParameters* commonRasterizationParameters, Vec3* position, float radius, RgbFloat* unoccludedColour, RgbFloat* occludedColour)
{
	Vec3 size;
	Vec3::Set(&size, radius, radius, radius);

	// Use the line program.
	glUseProgram(this->programs.line.programId);
	LineProgramAttributeLocations* attributeLocations = &this->programs.line.attributeLocations;
	LineProgramUniformLocations* uniformLocations = &this->programs.line.uniformLocations;

	// Activate the positions buffer.
	glBindBuffer(GL_ARRAY_BUFFER, this->lineSphereBuffers.position);
	glEnableVertexAttribArray(attributeLocations->position);
	glVertexAttribPointer(attributeLocations->position, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Set the general uniforms.
	glUniformMatrix4fv(uniformLocations->viewProjTransform, 1, GL_FALSE, commonRasterizationParameters->viewProjTransform.m);
	glUniform3fv(uniformLocations->translation, 1, (GLfloat*)position);
	glUniform3fv(uniformLocations->scale, 1, (GLfloat*)&size);

	// Disable back-face culling.
	glDisable(GL_CULL_FACE);

	// Diable alpha blending.
	glDisable(GL_BLEND);

	if (occludedColour != null)
	{
		// Use the occluded colour.
		glUniform3fv(uniformLocations->colour, 1, (GLfloat*)occludedColour);

		// Disable depth test.
		glDisable(GL_DEPTH_TEST);

		// Draw the line cube
		glDrawArrays(GL_LINES, 0, 800);
	}

	if (unoccludedColour != null)
	{
		// Use the unoccluded colour.
		glUniform3fv(uniformLocations->colour, 1, (GLfloat*)unoccludedColour);

		// Enable depth test.
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		// Draw the line cube
		glDrawArrays(GL_LINES, 0, 800);
	}

	// Disable the vertex attributes.
	glDisableVertexAttribArray(attributeLocations->position);
}

bool OpenGLRasterizer::LoadSystemTextureAssetIndexes()
{
	IEngine* engine = GetEngine();
	IAssetManager* assetManager = engine->GetAssetManager();

	bool success = true;

	this->systemTextureAssetIndexes.blankLightAtlas = assetManager->FindAssetIndex(AssetTypeTexture, "textures/system/blank-light-atlas.png");
	success &= this->systemTextureAssetIndexes.blankLightAtlas != -1;

	return success;
}

bool OpenGLRasterizer::LoadPrograms()
{
	bool success = false;

	if (this->LoadWorldMeshChunkProgram())
	{
		if (this->LoadStaticMeshChunkProgram())
		{
			if (this->LoadVolumeProgram())
			{
				if (this->LoadSprite3DProgram())
				{
					if (this->LoadSprite2DProgram())
					{
						if (this->LoadLineProgram())
						{
							success = true;
						}
					}
				}
			}
		}
	}

	return success;
}

bool OpenGLRasterizer::LoadWorldMeshChunkProgram()
{
	bool success = false;

	if (this->CreateProgram(&this->programs.worldMeshChunk.programId, "shaders/world-mesh-chunk-vs.glsl", "shaders/world-mesh-chunk-fs.glsl"))
	{
		success = true;

		// Get the attribute locations.
		success &= OpenGLHelper::FindAttribute(this->programs.worldMeshChunk.programId, &this->programs.worldMeshChunk.attributeLocations.position, "position");
		success &= OpenGLHelper::FindAttribute(this->programs.worldMeshChunk.programId, &this->programs.worldMeshChunk.attributeLocations.materialTexCoord, "materialTexCoord");
		success &= OpenGLHelper::FindAttribute(this->programs.worldMeshChunk.programId, &this->programs.worldMeshChunk.attributeLocations.lightAtlasTexCoord, "lightAtlasTexCoord");

		// Get the uniform locations.
		success &= OpenGLHelper::FindUniform(this->programs.worldMeshChunk.programId, &this->programs.worldMeshChunk.uniformLocations.viewProjTransform, "viewProjTransform");
		//success &= OpenGLHelper::FindUniform(this->programs.worldMeshChunk.programId, &this->programs.worldMeshChunk.uniformLocations.lightIslandOffset, "lightIslandOffset");
		//success &= OpenGLHelper::FindUniform(this->programs.worldMeshChunk.programId, &this->programs.worldMeshChunk.uniformLocations.lightIslandSize, "lightIslandSize");
		success &= OpenGLHelper::FindUniform(this->programs.worldMeshChunk.programId, &this->programs.worldMeshChunk.uniformLocations.diffuseSampler, "diffuseSampler");
		success &= OpenGLHelper::FindUniform(this->programs.worldMeshChunk.programId, &this->programs.worldMeshChunk.uniformLocations.lightAtlasSampler, "lightAtlasSampler");
	}

	return success;
}

bool OpenGLRasterizer::LoadStaticMeshChunkProgram()
{
	bool success = false;
	StaticMeshChunkProgram* program = &this->programs.staticMeshChunk;

	if (this->CreateProgram(&program->programId, "shaders/static-mesh-chunk-vs.glsl", "shaders/static-mesh-chunk-fs.glsl"))
	{
		success = true;

		// Get the attribute locations.
		success &= OpenGLHelper::FindAttribute(program->programId, &program->attributeLocations.position, "position");
		success &= OpenGLHelper::FindAttribute(program->programId, &program->attributeLocations.materialTexCoord, "materialTexCoord");

		// Get the uniform locations.
		success &= OpenGLHelper::FindUniform(program->programId, &program->uniformLocations.worldTransform, "worldTransform");
		success &= OpenGLHelper::FindUniform(program->programId, &program->uniformLocations.viewProjTransform, "viewProjTransform");
		success &= OpenGLHelper::FindUniform(program->programId, &program->uniformLocations.diffuseSampler, "diffuseSampler");
	}

	return success;
}

bool OpenGLRasterizer::LoadVolumeProgram()
{
	bool success = false;
	VolumeProgram* program = &this->programs.volume;

	if (this->CreateProgram(&program->programId, "shaders/volume-vs.glsl", "shaders/volume-fs.glsl"))
	{
		success = true;

		// Get the attribute locations.
		success &= OpenGLHelper::FindAttribute(program->programId, &program->attributeLocations.position, "position");

		// Get the uniform locations.
		success &= OpenGLHelper::FindUniform(program->programId, &program->uniformLocations.worldTransform, "worldTransform");
		success &= OpenGLHelper::FindUniform(program->programId, &program->uniformLocations.viewProjTransform, "viewProjTransform");
	}

	return success;
}

bool OpenGLRasterizer::LoadSprite3DProgram()
{
	bool success = false;
	Sprite3DProgram* program = &this->programs.sprite3D;

	if (this->CreateProgram(&program->programId, "shaders/sprite-3d-vs.glsl", "shaders/sprite-3d-fs.glsl"))
	{
		success = true;

		// Get the attribute locations.
		success &= OpenGLHelper::FindAttribute(program->programId, &program->attributeLocations.sizeMultiplier, "sizeMultiplier");

		// Get the uniform locations.
		success &= OpenGLHelper::FindUniform(program->programId, &program->uniformLocations.position, "position");
		success &= OpenGLHelper::FindUniform(program->programId, &program->uniformLocations.size, "size");
		success &= OpenGLHelper::FindUniform(program->programId, &program->uniformLocations.centerOfRotation, "centerOfRotation");
		//success &= OpenGLHelper::FindUniform(program->programId, &program->uniformLocations.xAxis, "xAxis");
		//success &= OpenGLHelper::FindUniform(program->programId, &program->uniformLocations.yAxis, "yAxis");
		success &= OpenGLHelper::FindUniform(program->programId, &program->uniformLocations.rotationTransform, "rotationTransform");
		success &= OpenGLHelper::FindUniform(program->programId, &program->uniformLocations.spritePosition, "spritePosition");
		success &= OpenGLHelper::FindUniform(program->programId, &program->uniformLocations.spriteSize, "spriteSize");
		success &= OpenGLHelper::FindUniform(program->programId, &program->uniformLocations.spriteSheetSize, "spriteSheetSize");
		success &= OpenGLHelper::FindUniform(program->programId, &program->uniformLocations.spriteSheetSampler, "spriteSheetSampler");
		success &= OpenGLHelper::FindUniform(program->programId, &program->uniformLocations.viewTransform, "viewTransform");
		success &= OpenGLHelper::FindUniform(program->programId, &program->uniformLocations.projTransform, "projTransform");
	}

	return success;
}

bool OpenGLRasterizer::LoadSprite2DProgram()
{
	bool success = false;

	if (this->CreateProgram(&this->programs.sprite2D.programId, "shaders/sprite-2d-vs.glsl", "shaders/sprite-2d-fs.glsl"))
	{
		success = true;

		// Get the attribute locations.
		success &= OpenGLHelper::FindAttribute(this->programs.sprite2D.programId, &this->programs.sprite2D.attributeLocations.sizeMultiplier, "sizeMultiplier");

		// Get the uniform locations.
		success &= OpenGLHelper::FindUniform(this->programs.sprite2D.programId, &this->programs.sprite2D.uniformLocations.position, "position");
		success &= OpenGLHelper::FindUniform(this->programs.sprite2D.programId, &this->programs.sprite2D.uniformLocations.size, "size");
		success &= OpenGLHelper::FindUniform(this->programs.sprite2D.programId, &this->programs.sprite2D.uniformLocations.viewportSize, "viewportSize");
		success &= OpenGLHelper::FindUniform(this->programs.sprite2D.programId, &this->programs.sprite2D.uniformLocations.spritePosition, "spritePosition");
		success &= OpenGLHelper::FindUniform(this->programs.sprite2D.programId, &this->programs.sprite2D.uniformLocations.spriteSize, "spriteSize");
		success &= OpenGLHelper::FindUniform(this->programs.sprite2D.programId, &this->programs.sprite2D.uniformLocations.spriteSheetSize, "spriteSheetSize");
		success &= OpenGLHelper::FindUniform(this->programs.sprite2D.programId, &this->programs.sprite2D.uniformLocations.spriteSheetSampler, "spriteSheetSampler");
		success &= OpenGLHelper::FindUniform(this->programs.sprite2D.programId, &this->programs.sprite2D.uniformLocations.tint, "tint");
	}

	return success;
}

bool OpenGLRasterizer::LoadLineProgram()
{
	bool success = false;

	if (this->CreateProgram(&this->programs.line.programId, "shaders/line-vs.glsl", "shaders/line-fs.glsl"))
	{
		success = true;

		// Get the attribute locations.
		success &= OpenGLHelper::FindAttribute(this->programs.line.programId, &this->programs.line.attributeLocations.position, "position");

		// Get the uniform locations.
		success &= OpenGLHelper::FindUniform(this->programs.line.programId, &this->programs.line.uniformLocations.viewProjTransform, "viewProjTransform");
		success &= OpenGLHelper::FindUniform(this->programs.line.programId, &this->programs.line.uniformLocations.translation, "translation");
		success &= OpenGLHelper::FindUniform(this->programs.line.programId, &this->programs.line.uniformLocations.scale, "scale");
		success &= OpenGLHelper::FindUniform(this->programs.line.programId, &this->programs.line.uniformLocations.colour, "colour");
	}

	return success;
}

bool OpenGLRasterizer::CreateProgram(GLuint* outProgram, const char* vertexShaderFilePath, const char* fragmentShaderFilePath)
{
	bool success = false;
	GLuint vertexShader = 0; 
	GLuint fragmentShader = 0;
	GLuint program = 0;

	if (this->CreateShader(&vertexShader, GL_VERTEX_SHADER, vertexShaderFilePath))
	{
		if (this->CreateShader(&fragmentShader, GL_FRAGMENT_SHADER, fragmentShaderFilePath))
		{
			if (this->LinkProgram(&program, vertexShader, fragmentShader))
			{
				*outProgram = program;
				success = true;
			}
		}
	}
	
	if (vertexShader != 0)
	{
		glDeleteShader(vertexShader);
	}

	if (fragmentShader != 0)
	{
		glDeleteShader(fragmentShader);
	}

	return success;
}

bool OpenGLRasterizer::CreateShader(GLuint* outShader, GLenum shaderType, const char* filePath)
{
	IEngine* engine = GetEngine();
	IAssetManager* assetManager = engine->GetAssetManager();
	ILogger* logger = engine->GetLogger();

	bool success = false;

	int shaderTextAssetIndex = assetManager->FindAssetIndex(AssetTypeText, filePath);
	if (shaderTextAssetIndex == -1)
	{
		logger->Write("Shader text asset not found: %s", filePath);
	}
	else
	{
		ITextAsset* shaderTextAsset = assetManager->GetTextAsset(shaderTextAssetIndex);
		const char* shaderText = shaderTextAsset->GetText();

		GLuint shader = glCreateShader(shaderType);
		glShaderSource(shader, 1, &shaderText, null);
		glCompileShader(shader);

		GLint status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == GL_TRUE)
		{
			*outShader = shader;
			success = true;
		}
		else
		{
			GLint infoLogLength;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

			GLchar *strInfoLog = new GLchar[infoLogLength + 1];
			glGetShaderInfoLog(shader, infoLogLength, null, strInfoLog);

			const char *strShaderType = null;
			switch (shaderType)
			{
			case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
			case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
			}

			logger->Write("Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
			delete[] strInfoLog;

			success = false;
		}
	}

	return success;
}

bool OpenGLRasterizer::LinkProgram(GLuint *outProgram, const GLuint vertexShader, const GLuint fragmentShader)
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	bool success = false;

	GLuint program = glCreateProgram();

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_TRUE)
	{
		*outProgram = program;
		success = true;
	}
	else
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, null, strInfoLog);
		logger->Write("Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;

		success = false;
	}

	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);

	return success;
}

bool OpenGLRasterizer::CreateSprite2DBuffers()
{
	float sizeMultipliers[] =
	{
		0, 0,
		1, 0,
		1, 1,

		0, 0,
		1, 1,
		0, 1
	};

	bool success = OpenGLHelper::CreateVertexBufferWithData(
		&this->sprite2DBuffers.sizeMultiplier, sizeof(sizeMultipliers), sizeMultipliers);

	return success;
}

bool OpenGLRasterizer::CreateSprite3DBuffers()
{
	float sizeMultipliers[] =
	{
		0, 1,
		1, 1,
		1, 0,

		0, 1,
		1, 0,
		0, 0
	};

	bool success = OpenGLHelper::CreateVertexBufferWithData(
		&this->sprite3DBuffers.sizeMultiplier, sizeof(sizeMultipliers), sizeMultipliers);

	return success;
}

bool OpenGLRasterizer::CreateLineCubeBuffers()
{
	float positions[] =
	{
		0, 0, 0, 1, 0, 0,	// Front bottom
		0, 1, 0, 1, 1, 0,	// Front top
		0, 0, 0, 0, 1, 0,	// Front left
		1, 0, 0, 1, 1, 0,	// Front right
		0, 0, 1, 1, 0, 1,	// Back bottom
		0, 1, 1, 1, 1, 1,	// Back top
		0, 0, 1, 0, 1, 1,	// Back left
		1, 0, 1, 1, 1, 1,	// Back right
		0, 0, 0, 0, 0, 1,	// Left bottom
		0, 1, 0, 0, 1, 1,	// Left top
		1, 0, 0, 1, 0, 1,	// Right bottom
		1, 1, 0, 1, 1, 1	// Right top
	};

	bool success = OpenGLHelper::CreateVertexBufferWithData(
		&this->lineCubeBuffers.position, sizeof(positions), positions);

	return success;
}

bool OpenGLRasterizer::CreateLineSphereBuffers()
{
	Vec3 positions[800];

	int positionIndex = 0;
	int numSegments = 20;
	int numSegmentPoints = 40;
	float segmentRadiansStep = (PI * 2.0f) / numSegments;
	float segmentPointsRadiansStep = (PI * 2.0f) / numSegmentPoints;
	float segmentRadians = 0;

	for (int i = 0; i < numSegments; i++) 
	{
		Vec3 segmentDirection;
		Vec3::Set(&segmentDirection, cosf(segmentRadians), 0.0f, sinf(segmentRadians));

		segmentRadians += segmentRadiansStep;

		float segmentPointsRadians = 0;

		for (int j = 0; j < numSegmentPoints; j++) 
		{
			float d = sinf(segmentPointsRadians);

			Vec3::Set(&positions[positionIndex], segmentDirection.x * d, cosf(segmentPointsRadians), segmentDirection.z * d);

			segmentPointsRadians += segmentPointsRadiansStep;
			positionIndex++;
		}
	}

	bool success = OpenGLHelper::CreateVertexBufferWithData(
		&this->lineSphereBuffers.position, sizeof(positions), (float*)positions);

	return success;
}

bool OpenGLRasterizer::LoadViewportSize()
{
	// Get the viewport size.
	GLint viewportRect[4];
	glGetIntegerv(GL_VIEWPORT, viewportRect);
	Vec2::Set(&this->viewportSize, (float)viewportRect[2], (float)viewportRect[3]);

	return true;
}