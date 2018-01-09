#include "Core/Worker.h"

void WorkerRunSectorCruncherEntryPoint(void* arg)
{
	Worker* worker = (Worker*)arg;
	worker->RunThreadEntryPoint();
}

Worker::Worker()
{
	this->currentTaskType = WorkerTaskTypeNone;
	this->currentTaskHasFinished = false;
	this->lightIslandIndexes = null;
	this->numberOfLightIslandIndexes = 0;
	this->currentLight = null;
	this->effectedChunksFieldForCurrentLightAtlas = null;
	this->effectedChunksFieldForCurrentLight = null;
	this->currentBounceNumber = 0;

	/*this->failureType1Count = 0;
	this->failureType2Count = 0;
	this->failureType3Count = 0;
	this->failureType4Count = 0;
	this->failureType5Count = 0;*/
}

Worker::~Worker()
{

}

void Worker::SetCurrentLightAtlasDetails(int* lightIslandIndexes, int numberofLightIslandIndexes, BitField* effectedChunksFieldForCurrentLightAtlas)
{
	this->lightIslandIndexes = lightIslandIndexes;
	this->numberOfLightIslandIndexes = numberofLightIslandIndexes;
	this->effectedChunksFieldForCurrentLightAtlas = effectedChunksFieldForCurrentLightAtlas;
}

void Worker::SetCurrentLight(ExpandedLight* light, BitField* effectedChunksFieldForCurrentLight)
{
	this->currentLight = light;
	this->effectedChunksFieldForCurrentLight = effectedChunksFieldForCurrentLight;
}

void Worker::ComputeLumelBasicDataAsync()
{
	IEngine* engine = GetEngine();
	IThreadManager* threadManager = engine->GetThreadManager();

	this->currentTaskType = WorkerTaskTypeComputeLumelBasicData;
	this->currentTaskHasFinished = false;

	threadManager->StartThread(WorkerRunSectorCruncherEntryPoint, this);
}

void Worker::ComputeLumelBounceTargetsAsync()
{
	IEngine* engine = GetEngine();
	IThreadManager* threadManager = engine->GetThreadManager();

	this->currentTaskType = WorkerTaskTypeComputeLumelBounceTargets;
	this->currentTaskHasFinished = false;

	threadManager->StartThread(WorkerRunSectorCruncherEntryPoint, this);
}

void Worker::ComputeDirectIlluminationIntensitiesForCurrentLightAsync()
{
	IEngine* engine = GetEngine();
	IThreadManager* threadManager = engine->GetThreadManager();

	this->currentTaskType = WorkerTaskTypeComputeDirectIlluminationIntensitiesForCurrentLight;
	this->currentTaskHasFinished = false;

	threadManager->StartThread(WorkerRunSectorCruncherEntryPoint, this);
}

void Worker::ComputeIndirectIlluminationBounceIntensitiesForCurrentLightAsync(int bounceNumber)
{
	IEngine* engine = GetEngine();
	IThreadManager* threadManager = engine->GetThreadManager();

	this->currentTaskType = WorkerTaskTypeComputeIndirectIlluminationBounceIntensitiesForCurrentLight;
	this->currentBounceNumber = bounceNumber;
	this->currentTaskHasFinished = false;

	threadManager->StartThread(WorkerRunSectorCruncherEntryPoint, this);
}

void Worker::FillBordersAsync()
{
	IEngine* engine = GetEngine();
	IThreadManager* threadManager = engine->GetThreadManager();

	this->currentTaskType = WorkerTaskTypeFillBorders;
	this->currentTaskHasFinished = false;

	threadManager->StartThread(WorkerRunSectorCruncherEntryPoint, this);
}

bool Worker::GetHasFinished()
{
	return this->currentTaskHasFinished;
}

void Worker::RunThreadEntryPoint()
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	if (this->currentTaskType == WorkerTaskTypeComputeLumelBasicData)
	{
		this->ComputeLumelBasicDataInternal();
	}
	else if (this->currentTaskType == WorkerTaskTypeComputeLumelBounceTargets)
	{
		this->ComputeLumelBounceTargetsInternal();
	}
	else if (this->currentTaskType == WorkerTaskTypeComputeDirectIlluminationIntensitiesForCurrentLight)
	{
		this->ComputeDirectIlluminationIntensitiesForCurrentLightInternal();
	}
	else if (this->currentTaskType == WorkerTaskTypeComputeIndirectIlluminationBounceIntensitiesForCurrentLight)
	{
		this->ComputeIndirectIlluminationBounceIntensitiesForCurrentLightInternal();
	}
	else if (this->currentTaskType == WorkerTaskTypeFillBorders)
	{
		this->FillBordersInternal();
	}

	this->currentTaskHasFinished = true;
}

