#include "Grabber.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "Public/DrawDebugHelpers.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandleComponent();
	SetupInputComponent();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PhysicsHandle) { return; }

	// If the physics handle is attached
	if (PhysicsHandle->GrabbedComponent)
	{
		// Move the object that we're holding
		PhysicsHandle->SetTargetLocation(GetReachLineEnd());
	}
}

// Ray-Cast and grab what's in reach
void UGrabber::Grab()
{
	// Line trace and reach any actors with physics body collision channel set
	auto HitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent();
	auto ActorHit = HitResult.GetActor();

	// If we hit something then attach a physics handle
	if (ActorHit)
	{
		if (!PhysicsHandle) { return; }

		PhysicsHandle->GrabComponent(
			  ComponentToGrab
			, NAME_None
			, ComponentToGrab->GetOwner()->GetActorLocation()
			, true);
	}
}

// Release what has been grabbed
void UGrabber::Release()
{
	if (!PhysicsHandle) { return; }

	PhysicsHandle->ReleaseComponent();
}

// Find attached physics handle component
void UGrabber::FindPhysicsHandleComponent()
{
	// Look for attached Physics Handle
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: Physics handle not found"), *(GetOwner()->GetName()));
	}
}

// Setup attached input component
void UGrabber::SetupInputComponent()
{
	// Look for attached Input Component (appears at run time)
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		// Bind the input component
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s: Input component not found"), *(GetOwner()->GetName()));
	}
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
	// Setup query parameters
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());

	// Line-Trace (AKA ray-cast) out to reach distance
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByObjectType(
		  OUT HitResult
		, GetReachLineStart()
		, GetReachLineEnd()
		, FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody)
		, TraceParameters
	);

	return HitResult;
}

// Return current start of reach line
FVector UGrabber::GetReachLineStart() const
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		  OUT PlayerViewPointLocation
		, OUT PlayerViewPointRotation);

	return PlayerViewPointLocation;
}

// Return current end of reach line
FVector UGrabber::GetReachLineEnd() const
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		  OUT PlayerViewPointLocation
		, OUT PlayerViewPointRotation);

	return PlayerViewPointLocation + (PlayerViewPointRotation.Vector() * Reach);
}

