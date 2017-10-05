#pragma once

#include "Include/Callisto.h"

class ActorControllerManager : public IActorControllerManager
{
public:
	virtual ~ActorControllerManager();
	virtual void RegisterActorController(IActorController* actorController);
	virtual int FindActorControllerIndex(const char* actorControllerName);
	virtual IActorController* GetActorController(int index);

private:
	void DeleteAllActorControllers();

	DynamicLengthSlottedArray<IActorController*> actorControllers;
};