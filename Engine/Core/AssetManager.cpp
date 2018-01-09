#include "Core/AssetManager.h"

AssetManager::AssetManager()
{
	this->loadingPhase = AssetManagerLoadingPhaseNone;
	this->justLoadedMapAsset = null;
	this->isLoadingCoreAssets = false;
	this->isLoading = false;
	this->assetLoadFailed = false;
	this->bytesLoadedFromDisk = 0;
}

AssetManager::~AssetManager()
{
	this->CleanUpAssets(true, true);
}

void AssetManager::BeginLoadingCoreAssets()
{
	this->BeginLoadingAsset(AssetTypeText, "shaders/world-mesh-chunk-fs.glsl", true);
	this->BeginLoadingAsset(AssetTypeText, "shaders/world-mesh-chunk-vs.glsl", true);
	this->BeginLoadingAsset(AssetTypeText, "shaders/static-mesh-chunk-fs.glsl", true);
	this->BeginLoadingAsset(AssetTypeText, "shaders/static-mesh-chunk-vs.glsl", true);
	this->BeginLoadingAsset(AssetTypeText, "shaders/volume-fs.glsl", true);
	this->BeginLoadingAsset(AssetTypeText, "shaders/volume-vs.glsl", true);
	this->BeginLoadingAsset(AssetTypeText, "shaders/sprite-3d-fs.glsl", true);
	this->BeginLoadingAsset(AssetTypeText, "shaders/sprite-3d-vs.glsl", true);
	this->BeginLoadingAsset(AssetTypeText, "shaders/sprite-2d-fs.glsl", true);
	this->BeginLoadingAsset(AssetTypeText, "shaders/sprite-2d-vs.glsl", true);
	this->BeginLoadingAsset(AssetTypeText, "shaders/line-fs.glsl", true);
	this->BeginLoadingAsset(AssetTypeText, "shaders/line-vs.glsl", true);
	this->BeginLoadingAsset(AssetTypeSpriteSheet, "sprite-sheets/fonts/debug.ss", true);
	this->BeginLoadingAsset(AssetTypeSpriteSheet, "sprite-sheets/icons/debug.ss", true);
	this->BeginLoadingAsset(AssetTypeTexture, "textures/sprite-sheets/fonts/debug.png", true);
	this->BeginLoadingAsset(AssetTypeTexture, "textures/sprite-sheets/icons/debug.png", true);
	this->BeginLoadingAsset(AssetTypeTexture, "textures/system/blank-light-atlas.png", true);
	
	this->loadingPhase = AssetManagerLoadingPhaseLoadingReferencedAssets;
	this->isLoadingCoreAssets = true;
}

void AssetManager::HeartBeat()
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	if (this->isLoading)
	{
		bool allAssetLoadJobsAreComplete = this->ExecuteAssetLoadJobs();

		if (this->loadingPhase == AssetManagerLoadingPhaseLoadingMapAsset)
		{
			if (this->justLoadedMapAsset != null)
			{
				this->loadingPhase = AssetManagerLoadingPhaseLoadingReferencedAssets;

				for (int i = 0; i < this->justLoadedMapAsset->GetNumberAssetRefs(); i++)
				{
					AssetRef* assetRef = this->justLoadedMapAsset->GetAssetRef(i);
					this->BeginLoadingAsset(assetRef->assetType, assetRef->filePath, false);
				}

				this->CleanUpAssets(false, false);
			}
		}
		else if (this->loadingPhase == AssetManagerLoadingPhaseLoadingReferencedAssets)
		{
			if (allAssetLoadJobsAreComplete)
			{
				for (int assetStoreIndex = 0; assetStoreIndex < AssetManagerNumberOfStores; assetStoreIndex++)
				{
					DynamicLengthSlottedArray<IAsset*>& assetStore = this->assetStores[assetStoreIndex];

					for (int assetIndex = 0; assetIndex < assetStore.GetNumberOfIndexes(); assetIndex++)
					{
						if (assetStore.GetIndexIsOccupied(assetIndex))
						{
							IAsset* asset = assetStore[assetIndex];
							if (!asset->ResolveReferencedAssets())
							{
								this->assetLoadFailed = true;
							}
						}
					}
				}

				this->loadingPhase = AssetManagerLoadingPhaseNone;
				this->isLoading = false;
				this->isLoadingCoreAssets = false;

				logger->Write("Finished loading. %ld bytes loaded from disk.", this->bytesLoadedFromDisk);
			}
		}
	}
}

