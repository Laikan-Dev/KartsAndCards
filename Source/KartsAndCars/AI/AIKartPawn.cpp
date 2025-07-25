// Fill out your copyright notice in the Description page of Project Settings.


#include "AIKartPawn.h"
#include "Kismet/GameplayStatics.h"
#include "KartsAndCars/Track/TrackSpline.h"	
#include "Components/SplineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/DoubleFloat.h"
AAIKartPawn::AAIKartPawn()
{
}

void AAIKartPawn::BeginPlay()
{
	Super::BeginPlay();

}

void AAIKartPawn::Accelerate(float Value)
{
	Super::Accelerate(Value);
}

void AAIKartPawn::Tick(float DeltaTime)
{
	{
		Super::Tick(DeltaTime);
		// Additional AI-specific logic can be added here
		Accelerate(GetAIThrottle()); // Example: AI always accelerates at full throttle
		Steer(GetAISteering()); // Example: AI steers based on throttle input
	}
}

void AAIKartPawn::Steer(float Value)
{
	Super::Steer(Value);
	// Additional AI-specific steering logic can be added here
	// For example, you could implement AI steering based on the track spline or other AI logic
}

float AAIKartPawn::GetAISteering()
{
	FVector ToForwardVec = GetActorForwardVector() * 500.0f;
	FVector ToProduct = TrackSpline->SplineComponent->FindDirectionClosestToWorldLocation(GetActorLocation() + ToForwardVec, ESplineCoordinateSpace::World);
	float Direction = UKismetMathLibrary::Dot_VectorVector(ToProduct, GetActorRightVector());
	float ToPow = FMath::Abs(Direction) + 1.0f;
	float Power = FMath::Pow(Direction, 2.0f); // Adjust the steering value based on the track direction
	float SteerValue = Power * AccelerationForce;
	return SteerValue;
}

float AAIKartPawn::GetAIThrottle()
{

	FVector Direction = TrackSpline->SplineComponent->FindDirectionClosestToWorldLocation(GetActorLocation(), ESplineCoordinateSpace::World);
	float Dot = UKismetMathLibrary::Dot_VectorVector(Direction, GetActorForwardVector());
	float Value = FMath::Clamp(UKismetMathLibrary::Abs(Dot), 0.5f, 1.0f) * FMath::Sign(Dot);

	return Value;
	
}
