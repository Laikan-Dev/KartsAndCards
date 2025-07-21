// Fill out your copyright notice in the Description page of Project Settings.


#include "KartPawnBase.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "KartPlayerController.h"
#include "GameFramework/Controller.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"


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
	//apply boucing force to the kart
	for (USceneComponent* Wheel : Wheels)
	{
		if (Wheel)
		{
			HandleSuspension(Wheel);
		}
	}
	CalculateAccelerationForce();
}

// Called to bind functionality to input
void AKartPawnBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Add the input mapping context for the kart
	if (AKartPlayerController* PlayerController = Cast<AKartPlayerController>(GetController()))
	{
		if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(KartInputMappingContext, 0);
		}
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Bind the action inputs

		EnhancedInputComponent->BindAction(AcelerateAction, ETriggerEvent::Triggered, this, &AKartPawnBase::Accelerate);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &AKartPawnBase::Steer);
	}
}

void AKartPawnBase::HandleSuspension(USceneComponent* WheelComp)

{
	FHitResult HitResult;
	FVector Start = WheelComp->GetComponentLocation();
	FVector End = Start + WheelComp->GetUpVector() * -60.f;
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

		FVector MovingBouce = CollisionBox->GetForwardVector() * 10000.f * AccelerationInput * CollisionBox->GetMass();
		CollisionBox->AddForceAtLocation(MovingBouce, WheelComp->GetComponentToWorld().GetLocation());

		FVector CenterOfMass = FVector(50.f, 0.f, 0.f) * AccelerationInput; // Adjust the center of mass based on acceleration input
		CollisionBox->SetCenterOfMass(CenterOfMass);

	}
}

void AKartPawnBase::CalculateAccelerationForce()
{
	AccelerationForce = AccelerationInput * FMath::Lerp(0.0, 15000.0, AccelerationInput);
	AccelerationInput = FMath::FInterpTo(AccelerationInput, 0.0, GetWorld()->GetDeltaSeconds(), 0.3);
}

void AKartPawnBase::Accelerate(const FInputActionValue& Value)
{
	AccelerationInput = FMath::FInterpTo(AccelerationInput, Value.Get<float>(), GetWorld()->GetDeltaSeconds(), 0.5);

}

void AKartPawnBase::Steer(const FInputActionValue& Value)
{
	float SteeringInput = Value.Get<float>() * 1000000 * AccelerationInput;
	FVector Torque = FVector(0.0, 0.0, SteeringInput);
	CollisionBox->AddTorqueInRadians(Torque);
}