void AssetManager::LoadMap(const char* filePath)
{
	this->justLoadedMapAsset = null;
	this->loadingPhase = AssetManagerLoadingPhaseLoadingMapAsset;
	this->bytesLoadedFromDisk = 0;

	this->MarkAllAssetsAsEvictable();
	this->BeginLoadingAsset(AssetTypeMap, filePath, false);
}

int AssetManager::FindAssetIndex(AssetType assetType, const char* filePath)
{
	int foundAssetIndex = -1;
	DynamicLengthSlottedArray<IAsset*>& assetStore = this->assetStores[assetType];

	for (int assetIndex = 0; assetIndex < assetStore.GetNumberOfIndexes(); assetIndex++)
	{
		if (assetStore.GetIndexIsOccupied(assetIndex))
		{
			IAsset* asset = assetStore[assetIndex];
			if (strcmp(asset->GetFilePath(), filePath) == 0)
			{
				foundAssetIndex = assetIndex;
				break;
			}
		}
	}

	return foundAssetIndex;
}

bool AssetManager::ResolveAssetRefIndex(AssetRef* assetRef)
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	assetRef->index = this->FindAssetIndex(assetRef->assetType, assetRef->filePath);

	bool success = assetRef->index != -1;

	if (!success)
	{
		logger->Write("Failed to resolve asset ref '%s'.", assetRef->filePath);
	}

	return success;
}

IMapAsset* AssetManager::GetMapAsset(int assetIndex)
{
	return (IMapAsset*)this->assetStores[AssetTypeMap][assetIndex];
}

IActorAsset* AssetManager::GetActorAsset(int assetIndex)
{
	return (IActorAsset*)this->assetStores[AssetTypeActor][assetIndex];
}

IWorldMeshAsset* AssetManager::GetWorldMeshAsset(int assetIndex)
{
	return (IWorldMeshAsset*)this->assetStores[AssetTypeWorldMesh][assetIndex];
}

IStaticMeshAsset* AssetManager::GetStaticMeshAsset(int assetIndex)
{
	return (IStaticMeshAsset*)this->assetStores[AssetTypeStaticMesh][assetIndex];
}

ITextureAsset* AssetManager::GetTextureAsset(int assetIndex)
{
	return (ITextureAsset*)this->assetStores[AssetTypeTexture][assetIndex];
}

ISpriteSheetAsset* AssetManager::GetSpriteSheetAsset(int assetIndex)
{
	return (ISpriteSheetAsset*)this->assetStores[AssetTypeSpriteSheet][assetIndex];
}

ITextAsset* AssetManager::GetTextAsset(int assetIndex)
{
	return (ITextAsset*)this->assetStores[AssetTypeText][assetIndex];
}

IMaterialAsset* AssetManager::GetMaterialAsset(int assetIndex)
{
	return (IMaterialAsset*)this->assetStores[AssetTypeMaterial][assetIndex];
}

IPVSAsset* AssetManager::GetPVSAsset(int assetIndex)
{
	return (IPVSAsset*)this->assetStores[AssetTypePVS][assetIndex];
}

bool AssetManager::GetIsLoadingCoreAssets()
{
	return this->isLoadingCoreAssets;
}

bool AssetManager::GetIsLoading()
{
	return this->isLoading;
}

bool AssetManager::GetLoadFailed()
{
	return this->assetLoadFailed;
}

