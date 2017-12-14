#pragma once
#include "Include/LightAtlasBuilder.h"

class RayTracer : public IRayTracer
{
public:
	virtual ~RayTracer();
	//virtual RgbFloat CalculateColourForChunkAtPosition(Vec3* worldPosition, Vec3* normal, int chunkIndex);
	void CalculateDirectIlluminationIntensityForLumel(float* outAverageIntensity, float* outAverageDistanceToLightSqr, Light* light, Vec3* lumelWorldPosition, Vec3* lumelNormal/*, float baseDistanceToLightSqr*/, ICollisionMesh* collisionMesh, int chunkIndex);

private:
	//RgbFloat CalculateLightColourForChunkAtPosition(Light* light, Vec3* worldPosition, Vec3* normal, ICollisionMesh* collisionMesh, int chunkIndex);
	//float CalculateDirectLightIntensityForChunkAtPosition(Light* light, Vec3* worldPosition, Vec3* normal, float baseDistanceToLightSqr, ICollisionMesh* collisionMesh, int chunkIndex, int numberOfSamples);
	float CalculateIndirectIlluminationIntensityForChunkAtPosition(Light* light, Vec3* worldPosition, Vec3* normal, ICollisionMesh* collisionMesh, int chunkIndex, int numberOfSamples, int recursionDepth);
};