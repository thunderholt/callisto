#pragma once

#include "Include/PVSBuilder.h"

class BruteForceSectorCruncher : public ISectorCruncher
{
public:
	BruteForceSectorCruncher();
	virtual ~BruteForceSectorCruncher();
	virtual void Run(int startSectorIndex, int numberOfSectorsToCrunch);
};
