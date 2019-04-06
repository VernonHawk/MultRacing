// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Car.generated.h"

UCLASS()
class MULTRACING_API ACar : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACar();

	~ACar() = default;

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
	float mMass {1000.f};

	// Higher means more drag
	UPROPERTY(EditAnywhere)
	float mDragCoefficient {16.f};

	// Higher means more rolling resistance
	UPROPERTY(EditAnywhere)
	float mRollingResistanceCoefficient {0.015f};

	// The force applied to the car when the throttle is fully down (N)
	UPROPERTY(EditAnywhere)
	float mMaxDrivingForce {10000.f};

	// Minimum radius of the car turning circle at full lock (m)
	UPROPERTY(EditAnywhere)
	float mMinTurningRadius {10.f};
	#pragma endregion 

	//UPROPERTY(Replicated)
	FVector mVelocity {0.f};

	//UPROPERTY(Replicated)
	float mThrottle {0.f};

	//UPROPERTY(Replicated)
	float mSteeringThrow {0.f};

	UPROPERTY(ReplicatedUsing = OnRep_Transform)
	FTransform mReplicatedTransform {};

	UFUNCTION()
	void OnRep_Transform();

	auto CalculateResistance() const -> FVector;

	void UpdateLocation(float DeltaTime);
	void UpdateRotation(float DeltaTime);

	#pragma region Move Forward
	void MoveForward(float Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveForward(float Value);
	void Server_MoveForward_Implementation(float Value);
	bool Server_MoveForward_Validate(float Value) const;
	#pragma endregion

	#pragma region Move Right
	void MoveRight(float Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveRight(float Value);
	void Server_MoveRight_Implementation(float Value);
	bool Server_MoveRight_Validate(float Value) const;
	#pragma endregion
};

FString GetNetRoleName(ENetRole Role);
