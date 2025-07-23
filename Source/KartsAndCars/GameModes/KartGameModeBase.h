// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KartGameModeBase.generated.h"

class AKartPawnBase;
/**
 * 
 */
UCLASS()
class KARTSANDCARS_API AKartGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AKartGameModeBase();

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Properties", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AKartPawnBase> DefaultKartPawnClass;
};
