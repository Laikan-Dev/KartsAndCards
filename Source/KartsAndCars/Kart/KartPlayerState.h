// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "KartPlayerState.generated.h"

class UAbilitySystemComponent;
class UKartAttributeSet;
/**
 * 
 */
UCLASS()
class KARTSANDCARS_API AKartPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AKartPlayerState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UKartAttributeSet* GetAttributeSet() const;

protected:
	//AbilitySystem
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UKartAttributeSet> KartAttributeSet;
};
