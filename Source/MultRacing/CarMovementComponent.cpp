// Made by Igor Morenets; April 2019

#include "CarMovementComponent.h"
#include "CarMove.h"
#include "Engine/World.h"

#pragma region Public
// Sets default values for this component's properties
UCarMovementComponent::UCarMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called every frame
void UCarMovementComponent::TickComponent(
	float const DeltaTime,
	ELevelTick const TickType,
	FActorComponentTickFunction* const ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
#pragma endregion

#pragma region Protected
// Called when the game starts
void UCarMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}
#pragma endregion 

#pragma region Private
void UCarMovementComponent::SimulateMove(FCarMove const& Move)
{
	auto const Force { GetOwner()->GetActorForwardVector() * (_MaxDrivingForce * Move.Throttle) };

	SetVelocity(GetVelocity() + (Force + CalculateResistance()) / _Mass * Move.DeltaTime);

	UpdateRotation(Move.DeltaTime, Move.SteeringThrow);
	UpdateLocation(Move.DeltaTime);
}

auto UCarMovementComponent::CalculateResistance() const -> FVector
{
	auto const VelocityDirection { GetVelocity().GetSafeNormal() };

	auto const AirResistance { VelocityDirection * (-GetVelocity().SizeSquared() * _DragCoefficient) };

	auto const GravityAcceleration { GetWorld()->GetGravityZ() / -100 }; // convert negative cm to m
	auto const NormalForce { GravityAcceleration * _Mass };
	auto const RollingResistance { VelocityDirection * -_RollingResistanceCoefficient * NormalForce };

	return AirResistance + RollingResistance;
}

void UCarMovementComponent::UpdateLocation(float const DeltaTime)
{
	auto const Translation { GetVelocity() * (DeltaTime * 100) }; // convert centimeters to meters

	auto HitResult = FHitResult {};
	GetOwner()->AddActorWorldOffset(Translation, true, &HitResult);

	if (HitResult.IsValidBlockingHit())
		SetVelocity(GetVelocity() * 0.1f);
}

void UCarMovementComponent::UpdateRotation(float const DeltaTime, float const SteeringThrow)
{
	auto const DeltaLocation { 
		FVector::DotProduct(GetVelocity(), GetOwner()->GetActorForwardVector())
	};
	auto const Angle { DeltaLocation / _MinTurningRadius * SteeringThrow * DeltaTime };
	auto const DeltaRotation = FQuat { GetOwner()->GetActorUpVector(), Angle };

	SetVelocity(DeltaRotation.RotateVector(GetVelocity()));

	GetOwner()->AddActorLocalRotation(DeltaRotation);
}
#pragma endregion 