void AssetManager::BeginLoadingAsset(AssetType assetType, const char* filePath, bool isStayResident)
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	this->isLoading = true;

	IAsset* existingAsset = this->FindExistingAsset(assetType, filePath);
	if (existingAsset != null)
	{
		this->HandleJustLoadedAsset(existingAsset);
	}
	else
	{
		bool assetLoadJobAlreadyExists = this->FindAssetLoadJobIndex(filePath) != -1;
		if (!assetLoadJobAlreadyExists)
		{
			bool assetIsAlreadyLoaded = this->FindAssetIndex(assetType, filePath) != -1;
			if (!assetIsAlreadyLoaded)
			{
				AssetLoadJob* assetLoadJob = &this->assetLoadJobs.PushAndGet();
				assetLoadJob->assetType = assetType;
				strcpy(assetLoadJob->filePath, filePath);
				assetLoadJob->isStayResident = isStayResident;
				assetLoadJob->loadingFileIndex = -1;
				assetLoadJob->isComplete = false;
			}
		}
	}
}

IAsset* AssetManager::FindExistingAsset(AssetType assetType, const char* filePath)
{
	IAsset* existingAsset = null;

	DynamicLengthSlottedArray<IAsset*>& assetStore = this->assetStores[assetType];

	for (int assetIndex = 0; assetIndex < assetStore.GetNumberOfIndexes(); assetIndex++)
	{
		IAsset* asset = assetStore[assetIndex];
		if (asset != null && strcmp(asset->GetFilePath(), filePath) == 0)
		{
			existingAsset = asset;
			break;
		}
	}

	return existingAsset;
}

bool AssetManager::ExecuteAssetLoadJobs()
{
	IEngine* engine = GetEngine();
	IFileLoader* fileLoader = engine->GetFileLoader();
	ILogger* logger = engine->GetLogger();

	bool allAssetLoadJobsAreComplete = true;

	for (int i = 0; i < this->assetLoadJobs.GetLength(); i++)
	{
		AssetLoadJob* assetLoadJob = &this->assetLoadJobs[i];

		if (!assetLoadJob->isComplete)
		{
			allAssetLoadJobsAreComplete = false;

			if (assetLoadJob->loadingFileIndex == -1)
			{
				assetLoadJob->loadingFileIndex = fileLoader->BeginLoadingFile(assetLoadJob->filePath);
			}
			else
			{
				LoadingFileState loadingFileState = fileLoader->GetFileState(assetLoadJob->loadingFileIndex);
				
				if (loadingFileState == LoadingFileStateLoaded)
				{
					Buffer* fileData = fileLoader->GetFileData(assetLoadJob->loadingFileIndex);
					this->bytesLoadedFromDisk += fileData->length;

					IAsset* asset = null;
					bool assetLoadedSuccessfully = this->LoadAsset(&asset, assetLoadJob->assetType, assetLoadJob->filePath, fileData, assetLoadJob->isStayResident);
					
					fileLoader->FreeFile(assetLoadJob->loadingFileIndex);
					
					if (!assetLoadedSuccessfully)
					{
						this->assetLoadFailed = true;
						break;
					}

					assetLoadJob->isComplete = true;

					this->HandleJustLoadedAsset(asset);
				}
				else if (loadingFileState == LoadingFileStateFailed)
				{
					this->assetLoadFailed = true;
					logger->Write("Failed to load asset '%s'.", assetLoadJob->filePath);
					break;
				}
			}
		}
	}

	if (allAssetLoadJobsAreComplete)
	{
		this->assetLoadJobs.Clear();
	}

	return allAssetLoadJobsAreComplete;
}

void AssetManager::HandleJustLoadedAsset(IAsset* asset)
{
	asset->SetIsEvictable(false);

	if (this->loadingPhase == AssetManagerLoadingPhaseLoadingMapAsset)
	{
		this->justLoadedMapAsset = (IMapAsset*)asset;
	}
}

