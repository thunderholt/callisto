#include "Core/RayTracer.h"

RayTracer::~RayTracer()
{
}

RgbFloat RayTracer::CalculateColourForChunkAtPosition(Vec3* worldPosition, Vec3* normal, int chunkIndex)
{
	IEngine* engine = GetEngine();
	IWorldMeshAsset* worldMesh = engine->GetWorldMeshAsset();
	ICollisionMesh* collisionMesh = worldMesh->GetCollisionMesh();

	RgbFloat accumulatedColour;
	RgbFloat::Zero(&accumulatedColour);

	CollisionMeshChunk* chunk = collisionMesh->GetChunk(chunkIndex);

	for (int i = 0; i < chunk->effectiveLightIndexes.GetLength(); i++)
	{
		int lightIndex = chunk->effectiveLightIndexes[i];
	//for (int lightIndex = 0; lightIndex < engine->GetNumberOfLights(); lightIndex++)
	//{
		Light* light = engine->GetLight(lightIndex);
		RgbFloat lightColour = this->CalculateLightColourForChunkAtPosition(light, worldPosition, normal, collisionMesh, chunkIndex);
		RgbFloat::Add(&accumulatedColour, &accumulatedColour, &lightColour);
	}

	return accumulatedColour;
}

RgbFloat RayTracer::CalculateLightColourForChunkAtPosition(Light* light, Vec3* worldPosition, Vec3* normal, ICollisionMesh* collisionMesh, int chunkIndex)
{
	/*RgbFloat accumulatedColour;
	RgbFloat::Zero(&accumulatedColour);

	int numberOfSamples = 16;

	CollisionLine line;
	line.from = *worldPosition;

	for (int i = 0; i < numberOfSamples; i++)
	{
		int lightNodeIndex = Math::GenerateRandomInt(0, light->nodes.GetLength() - 1);
		LightNode* lightNode = &light->nodes[lightNodeIndex];

		Vec3 texelToLight;
		Vec3::Sub(&texelToLight, &lightNode->worldPosition, worldPosition);

		Vec3 texelToLightNormal;
		Vec3::Normalize(&texelToLightNormal, &texelToLight);

		float coneFactor = Math::Clamp(Vec3::Dot(&texelToLightNormal, &lightNode->invDirection), 0.0f, 1.0f);

		float minConeFactor = 0.8f;
		if (coneFactor >= minConeFactor)
		{
			line.to = lightNode->worldPosition;
			CollisionLine::FromOwnFromAndToPoints(&line);

			if (!collisionMesh->DetermineIfLineIntersectsMesh(&line, chunkIndex))
			{
				float texelToLightDistanceSqr = Vec3::LengthSqr(&texelToLight);

				float attentuation = Math::Clamp(1.0f - (texelToLightDistanceSqr / lightNode->distanceSqr), 0.0f, 1.0f);
		
				float lambert = Math::Clamp(Vec3::Dot(normal, &lightNode->invDirection), 0.0f, 1.0f);

				coneFactor = Math::InverseLerp(coneFactor, minConeFactor, 1.0f);

				//lambert = Math::Clamp(lambert, minLambert, 1.0f);

				float intensity = attentuation * lambert * coneFactor;

				RgbFloat sampleColour;
				RgbFloat::Scale(&sampleColour, &lightNode->colour, intensity);

				RgbFloat::Add(&accumulatedColour, &accumulatedColour, &sampleColour);
			}
		}
	}

	RgbFloat averageColour;
	RgbFloat::Scale(&averageColour, &accumulatedColour, 1.0f / (float)numberOfSamples);

	return averageColour;*/

	float directLightIntensity = this->CalculateDirectLightIntensityForChunkAtPosition(light, worldPosition, normal, 0, collisionMesh, chunkIndex, 40);
	float indirectLightIntensity = this->CalculateIndirectLightIntensityForChunkAtPosition(light, worldPosition, normal, collisionMesh, chunkIndex, 22, 0);
	float totalLightIntensity = directLightIntensity + indirectLightIntensity;

	RgbFloat lightColour;
	RgbFloat::Scale(&lightColour, &light->colour, totalLightIntensity);

	return lightColour;
}