void Worker::ComputeLumelBasicDataInternal()
{
	IEngine* engine = GetEngine();
	IWorldMeshAsset* worldMeshAsset = engine->GetWorldMeshAsset();
	ICollisionMesh* collisionMesh = worldMeshAsset->GetCollisionMesh();
	IRayTracer* rayTracer = engine->GetRayTracer();
	ILogger* logger = engine->GetLogger();

	ILightAtlas* lightAtlas = engine->GetCurrentLightAtlas();
	Vec2i lightAtlasSize = lightAtlas->GetSize();
	Lumel* lumels = lightAtlas->GetLumels();

	Vec2 pixelCentre = this->CalculatePixelCentre();

	for (int i = 0; i < this->numberOfLightIslandIndexes; i++)
	{
		int lightIslandIndex = this->lightIslandIndexes[i];

		WorldMeshLightIsland* lightIsland = worldMeshAsset->GetLightIsland(lightIslandIndex);
		CollisionMeshChunk* chunk = collisionMesh->GetChunk(lightIsland->chunkIndex);

		for (int y = lightIsland->offset.y; y < lightIsland->offset.y + lightIsland->size.y; y++)
		{
			for (int x = lightIsland->offset.x; x < lightIsland->offset.x + lightIsland->size.x; x++)
			{
				int lumelIndex = y * lightAtlasSize.x + x;

				Vec2 uv;
				Vec2::Set(&uv, (float)x, (float)y);
				uv.x /= (float)lightAtlasSize.x;
				uv.y /= (float)lightAtlasSize.y;
				Vec2::Add(&uv, &uv, &pixelCentre);

				Lumel* lumel = &lumels[lumelIndex];
				lumel->chunkIndex = lightIsland->chunkIndex;

				bool faceFound = this->FindWorldPositionForLightIslandTexel(&lumel->worldPosition, &lumel->normal, &lumel->faceIndex, collisionMesh, chunk, uv);
				if (faceFound)
				{
					lumel->isParticipant = true;
					lumel->state = LumelStateSet;
					RgbFloat::Set(&lumel->colour, 0.0f, 0.0f, 0.0f);
				}
			}
		}
	}
}

void Worker::ComputeLumelBounceTargetsInternal()
{
	IEngine* engine = GetEngine();
	IWorldMeshAsset* worldMeshAsset = engine->GetWorldMeshAsset();
	ICollisionMesh* collisionMesh = worldMeshAsset->GetCollisionMesh();
	IRayTracer* rayTracer = engine->GetRayTracer();
	ILogger* logger = engine->GetLogger();
	const Config* config = engine->GetConfig();

	ILightAtlas* lightAtlas = engine->GetCurrentLightAtlas();
	Vec2i lightAtlasSize = lightAtlas->GetSize();
	Lumel* lumels = lightAtlas->GetLumels();

	Vec2 pixelCentre = this->CalculatePixelCentre();

	NormalWithinHemisphereCalculationMetrics normalWithinHemisphereCalculationMetrics;
	Math::BuildNormalWithinHemisphereCalculationMetrics(&normalWithinHemisphereCalculationMetrics, config->indirectIlluminationHemispehereCircleCount, PI * 0.45f, config->numberOfIndirectIlluminationSamplesPerBounce);

	for (int i = 0; i < this->numberOfLightIslandIndexes; i++)
	{
		int lightIslandIndex = this->lightIslandIndexes[i];
		
		WorldMeshLightIsland* lightIsland = worldMeshAsset->GetLightIsland(lightIslandIndex);

		for (int y = lightIsland->offset.y; y < lightIsland->offset.y + lightIsland->size.y; y++)
		{
			for (int x = lightIsland->offset.x; x < lightIsland->offset.x + lightIsland->size.x; x++)
			{
				CollisionMeshChunk* chunk = collisionMesh->GetChunk(lightIsland->chunkIndex);

				int lumelIndex = y * lightAtlasSize.x + x;

				Lumel* lumel = &lumels[lumelIndex];

				if (lumel->isParticipant)
				{
					for (int circleIndex = 0; circleIndex < normalWithinHemisphereCalculationMetrics.numberOfCircles; circleIndex++)
					{
						int numberOfSegmentsForCircle = normalWithinHemisphereCalculationMetrics.segmentCountsByCircleIndex[circleIndex];

						for (int segmentIndex = 0; segmentIndex < numberOfSegmentsForCircle; segmentIndex++)
						{
							CollisionLine line;
							line.from = lumel->worldPosition;

							Vec3 purturbedLumelNormal;

							CollisionFace* face = collisionMesh->GetFace(lumel->faceIndex);

							Math::CalculateNormalWithinHemisphere(
								&purturbedLumelNormal, &lumel->normal, &face->freeNormalisedEdges[0],
								&normalWithinHemisphereCalculationMetrics, circleIndex, segmentIndex);

							Vec3::ScaleAndAdd(&line.to, &line.from, &purturbedLumelNormal, 10000.0f); // FIXME - use a properly computed scale.
							CollisionLine::FromOwnFromAndToPoints(&line);

							int bounceTargetLumelIndex = -1;
							bool skyIsVisible = false;

							this->FindBounceTarget(&bounceTargetLumelIndex, &skyIsVisible, lumels, collisionMesh, &line, lightIsland, &lightAtlasSize);

							if (bounceTargetLumelIndex != -1)
							{
								lumel->bounceTargets[lumel->numberOfBounceTargets].lumelIndex = bounceTargetLumelIndex;
								lumel->numberOfBounceTargets++;
							}
						}
					}
				}
			}
		}
	}
}

