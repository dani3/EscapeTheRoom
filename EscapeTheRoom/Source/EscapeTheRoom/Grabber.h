#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/InputComponent.h"
#include "Grabber.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ESCAPETHEROOM_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabber();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	float Reach = 100.f;

	UPhysicsHandleComponent * PhysicsHandle = nullptr;

	UInputComponent * InputComponent = nullptr;	

	// Ray-cast and grab what's in reach
	void Grab();
	// Release what has been grabbed
	void Release();
	// Find attached physics handle component
	void FindPhysicsHandleComponent();
	// Setup attached input component
	void SetupInputComponent();
	// Return hit for first physics body in reach
	FHitResult GetFirstPhysicsBodyInReach() const;
	// Return current start of reach line
	FVector GetReachLineStart() const;
	// Return current end of reach line
	FVector GetReachLineEnd() const;
};
