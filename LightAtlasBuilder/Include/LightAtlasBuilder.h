#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "Include/Common.h"
#include "Include/Types.h"
#include "Include/Interfaces.h"
#include "Util/Util.h"

void CreateFactory();
void DestroyFactory();
IFactory* GetFactory();

void CreateEngine();
void DestroyEngine();
IEngine* GetEngine();