void Worker::ComputeDirectIlluminationIntensitiesForCurrentLightInternal()
{
	IEngine* engine = GetEngine();
	IWorldMeshAsset* worldMeshAsset = engine->GetWorldMeshAsset();
	ICollisionMesh* collisionMesh = worldMeshAsset->GetCollisionMesh();
	IRayTracer* rayTracer = engine->GetRayTracer();
	ILogger* logger = engine->GetLogger();

	ILightAtlas* lightAtlas = engine->GetCurrentLightAtlas();
	Vec2i lightAtlasSize = lightAtlas->GetSize();
	Lumel* lumels = lightAtlas->GetLumels();

	Vec2 pixelCentre = this->CalculatePixelCentre();

	for (int i = 0; i < this->numberOfLightIslandIndexes; i++)
	{
		int lightIslandIndex = this->lightIslandIndexes[i];

		WorldMeshLightIsland* lightIsland = worldMeshAsset->GetLightIsland(lightIslandIndex);

		// Check if the light island chunks are effected by the current light.
		if (this->effectedChunksFieldForCurrentLight->GetBit(lightIsland->chunkIndex))
		{
			for (int y = lightIsland->offset.y; y < lightIsland->offset.y + lightIsland->size.y; y++)
			{
				for (int x = lightIsland->offset.x; x < lightIsland->offset.x + lightIsland->size.x; x++)
				{
					CollisionMeshChunk* chunk = collisionMesh->GetChunk(lightIsland->chunkIndex);

					int lumelIndex = y * lightAtlasSize.x + x;

					Lumel* lumel = &lumels[lumelIndex];

					if (lumel->isParticipant)
					{
						lumel->intensityCacheItemsByBounceNumber[0] = rayTracer->CalculateDirectIlluminationIntensityForLumel(
							this->currentLight, &lumel->worldPosition, &lumel->normal, collisionMesh, lightIsland->chunkIndex);
					}
				}
			}
		}
	}
}

void Worker::ComputeIndirectIlluminationBounceIntensitiesForCurrentLightInternal()
{
	IEngine* engine = GetEngine();
	IWorldMeshAsset* worldMeshAsset = engine->GetWorldMeshAsset();
	ICollisionMesh* collisionMesh = worldMeshAsset->GetCollisionMesh();
	IRayTracer* rayTracer = engine->GetRayTracer();
	ILogger* logger = engine->GetLogger();
	const Config* config = engine->GetConfig();

	ILightAtlas* lightAtlas = engine->GetCurrentLightAtlas();
	Vec2i lightAtlasSize = lightAtlas->GetSize();
	Lumel* lumels = lightAtlas->GetLumels();

	Vec2 pixelCentre;
	Vec2::Set(&pixelCentre, 1.0f / lightAtlasSize.x / 2.0f, 1.0f / lightAtlasSize.y / 2.0f);

	for (int i = 0; i < this->numberOfLightIslandIndexes; i++)
	{
		int lightIslandIndex = this->lightIslandIndexes[i];

		WorldMeshLightIsland* lightIsland = worldMeshAsset->GetLightIsland(lightIslandIndex);

		// Check if the light island chunks are effected by the current light.
		if (this->effectedChunksFieldForCurrentLight->GetBit(lightIsland->chunkIndex))
		{
			for (int y = lightIsland->offset.y; y < lightIsland->offset.y + lightIsland->size.y; y++)
			{
				for (int x = lightIsland->offset.x; x < lightIsland->offset.x + lightIsland->size.x; x++)
				{
					CollisionMeshChunk* chunk = collisionMesh->GetChunk(lightIsland->chunkIndex);

					int lumelIndex = y * lightAtlasSize.x + x;

					Lumel* lumel = &lumels[lumelIndex];

					if (lumel->isParticipant)
					{
						lumel->intensityCacheItemsByBounceNumber[this->currentBounceNumber] = 0.0f;

						for (int bounceTargetIndex = 0; bounceTargetIndex < lumel->numberOfBounceTargets; bounceTargetIndex++)
						{
							int bounceTargetLumelIndex = lumel->bounceTargets[bounceTargetIndex].lumelIndex;
							Lumel* bounceTargetLumel = &lumels[bounceTargetLumelIndex];

							float distanceToBounceTargetSqr = Vec3::DistanceSqr(&lumel->worldPosition, &bounceTargetLumel->worldPosition);
							float attentuation = Math::Clamp(1.0f - (distanceToBounceTargetSqr / this->currentLight->distanceSqr), 0.0f, 1.0f);
							float absorbtionFactor = 0.5f;

							float bounceIntensity = bounceTargetLumel->intensityCacheItemsByBounceNumber[this->currentBounceNumber - 1] * attentuation * absorbtionFactor;

							lumel->intensityCacheItemsByBounceNumber[this->currentBounceNumber] += bounceIntensity;
						}

						if (lumel->numberOfBounceTargets > 0)
						{
							lumel->intensityCacheItemsByBounceNumber[this->currentBounceNumber] /= (float)lumel->numberOfBounceTargets;
						}
					}
				}
			}
		}
	}
}

void Worker::FillBordersInternal()
{
	IEngine* engine = GetEngine();
	IWorldMeshAsset* worldMeshAsset = engine->GetWorldMeshAsset();
	ILogger* logger = engine->GetLogger();

	for (int i = 0; i < this->numberOfLightIslandIndexes; i++)
	{
		int lightIslandIndex = this->lightIslandIndexes[i];

		WorldMeshLightIsland* lightIsland = worldMeshAsset->GetLightIsland(lightIslandIndex);

		this->FillLightIslandBorders(lightIsland);
	}
}

bool Worker::FindWorldPositionForLightIslandTexel(Vec3* outWorldPosition, Vec3* outNormal, int* outFaceIndex, ICollisionMesh* collisionMesh, CollisionMeshChunk* chunk, Vec2 uv)
{
	bool faceFound = false;

	for (int faceIndex = chunk->startFaceIndex; faceIndex < chunk->startFaceIndex + chunk->numberOfFaces; faceIndex++)
	{
		CollisionFace* face = collisionMesh->GetFace(faceIndex);

		if (Math::CalculateWorldPositionAndNormalFromUV(outWorldPosition, outNormal, face->points, face->normals, face->lightAtlasUVs, &uv))
		{
			faceFound = true;
			*outFaceIndex = faceIndex;
			break;
		}
	}

	return faceFound;
}

