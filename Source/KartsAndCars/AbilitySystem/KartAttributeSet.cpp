// Fill out your copyright notice in the Description page of Project Settings.


#include "KartAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"


UKartAttributeSet::UKartAttributeSet()
{
	// Initialize attributes with default values
	InitSpeed(0.0);
	InitMaxSpeed(100.0);
	InitNitro(0.0);
	InitMaxNitro(100.0);
	
}

void UKartAttributeSet::OnRep_Speed(const FGameplayAttributeData& OldSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UKartAttributeSet, Speed, OldSpeed);
	const float OldValue = OldSpeed.GetCurrentValue();
	const float NewSpeed = GetSpeed();
	OnSpeedChanged.Broadcast(this, OldValue, NewSpeed);
}

void UKartAttributeSet::OnRep_MaxSpeed(const FGameplayAttributeData& OldMaxSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UKartAttributeSet, MaxSpeed, OldMaxSpeed);
	const float CurrentSpeed = GetSpeed();
	OnSpeedChanged.Broadcast(this, CurrentSpeed, CurrentSpeed);
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

void UKartAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetSpeedAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxSpeed());
	}
	Super::PreAttributeChange(Attribute, NewValue);
}

void UKartAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetSpeedAttribute())
	{
		OnSpeedChanged.Broadcast(this, OldValue, NewValue);
	}
	else if (Attribute == GetMaxSpeedAttribute())
	{
		const float CurrentSpeed = GetSpeed();
		OnSpeedChanged.Broadcast(this, CurrentSpeed, NewValue);
	}
}

void UKartAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	if (Data.EvaluatedData.Attribute == GetSpeedAttribute())
	{
		SetSpeed(FMath::Clamp(GetSpeed(), 0.0f, GetMaxSpeed()));
	}
	UAbilitySystemComponent* ASC = Data.EffectSpec.GetContext().GetInstigatorAbilitySystemComponent();
	AActor* ASCOwner = ASC->AbilityActorInfo->OwnerActor.Get();
	ASCOwner->GetActorLocation();
}
