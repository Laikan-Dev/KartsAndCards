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
#include "Kismet/GameplayStatics.h"
#include "KartsAndCars/Track/TrackSpline.h"
#include "Components/SplineComponent.h"


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
	CollisionBox->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Camera, ECR_Overlap);
	CollisionBox->SetMassOverrideInKg(NAME_None, 2000.f, true); // Set a mass for the kart
	CollisionBox->SetCenterOfMass(FVector(-30.f, 0.f, 0.f)); // Adjust the center of mass for better stability

	// Set config for the boucing
	CollisionBox->SetSimulatePhysics(true);
	CollisionBox->SetLinearDamping(3.f);
	CollisionBox->SetAngularDamping(5.f);
	RootComponent = CollisionBox;


	// Create and set up the Kart mesh component
	KartMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KartMesh"));
	KartMesh->SetupAttachment(PivotPoint);
	KartMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Overlap);

	// Set the spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(PivotPoint);
	SpringArm->bUsePawnControlRotation = false; // Allow the spring arm to use the pawn's control rotation
	SpringArm->bEnableCameraRotationLag = true; // Enable camera rotation lag

	// Set the camera component
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	// Create wheel components and attach them to the root component
	FLWheel = CreateDefaultSubobject<USceneComponent>(TEXT("FLWheel"));
	FLWheel->SetupAttachment(PivotPoint);
	FLWheel->ComponentTags.Add(TEXT("FrontWheel")); // Tag for identification
	FLWheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FLWheelMesh"));
	FLWheelMesh->SetCollisionObjectType(ECC_Vehicle);
	FLWheelMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Overlap);
	FLWheelMesh->SetupAttachment(FLWheel);

	FRWheel = CreateDefaultSubobject<USceneComponent>(TEXT("FRWheel"));
	FRWheel->SetupAttachment(PivotPoint);
	FRWheel->ComponentTags.Add(TEXT("FrontWheel")); // Tag for identification
	FRWheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FRWheelMesh"));
	FRWheelMesh->SetCollisionObjectType(ECC_Vehicle);
	FRWheelMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Overlap);
	FRWheelMesh->SetupAttachment(FRWheel);

	BLWheel = CreateDefaultSubobject<USceneComponent>(TEXT("BLWheel"));
	BLWheel->SetupAttachment(PivotPoint);
	BLWheel->ComponentTags.Add(TEXT("BackWheel")); // Tag for identification
	BLWheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BLWheelMesh"));
	BLWheelMesh->SetCollisionObjectType(ECC_Vehicle);
	BLWheelMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Overlap);
	BLWheelMesh->SetupAttachment(BLWheel);

	BRWheel = CreateDefaultSubobject<USceneComponent>(TEXT("BRWheel"));
	BRWheel->SetupAttachment(PivotPoint);
	BRWheel->ComponentTags.Add(TEXT("BackWheel")); // Tag for identification
	BRWheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BRWheelMesh"));
	BRWheelMesh->SetCollisionObjectType(ECC_Vehicle);
	BRWheelMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Overlap);
	BRWheelMesh->SetupAttachment(BRWheel);

}

// Called when the game starts or when spawned
void AKartPawnBase::BeginPlay()
{
	Super::BeginPlay();

	//Adding wheel components to the Wheels array
	Wheels.Add(FLWheel);
	Wheels.Add(FRWheel);
	Wheels.Add(BLWheel);
	Wheels.Add(BRWheel);

	if (FLWheelMesh && FRWheelMesh && BLWheelMesh && BRWheelMesh)
	{
		WheelMeshes.Add(FLWheelMesh);
		WheelMeshes.Add(FRWheelMesh);
		WheelMeshes.Add(BLWheelMesh);
		WheelMeshes.Add(BRWheelMesh);
	}
	TSubclassOf<ATrackSpline> TrackSplineClass = ATrackSpline::StaticClass();
	TrackSpline = Cast<ATrackSpline>(UGameplayStatics::GetActorOfClass(GetWorld(), TrackSplineClass));
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
	GetTrackProgress();
	bIsTopple();
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

		EnhancedInputComponent->BindAction(AcelerateAction, ETriggerEvent::Triggered, this, &AKartPawnBase::AccelerateEntry);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &AKartPawnBase::SteerEntry);

		//Drift
		EnhancedInputComponent->BindAction(DriftAction, ETriggerEvent::Triggered, this, &AKartPawnBase::DriftEntry);
		EnhancedInputComponent->BindAction(DriftAction, ETriggerEvent::Completed, this, &AKartPawnBase::StopDrift);
		EnhancedInputComponent->BindAction(DriftAction, ETriggerEvent::Canceled, this, &AKartPawnBase::StopDrift);

	}
}