void Worker::FillLightIslandBorders(WorldMeshLightIsland* lightIsland)
{
	IEngine* engine = GetEngine();
	ILightAtlas* lightAtlas = engine->GetCurrentLightAtlas();

	Vec2i lightAtlasSize = lightAtlas->GetSize();
	Lumel* lumels = lightAtlas->GetLumels();

	int xFrom = lightIsland->offset.x;
	int xTo = lightIsland->offset.x + lightIsland->size.x - 1;
	int yFrom = lightIsland->offset.y;
	int yTo = lightIsland->offset.y + lightIsland->size.y - 1;

	for (int y = yFrom; y <= yTo; y++)
	{
		for (int x = xFrom; x <= xTo; x++)
		{
			int lumelIndex = y * lightAtlasSize.x + x;

			Lumel* lumel = &lumels[lumelIndex];
			if (lumel->state == LumelStateNotSet)
			{
				int numberOfSamples = 0;
				RgbFloat accumulatedColour;
				RgbFloat::Zero(&accumulatedColour);

				for (int j = -1; j <= 1; j++)
				{
					for (int i = -1; i <= 1; i++)
					{
						if (i == 0 && j == 0)
						{
							continue;
						}

						if (i == -1 && x == xFrom)
						{
							continue;
						}

						if (i == 1 && x == xTo)
						{
							continue;
						}

						if (j == -1 && y == yFrom)
						{
							continue;
						}

						if (j == 1 && y == yTo)
						{
							continue;
						}

						int sampleLumelIndex = (y + j) * lightAtlasSize.x + (x + i);
						Lumel* sampleLumel = &lumels[sampleLumelIndex];
						if (sampleLumel->state == LumelStateSet)
						{
							RgbFloat::Add(&accumulatedColour, &accumulatedColour, &sampleLumel->colour);
							numberOfSamples++;
						}
					}
				}

				if (numberOfSamples > 0)
				{
					RgbFloat averageColour;
					RgbFloat::Scale(&averageColour, &accumulatedColour, 1.0f / (float)numberOfSamples);
					lumel->colour = averageColour;
					lumel->state = LumelStateTempFilled;
				}
			}
		}
	}

	for (int y = yFrom; y <= yTo; y++)
	{
		for (int x = xFrom; x <= xTo; x++)
		{
			int lumelIndex = y * lightAtlasSize.x + x;

			Lumel* lumel = &lumels[lumelIndex];
			if (lumel->state == LumelStateTempFilled)
			{
				lumel->state = LumelStateSet;
			}
		}
	}
}

Vec2 Worker::CalculatePixelCentre()
{
	IEngine* engine = GetEngine();
	ILightAtlas* lightAtlas = engine->GetCurrentLightAtlas();
	Vec2i lightAtlasSize = lightAtlas->GetSize();

	Vec2 pixelCentre;
	Vec2::Set(&pixelCentre, 1.0f / lightAtlasSize.x / 2.0f, 1.0f / lightAtlasSize.y / 2.0f);

	return pixelCentre;
}

void Worker::FindBounceTarget(int* outBounceTargetLumelIndex, bool* outSkyIsVisible, Lumel* lumels, ICollisionMesh* collisionMesh, CollisionLine* line, WorldMeshLightIsland* lightIsland, Vec2i* lightAtlasSize)
{
	*outBounceTargetLumelIndex = -1;

	Vec3 bounceTargetWorldPosition;
	MeshChunkFaceIndex bounceTargetMeshChunkFaceIndex;

	if (!collisionMesh->FindNearestLineIntersectWithMesh(&bounceTargetWorldPosition, &bounceTargetMeshChunkFaceIndex, line, lightIsland->chunkIndex, -1))
	{
		*outSkyIsVisible = true;
	}
	else
	{
		if (this->effectedChunksFieldForCurrentLightAtlas->GetBit(bounceTargetMeshChunkFaceIndex.chunkIndex))
		{
			CollisionFace* bounceTargetFace = collisionMesh->GetFace(bounceTargetMeshChunkFaceIndex.faceIndex);

			Vec3 barycentricCoords;
			Math::CalculateBarycentricCoords(&barycentricCoords, bounceTargetFace->points, &bounceTargetWorldPosition);

			Vec2 uv;
			Math::BarycentricMix(&uv, bounceTargetFace->lightAtlasUVs, &barycentricCoords);

			int bounceTargetLumelX = (int)(uv.x * lightAtlasSize->x);
			int bounceTargetLumelY = (int)(uv.y * lightAtlasSize->y);

			int potentialBounceTargetLumelIndex = bounceTargetLumelY * lightAtlasSize->x + bounceTargetLumelX;
			Lumel* bounceTargetLumel = &lumels[potentialBounceTargetLumelIndex];

			if (!bounceTargetLumel->isParticipant)
			{
				int participantBounceTargetLumelInVicinityIndex = this->FindParticipantLumelInVicinity(lumels, bounceTargetLumelX, bounceTargetLumelY, lightIsland, lightAtlasSize);
				if (participantBounceTargetLumelInVicinityIndex != -1)
				{
					bounceTargetLumel = &lumels[participantBounceTargetLumelInVicinityIndex];
					potentialBounceTargetLumelIndex = participantBounceTargetLumelInVicinityIndex;
				}
			}

			if (bounceTargetLumel->isParticipant)
			{
				*outBounceTargetLumelIndex = potentialBounceTargetLumelIndex;
			}
		}
	}
}

