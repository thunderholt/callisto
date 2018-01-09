#include "Core/RayTracer.h"

RayTracer::~RayTracer()
{
}

float RayTracer::CalculateDirectIlluminationIntensityForLumel(ExpandedLight* light, Vec3* lumelWorldPosition, Vec3* lumelNormal, ICollisionMesh* collisionMesh, int chunkIndex)
{
	// Calculate the max intensity.
	float maxIntensity = 0.0f;

	for (int lightPointIndex = 0; lightPointIndex < light->numberOfIntensityPoints; lightPointIndex++)
	{
		Vec3* lightPoint = &light->intensityPoints[lightPointIndex];

		Vec3 lumelToLight;
		Vec3::Sub(&lumelToLight, lightPoint, lumelWorldPosition);

		Vec3 lumelToLightNormal;
		Vec3::Normalize(&lumelToLightNormal, &lumelToLight);

		float lumelToLightDistanceSqr = Vec3::LengthSqr(&lumelToLight);

		float attentuation = Math::Clamp(1.0f - (lumelToLightDistanceSqr / light->distanceSqr), 0.0f, 1.0f);

		float lambert = Math::Clamp(Vec3::Dot(lumelNormal, &lumelToLightNormal), 0.0f, 1.0f);

		float coneFactor = 1.0f;

		if (light->type == LightTypeArea)
		{
			float coneAngle = Math::Clamp(Vec3::Dot(&lumelToLightNormal, &light->invDirection), 0.0f, 1.0f); // Is clamp needed?

			coneFactor = Math::InverseLerp(coneAngle, light->minConeAngle, 1.0f);
		}

		float intensity = attentuation * lambert * coneFactor;

		if (intensity > maxIntensity)
		{
			maxIntensity = intensity;
		}
	}

	// Calculate the shadow factor.
	float shadowFactor = 0.0f;

	if (maxIntensity > 0.0f)
	{
		int accumulatedShadowSamples = 0;

		for (int shadowPointIndex = 0; shadowPointIndex < light->numberOfShadowPoints; shadowPointIndex++)
		{
			Vec3* shadowPoint = &light->shadowPoints[shadowPointIndex];

			Vec3 lumelToLight;
			Vec3::Sub(&lumelToLight, shadowPoint, lumelWorldPosition);

			Vec3 lumelToLightNormal;
			Vec3::Normalize(&lumelToLightNormal, &lumelToLight);

			CollisionLine line;
			line.from = *lumelWorldPosition;
			line.to = *shadowPoint;
			CollisionLine::FromOwnFromAndToPoints(&line);

			if (!collisionMesh->DetermineIfLineIntersectsMesh(&line, chunkIndex, -1))
			{
				accumulatedShadowSamples++;
			}
		}

		shadowFactor = accumulatedShadowSamples / (float)light->numberOfShadowPoints;
	}

	float finalIntensity = maxIntensity * shadowFactor;

	return finalIntensity;
}

