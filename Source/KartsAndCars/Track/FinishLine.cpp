// Fill out your copyright notice in the Description page of Project Settings.


#include "FinishLine.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "KartsAndCars/Kart/KartPawnBase.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AFinishLine::AFinishLine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = RootComp;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(RootComp);
	CollisionBox->SetRelativeScale3D(FVector(1.0f, 50.0f, 0.1f)); // Adjust the scale as needed

	FinishLineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FinishLineMesh"));
	FinishLineMesh->SetupAttachment(RootComp);


}

// Called when the game starts or when spawned
void AFinishLine::BeginPlay()
{
	Super::BeginPlay();
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AFinishLine::OnComponentBeginOverlap);
	
}

void AFinishLine::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AKartPawnBase* PlayerKart = Cast<AKartPawnBase>(OtherActor);
	if (PlayerKart)
	{
		if (PlayerKart->GetTrackProgress())
		{
			float Value = LapCount + 1;
			LapCount = FMath::Clamp(Value, 0, 4); // Ensure LapCount does not go below 0
		}
		else
		{
			float Value = LapCount - 1;
			LapCount = FMath::Clamp(Value, 0, 4); // Ensure LapCount does not go below 0

		}
	}
}
// Called every frame
void AFinishLine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

