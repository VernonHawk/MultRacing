// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "MultRacingGameMode.h"
#include "MultRacingPawn.h"
#include "MultRacingHud.h"

AMultRacingGameMode::AMultRacingGameMode()
{
	DefaultPawnClass = AMultRacingPawn::StaticClass();
	HUDClass = AMultRacingHud::StaticClass();
}
