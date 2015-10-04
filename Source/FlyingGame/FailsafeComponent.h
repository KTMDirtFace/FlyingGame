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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Failsafe|Collision")
	/** Enable/Disable the failsafe features */
	bool bEnableFailsafe;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Failsafe|Collision")
	/** How many seconds we should look back in time for collision hits */
	float CollisionSecondsBack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Failsafe|Collision")
	/** If there is more than this amount of collisions in CollisionSecondsBack count it as grinding */
	int32 CollisionGrindLimitCount;

	// Collision		//
	/** See if the actor is grinding into collision */
	UFUNCTION(BlueprintCallable, Category = "Failsafe|Collision")
	bool IsGridingOnCollision() const;
private:
	// Returns the number of collision hits that happend in the last "secondsBack" amount of time
	int GetNumHits(float secondsBack) const;

private:
	// Speed			//
	float	m_targetSpeed;								// Speed the vehicle is currently trying to get to.
	float	m_actualSpeed;								// actual speed of the vehicle

	// Collision		//
	float	m_timeSinceLastCollision;					// How long ago did it hit something
	TCircularBuffer<float>	*m_collisionHistoryBuffer;	// Each entry will be a GWorldTimestamp when a collision happend.
	uint32					m_collisionHistoryIndex;
	
};
