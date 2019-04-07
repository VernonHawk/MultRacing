// Made by Igor Morenets; April 2019

#include "CarMovementReplicationComponent.h"

// Sets default values for this component's properties
UCarMovementReplicationComponent::UCarMovementReplicationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.
	PrimaryComponentTick.bCanEverTick = true;
	
}

// Called every frame
void UCarMovementReplicationComponent::TickComponent(
	float const DeltaTime,
	ELevelTick const TickType,
	FActorComponentTickFunction* const ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// Called when the game starts
void UCarMovementReplicationComponent::BeginPlay()
{
	Super::BeginPlay();
}
