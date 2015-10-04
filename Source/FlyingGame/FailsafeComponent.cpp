// Fill out your copyright notice in the Description page of Project Settings.

#include "FlyingGame.h"
#include "FailsafeComponent.h"


#pragma optimize("", off)

// Sets default values for this component's properties
UFailsafeComponent::UFailsafeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	m_targetSpeed = 0.0f;
	m_actualSpeed = 0.0f;

	m_timeSinceLastCollision = 0.0f;

	m_collisionHistoryBuffer = new TCircularBuffer<float>(128); // size gets rounded up power of 2.. 8, 16, 32, 128, 256 ..etc
	m_collisionHistoryIndex = 0;
}

void UFailsafeComponent::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	// Reset the collision hit timer
	m_timeSinceLastCollision = 0.0f;

	// update the Circular History Buffer
	TCircularBuffer<float> &historyBuffer = *m_collisionHistoryBuffer;
	historyBuffer[m_collisionHistoryIndex] = GWorld->GetTimeSeconds();
	m_collisionHistoryIndex = historyBuffer.GetNextIndex(m_collisionHistoryIndex);
}

// Called when the game starts
void UFailsafeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UFailsafeComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...

	// Update Time Since Last Collision Counter
	m_timeSinceLastCollision += DeltaTime;
}

#pragma optimize("", on)