int Worker::FindParticipantLumelInVicinity(Lumel* lumels, int x, int y, WorldMeshLightIsland* lightIsland, Vec2i* lightAtlasSize)
{
	int lumelIndex = -1;
	bool found = false;

	for (int y2 = -2; y2 <= 2 && !found; y2++)
	{
		for (int x2 = -2; x2 <= 2 && !found; x2++)
		{
			if (x + x2 >= lightIsland->offset.x &&
				y + y2 >= lightIsland->offset.y &&
				x + x2 < lightIsland->offset.x + lightIsland->size.x &&
				y + y2 < lightIsland->offset.y + lightIsland->size.y)
			{
				int potentialLumelIndex = (y + y2) * lightAtlasSize->x + (x + x2);

				Lumel* potentialLumel = &lumels[potentialLumelIndex];
				if (potentialLumel->isParticipant)
				{
					lumelIndex = potentialLumelIndex;
					found = true;
					break;
				}
			}
		}
	}

	return lumelIndex;
}

/*void Worker::FillLightIslandBorders(ILightAtlas* lightAtlas, CollisionMeshChunk* chunk)
{
	Vec2i lightAtlasSize = lightAtlas->GetSize();
	RgbFloat* texels = lightAtlas->GetTexels();

	int xFrom = chunk->lightIslandOffset.x;
	int xTo = chunk->lightIslandOffset.x + chunk->lightIslandSize.x - 1;
	int yFrom = chunk->lightIslandOffset.y;
	int yTo = chunk->lightIslandOffset.y + chunk->lightIslandSize.y - 1;

	for (int y = yFrom; y <= yTo; y++)
	{
		for (int x = xFrom; x <= xTo; x++)
		{
			int texelIndex = y * lightAtlasSize.x + x;

			RgbFloat* texel = &texels[texelIndex];
			if (texel->r == -1.0f)
			{
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

						if (i == -1 && x == xFrom)
						{
							continue;
						}

						if (i == 1 && x == xTo)
						{
							continue;
						}

						if (j == -1 && y == yFrom)
						{
							continue;
						}

						if (j == 1 && y == xTo)
						{
							continue;
						}

						int sampleTexelIndex = (y + j) * lightAtlasSize.x + (x + i);
						RgbFloat* sample = &texels[sampleTexelIndex];
						if (sample->r >= 0.0f)
						{
							RgbFloat::Add(&accumulatedSamples, &accumulatedSamples, sample);
							numberOfSamples++;
						}
					}
				}

				if (numberOfSamples > 0)
				{
					RgbFloat averageSample;
					RgbFloat::Scale(&averageSample, &accumulatedSamples, -1.0f / (float)numberOfSamples);
					*texel = averageSample;
				}
			}
		}
	}

	for (int y = yFrom; y <= yTo; y++)
	{
		for (int x = xFrom; x <= xTo; x++)
		{
			int texelIndex = y * lightAtlasSize.x + x;

			RgbFloat* texel = &texels[texelIndex];
			if (texel->r < 0)
			{
				RgbFloat::Scale(texel, texel, -1.0f);
			}
		}
	}
}*/

/*int Worker::FindFaceIndexForLightIslandTexel(ICollisionMesh* collisionMesh, CollisionMeshChunk* chunk, Vec2 uv)
{
	int faceIndex = -1;

	for (int i = 0; i < chunk->numberOfFaces; i++)
	{
		CollisionFace* face = collisionMesh->GetFace(chunk->startFaceIndex + i);

		Vec3 worldPosition;
		if (Math::CalculateWorldPositionFromUV(&worldPosition, face->points, face->uvs, &uv))
		{
			faceIndex = i;
			break;
		}
	}

	return faceIndex;
}*/

/*void Worker::Init(int startLightIslandIndex, int numberOfLightIslandIndexes)
{
	this->startLightIslandIndex = startLightIslandIndex;
	this->numberOfLightIslandIndexes = numberOfLightIslandIndexes;
}*/


/*void Worker::AccumulateIndirectIlluminationIntensitiesForCurrentLightAsync(NormalWithinHemisphereCalculationMetrics* normalWithinHemisphereCalculationMetrics, int hemisphereCircleIndex, int hemisphereSegmentIndex, int bounceNumber)
{
	IEngine* engine = GetEngine();
	IThreadManager* threadManager = engine->GetThreadManager();

	this->currentTaskType = WorkerTaskTypeAccumulateIndirectIlluminationIntensitiesForCurrentLight;
	this->currentNormalWithinHemisphereCalculationMetrics = normalWithinHemisphereCalculationMetrics;
	this->currentHemisphereCircleIndex = hemisphereCircleIndex;
	this->currentHemisphereSegmentIndex = hemisphereSegmentIndex;
	this->currentBounceNumber = bounceNumber;
	this->currentTaskHasFinished = false;

	threadManager->StartThread(WorkerRunSectorCruncherEntryPoint, this);
}*/

/*
void Worker::AverageIndirectIlluminationIntensitiesForCurrentLightAsync(int bounceNumber)
{
	IEngine* engine = GetEngine();
	IThreadManager* threadManager = engine->GetThreadManager();

	this->currentTaskType = WorkerTaskTypeAverageIndirectIlluminationIntensitiesForCurrentLight;
	this->currentBounceNumber = bounceNumber;
	this->currentTaskHasFinished = false;

	threadManager->StartThread(WorkerRunSectorCruncherEntryPoint, this);
}
*/
/*
void Worker::CompositeColourForCurrentLightAsync()
{
	IEngine* engine = GetEngine();
	IThreadManager* threadManager = engine->GetThreadManager();

	this->currentTaskType = WorkerTaskTypeCompositeColourForCurrentLight;
	this->currentTaskHasFinished = false;

	threadManager->StartThread(WorkerRunSectorCruncherEntryPoint, this);
}*/

