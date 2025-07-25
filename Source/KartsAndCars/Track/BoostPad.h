// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoostPad.generated.h"

UCLASS()
class KARTSANDCARS_API ABoostPad : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoostPad();
private:
	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* RootComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BoostPadMesh;
	// Function to handle overlap events
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Modifier for the boost speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boost Settings")
	float BoostSpeedModifier = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boost Settings")
	float BoostDuration = 5.0f; // Duration of the boost effect in seconds

};
