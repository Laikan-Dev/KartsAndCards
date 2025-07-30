// Fill out your copyright notice in the Description page of Project Settings.


#include "KartPlayerState.h"
#include "KartsAndCars/AbilitySystem/KartAbilitySystemComponent.h"
#include "KartsAndCars/AbilitySystem/KartAttributeSet.h"

AKartPlayerState::AKartPlayerState()
{
	NetUpdateFrequency = 100.0f; // Set a high update frequency for smooth multiplayer experience

	//AbilitySystem Defaults
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	KartAttributeSet = CreateDefaultSubobject<UAttributeSet>(TEXT("KartAttributeSet"));
}

UAbilitySystemComponent* AKartPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
