#pragma once

#include "Include/Callisto.h"

class VolumeManager : public IVolumeManager
{
public:
	virtual ~VolumeManager();
	virtual int CreateVolume(VolumeSpec* spec);
	virtual IVolume* GetVolume(int volumeIndex);
	virtual void DeleteVolume(int volumeIndex);
	//virtual void Reset();

private:
	void BuildCylinderData(VolumeData* out, CylinderVolumeSpec* cylinderSpec);
	void DeleteAllVolumes();

	DynamicLengthSlottedArray<IVolume*> volumes;
};
