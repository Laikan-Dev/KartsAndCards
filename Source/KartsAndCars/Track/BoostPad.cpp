// Fill out your copyright notice in the Description page of Project Settings.


#include "BoostPad.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "KartsAndCars/Kart/KartPawnBase.h"

// Sets default values
ABoostPad::ABoostPad()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create and set up the root component
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = RootComp;

	// Create and set up the collision box component
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(RootComp);

	BoostPadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoostPadMesh"));
	BoostPadMesh->SetupAttachment(RootComp);

}

// Called when the game starts or when spawned
void ABoostPad::BeginPlay()
{
	Super::BeginPlay();

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ABoostPad::OnOverlapBegin);
	
}

void ABoostPad::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("BoostPad: Overlap Begin!"));
	AKartPawnBase* KartPawn = Cast<AKartPawnBase>(OtherActor);
	if (KartPawn && KartPawn->bIsOnTheGround())
	{
		KartPawn->BoostKart(BoostSpeedModifier, BoostDuration);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("BoostPad: Kart boosted!"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BoostPad: Kart is not on the ground!"));
	}
}

// Called every frame
void ABoostPad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

