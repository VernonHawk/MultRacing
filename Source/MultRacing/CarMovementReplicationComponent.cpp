// Made by Igor Morenets; April 2019

#include "CarMovementReplicationComponent.h"
#include "GameFramework/Actor.h"
#include "UnrealNetwork.h"

#pragma region Public
// Sets default values for this component's properties
UCarMovementReplicationComponent::UCarMovementReplicationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
}

// Called every frame
void UCarMovementReplicationComponent::TickComponent(
	float const DeltaTime,
	ELevelTick const TickType,
	FActorComponentTickFunction* const ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!_OwnerMovement) return;

	auto const LastMove { _OwnerMovement->GetLastMove() };

	if (GetOwnerRole() == ROLE_AutonomousProxy) // Local player
	{
		_UnackedMoves.AddTail(LastMove);
		Server_SendMove(LastMove);
	} 
	else if (GetOwnerRole() == ROLE_SimulatedProxy) // Simulated player
	{
		_OwnerMovement->SimulateMove(_ServerState.LastMove);
	} 
	else if (GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy) // Listen server
	{
		UpdateServerState(LastMove);
	}
}
#pragma endregion

#pragma region Protected
// Called when the game starts
void UCarMovementReplicationComponent::BeginPlay()
{
	Super::BeginPlay();

	_OwnerMovement = GetOwner()->FindComponentByClass<UCarMovementComponent>();
}

void UCarMovementReplicationComponent::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty, FDefaultAllocator>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCarMovementReplicationComponent, _ServerState)
}
#pragma endregion 

#pragma region Private
void UCarMovementReplicationComponent::UpdateServerState(FCarMove const& Move)
{
	_ServerState.LastMove  = Move;
	_ServerState.Velocity  = _OwnerMovement->GetVelocity();
	_ServerState.Transform = GetOwner()->GetActorTransform();
}

void UCarMovementReplicationComponent::OnRep_ServerState()
{
	if (!_OwnerMovement) return;

	// Accept server state
	GetOwner()->SetActorTransform(_ServerState.Transform);
	_OwnerMovement->SetVelocity(_ServerState.Velocity);

	// Remove acknowledged (old) moves
	while (_UnackedMoves.Num() > 0 && _UnackedMoves.GetHead()->GetValue().Time <= _ServerState.LastMove.Time)
	{
		_UnackedMoves.RemoveNode(_UnackedMoves.GetHead(), true);
	}

	// Simulate remaining moves
	for (auto const& Move : _UnackedMoves)
	{
		_OwnerMovement->SimulateMove(Move);
	}
}

void UCarMovementReplicationComponent::Server_SendMove_Implementation(FCarMove const& Move)
{
	if (!_OwnerMovement) return;

	_OwnerMovement->SimulateMove(Move);

	UpdateServerState(Move);
}

bool UCarMovementReplicationComponent::Server_SendMove_Validate(FCarMove const& Move) const
{
	auto const ValidTransform { FMath::Abs(Move.SteeringThrow) <= 1.f && FMath::Abs(Move.Throttle) <= 1.f };
	auto const ValidTime { Move.DeltaTime > 0.f && Move.Time >= 0.f };

	return ValidTransform && ValidTime;
}
#pragma endregion
