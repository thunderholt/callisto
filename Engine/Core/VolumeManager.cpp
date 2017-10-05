#include "Core/VolumeManager.h"

VolumeManager::~VolumeManager()
{
	this->DeleteAllVolumes();
}

int VolumeManager::CreateVolume(VolumeSpec* spec)
{
	IFactory* factory = GetFactory();

	VolumeData volumeData;
	this->BuildCylinderData(&volumeData, &spec->cylinderSpec);

	IVolume* volume = factory->MakeVolume();
	volume->Populate(spec, &volumeData);

	delete[] volumeData.positions;
	delete[] volumeData.indecies;

	int volumeIndex = this->volumes.FindAndOccupyAvailableIndex();
	this->volumes[volumeIndex] = volume;

	return volumeIndex;
}

IVolume* VolumeManager::GetVolume(int volumeIndex)
{
	return this->volumes[volumeIndex];
}

void VolumeManager::DeleteVolume(int volumeIndex)
{
	IVolume* volume = this->volumes[volumeIndex];
	if (volume != null)
	{
		delete volume;
		this->volumes[volumeIndex] = null;
	}

	this->volumes.UnoccupyIndex(volumeIndex);
}

/*void VolumeManager::Reset()
{
	this->DeleteAllVolumes();
}*/

void VolumeManager::BuildCylinderData(VolumeData* out, CylinderVolumeSpec* cylinderSpec)
{
	int numberOfPositions = cylinderSpec->numberOfSides * 2;
	Vec3* positions = new Vec3[numberOfPositions];

	// Safety check for the static analyser.
	if (cylinderSpec->numberOfSides < 2)
	{
		cylinderSpec->numberOfSides = 2;
	}

	int numberOfFaces =
		cylinderSpec->numberOfSides * 2 // The sides
		+ cylinderSpec->numberOfSides - 2 // The top cap
		+ cylinderSpec->numberOfSides - 2; // The bottom cap

	int numberOfIndecies = numberOfFaces * 3;
	unsigned short* indecies = new unsigned short[numberOfIndecies];

	float angleStride = PI * 2.0f / cylinderSpec->numberOfSides;
	Vec3 direction;

	// Calculate the positions for the top ring.
	for (unsigned short sideIndex = 0; sideIndex < cylinderSpec->numberOfSides; sideIndex++)
	{
		float angle = angleStride * sideIndex;
		Vec3::Set(&direction, sinf(angle), 1.0f, cosf(angle));
		Vec3::Mul(&positions[sideIndex], &direction, &cylinderSpec->size);
	}

	// Calculate the positions for the bottom ring.
	for (unsigned short sideIndex = 0; sideIndex < cylinderSpec->numberOfSides; sideIndex++)
	{
		float angle = angleStride * sideIndex;
		Vec3::Set(&direction, sinf(angle), 0.0f, cosf(angle));
		Vec3::Mul(&positions[cylinderSpec->numberOfSides + sideIndex], &direction, &cylinderSpec->size);
	}

	// Calculate the indecies for the sides.
	int i = 0;
	for (unsigned short sideIndex = 0; sideIndex < cylinderSpec->numberOfSides; sideIndex++)
	{
		int nextSideIndex = sideIndex < cylinderSpec->numberOfSides - 1 ? sideIndex + 1 : 0;

		// Face 1.
		indecies[i++] = sideIndex;
		indecies[i++] = cylinderSpec->numberOfSides + nextSideIndex;
		indecies[i++] = nextSideIndex;

		// Face 2.
		indecies[i++] = sideIndex;
		indecies[i++] = cylinderSpec->numberOfSides + sideIndex;
		indecies[i++] = cylinderSpec->numberOfSides + nextSideIndex;
	}

	// Calculate the indecies for the top cap.
	for (unsigned short faceIndex = 0; faceIndex < cylinderSpec->numberOfSides - 2; faceIndex++)
	{
		indecies[i++] = 0;
		indecies[i++] = faceIndex + 1;
		indecies[i++] = faceIndex + 2;
	}

	// Calculate the indecies for the bottom cap.
	for (unsigned short faceIndex = 0; faceIndex < cylinderSpec->numberOfSides - 2; faceIndex++)
	{
		indecies[i++] = cylinderSpec->numberOfSides;
		indecies[i++] = cylinderSpec->numberOfSides + faceIndex + 2;
		indecies[i++] = cylinderSpec->numberOfSides + faceIndex + 1;
	}

	// Copy values to output.
	out->positions = positions;
	out->numberOfPositions = numberOfPositions;
	out->indecies = indecies;
	out->numberOfIndecies = numberOfIndecies;
	out->numberOfFaces = numberOfFaces;
}

void VolumeManager::DeleteAllVolumes()
{
	for (int volumeIndex = 0; volumeIndex < this->volumes.GetNumberOfIndexes(); volumeIndex++)
	{
		IVolume* volume = this->volumes[volumeIndex];
		if (volume != null)
		{
			delete volume;
		}
	}

	this->volumes.UnoccupyAllIndexes();
}