// Made by Igor Morenets; April 2019

#include "Car.h"
#include "Components/InputComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "UnrealNetwork.h"

#pragma region Public
// Sets default values
ACar::ACar() : 
	_MovementComponent { 
		CreateDefaultSubobject<UCarMovementComponent>(TEXT("MovementComponent"))
	},
	_MovementReplicationComponent {
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

	if (!_MovementComponent) return;

	auto const Move = FCarMove { 
		_MovementComponent->GetThrottle(), 
		_MovementComponent->GetSteeringThrow(), 
		DeltaTime, 
		GetWorld()->TimeSeconds
	};

	if (Role == ROLE_SimulatedProxy) // is another player
	{
		_MovementComponent->SimulateMove(_ServerState.LastMove);
		return;
	}

	if (!IsLocallyControlled()) // is a dedicated server
	{
		_MovementComponent->SimulateMove(Move);
		return;
	}

	Server_SendMove(Move);

	if (!HasAuthority()) // is not a server
	{
		_UnackedMoves.AddTail(Move);
		_MovementComponent->SimulateMove(Move);
	}
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

void ACar::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty, FDefaultAllocator>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACar, _ServerState)
}
#pragma endregion

#pragma region Private
void ACar::OnRep_ServerState()
{
	if (!_MovementComponent) return;

	// Accept server state
	SetActorTransform(_ServerState.Transform);
	_MovementComponent->SetVelocity(_ServerState.Velocity);

	// Remove acknowledged (old) moves
	while (_UnackedMoves.Num() > 0 && _UnackedMoves.GetHead()->GetValue().Time <= _ServerState.LastMove.Time)
	{
		_UnackedMoves.RemoveNode(_UnackedMoves.GetHead(), true);
	}

	// Simulate remaining moves
	for (auto const& Move : _UnackedMoves)
	{
		_MovementComponent->SimulateMove(Move);
	}
}

void ACar::MoveForward(float const Value)
{
	if (!_MovementComponent) return;

	_MovementComponent->SetThrottle(Value);
}

void ACar::MoveRight(float const Value)
{
	if (!_MovementComponent) return;

	_MovementComponent->SetSteeringThrow(Value);
}

void ACar::Server_SendMove_Implementation(FCarMove const& Move)
{
	if (!_MovementComponent) return;

	_MovementComponent->SimulateMove(Move);

	_ServerState.LastMove  = Move;
	_ServerState.Transform = GetActorTransform();
	_ServerState.Velocity  = _MovementComponent->GetVelocity();
}

bool ACar::Server_SendMove_Validate(FCarMove const& Move) const
{
	auto const ValidTransform { FMath::Abs(Move.SteeringThrow) <= 1.f && FMath::Abs(Move.Throttle) <= 1.f };
	auto const ValidTime { Move.DeltaTime > 0.f && Move.Time >= 0.f};

	return ValidTransform && ValidTime;
}
#pragma endregion
