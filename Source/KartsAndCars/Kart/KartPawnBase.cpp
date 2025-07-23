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
	CollisionBox->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Ignore);
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
	SpringArm->bUsePawnControlRotation = false; // Allow the spring arm to use the pawn's control rotation
	SpringArm->bEnableCameraRotationLag = true; // Enable camera rotation lag

	// Set the camera component
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	// Create wheel components and attach them to the root component
	FLWheel = CreateDefaultSubobject<USceneComponent>(TEXT("FLWheel"));
	FLWheel->SetupAttachment(CollisionBox);
	FLWheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FLWheelMesh"));
	FLWheelMesh->SetCollisionObjectType(ECC_Vehicle);
	FLWheelMesh->SetupAttachment(FLWheel);

	FRWheel = CreateDefaultSubobject<USceneComponent>(TEXT("FRWheel"));
	FRWheel->SetupAttachment(CollisionBox);
	FRWheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FRWheelMesh"));
	FRWheelMesh->SetCollisionObjectType(ECC_Vehicle);
	FRWheelMesh->SetupAttachment(FRWheel);

	BLWheel = CreateDefaultSubobject<USceneComponent>(TEXT("BLWheel"));
	BLWheel->SetupAttachment(CollisionBox);
	BLWheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BLWheelMesh"));
	BLWheelMesh->SetCollisionObjectType(ECC_Vehicle);
	BLWheelMesh->SetupAttachment(BLWheel);

	BRWheel = CreateDefaultSubobject<USceneComponent>(TEXT("BRWheel"));
	BRWheel->SetupAttachment(CollisionBox);
	BRWheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BRWheelMesh"));
	BRWheelMesh->SetCollisionObjectType(ECC_Vehicle);
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

		float WheelBoucingDistance = HitResult.Distance * -1.f + 32.f; // Adjust the distance to apply the boucing force
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
	AccelerationForce = AccelerationInput * FMath::Lerp(0.0, 15000.0, AccelerationInput);
	AccelerationInput = FMath::FInterpTo(AccelerationInput, 0.0, GetWorld()->GetDeltaSeconds(), 0.3);

	for (UStaticMeshComponent* Wheel : WheelMeshes)
	{
		Wheel->AddLocalRotation(FRotator(AccelerationForce / -1000.0f, 0.0f, 0.0f));
	}
}

void AKartPawnBase::Accelerate(const FInputActionValue& Value)
{
	float Target = UKismetMathLibrary::SelectFloat(Value.Get<float>(), 0.0, bIsOnTheGround());
	AccelerationInput = FMath::FInterpTo(AccelerationInput, Value.Get<float>(), GetWorld()->GetDeltaSeconds(), 0.5);

}

void AKartPawnBase::CalculateAcceleratingBouce(USceneComponent* WheelComp)
{
	FVector MovingBouce = CollisionBox->GetForwardVector() * 2000.f * AccelerationInput * CollisionBox->GetMass() * SpeedModifier;
	FVector ForceCalc = UKismetMathLibrary::SelectVector(FVector::ZeroVector, FVector(0.0, 0.0, -30000.0), bIsOnTheGround());
	FVector ForceDirection = MovingBouce + ForceCalc;
	CollisionBox->AddForceAtLocation(ForceDirection, WheelComp->GetComponentToWorld().GetLocation());

	FVector CenterOfMass = FVector(-50.f, 0.f, 0.f) * AccelerationInput; // Adjust the center of mass based on acceleration input
	CollisionBox->SetCenterOfMass(CenterOfMass);
}

void AKartPawnBase::Steer(const FInputActionValue& Value)
{
	float SteeringInput = Value.Get<float>() * 1000000 * AccelerationInput;
	FVector Torque = FVector(0.0, 0.0, SteeringInput);
	CollisionBox->AddTorqueInRadians(Torque);

	float RotationToFrontWheels = FMath::FInterpTo(FRWheel->GetRelativeRotation().Yaw, Value.Get<float>(), GetWorld()->GetDeltaSeconds(), 3.0f);

	FLWheel->SetRelativeRotation(FRotator(0.0f, RotationToFrontWheels, 0.0f));
	FRWheel->SetRelativeRotation(FRotator(0.0f, RotationToFrontWheels, 0.0f));
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

