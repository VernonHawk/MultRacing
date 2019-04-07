// Made by Igor Morenets; April 2019

#pragma once

#include "CarMove.h"
#include "Engine/World.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CarMovementComponent.generated.h"

struct FCarMove;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTRACING_API UCarMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCarMovementComponent();

	// Called every frame
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SimulateMove(FCarMove const& Move);

	auto GetLastMove()		const -> FCarMove const& { return _LastMove; }
	auto GetVelocity()		const -> FVector		 { return _Velocity; }
	auto GetThrottle()		const -> float			 { return _Throttle; }
	auto GetSteeringThrow() const -> float			 { return _SteeringThrow; }

	void SetVelocity(FVector const& Velocity)		 { _Velocity	  = Velocity; }
	void SetThrottle(float const Throttle)			 { _Throttle	  = Throttle; }
	void SetSteeringThrow(float const SteeringThrow) { _SteeringThrow = SteeringThrow; }

protected:
	// Called when the game starts
	void BeginPlay() override;

private:
	auto CalculateResistance() const -> FVector;

	void UpdateLocation(float DeltaTime);
	void UpdateRotation(float DeltaTime, float SteeringThrow);

	FCarMove _LastMove {};

	FVector _Velocity { 0.f };
	float _Throttle { 0.f };
	float _SteeringThrow { 0.f };

	#pragma region General Car Attributes
	// The mass of the car (kg)
	UPROPERTY(EditAnywhere)
	float _Mass { 1000.f };

	// Higher means more drag
	UPROPERTY(EditAnywhere)
	float _DragCoefficient { 16.f };

	// Higher means more rolling resistance
	UPROPERTY(EditAnywhere)
	float _RollingResistanceCoefficient { 0.015f };

	// The force applied to the car when the throttle is fully down (N)
	UPROPERTY(EditAnywhere)
	float _MaxDrivingForce { 10000.f };

	// Minimum radius of the car turning circle at full lock (m)
	UPROPERTY(EditAnywhere)
	float _MinTurningRadius { 10.f };
	#pragma endregion
};
