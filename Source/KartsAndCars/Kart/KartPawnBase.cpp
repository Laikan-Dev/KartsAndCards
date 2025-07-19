// Fill out your copyright notice in the Description page of Project Settings.


#include "KartPawnBase.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
AKartPawnBase::AKartPawnBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create and set up the root component
	//RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));

	// Set the box collision component
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Block);

	// Set config for the boucing
	CollisionBox->SetSimulatePhysics(true);
	CollisionBox->SetLinearDamping(3.f);
	CollisionBox->SetAngularDamping(5.f);
	RootComponent = CollisionBox;


	// Create and set up the Kart mesh component
	KartMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KartMesh"));
	KartMesh->SetupAttachment(CollisionBox);

	// Set the spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(CollisionBox);
	SpringArm->bUsePawnControlRotation = true; // Allow the spring arm to use the pawn's control rotation

	// Set the camera component
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	// Create wheel components and attach them to the root component
	FLWheel = CreateDefaultSubobject<USceneComponent>(TEXT("FLWheel"));
	FLWheel->SetupAttachment(CollisionBox);
	FRWheel = CreateDefaultSubobject<USceneComponent>(TEXT("FRWheel"));
	FRWheel->SetupAttachment(CollisionBox);
	BLWheel = CreateDefaultSubobject<USceneComponent>(TEXT("BLWheel"));
	BLWheel->SetupAttachment(CollisionBox);
	BRWheel = CreateDefaultSubobject<USceneComponent>(TEXT("BRWheel"));
	BRWheel->SetupAttachment(CollisionBox);
}

// Called when the game starts or when spawned
void AKartPawnBase::BeginPlay()
{
	Super::BeginPlay();

	if (FLWheel && FRWheel && BLWheel && BRWheel)
	{
		Wheels.Add(FLWheel);
		Wheels.Add(FRWheel);
		Wheels.Add(BLWheel);
		Wheels.Add(BRWheel);
	}
}

// Called every frame
void AKartPawnBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (USceneComponent* Wheel : Wheels)
	{
		if (Wheel)
		{
			HandleSuspension(Wheel);
		}
	}
}

// Called to bind functionality to input
void AKartPawnBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AKartPawnBase::HandleSuspension(USceneComponent* WheelComp)
{
	FHitResult HitResult;
	FVector Start = WheelComp->GetComponentLocation();
	FVector End =Start + FVector(0.0, 0.0, -60.0);
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	bool TraceResult = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), Start, End, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, OUT HitResult, true);
	if(TraceResult)
	{
		FVector DirectionResult = UKismetMathLibrary::GetDirectionUnitVector(HitResult.TraceEnd, HitResult.TraceStart);
		float DistanceResult =  1.f - UKismetMathLibrary::NormalizeToRange(HitResult.Distance, 0.0f, 60.0f);
		FVector ForceDirection = DistanceResult * DirectionResult * KartBoucingForce;
		CollisionBox->AddForceAtLocation(ForceDirection, WheelComp->GetComponentToWorld().GetLocation());
	}
}

