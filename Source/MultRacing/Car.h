// Made by Igor Morenets; April 2019

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CarMovementComponent.h"
#include "CarMovementReplicationComponent.h"
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

private:
	UPROPERTY(VisibleAnywhere)
	UCarMovementComponent* const _Movement { nullptr };

	UPROPERTY(VisibleAnywhere)
	UCarMovementReplicationComponent* const _MovementReplication { nullptr };

	void MoveForward(float Value);
	void MoveRight(float Value);
};
