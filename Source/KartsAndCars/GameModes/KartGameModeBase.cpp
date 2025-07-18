// Fill out your copyright notice in the Description page of Project Settings.


#include "KartGameModeBase.h"
#include "KartsAndCars/Kart/KartPawnBase.h"

AKartGameModeBase::AKartGameModeBase()
{
	// Set default pawn class to our custom pawn class
	DefaultPawnClass = DefaultKartPawnClass;
	
}