/*void Worker::DumpStats()
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	logger->Write("Failure Type 1 Count: %d", this->failureType1Count);
	logger->Write("Failure Type 2 Count: %d", this->failureType2Count);
	logger->Write("Failure Type 3 Count: %d", this->failureType3Count);
	logger->Write("Failure Type 4 Count: %d", this->failureType4Count);
	logger->Write("Failure Type 5 Count: %d", this->failureType5Count);
}*/

/*int Worker::GetTotalFailures()
{
	return this->failureType1Count + this->failureType2Count + this->failureType3Count + failureType4Count + failureType5Count;
}*/


/*void Worker::StartNewIndirectIlluminationBounce()
{
	IEngine* engine = GetEngine();
	IWorldMeshAsset* worldMeshAsset = engine->GetWorldMeshAsset();
	ICollisionMesh* collisionMesh = worldMeshAsset->GetCollisionMesh();
	IRayTracer* rayTracer = engine->GetRayTracer();
	ILogger* logger = engine->GetLogger();

	ILightAtlas* lightAtlas = engine->GetLightAtlas();
	Vec2i lightAtlasSize = lightAtlas->GetSize();
	Lumel* lumels = lightAtlas->GetLumels();

	Vec2 pixelCentre;
	Vec2::Set(&pixelCentre, 1.0f / lightAtlasSize.x / 2.0f, 1.0f / lightAtlasSize.y / 2.0f);

	for (int lightIslandIndex = this->startLightIslandIndex;
		lightIslandIndex < this->startLightIslandIndex + this->numberOfLightIslandIndexes && lightIslandIndex < worldMeshAsset->GetNumberOfLightIslands();
		lightIslandIndex++)
	{
		WorldMeshLightIsland* lightIsland = worldMeshAsset->GetLightIsland(lightIslandIndex);

		// TODO - check if the light island chunks are effected by the current light.

		for (int y = lightIsland->offset.y; y < lightIsland->offset.y + lightIsland->size.y; y++)
		{
			for (int x = lightIsland->offset.x; x < lightIsland->offset.x + lightIsland->size.x; x++)
			{
				CollisionMeshChunk* chunk = collisionMesh->GetChunk(lightIsland->chunkIndex);

				int lumelIndex = y * lightAtlasSize.x + x;

				Lumel* lumel = &lumels[lumelIndex];

				if (lumel->isParticipant)
				{
					lumel->intensityCacheItemsByBounceNumber[this->currentBounceNumber] = 0;
				}
			}
		}
	}
}*/

