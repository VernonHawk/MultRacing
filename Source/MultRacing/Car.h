// Made by Igor Morenets; April 2019

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CarState.h"
#include "List.h"
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

	void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty, FDefaultAllocator>& OutLifetimeProps
	) const override;

private:
	UPROPERTY(VisibleAnywhere)
	UCarMovementComponent* const _MovementComponent { nullptr };

	UPROPERTY(VisibleAnywhere)
	UCarMovementReplicationComponent* const _MovementReplicationComponent { nullptr };

	TDoubleLinkedList<FCarMove> _UnackedMoves {};

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FCarState _ServerState {};

	UFUNCTION()
	void OnRep_ServerState();

	void MoveForward(float Value);
	void MoveRight(float Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FCarMove const& Move);
	void Server_SendMove_Implementation(FCarMove const& Move);
	bool Server_SendMove_Validate(FCarMove const& Move) const;
};
