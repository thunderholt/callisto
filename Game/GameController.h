#pragma once

#include "Include/Callisto.h"

class GameController : public IGameController
{
public:
	virtual ~GameController();
	virtual int ParseActorType(const char* actorTypeString);
};