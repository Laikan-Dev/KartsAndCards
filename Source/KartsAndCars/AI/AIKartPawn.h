// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KartsAndCars/Kart/KartPawnBase.h"
#include "AIKartPawn.generated.h"

/**
 * 
 */
UCLASS()
class KARTSANDCARS_API AAIKartPawn : public AKartPawnBase
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AAIKartPawn();
	
protected:
	virtual void BeginPlay() override;
	virtual void Accelerate(float Value) override;
	virtual void Tick(float DeltaTime) override;
	virtual void Steer(float Value) override;

public:

	UFUNCTION(BlueprintCallable, Category = "Kart Functions")
	float GetAISteering();

	UFUNCTION(BlueprintCallable, Category = "Kart Functions")
	float GetAIThrottle();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Properties")
	float ThrottleInput = 1.0f;
	
};