void AKartPawnBase::HandleSuspension(USceneComponent* WheelComp)
{
	float WheelValue;
	if (WheelComp->ComponentHasTag(TEXT("FrontWheel")))
	{
		WheelValue = FrontWheelRadius;
	}
	else if (WheelComp->ComponentHasTag(TEXT("BackWheel")))
	{
		WheelValue = BackWheelRadius;
	}

	FHitResult HitResult;
	FVector Start = WheelComp->GetComponentToWorld().GetLocation();
	FVector End = WheelComp->GetUpVector() * -60.f + Start;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	bool TraceResult = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), Start, End, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, OUT HitResult, true);
	if (HitResult.bBlockingHit)
	{
		FVector DirectionResult = UKismetMathLibrary::GetDirectionUnitVector(HitResult.TraceEnd, HitResult.TraceStart);
		float DistanceResult = 1.f - UKismetMathLibrary::NormalizeToRange(HitResult.Distance, 0.0f, 60.0f);
		FVector ForceDirection = DistanceResult * DirectionResult * KartBoucingForce;
		CollisionBox->AddForceAtLocation(ForceDirection, WheelComp->GetComponentToWorld().GetLocation());

		float WheelBoucingDistance = HitResult.Distance * -1.f + (WheelValue / 2.0f); // Adjust the distance to apply the boucing force
		float WheelLocationZ = FMath::FInterpTo(WheelComp->GetChildComponent(0)->GetRelativeLocation().Z, WheelBoucingDistance, GetWorld()->GetDeltaSeconds(), 3.f);
		FVector WheelLocation(0.f, 0.f, WheelLocationZ);
		WheelComp->GetChildComponent(0)->SetRelativeLocation(WheelLocation);
		CalculateAcceleratingBouce(WheelComp);
	}
	else
	{
		CalculateAcceleratingBouce(WheelComp);
	}
}

void AKartPawnBase::CalculateAccelerationForce()
{
	AccelerationForce = AccelerationInput * FMath::Lerp(0.0, MaxAcceleration, AccelerationInput);
	AccelerationInput = FMath::FInterpTo(AccelerationInput, 0.0, GetWorld()->GetDeltaSeconds(), 0.3);

	for (UStaticMeshComponent* Wheel : WheelMeshes)
	{
		Wheel->AddLocalRotation(FRotator(AccelerationForce / -1000.0f, 0.0f, 0.0f));
	}
}

void AKartPawnBase::AccelerateEntry(const FInputActionValue& Value)
{
	Accelerate(Value.Get<float>());
}

void AKartPawnBase::Accelerate(float Value)
{
	float Target = UKismetMathLibrary::SelectFloat(Value, 0.0, bIsOnTheGround());
	AccelerationInput = FMath::FInterpTo(AccelerationInput, Value, GetWorld()->GetDeltaSeconds(), 0.5);
}

void AKartPawnBase::CalculateAcceleratingBouce(USceneComponent* WheelComp)
{
	FVector MovingBouce = CollisionBox->GetForwardVector() * 2000.f * AccelerationInput * CollisionBox->GetMass() * SpeedModifier;
	FVector ForceCalc = UKismetMathLibrary::SelectVector(FVector::ZeroVector, FVector(0.0, 0.0, DownForce), bIsOnTheGround());
	FVector ForceDirection = MovingBouce + ForceCalc;
	CollisionBox->AddForceAtLocation(ForceDirection, WheelComp->GetComponentToWorld().GetLocation());

	FVector CenterOfMass = FVector(-50.f, 0.f, 0.f) * AccelerationInput; // Adjust the center of mass based on acceleration input
	CollisionBox->SetCenterOfMass(CenterOfMass);
}

