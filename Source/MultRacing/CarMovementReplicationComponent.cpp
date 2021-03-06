// Made by Igor Morenets; April 2019

#include "CarMovementReplicationComponent.h"
#include "HermiteCubicSpline.h"
#include "RoleHelpers.h"
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

	if (!_OwnerMovement)
		return;

	auto const LastMove { _OwnerMovement->GetLastMove() };

	if (IsLocalClient(GetOwner()))
	{
		_UnackedMoves.AddTail(LastMove);
		Server_SendMove(LastMove);
	} 
	else if (IsRemoteClient(GetOwner()))
	{
		RemoteClientTick(DeltaTime);
	} 
	else if (IsListenServer(GetOwner()))
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
void UCarMovementReplicationComponent::RemoteClientTick(float const DeltaTime)
{
	_ClientTimeSinceUpdate += DeltaTime;

	if (_ClientTimeBetweenLastUpdates < KINDA_SMALL_NUMBER)
		return;

	if (!_OwnerMovement)
		return;

	auto const VelocityToDerivative { _ClientTimeBetweenLastUpdates * 100 };

	auto const Spline = FHermiteCubicSpline { 
		_ClientStartTransform.GetLocation(), 
		_ClientStartVelocity * VelocityToDerivative,
		_ServerState.Transform.GetLocation(),
		_ServerState.Velocity * VelocityToDerivative
	};

	auto const LerpRatio { _ClientTimeSinceUpdate / _ClientTimeBetweenLastUpdates };

	_OwnerMovement->SetVelocity(InterpolateDerivative(Spline, LerpRatio) / VelocityToDerivative);

	if (!_MeshOffsetRoot)
		return;

	_MeshOffsetRoot->SetWorldLocation(InterpolateLocation(Spline, LerpRatio));

	auto const NewRotation { 
		FQuat::Slerp(_ClientStartTransform.GetRotation(), _ServerState.Transform.GetRotation(), LerpRatio)
	};
	_MeshOffsetRoot->SetWorldRotation(NewRotation);
}

void UCarMovementReplicationComponent::UpdateServerState(FCarMove const& Move)
{
	_ServerState.LastMove  = Move;
	_ServerState.Velocity  = _OwnerMovement->GetVelocity();
	_ServerState.Transform = GetOwner()->GetActorTransform();
}

void UCarMovementReplicationComponent::OnRep_ServerState()
{
	if (IsLocalClient(GetOwner()))
	{
		LocalClient_OnRep_ServerState();
	}
	else if (IsRemoteClient(GetOwner()))
	{
		RemoteClient_OnRep_ServerState();
	}
}

void UCarMovementReplicationComponent::LocalClient_OnRep_ServerState()
{
	if (!_OwnerMovement)
		return;

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

void UCarMovementReplicationComponent::RemoteClient_OnRep_ServerState()
{
	if (!_OwnerMovement)
		return;

	_ClientTimeBetweenLastUpdates = _ClientTimeSinceUpdate;
	_ClientTimeSinceUpdate		  = 0;

	if (_MeshOffsetRoot)
	{
		_ClientStartTransform.SetLocation(_MeshOffsetRoot->GetComponentLocation());
		_ClientStartTransform.SetRotation(_MeshOffsetRoot->GetComponentQuat());
	}

	_ClientStartVelocity		  = _OwnerMovement->GetVelocity();

	GetOwner()->SetActorTransform(_ServerState.Transform);
}

void UCarMovementReplicationComponent::Server_SendMove_Implementation(FCarMove const& Move)
{
	if (!_OwnerMovement)
		return;

	_ClientSimulatedTime += Move.DeltaTime;
	_OwnerMovement->SimulateMove(Move);

	UpdateServerState(Move);
}

bool UCarMovementReplicationComponent::Server_SendMove_Validate(FCarMove const& Move) const
{
	auto const ValidTransform    { FMath::Abs(Move.SteeringThrow) <= 1.f && FMath::Abs(Move.Throttle) <= 1.f };
	auto const TimeIsPositive    { Move.DeltaTime > 0.f && Move.Time >= 0.f };
	auto const TimeBehindServers { _ClientSimulatedTime + Move.DeltaTime < GetWorld()->TimeSeconds };

	return ValidTransform && TimeIsPositive && TimeBehindServers;
}
#pragma endregion
