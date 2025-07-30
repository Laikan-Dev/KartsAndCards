// Fill out your copyright notice in the Description page of Project Settings.


#include "KartAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"

UKartAttributeSet::UKartAttributeSet()
{
}

void UKartAttributeSet::OnRep_Speed(const FGameplayAttributeData& OldSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UKartAttributeSet, Speed, OldSpeed);
}

void UKartAttributeSet::OnRep_MaxSpeed(const FGameplayAttributeData& OldMaxSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UKartAttributeSet, MaxSpeed, OldMaxSpeed);
}

void UKartAttributeSet::OnRep_Nitro(const FGameplayAttributeData& OldNitro)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UKartAttributeSet, Nitro, OldNitro);
}

void UKartAttributeSet::OnRep_MaxNitro(const FGameplayAttributeData& OldMaxNitro)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UKartAttributeSet, MaxNitro, OldMaxNitro);
}

void UKartAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UKartAttributeSet, Speed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UKartAttributeSet, MaxSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UKartAttributeSet, Nitro, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UKartAttributeSet, MaxNitro, COND_None, REPNOTIFY_Always);
}
