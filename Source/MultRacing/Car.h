// Made by Igor Morenets; April 2019

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CarState.h"
#include "List.h"
#include "Car.generated.h"

UCLASS()
class MULTRACING_API ACar : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACar();

	// Called every frame
	void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

	void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty, FDefaultAllocator>& OutLifetimeProps
	) const override;

private:
	#pragma region General Car Attributes
	// The mass of the car (kg)
	UPROPERTY(EditAnywhere)
	float mMass { 1000.f };

	// Higher means more drag
	UPROPERTY(EditAnywhere)
	float mDragCoefficient { 16.f };

	// Higher means more rolling resistance
	UPROPERTY(EditAnywhere)
	float mRollingResistanceCoefficient { 0.015f };

	// The force applied to the car when the throttle is fully down (N)
	UPROPERTY(EditAnywhere)
	float mMaxDrivingForce { 10000.f };

	// Minimum radius of the car turning circle at full lock (m)
	UPROPERTY(EditAnywhere)
	float mMinTurningRadius { 10.f };
	#pragma endregion 

	FVector mVelocity { 0.f };
	float mThrottle { 0.f };
	float mSteeringThrow { 0.f };

	TDoubleLinkedList<FCarMove> mUnackedMoves {};

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FCarState mServerState {};

	UFUNCTION()
	void OnRep_ServerState();

	void SimulateMove(FCarMove const& Move);

	auto CalculateResistance() const -> FVector;

	void UpdateLocation(float DeltaTime);
	void UpdateRotation(float DeltaTime, float SteeringThrow);

	void MoveForward(float Value);
	void MoveRight(float Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FCarMove const& Move);
	void Server_SendMove_Implementation(FCarMove const& Move);
	bool Server_SendMove_Validate(FCarMove const& Move) const;
};
