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

	if  (HasAuthority()) // is a server
	{
		NetUpdateFrequency = 10;
	}
}

// Called every frame
void ACar::Tick(float const DeltaTime)
{
	Super::Tick(DeltaTime);

	auto const Force {GetActorForwardVector() * (mMaxDrivingForce * mThrottle)};

	mVelocity += (Force + CalculateResistance()) / mMass * DeltaTime;

	UpdateRotation(DeltaTime); 
	UpdateLocation(DeltaTime);
	
	if (HasAuthority()) // is a server
	{
		mReplicatedTransform = GetActorTransform();
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

	DOREPLIFETIME(ACar, mReplicatedTransform)
	DOREPLIFETIME(ACar, mVelocity)
	DOREPLIFETIME(ACar, mThrottle)
	DOREPLIFETIME(ACar, mSteeringThrow)
}
#pragma endregion

#pragma region Private
void ACar::OnRep_Transform()
{
	SetActorTransform(mReplicatedTransform);
}

auto ACar::CalculateResistance() const -> FVector
{
	auto const VelocityDirection {mVelocity.GetSafeNormal()};
	auto const AirResistance {VelocityDirection * (-mVelocity.SizeSquared() * mDragCoefficient)};

	auto const GravityAcceleration {GetWorld()->GetGravityZ() / -100}; // convert negative cm to m
	auto const NormalForce {GravityAcceleration * mMass};
	auto const RollingResistance {VelocityDirection * -mRollingResistanceCoefficient * NormalForce};

	return AirResistance + RollingResistance;
}

void ACar::UpdateLocation(float const DeltaTime)
{
	auto const Translation {mVelocity * (DeltaTime * 100)}; // convert centimeters to meters

	auto HitResult = FHitResult {};
	AddActorWorldOffset(Translation, true, &HitResult);

	if (HitResult.IsValidBlockingHit())
		mVelocity *= 0.1f;
}

void ACar::UpdateRotation(float const DeltaTime)
{
	auto const DeltaLocation {FVector::DotProduct(mVelocity, GetActorForwardVector()) * DeltaTime};
	auto const Angle {DeltaLocation / mMinTurningRadius * mSteeringThrow};
	auto const Delta = FQuat {GetActorUpVector(), Angle};

	mVelocity = Delta.RotateVector(mVelocity);

	AddActorLocalRotation(Delta);
}

#pragma region Move Forward
void ACar::MoveForward(float const Value)
{
	mThrottle = Value;

	Server_MoveForward(Value);
}

void ACar::Server_MoveForward_Implementation(float const Value)
{
	mThrottle = Value;
}

bool ACar::Server_MoveForward_Validate(float const Value) const
{
	return FMath::Abs(Value) <= 1.f;
}
#pragma endregion

#pragma region Move Right
void ACar::MoveRight(float const Value)
{
	mSteeringThrow = Value;

	Server_MoveRight(Value);
}

void ACar::Server_MoveRight_Implementation(float const Value)
{
	mSteeringThrow = Value;
}

bool ACar::Server_MoveRight_Validate(float const Value) const
{
	return FMath::Abs(Value) <= 1.f;
}
#pragma endregion
#pragma endregion

#pragma region Free functions
FString GetNetRoleName(ENetRole const Role)
{
	switch (Role)
	{
		case ROLE_None:
			return TEXT("None");
		case ROLE_SimulatedProxy:
			return TEXT("SimulatedProxy");
		case ROLE_AutonomousProxy:
			return TEXT("AutonomousProxy");
		case ROLE_Authority:
			return TEXT("Authority");
		default:
			return TEXT("ERROR");
	}
}
#pragma endregion 