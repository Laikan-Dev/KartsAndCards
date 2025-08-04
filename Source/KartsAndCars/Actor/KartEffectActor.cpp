// Fill out your copyright notice in the Description page of Project Settings.


#include "KartEffectActor.h"
#include "Components/SphereComponent.h"
#include "AbilitySystemInterface.h"
#include "KartsAndCars/AbilitySystem/KartAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "KartsAndCars/AbilitySystem/KartAbilitySystemComponent.h"

// Sets default values
AKartEffectActor::AKartEffectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot")));

}

// Called when the game starts or when spawned
void AKartEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AKartEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	// Fix: Replace 'check' with an if-statement to avoid unreachable code
	if (TargetASC == nullptr)
	{
		return;
	}

	check(GameplayEffectClass);
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1.0f, EffectContextHandle);
	TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
}

