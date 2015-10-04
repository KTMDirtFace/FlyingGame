// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "FlyingGame.h"
#include "FlyingGamePawn.h"
#include "DrawDebugHelpers.h"
#include "FailsafeComponent.h"

AFlyingGamePawn::AFlyingGamePawn()
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Flying/Meshes/UFO.UFO"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create static mesh component
	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh0"));
	PlaneMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	RootComponent = PlaneMesh;

	// Create a spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->AttachTo(RootComponent);
	SpringArm->TargetArmLength = 160.0f; // The camera follows at this distance behind the character	
	SpringArm->SocketOffset = FVector(0.f,0.f,60.f);
	SpringArm->bEnableCameraLag = false;
	SpringArm->CameraLagSpeed = 15.f;

	// Create camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->AttachTo(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false; // Don't rotate camera with controller

	// Create FailsafeComponent.
	FailsafeComponent = CreateDefaultSubobject<UFailsafeComponent>(TEXT("FailsafeComponent0"));

	// Set handling parameters
	Acceleration = 500.f;
	TurnSpeed = 50.f;
	MaxSpeed = 4000.f;
	MinSpeed = 0;
	CurrentForwardSpeed = 0;
	CurrentThrottle = 0;

	DebugDraw = false;
}

void AFlyingGamePawn::Tick(float DeltaSeconds)
{
	const FVector LocalMove = FVector(CurrentForwardSpeed * DeltaSeconds, 0.f, 0.f);
							  // FVector(0.0f, 0.0f, CurrentThrottle * DeltaSeconds);

	// Move plan forwards (with sweep so we stop when we collide with things)
	AddActorLocalOffset(LocalMove, true);

	// Calculate change in rotation this frame
	FRotator DeltaRotation(0,0,0);
	DeltaRotation.Pitch = CurrentPitchSpeed * DeltaSeconds;
	DeltaRotation.Yaw = CurrentYawSpeed * DeltaSeconds;
	DeltaRotation.Roll = CurrentRollSpeed * DeltaSeconds;

	// Rotate plane
	AddActorLocalRotation(DeltaRotation);

	// Debug Draw Test Stuff
	if (DebugDraw)
	{
		FVector upVec = this->GetActorUpVector();
		FVector myLoc = this->GetActorLocation();
		FVector endLoc = myLoc + (upVec * 1000);
		// Is It rendering behind because of the spring arm?

//		DrawDebugDirectionalArrow(GWorld, myLoc, endLoc , 20, FColor::Red, false, -1.0f, 0, 8.0f);
	}

	// Call any parent class Tick implementation
	Super::Tick(DeltaSeconds);
}

void AFlyingGamePawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// Set velocity to zero upon collision
	CurrentForwardSpeed = 0.f;

	if (FailsafeComponent)
	{
		FailsafeComponent->NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	}
}


void AFlyingGamePawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	check(InputComponent);

	// Bind controls to functions
	InputComponent->BindAxis("Pitch", this, &AFlyingGamePawn::PitchInput);
	InputComponent->BindAxis("Roll", this, &AFlyingGamePawn::RollInput);
	InputComponent->BindAxis("Yaw", this, &AFlyingGamePawn::YawInput);
	InputComponent->BindAxis("Throttle", this, &AFlyingGamePawn::ThrottleInput);
}

void AFlyingGamePawn::PitchInput(float Val)
{
	CurrentPitchSpeed = Val * TurnSpeed;

/* Old from template
// Target pitch speed is based in input
float TargetPitchSpeed = (Val * TurnSpeed * -1.f);

// When steering, we decrease pitch slightly
TargetPitchSpeed += (FMath::Abs(CurrentYawSpeed) * -0.2f);

// Smoothly interpolate to target pitch speed
CurrentPitchSpeed = FMath::FInterpTo(CurrentPitchSpeed, TargetPitchSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
*/
}

void AFlyingGamePawn::RollInput(float Val)
{
	CurrentRollSpeed = Val * TurnSpeed;

/* old from template
// Target yaw speed is based on input
float TargetYawSpeed = (Val * TurnSpeed);

// Smoothly interpolate to target yaw speed
CurrentYawSpeed = FMath::FInterpTo(CurrentYawSpeed, TargetYawSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

// Is there any left/right input?
const bool bIsTurning = FMath::Abs(Val) > 0.2f;

// If turning, yaw value is used to influence roll
// If not turning, roll to reverse current roll value
float TargetRollSpeed = bIsTurning ? (CurrentYawSpeed * 0.5f) : (GetActorRotation().Roll * -2.f);

// Smoothly interpolate roll speed
CurrentRollSpeed = FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
*/
}

void AFlyingGamePawn::YawInput(float Val)
{
	CurrentYawSpeed = Val * TurnSpeed;
}

void AFlyingGamePawn::ThrottleInput(float Val)
{
	//  Doing some testing here.
	UPrimitiveComponent *mesh = Cast<UPrimitiveComponent>(this->GetComponentByClass(UPrimitiveComponent::StaticClass()));

	if (mesh)
	{
		FVector centerOfMass = mesh->GetCenterOfMass();

		FVector force = FVector(0.f, 0.f, Val * 100000 * 4);
		FTransform myTransform = this->GetTransform();
		// transform force into the right space
		force = myTransform.TransformFVector4(force);

		if (Val > 0 )
		{
			mesh->AddForceAtLocation(force, centerOfMass);

			if (DebugDraw)
			{
				// draw the force
 				DrawDebugDirectionalArrow(GWorld, centerOfMass, centerOfMass + force, 20, FColor::Blue, false, -1.0f, 0, 3.0f );
				DrawDebugSphere(GWorld, centerOfMass, 50, 10, FColor::Yellow);
			}
		}

	}
//	USkeletalMeshComponent* SkeletalMeshComp = Cast<USkeletalMeshComponent>(Actor->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	/*
	// Is there no input?
	bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);
	// If input is not held down, reduce speed
	float CurrentAcc = bHasInput ? (Val * Acceleration) : (-0.5f * Acceleration);
	// Calculate new speed
	float NewForwardSpeed = CurrentForwardSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);
	// Clamp between MinSpeed and MaxSpeed
	CurrentForwardSpeed = FMath::Clamp(NewForwardSpeed, MinSpeed, MaxSpeed);
	*/
}