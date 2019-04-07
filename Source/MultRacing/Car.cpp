// Made by Igor Morenets; April 2019

#include "Car.h"
#include "Components/InputComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "UnrealNetwork.h"

#pragma region Public
// Sets default values
ACar::ACar()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void ACar::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority()) // is a server
	{
		NetUpdateFrequency = 1;
	}
}

// Called every frame
void ACar::Tick(float const DeltaTime)
{
	Super::Tick(DeltaTime);

	auto const Move = FCarMove { mThrottle, mSteeringThrow, DeltaTime, GetWorld()->TimeSeconds };

	if (IsLocallyControlled() && !HasAuthority()) // is the owner and not a server
	{
		Server_SendMove(Move);
		mUnackedMoves.AddTail(Move);
	}

	SimulateMove(Move);
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

	DOREPLIFETIME(ACar, mServerState)
}
#pragma endregion

#pragma region Private
void ACar::OnRep_ServerState()
{
	// Accept server state
	SetActorTransform(mServerState.Transform);
	mVelocity = mServerState.Velocity;

	// Remove acknowledged (old) moves
	while (mUnackedMoves.Num() > 0 && mUnackedMoves.GetHead()->GetValue().Time <= mServerState.LastMove.Time)
	{
		mUnackedMoves.RemoveNode(mUnackedMoves.GetHead(), true);
	}

	// Simulate remaining moves
	for (auto const& Move : mUnackedMoves)
	{
		SimulateMove(Move);
	}
}

void ACar::SimulateMove(FCarMove const& Move)
{
	auto const Force { GetActorForwardVector() * (mMaxDrivingForce * Move.Throttle) };

	mVelocity += (Force + CalculateResistance()) / mMass * Move.DeltaTime;

	UpdateRotation(Move.DeltaTime, Move.SteeringThrow);
	UpdateLocation(Move.DeltaTime);
}

auto ACar::CalculateResistance() const -> FVector
{
	auto const VelocityDirection { mVelocity.GetSafeNormal() };

	auto const AirResistance { VelocityDirection * (-mVelocity.SizeSquared() * mDragCoefficient) };

	auto const GravityAcceleration { GetWorld()->GetGravityZ() / -100 }; // convert negative cm to m
	auto const NormalForce { GravityAcceleration * mMass };
	auto const RollingResistance { VelocityDirection * -mRollingResistanceCoefficient * NormalForce };

	return AirResistance + RollingResistance;
}

void ACar::UpdateLocation(float const DeltaTime)
{
	auto const Translation { mVelocity * (DeltaTime * 100) }; // convert centimeters to meters

	auto HitResult = FHitResult {};
	AddActorWorldOffset(Translation, true, &HitResult);

	if (HitResult.IsValidBlockingHit())
		mVelocity *= 0.1f;
}

void ACar::UpdateRotation(float const DeltaTime, float const SteeringThrow)
{
	auto const DeltaLocation { FVector::DotProduct(mVelocity, GetActorForwardVector()) * DeltaTime };
	auto const Angle { DeltaLocation / mMinTurningRadius * SteeringThrow };
	auto const DeltaRotation = FQuat { GetActorUpVector(), Angle };

	mVelocity = DeltaRotation.RotateVector(mVelocity);

	AddActorLocalRotation(DeltaRotation);
}

void ACar::MoveForward(float const Value)
{
	mThrottle = Value;
}

void ACar::MoveRight(float const Value)
{
	mSteeringThrow = Value;
}

void ACar::Server_SendMove_Implementation(FCarMove const& Move)
{
	SimulateMove(Move);

	mServerState.LastMove  = Move;
	mServerState.Transform = GetActorTransform();
	mServerState.Velocity  = mVelocity;
}

bool ACar::Server_SendMove_Validate(FCarMove const& Move) const
{
	auto const ValidTransform { FMath::Abs(Move.SteeringThrow) <= 1.f && FMath::Abs(Move.Throttle) <= 1.f };
	auto const ValidTime { Move.DeltaTime > 0.f && Move.Time >= 0.f};

	return ValidTransform && ValidTime;
}
#pragma endregion