/*
void Worker::AccumulateIndirectIlluminationIntensitiesForCurrentLightInternal()
{
	IEngine* engine = GetEngine();
	IWorldMeshAsset* worldMeshAsset = engine->GetWorldMeshAsset();
	ICollisionMesh* collisionMesh = worldMeshAsset->GetCollisionMesh();
	IRayTracer* rayTracer = engine->GetRayTracer();
	ILogger* logger = engine->GetLogger();
	const Config* config = engine->GetConfig();

	ILightAtlas* lightAtlas = engine->GetLightAtlas();
	Vec2i lightAtlasSize = lightAtlas->GetSize();
	Lumel* lumels = lightAtlas->GetLumels();

	Vec2 pixelCentre;
	Vec2::Set(&pixelCentre, 1.0f / lightAtlasSize.x / 2.0f, 1.0f / lightAtlasSize.y / 2.0f);

	int logCounter = 0;

	for (int lightIslandIndex = this->startLightIslandIndex;
		lightIslandIndex < this->startLightIslandIndex + this->numberOfLightIslandIndexes && lightIslandIndex < worldMeshAsset->GetNumberOfLightIslands();
		lightIslandIndex++)
	{
		if (logCounter == 9)
		{
			logger->Write("Light island index: %d/%d", (lightIslandIndex - this->startLightIslandIndex) + 1, numberOfLightIslandIndexes);
			logCounter = 0;
		}

		logCounter++;

		WorldMeshLightIsland* lightIsland = worldMeshAsset->GetLightIsland(lightIslandIndex);

		// TODO - check if the light island chunks are effected by the current light.

		for (int y = lightIsland->offset.y; y < lightIsland->offset.y + lightIsland->size.y; y++)
		{
			for (int x = lightIsland->offset.x; x < lightIsland->offset.x + lightIsland->size.x; x++)
			{
				CollisionMeshChunk* chunk = collisionMesh->GetChunk(lightIsland->chunkIndex);

				int lumelIndex = y * lightAtlasSize.x + x;

				Lumel* lumel = &lumels[lumelIndex];

				if (lumel->isParticipant)
				{
					CollisionLine line;
					line.from = lumel->worldPosition;

					Vec3 purturbedLumelNormal;

					CollisionFace* face = collisionMesh->GetFace(lumel->faceIndex);

					Vec3 fakeBinormal;
					Vec3::Set(&fakeBinormal, 0.0f, 0.0f, -1.0f);
					
					Math::CalculateNormalWithinHemisphere(
						&purturbedLumelNormal, &lumel->normal, &face->freeNormalisedEdges[0], 
						this->currentNormalWithinHemisphereCalculationMetrics, this->currentHemisphereCircleIndex, this->currentHemisphereSegmentIndex);
		
					Vec3::ScaleAndAdd(&line.to, &line.from, &purturbedLumelNormal, 10000.0f); // FIXME - use a properly computed scale.
					CollisionLine::FromOwnFromAndToPoints(&line);

					Vec3 bounceTargetWorldPosition;
					MeshChunkFaceIndex bounceTargetMeshChunkFaceIndex;
					if (collisionMesh->FindNearestLineIntersectWithMesh(&bounceTargetWorldPosition, &bounceTargetMeshChunkFaceIndex, &line, lightIsland->chunkIndex, -1))
					{
						CollisionFace* bounceTargetFace = collisionMesh->GetFace(bounceTargetMeshChunkFaceIndex.faceIndex);

						Vec3 barycentricCoords;
						Math::CalculateBarycentricCoords(&barycentricCoords, bounceTargetFace->points, &bounceTargetWorldPosition);

						Vec2 uv;
						Math::BarycentricMix(&uv, bounceTargetFace->lightAtlasUVs, &barycentricCoords);

						int bounceTargetLumelX = (int)(uv.x * lightAtlasSize.x);
						int bounceTargetLumelY = (int)(uv.y * lightAtlasSize.y);

						int bounceTargetLumelIndex = bounceTargetLumelY * lightAtlasSize.x + bounceTargetLumelX;
						Lumel* bounceTargetLumel = &lumels[bounceTargetLumelIndex];

						if (!bounceTargetLumel->isParticipant)
						{
							bool found = false;
							for (int y2 = -2; y2 <= 2 && !found; y2++)
							{
								for (int x2 = -2; x2 <= 2 && !found; x2++)
								{
									if (x + x2 >= lightIsland->offset.x &&
										y + y2 >= lightIsland->offset.y &&
										x + x2 < lightIsland->offset.x + lightIsland->size.x &&
										y + y2 < lightIsland->offset.y + lightIsland->size.y)
									{
										int otherLumelIndex = (y + y2) * lightAtlasSize.x + (x + x2);

										Lumel* otherLumel = &lumels[otherLumelIndex];
										if (otherLumel->isParticipant)
										{
											bounceTargetLumel = otherLumel;
											bounceTargetLumelIndex = otherLumelIndex;
											found = true;
										}
									}
								}
							}
						}

						if (bounceTargetLumel->isParticipant)
						{
							float distanceToBounceTargetSqr = Vec3::DistanceSqr(&lumel->worldPosition, &bounceTargetLumel->worldPosition);
							float attentuation = Math::Clamp(1.0f - (distanceToBounceTargetSqr / this->currentLight->distanceSqr), 0.0f, 1.0f);
							float absorbtionFactor = 0.9f;

							float bounceIntensity = bounceTargetLumel->intensityCacheItemsByBounceNumber[this->currentBounceNumber - 1] * attentuation * absorbtionFactor;

							lumel->intensityCacheItemsByBounceNumber[this->currentBounceNumber] += bounceIntensity;

							lumel->numberOfSamplesForCurrentBounce++;
						}
						else
						{
							this->failureType1Count++;
						}
					}
					else
					{
						this->failureType2Count++;
						logger->Write("Bad line: %f,%f,%f : %f,%f,%f", line.from.x, line.from.y, line.from.z, line.to.x, line.to.y, line.to.z);
					}
				}
			}
		}
	}
}*/

/*void Worker::AverageIndirectIlluminationIntensitiesForCurrentLightInternal()
{
	IEngine* engine = GetEngine();
	IWorldMeshAsset* worldMeshAsset = engine->GetWorldMeshAsset();
	ICollisionMesh* collisionMesh = worldMeshAsset->GetCollisionMesh();
	IRayTracer* rayTracer = engine->GetRayTracer();
	ILogger* logger = engine->GetLogger();

	ILightAtlas* lightAtlas = engine->GetLightAtlas();
	Vec2i lightAtlasSize = lightAtlas->GetSize();
	Lumel* lumels = lightAtlas->GetLumels();

	Vec2 pixelCentre;
	Vec2::Set(&pixelCentre, 1.0f / lightAtlasSize.x / 2.0f, 1.0f / lightAtlasSize.y / 2.0f);

	int logCounter = 0;

	for (int lightIslandIndex = this->startLightIslandIndex;
		lightIslandIndex < this->startLightIslandIndex + this->numberOfLightIslandIndexes && lightIslandIndex < worldMeshAsset->GetNumberOfLightIslands();
		lightIslandIndex++)
	{
		if (logCounter == 9)
		{
			logger->Write("Light island index: %d/%d", (lightIslandIndex - this->startLightIslandIndex) + 1, numberOfLightIslandIndexes);
			logCounter = 0;
		}

		logCounter++;

		WorldMeshLightIsland* lightIsland = worldMeshAsset->GetLightIsland(lightIslandIndex);

		// TODO - check if the light island chunks are effected by the current light.

		for (int y = lightIsland->offset.y; y < lightIsland->offset.y + lightIsland->size.y; y++)
		{
			for (int x = lightIsland->offset.x; x < lightIsland->offset.x + lightIsland->size.x; x++)
			{
				CollisionMeshChunk* chunk = collisionMesh->GetChunk(lightIsland->chunkIndex);

				int lumelIndex = y * lightAtlasSize.x + x;

				Lumel* lumel = &lumels[lumelIndex];

				if (lumel->isParticipant)
				{
					lumel->intensityCacheItemsByBounceNumber[this->currentBounceNumber] /= (float)lumel->numberOfBounceSamples;
				}
			}
		}
	}
}*/

