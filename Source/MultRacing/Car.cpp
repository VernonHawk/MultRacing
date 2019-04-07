// Made by Igor Morenets; April 2019

#include "Car.h"
#include "Components/InputComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "UnrealNetwork.h"

#pragma region Public
// Sets default values
ACar::ACar() : 
	_Movement { CreateDefaultSubobject<UCarMovementComponent>(TEXT("MovementComponent")) },
	_MovementReplication {
		CreateDefaultSubobject<UCarMovementReplicationComponent>(TEXT("MovementReplicationComponent"))
	}
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void ACar::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority()) // is a server
	{
		NetUpdateFrequency = 5;
	}
}

// Called every frame
void ACar::Tick(float const DeltaTime)
{
	Super::Tick(DeltaTime);
}
#pragma endregion

#pragma region Protected
// Called to bind functionality to input
void ACar::SetupPlayerInputComponent(UInputComponent* const PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACar::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACar::MoveRight);
}
#pragma endregion

#pragma region Private
void ACar::MoveForward(float const Value)
{
	if (!_Movement) return;

	_Movement->SetThrottle(Value);
}

void ACar::MoveRight(float const Value)
{
	if (!_Movement) return;

	_Movement->SetSteeringThrow(Value);
}
#pragma endregion
