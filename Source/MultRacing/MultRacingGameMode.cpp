// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "MultRacingGameMode.h"
#include "Car.h"

AMultRacingGameMode::AMultRacingGameMode()
{
	DefaultPawnClass = ACar::StaticClass();
}
