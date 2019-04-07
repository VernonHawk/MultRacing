// Made by Igor Morenets; April 2019

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CarMovementReplicationComponent.generated.h"


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

private:

		
};