/*
float Worker::RecurseIndirectIlluminationBounces(Lumel* lumels, Lumel* lumel, float baseDistanceToLightSqr, int recursionDepth)
{
	float indirectIlluminationIntensity = 0.0f;

	if (lumel->bounceTarget.lumelIndex != -1)
	{
		Lumel* bounceTargetLumel = &lumels[lumel->bounceTarget.lumelIndex];

		float distanceToBounceTargetSqr = Vec3::DistanceSqr(&lumel->worldPosition, &bounceTargetLumel->worldPosition);
		float remainingDistanceSqr = Math::Max(this->currentLight->distanceSqr - baseDistanceToLightSqr - bounceTargetLumel->directIlluminationAverageDistanceToLightSqrCache, 0);
		if (remainingDistanceSqr > 0)
		{
			float attentuation = Math::Clamp(1.0f - (distanceToBounceTargetSqr / remainingDistanceSqr), 0.0f, 1.0f);

			float absorbtionFactor = 0.9f;

			indirectIlluminationIntensity = bounceTargetLumel->directIlluminationIntensityCache * attentuation;

			if (recursionDepth < 0)
			{
				indirectIlluminationIntensity += this->RecurseIndirectIlluminationBounces(lumels, bounceTargetLumel, baseDistanceToLightSqr + distanceToBounceTargetSqr, recursionDepth + 1);
			}

			indirectIlluminationIntensity *= absorbtionFactor;
		}

		//indirectIlluminationIntensity = bounceTargetLumel->directIlluminationIntensityCache * 0.9f;

//		if (recursionDepth < 1)
	//	{
		//	indirectIlluminationIntensity += this->RecurseIndirectIlluminationBounces(lumels, bounceTargetLumel, baseDistanceToLightSqr, recursionDepth + 1);
		//}
	}
	else
	{
		this->failureType5Count++;
	}

	return indirectIlluminationIntensity;
}
*/

/*
void Worker::CompositeColourForCurrentLightInternal()
{
	IEngine* engine = GetEngine();
	IWorldMeshAsset* worldMeshAsset = engine->GetWorldMeshAsset();
	ILogger* logger = engine->GetLogger();
	const Config* config = engine->GetConfig();

	ILightAtlas* lightAtlas = engine->GetLightAtlas();
	Vec2i lightAtlasSize = lightAtlas->GetSize();
	Lumel* lumels = lightAtlas->GetLumels();

	for (int lightIslandIndex = this->startLightIslandIndex;
		lightIslandIndex < this->startLightIslandIndex + this->numberOfLightIslandIndexes && lightIslandIndex < worldMeshAsset->GetNumberOfLightIslands();
		lightIslandIndex++)
	{
		WorldMeshLightIsland* lightIsland = worldMeshAsset->GetLightIsland(lightIslandIndex);

		for (int y = lightIsland->offset.y; y < lightIsland->offset.y + lightIsland->size.y; y++)
		{
			for (int x = lightIsland->offset.x; x < lightIsland->offset.x + lightIsland->size.x; x++)
			{
				int lumelIndex = y * lightAtlasSize.x + x;

				Lumel* lumel = &lumels[lumelIndex];
				//if (lumel->state == LumelStateSet)
				if (lumel->isParticipant)
				{
					float intensity = lumel->directIlluminationIntensityCache;
					
					if (lumel->numberOfBounceSamples > 0)
					{
						intensity += (lumel->indirectIlluminationIntensitySampleCache / (float)lumel->numberOfBounceSamples);

						if (lumel->numberOfBounceSamples != this->currentNormalWithinHemisphereCalculationMetrics->outputNormalCount)
						{
							//logger->Write("Arse: %d", lumel->numberOfBounceSamples);
							this->failureType3Count++;
						}
					}
					else
					{
						this->failureType4Count++;
					}
					

					RgbFloat lumelColour;
					RgbFloat::Scale(&lumelColour, &this->currentLight->colour, intensity);

					RgbFloat::Add(&lumel->colour, &lumel->colour, &lumelColour);
				}
			}
		}
	}
}*/


/*bool Worker::FindWorldPositionForLightIslandTexel(Vec3* outWorldPosition, Vec3* outNormal, ICollisionMesh* collisionMesh, MeshChunkFaceIndex* chunkFaceIndex, Vec2 uv)
{
	bool faceFound = false;

	CollisionFace* face = collisionMesh->GetFace(chunkFaceIndex->faceIndex);

	if (Math::CalculateWorldPositionAndNormalFromUV(outWorldPosition, outNormal, face->points, face->normals, face->uvs, &uv))
	{
		faceFound = true;
	}

	return faceFound;
}*/

/*bool Worker::CheckIfChunkIsEffectedByLight(CollisionMeshChunk* chunk, ExpandedLight* light)
{
	bool isEffectedByLight = false;

	// FIXME!!

	for (int lightNodeIndex = 0; lightNodeIndex < light->nodes.GetLength(); lightNodeIndex++)
	{
		LightNode* lightNode = &light->nodes[lightNodeIndex];

		Sphere lightNodeSphere;
		Sphere::Set(&lightNodeSphere, &lightNode->worldPosition, lightNode->distance);

		if (Sphere::CheckIntersectsAABB(&lightNodeSphere, &chunk->aabb))
		{
			isEffectedByLight = true;
			break;
		}
	}

	return isEffectedByLight;
}*/