/*
float RayTracer::CalculateDirectIlluminationIntensityForLumel(Light* light, Vec3* lumelWorldPosition, Vec3* lumelNormal, ICollisionMesh* collisionMesh, int chunkIndex)
{
	float maxIntensity = 0.0f;

	CollisionMeshChunk* chunk = collisionMesh->GetChunk(light->ownerChunkIndex);

	for (int faceIndex = chunk->startFaceIndex; faceIndex < chunk->startFaceIndex + chunk->numberOfFaces; faceIndex++)
	{
		CollisionFace* face = collisionMesh->GetFace(faceIndex);

		Vec3 invFaceNormal;
		Vec3::Scale(&invFaceNormal, &face->facePlane.normal, -1.0f);

		Ray3 ray;
		ray.origin = *lumelWorldPosition;
		ray.normal = invFaceNormal;

		Vec3 facePlaneIntersection;
		if (Ray3::CalculateIntersectionWithPlane(&facePlaneIntersection, &ray, &face->facePlane))
		{
			Vec3 nearestPointOnFace;

			if (CollisionFace::DetermineIfPointOnFacePlaneIsWithinCollisionFace(face, &facePlaneIntersection))
			{
				nearestPointOnFace = facePlaneIntersection;
			}
			else
			{
				CollisionFace::FindNearestPointOnCollisionFacePerimeterToPoint(&nearestPointOnFace, face, &facePlaneIntersection);
			}

			Vec3 lumelToLight;
			Vec3::Sub(&lumelToLight, &nearestPointOnFace, lumelWorldPosition);

			Vec3 lumelToLightNormal;
			Vec3::Normalize(&lumelToLightNormal, &lumelToLight);

			float lumelToLightDistanceSqr = Vec3::LengthSqr(&lumelToLight);

			float attentuation = Math::Clamp(1.0f - (lumelToLightDistanceSqr / light->distanceSqr), 0.0f, 1.0f);

			float lambert = Math::Clamp(Vec3::Dot(lumelNormal, &lumelToLightNormal), 0.0f, 1.0f);

			float coneAngle = Math::Clamp(Vec3::Dot(&lumelToLightNormal, &invFaceNormal), 0.0f, 1.0f); // Is clamp needed?

			float coneFactor = Math::InverseLerp(coneAngle, light->minConeAngle, 1.0f);

			float intensity = attentuation * lambert * coneFactor;

			if (intensity > maxIntensity)
			{
				maxIntensity = intensity;
			}
		}
	}

	////////////// Calculate shadow factor /////////////

	float shadowFactor = 0.0f;

	if (maxIntensity > 0.0f)
	{
		int numberOfSamplesPerBlock = 4;
		int numberOfShadowSamples = 0;
		int accumulatedShadowSamples = 0;
		//int numberOfShadowSamples = 0;

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

				//float coneAngle = Math::Clamp(Vec3::Dot(&lumelToLightNormal, &lightNode->invDirection), 0.0f, 1.0f); // Is clamp needed?

				//if (coneAngle >= light->minConeAngle)
				//{
					CollisionLine line;
					line.from = *lumelWorldPosition;
					line.to = lightNode->worldPosition;
					CollisionLine::FromOwnFromAndToPoints(&line);

					if (!collisionMesh->DetermineIfLineIntersectsMesh(&line, light->ownerChunkIndex, chunkIndex))
					{
						accumulatedShadowSamples++;
					}
				//}

				numberOfShadowSamples++;
			}
		}

		shadowFactor = accumulatedShadowSamples / (float)numberOfShadowSamples;
	}

	maxIntensity *= shadowFactor;

	return maxIntensity;
}*/

/*float RayTracer::CalculateDirectIlluminationIntensityForLumel(Light* light, Vec3* lumelWorldPosition, Vec3* lumelNormal, ICollisionMesh* collisionMesh, int chunkIndex)
{
	int numberOfSamplesPerBlock = 4;
	int numberOfSamples = 0;

	//float accumulatedIntensity = 0.0f;
	//float accumulatedDistanceToLightSqr = 0.0f;

	


	float maxIntensity = 0.0f;
	int accumulatedShadowSamples = 0;
	//int numberOfShadowSamples = 0;

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
			//accumulatedDistanceToLightSqr += texelToLightDistanceSqr;

			float coneAngle = Math::Clamp(Vec3::Dot(&lumelToLightNormal, &lightNode->invDirection), 0.0f, 1.0f); // Is clamp needed?

			if (coneAngle >= light->minConeAngle)
			{
				float attentuation = Math::Clamp(1.0f - (texelToLightDistanceSqr / light->distanceSqr), 0.0f, 1.0f);

				float lambert = Math::Clamp(Vec3::Dot(lumelNormal, &lumelToLightNormal), 0.0f, 1.0f);

				float coneFactor = Math::InverseLerp(coneAngle, light->minConeAngle, 1.0f);

				float intensity = attentuation * lambert;// *coneFactor;

				if (intensity > 0.0f)
				{
					if (intensity > maxIntensity)
					{
						maxIntensity = intensity;
					}

					CollisionLine line;
					line.from = *lumelWorldPosition;
					line.to = lightNode->worldPosition;
					CollisionLine::FromOwnFromAndToPoints(&line);

					if (!collisionMesh->DetermineIfLineIntersectsMesh(&line, light->ownerChunkIndex, chunkIndex))
					{
						accumulatedShadowSamples++;
					}

					//numberOfShadowSamples++;
				}
			}

			numberOfSamples++;
		}
	}

	//float averageIntensity = accumulatedIntensity / numberOfSamples;

	//return averageIntensity;

	float shadowFactor = accumulatedShadowSamples / (float)numberOfSamples;

	float intensity = maxIntensity;// *shadowFactor;

	return intensity;
}*/

/*
void RayTracer::CalculateDirectIlluminationIntensityForLumel(float* outAverageIntensity, float* outAverageDistanceToLightSqr, Light* light, Vec3* lumelWorldPosition, Vec3* lumelNormal, ICollisionMesh* collisionMesh, int chunkIndex)
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
}*/
/*
float RayTracer::CalculateIndirectIlluminationIntensityForChunkAtPosition(Light* light, Vec3* worldPosition, Vec3* normal, ICollisionMesh* collisionMesh, int chunkIndex, int numberOfSamples, int recursionDepth)
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
	return 0.0f;
}*/