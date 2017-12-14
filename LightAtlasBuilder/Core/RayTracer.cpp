#include "Core/RayTracer.h"

RayTracer::~RayTracer()
{
}

/*RgbFloat RayTracer::CalculateColourForChunkAtPosition(Vec3* worldPosition, Vec3* normal, int chunkIndex)
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
		Light* light = engine->GetLight(lightIndex);
		RgbFloat lightColour = this->CalculateLightColourForChunkAtPosition(light, worldPosition, normal, collisionMesh, chunkIndex);
		RgbFloat::Add(&accumulatedColour, &accumulatedColour, &lightColour);
	}

	return accumulatedColour;
}*/

/*RgbFloat RayTracer::CalculateLightColourForChunkAtPosition(Light* light, Vec3* worldPosition, Vec3* normal, ICollisionMesh* collisionMesh, int chunkIndex)
{
	float directLightIntensity = this->CalculateDirectLightIntensityForChunkAtPosition(light, worldPosition, normal, 0, collisionMesh, chunkIndex, 40);
	float indirectLightIntensity = this->CalculateIndirectLightIntensityForChunkAtPosition(light, worldPosition, normal, collisionMesh, chunkIndex, 22, 0);
	float totalLightIntensity = directLightIntensity + indirectLightIntensity;

	RgbFloat lightColour;
	RgbFloat::Scale(&lightColour, &light->colour, totalLightIntensity);

	return lightColour;
}*/

void RayTracer::CalculateDirectIlluminationIntensityForLumel(float* outAverageIntensity, float* outAverageDistanceToLightSqr, Light* light, Vec3* lumelWorldPosition, Vec3* lumelNormal/*, float baseDistanceToLightSqr*/, ICollisionMesh* collisionMesh, int chunkIndex)
{
	int numberOfSamplesPerBlock = 4;
	int numberOfSamples = 0;

	float accumulatedIntensity = 0.0f;
	float accumulatedDistanceToLightSqr = 0.0f;

	CollisionLine line;
	line.from = *lumelWorldPosition;

	for (int lightBlockIndex = 0; lightBlockIndex < light->numberOfBlocks; lightBlockIndex++)
	{
		LightBlock* lightBlock = &light->blocks[lightBlockIndex];

		for (int i = 0; i < numberOfSamplesPerBlock; i++)
		{
			int lightNodeIndex = Math::GenerateRandomInt(0, lightBlock->numberOfNodes - 1);
			LightNode* lightNode = &lightBlock->nodes[lightNodeIndex];

			Vec3 lumelToLight;
			Vec3::Sub(&lumelToLight, &lightNode->worldPosition, lumelWorldPosition);

			Vec3 lumelToLightNormal;
			Vec3::Normalize(&lumelToLightNormal, &lumelToLight);

			float texelToLightDistanceSqr = Vec3::LengthSqr(&lumelToLight);
			accumulatedDistanceToLightSqr += texelToLightDistanceSqr;

			float coneAngle = Math::Clamp(Vec3::Dot(&lumelToLightNormal, &lightNode->invDirection), 0.0f, 1.0f); // Is clamp needed?

			if (coneAngle >= light->minConeAngle)
			{
				float attentuation = Math::Clamp(1.0f - (texelToLightDistanceSqr / light->distanceSqr), 0.0f, 1.0f);

				float lambert = Math::Clamp(Vec3::Dot(lumelNormal, &lumelToLightNormal), 0.0f, 1.0f);

				float coneFactor = Math::InverseLerp(coneAngle, light->minConeAngle, 1.0f);

				float intensity = attentuation * lambert * coneFactor;

				if (intensity > 0.0f)
				{
					line.to = lightNode->worldPosition;
					CollisionLine::FromOwnFromAndToPoints(&line);

					if (!collisionMesh->DetermineIfLineIntersectsMesh(&line, light->ownerChunkIndex, chunkIndex))
					{
						accumulatedIntensity += intensity;
					}
				}
			}

			numberOfSamples++;
		}
	}

	*outAverageIntensity = accumulatedIntensity / numberOfSamples;
	*outAverageDistanceToLightSqr = accumulatedDistanceToLightSqr / numberOfSamples;
}

float RayTracer::CalculateIndirectIlluminationIntensityForChunkAtPosition(Light* light, Vec3* worldPosition, Vec3* normal, ICollisionMesh* collisionMesh, int chunkIndex, int numberOfSamples, int recursionDepth)
{
	/*//int numberOfSamples = 16;

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

	return averageIntensity;*/
	return 0.0f;
}