void AKartPawnBase::SteerEntry(const FInputActionValue& Value)
{
	Steer(Value.Get<float>());
}

void AKartPawnBase::Steer(float Value)
{
	float SteeringInput = Value * SteeringTorque * (AccelerationInput * SteeringMultiplier);
	FVector Torque = FVector(0.0, 0.0, SteeringInput);
	CollisionBox->AddTorqueInRadians(Torque);

	float RotationToFrontWheels = FMath::FInterpTo(FRWheel->GetRelativeRotation().Yaw, Value, GetWorld()->GetDeltaSeconds(), 3.0f);

	FLWheel->SetRelativeRotation(FRotator(0.0f, RotationToFrontWheels, 0.0f));
	FRWheel->SetRelativeRotation(FRotator(0.0f, RotationToFrontWheels, 0.0f));
}

void AKartPawnBase::DriftEntry()
{
	StartDrift();
}

void AKartPawnBase::StartDrift()
{
	SteeringMultiplier = 4.0f; // Reduce steering multiplier for drifting
	bIsDrifting = true; // Set drifting state to true

}

void AKartPawnBase::StopDrift()
{
}

void AKartPawnBase::BoostKart(float Speed, float Time)
{
	SpeedModifier = Speed; // Set the speed modifier to the boost speed
	FTimerHandle BoostTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(BoostTimerHandle, this, &AKartPawnBase::ResetBoost, Time, false);
}

void AKartPawnBase::ResetBoost()
{
	SpeedModifier = 1.0f; // Reset the speed modifier to normal
}

bool AKartPawnBase::bIsOnTheGround()
{
	FHitResult HitResult;
	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0.f, 0.f, 35.f); // Check 60 units down from the kart's position

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	bool TraceResult = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), Start, End, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, OUT HitResult, true);

	return TraceResult;
}

bool AKartPawnBase::GetTrackProgress()
{
	if (TrackSpline)
	{
		FVector ClosestLocation = TrackSpline->SplineComponent->FindLocationClosestToWorldLocation(GetActorLocation(), ESplineCoordinateSpace::World);
		float TrackProgress = TrackSpline->SplineComponent->GetDistanceAlongSplineAtLocation(ClosestLocation, ESplineCoordinateSpace::World);

		// Normalize the track progress to a value between 0 and 1
		FVector LocatToNormalize = TrackSpline->SplineComponent->FindLocationClosestToWorldLocation(GetActorLocation(), ESplineCoordinateSpace::World);
		float Dot = UKismetMathLibrary::Dot_VectorVector(LocatToNormalize, GetActorForwardVector());
		bool bIsOnTrack = (Dot > 0.f);
		return bIsOnTrack;
	}
	return false;
}

float AKartPawnBase::GetCourseProgress()
{
	FVector ClosestLoc = TrackSpline->SplineComponent->FindLocationClosestToWorldLocation(GetActorLocation(), ESplineCoordinateSpace::World);
	float Distance = TrackSpline->SplineComponent->GetDistanceAlongSplineAtLocation(ClosestLoc, ESplineCoordinateSpace::World);
	float Value = Distance / TrackSpline->SplineComponent->GetSplineLength() + Laps;

	return Value;
}

bool AKartPawnBase::bIsTopple()
{
	float UpVector = UKismetMathLibrary::Dot_VectorVector(GetActorUpVector(), FVector(0.f, 0.f, 1.f));
	if (UpVector < 0.8f && !ResetPosTimer.IsValid())
	{
		GetWorld()->GetTimerManager().SetTimer(ResetPosTimer, this, &AKartPawnBase::ResetPosition, 3.0f, false);
	}
	return false;
}

void AKartPawnBase::ResetPosition()
{
	ResetPosTimer.Invalidate();
	FVector Loc = TrackSpline->SplineComponent->FindLocationClosestToWorldLocation(GetActorLocation(), ESplineCoordinateSpace::World);
	FRotator Rot = TrackSpline->SplineComponent->FindLocationClosestToWorldLocation(GetActorLocation(), ESplineCoordinateSpace::World).Rotation();
	SetActorLocationAndRotation(Loc, Rot);
	AccelerationInput = 0.0f; // Reset acceleration input

}

