// Made by Igor Morenets; April 2019

#pragma once

#include "CarState.h"
#include "CarMovementComponent.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "List.h"
#include "CarMovementReplicationComponent.generated.h"

struct FCarMove;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTRACING_API UCarMovementReplicationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCarMovementReplicationComponent();

	// Called every frame
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
	// Called when the game starts
	void BeginPlay() override;

	void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty, FDefaultAllocator>& OutLifetimeProps
	) const override;

private:
	void RemoteClientTick(float DeltaTime);
	
	void UpdateServerState(FCarMove const& Move);

	UFUNCTION()
	void OnRep_ServerState();
	void LocalClient_OnRep_ServerState();
	void RemoteClient_OnRep_ServerState();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FCarMove const& Move);
	void Server_SendMove_Implementation(FCarMove const& Move);
	bool Server_SendMove_Validate(FCarMove const& Move) const;

	TDoubleLinkedList<FCarMove> _UnackedMoves {};

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FCarState _ServerState {};

	FTransform _ClientStartTransform {};
	float _ClientTimeSinceUpdate { 0.f };
	float _ClientTimeBetweenLastUpdates { 0.f };

	UPROPERTY(VisibleAnywhere) // non-owning
	UCarMovementComponent* _OwnerMovement { nullptr };
};
