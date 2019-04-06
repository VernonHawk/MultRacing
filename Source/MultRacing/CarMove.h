// Made by Igor Morenets; April 2019

#pragma once

#include "CoreMinimal.h"
#include "CarMove.generated.h"

USTRUCT()
struct FCarMove
{
	GENERATED_BODY()

	UPROPERTY()
	float Throttle { 0.f };

	UPROPERTY()
	float SteeringThrow { 0.f };

	// Frame time
	UPROPERTY()
	float DeltaTime { 0.f };

	// Move start time
	UPROPERTY()
	float Time { 0.f };
};