bool AssetManager::LoadAsset(IAsset** outAsset, AssetType assetType, const char* filePath, Buffer* fileData, bool isStayResident)
{
	IFactory* factory = GetFactory();
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	IAsset* asset = null;

	if (assetType == AssetTypeMap)
	{
		asset = factory->MakeMapAsset(filePath, fileData, isStayResident);
	}
	else if (assetType == AssetTypeWorldMesh)
	{
		asset = factory->MakeWorldMeshAsset(filePath, fileData, isStayResident);
	}
	else if (assetType == AssetTypeStaticMesh)
	{
		asset = factory->MakeStaticMeshAsset(filePath, fileData, isStayResident);
	}
	else if (assetType == AssetTypeActor)
	{
		asset = factory->MakeActorAsset(filePath, fileData, isStayResident);
	}
	else if (assetType == AssetTypeTexture)
	{
		asset = factory->MakeTextureAsset(filePath, fileData, isStayResident);
	}
	else if (assetType == AssetTypeSpriteSheet)
	{
		asset = factory->MakeSpriteSheetAsset(filePath, fileData, isStayResident);
	}
	else if (assetType == AssetTypeText)
	{
		asset = factory->MakeTextAsset(filePath, fileData, isStayResident);
	}
	else if (assetType == AssetTypeMaterial)
	{
		asset = factory->MakeMaterialAsset(filePath, fileData, isStayResident);
	}
	else if (assetType == AssetTypePVS)
	{
		asset = factory->MakePVSAsset(filePath, fileData, isStayResident);
	}
	else
	{
		logger->Write("Unknown asset type: %s.", filePath);
	}

	if (asset != null)
	{
		this->assetStores[assetType].AddItem(asset);
		*outAsset = asset;
	}

	bool success = asset->GetLoadedSuccessfully();

	if (success)
	{
		logger->Write("Asset '%s' loaded successfully.", filePath);
	}
	else
	{
		logger->Write("Failed to load asset '%s'.", filePath);
	}
	
	return success;
}

void AssetManager::CleanUpAssets(bool includeStayResidentAssets, bool includeNonEvictableAssets)
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	for (int assetStoreIndex = 0; assetStoreIndex < AssetManagerNumberOfStores; assetStoreIndex++)
	{
		DynamicLengthSlottedArray<IAsset*>& assetStore = this->assetStores[assetStoreIndex];

		for (int assetIndex = 0; assetIndex < assetStore.GetNumberOfIndexes(); assetIndex++)
		{
			if (assetStore.GetIndexIsOccupied(assetIndex))
			{
				IAsset* asset = assetStore[assetIndex];
				if (asset != null &&
					(!asset->GetIsStayResident() || includeStayResidentAssets) &&
					(asset->GetIsEvictable() || includeNonEvictableAssets))
				{
					logger->Write("Cleaning up asset '%s'.", asset->GetFilePath());

					SafeDeleteAndNull(assetStore[assetIndex]);
					assetStore.UnoccupyIndex(assetIndex);
				}
			}
		}
	}
}

int AssetManager::FindAssetLoadJobIndex(const char* filePath)
{
	int index = -1;

	for (int i = 0; i < this->assetLoadJobs.GetLength(); i++)
	{
		AssetLoadJob* assetLoadJob = &this->assetLoadJobs[i];
		if (strcmp(assetLoadJob->filePath, filePath) == 0)
		{
			index = i;
			break;
		}
	}

	return index;
}

void AssetManager::MarkAllAssetsAsEvictable()
{
	for (int assetStoreIndex = 0; assetStoreIndex < AssetManagerNumberOfStores; assetStoreIndex++)
	{
		DynamicLengthSlottedArray<IAsset*>& assetStore = this->assetStores[assetStoreIndex];

		for (int assetIndex = 0; assetIndex < assetStore.GetNumberOfIndexes(); assetIndex++)
		{
			if (assetStore.GetIndexIsOccupied(assetIndex))
			{
				IAsset* asset = assetStore[assetIndex];
				if (asset != null && !asset->GetIsStayResident())
				{
					asset->SetIsEvictable(true);
				}
			}
		}
	}
}