float RayTracer::CalculateDirectLightIntensityForChunkAtPosition(Light* light, Vec3* worldPosition, Vec3* normal, float baseDistanceToLightSqr, ICollisionMesh* collisionMesh, int chunkIndex, int numberOfSamples)
{
	//int numberOfSamples = 16;

	float accumulatedIntensity = 0.0f;

	CollisionLine line;
	line.from = *worldPosition;

	for (int i = 0; i < numberOfSamples; i++)
	{
		int lightNodeIndex = Math::GenerateRandomInt(0, light->nodes.GetLength() - 1);
		LightNode* lightNode = &light->nodes[lightNodeIndex];

		Vec3 texelToLight;
		Vec3::Sub(&texelToLight, &lightNode->worldPosition, worldPosition);

		Vec3 texelToLightNormal;
		Vec3::Normalize(&texelToLightNormal, &texelToLight);

		float coneFactor = Math::Clamp(Vec3::Dot(&texelToLightNormal, &lightNode->invDirection), 0.0f, 1.0f);

		float minConeFactor = 0.8f;
		if (coneFactor >= minConeFactor)
		{
			line.to = lightNode->worldPosition;
			CollisionLine::FromOwnFromAndToPoints(&line);

			if (!collisionMesh->DetermineIfLineIntersectsMesh(&line, chunkIndex))
			{
				float texelToLightDistanceSqr = baseDistanceToLightSqr + Vec3::LengthSqr(&texelToLight);

				float attentuation = Math::Clamp(1.0f - (texelToLightDistanceSqr / lightNode->distanceSqr), 0.0f, 1.0f);

				float lambert = Math::Clamp(Vec3::Dot(normal, &lightNode->invDirection), 0.0f, 1.0f);

				coneFactor = Math::InverseLerp(coneFactor, minConeFactor, 1.0f);

				//lambert = Math::Clamp(lambert, minLambert, 1.0f);

				float intensity = attentuation * lambert * coneFactor;

				accumulatedIntensity += intensity;
			}
		}
	}

	float averageIntensity = accumulatedIntensity / numberOfSamples;

	return averageIntensity;
}

float RayTracer::CalculateIndirectLightIntensityForChunkAtPosition(Light* light, Vec3* worldPosition, Vec3* normal, ICollisionMesh* collisionMesh, int chunkIndex, int numberOfSamples, int recursionDepth)
{
	//int numberOfSamples = 16;

	float accumulatedIntensity = 0.0f;
	float bounceIntensityMultiplier = 0.8f;

	CollisionLine line;
	line.from = *worldPosition;

	for (int i = 0; i < numberOfSamples; i++)
	{
		Vec3 purturbedNormal;
		float puturbationAmount = 0.95f;
		purturbedNormal.x = normal->x + ((Math::GenerateRandomFloat() * puturbationAmount * 2.0f) - puturbationAmount);
		purturbedNormal.y = normal->y + ((Math::GenerateRandomFloat() * puturbationAmount * 2.0f) - puturbationAmount);
		purturbedNormal.z = normal->z + ((Math::GenerateRandomFloat() * puturbationAmount * 2.0f) - puturbationAmount);

		Vec3::ScaleAndAdd(&line.to, &line.from, &purturbedNormal, 10000.0f); // FIXME - use a properly computed scale.
		CollisionLine::FromOwnFromAndToPoints(&line);

		Vec3 bounceWorldPosition;
		CollisionChunkFaceIndex bounceChunkFaceIndex;
		if (collisionMesh->FindNearestLineIntersectWithMesh(&bounceWorldPosition, &bounceChunkFaceIndex, &line, chunkIndex))
		{
			//CollisionMeshChunk* bounceChunk = collisionMesh->GetChunk(outBounceChunkFaceIndex.chunkIndex);
			float bounceDistanceSqr = Vec3::DistanceSqr(worldPosition, &bounceWorldPosition);
			CollisionFace* bounceFace = collisionMesh->GetFace(bounceChunkFaceIndex.faceIndex);

			float intensity =
				this->CalculateDirectLightIntensityForChunkAtPosition(light, &bounceWorldPosition, &bounceFace->facePlane.normal, bounceDistanceSqr, collisionMesh, bounceChunkFaceIndex.chunkIndex, 4);

			if (recursionDepth < 2)
			{
				intensity +=
					this->CalculateIndirectLightIntensityForChunkAtPosition(light, &bounceWorldPosition, &bounceFace->facePlane.normal, collisionMesh, bounceChunkFaceIndex.chunkIndex, 6, recursionDepth + 1);
			}

			accumulatedIntensity += intensity * bounceIntensityMultiplier;
		}
	}

	float averageIntensity = accumulatedIntensity / numberOfSamples;

	return averageIntensity;
}