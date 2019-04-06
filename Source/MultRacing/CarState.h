// Made by Igor Morenets; April 2019

#pragma once

#include "CoreMinimal.h"
#include "CarMove.h"
#include "CarState.generated.h"

USTRUCT()
struct FCarState
{
	GENERATED_BODY()

	UPROPERTY()
	FCarMove LastMove {};

	UPROPERTY()
	FVector Velocity { 0.f };

	UPROPERTY()
	FTransform Transform {};
};
