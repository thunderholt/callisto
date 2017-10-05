#pragma once

#include "Include/Callisto.h"

class Volume : public IVolume
{
public:
	Volume();
	virtual ~Volume();
	virtual void Populate(VolumeSpec* spec, VolumeData* volumeData);
	virtual VolumeSpec* GetSpec();
	virtual VolumeBuffers* GetBuffers();
	virtual ICollisionMesh* GetCollisionMesh();
	virtual int GetNumberOfFaces();

private:
	VolumeSpec spec;
	VolumeBuffers buffers;
	ICollisionMesh* collisionMesh;
	int numberOfFaces;
};