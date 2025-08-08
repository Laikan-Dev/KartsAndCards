// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemInterface.h"
#include "KartsAndCars/AbilitySystem/KartAttributeSet.h"
#include "KartPawnBase.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class ATrackSpline;
class UAbilitySystemComponent;
class UAttributeSet;

UCLASS()
class KARTSANDCARS_API AKartPawnBase : public APawn, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AKartPawnBase();

	
	//AbilitySystem
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UKartAttributeSet* GetAttributeSet() const;
	virtual void PossessedBy(AController* NewController) override;

private:
	void InitAbilitySystemComponent();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USceneComponent* RootComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USceneComponent* PivotPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* KartMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Properties")
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	UCameraComponent* Camera;
	// Input mapping context for the kart
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Input")
	UInputMappingContext* KartInputMappingContext;

	// Input action for moving the kart
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Input")
	UInputAction* AcelerateAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Input")
	UInputAction* SteeringAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Input")
	UInputAction* DriftAction;

	
	// Wheel components for the kart
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Properties")
	USceneComponent* FLWheel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Properties")
	UStaticMeshComponent* FLWheelMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Properties")
	USceneComponent* FRWheel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Properties")
	UStaticMeshComponent* FRWheelMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Properties")
	USceneComponent* BLWheel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Properties")
	UStaticMeshComponent* BLWheelMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Properties")
	USceneComponent* BRWheel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Properties")
	UStaticMeshComponent* BRWheelMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Properties")
	float BackWheelRadius = 64.0f; // Radius of the wheels for suspension calculations
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Properties")
	float FrontWheelRadius = 64.0f; // Radius of the wheels for suspension calculations


	//AbilitySystem
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UKartAttributeSet> KartAttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

	UFUNCTION(BlueprintCallable)
	void GiveDefaultAbilities();
	void InitDefaultAttributes() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Abilities")
	TSubclassOf<UGameplayEffect> DefaultAttributeEffect;

	virtual void OnRep_PlayerState() override;


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Properties")
	float KartBoucingForce = 90000.f; // Force applied to the wheels for suspension

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Properties")
	float DownForce = -60000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Properties")
	float SteeringTorque = 2000000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Properties")
	float SteeringMultiplier = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Properties")
	float AccelerationForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Properties")
	float MaxAcceleration = 15000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Properties")
	float AccelerationInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Properties")
	bool bIsDrifting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Properties")
	float DriftSteer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Properties")
	FRotator DriftRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Properties")
	TArray<USceneComponent*> Wheels; // Array to hold all wheel components

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Properties")
	TArray<UStaticMeshComponent*> WheelMeshes; // Array to hold all wheel meshes

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Properties")
	ATrackSpline* TrackSpline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boosts Settings")
	float SpeedModifier = 1.0f; // Speed modifier for the kart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track")
	int32 Laps;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track")
	FTimerHandle ResetPosTimer;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Function to handle suspension for the kart
	UFUNCTION(BlueprintCallable, Category = "Kart Functions")
	void HandleSuspension(USceneComponent* WheelComp);
	//Calculate the aceleration force based on the kart's speed
	UFUNCTION(BlueprintCallable, Category = "Kart Functions")
	void CalculateAccelerationForce();

	UFUNCTION(BlueprintCallable)
	void ApplyBoostEffect(TSubclassOf<UGameplayEffect> GEClass, float level);
	//Input action Functions
	void AccelerateEntry(const FInputActionValue& Value);
	virtual void Accelerate(float Value);
	void CalculateAcceleratingBouce(USceneComponent* WheelComp);
	void SteerEntry(const FInputActionValue& Value);
	virtual void Steer(float Value);
	void DriftEntry();
	virtual void StartDrift();
	virtual void StopDrift();

	UFUNCTION(BlueprintCallable, Category = "Kart Functions")
	void BoostKart(float Speed, float Time);

	UFUNCTION(BlueprintCallable, Category = "Kart Functions")
	void ResetBoost();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kart Functions")
	bool bIsOnTheGround();

	UFUNCTION(BlueprintCallable, Category = "Kart Functions")
	bool GetTrackProgress();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Track")
	float GetCourseProgress();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Track")
	bool bIsTopple();

	UFUNCTION(BlueprintCallable, Category = "Track")
	void ResetPosition();
	
};
