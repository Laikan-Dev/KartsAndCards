// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "KartAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class KARTSANDCARS_API UKartAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UKartAttributeSet();

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Speed, Category = "Vital Attributes")
	FGameplayAttributeData Speed;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxSpeed, Category = "Vital Attributes")
	FGameplayAttributeData MaxSpeed;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Nitro, Category = "Vital Attributes")
	FGameplayAttributeData Nitro;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxNitro, Category = "Vital Attributes")
	FGameplayAttributeData MaxNitro;

	UFUNCTION()
	void OnRep_Speed(const FGameplayAttributeData& OldSpeed);
	UFUNCTION()
	void OnRep_MaxSpeed(const FGameplayAttributeData& OldMaxSpeed);

	UFUNCTION()
	void OnRep_Nitro(const FGameplayAttributeData& OldNitro);
	UFUNCTION()
	void OnRep_MaxNitro(const FGameplayAttributeData& OldMaxNitro);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
