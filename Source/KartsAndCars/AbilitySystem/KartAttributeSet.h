// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "KartAttributeSet.generated.h"


#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName) \

#define NUMERIC_VALUE(AttributeSetName, PropertyName) \
	AttributeSetName->Get##PropertyName##Attribute().GetNumericValue(AttributeSetName)


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAttributeChangedEvent, UAttributeSet*, AttributeSet, float, OldValue, float, NewValue);
/**
 * 
 */
UCLASS()
class KARTSANDCARS_API UKartAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UKartAttributeSet();
//Atribuites
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Speed, Category = "Vital Attributes")
	FGameplayAttributeData Speed;
	ATTRIBUTE_ACCESSORS(UKartAttributeSet, Speed);

	UPROPERTY(BlueprintAssignable)
	FAttributeChangedEvent OnSpeedChanged;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxSpeed, Category = "Vital Attributes")
	FGameplayAttributeData MaxSpeed;
	ATTRIBUTE_ACCESSORS(UKartAttributeSet, MaxSpeed);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Nitro, Category = "Vital Attributes")
	FGameplayAttributeData Nitro;
	ATTRIBUTE_ACCESSORS(UKartAttributeSet, Nitro);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxNitro, Category = "Vital Attributes")
	FGameplayAttributeData MaxNitro;
	ATTRIBUTE_ACCESSORS(UKartAttributeSet, MaxNitro);

	//Replication Func
	UFUNCTION()
	void OnRep_Speed(const FGameplayAttributeData& OldSpeed);
	UFUNCTION()
	void OnRep_MaxSpeed(const FGameplayAttributeData& OldMaxSpeed);

	UFUNCTION()
	void OnRep_Nitro(const FGameplayAttributeData& OldNitro);
	UFUNCTION()
	void OnRep_MaxNitro(const FGameplayAttributeData& OldMaxNitro);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//Rules
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
};
