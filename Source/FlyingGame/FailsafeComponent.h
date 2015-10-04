// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "FailsafeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FLYINGGAME_API UFailsafeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFailsafeComponent();

	// The pawn will notify us of a hit
	void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit);

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	// Collision		//

private:
	// Speed			//
	float	m_targetSpeed;				// Speed the vehicle is currently trying to get to.
	float	m_actualSpeed;				// actual speed of the vehicle

	// Collision		//
	float	m_timeSinceLastCollision;	// How long ago did it hit something
	TCircularBuffer<float>	*m_collisionHistoryBuffer;
	uint32					m_collisionHistoryIndex;
	
};
