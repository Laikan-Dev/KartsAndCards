// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KartEffectActor.generated.h"

class UGameplayEffect;
UCLASS()
class KARTSANDCARS_API AKartEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKartEffectActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:

	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TObjectPtr<UGameplayEffect> InstantGameplayEffectClass;

	


};
