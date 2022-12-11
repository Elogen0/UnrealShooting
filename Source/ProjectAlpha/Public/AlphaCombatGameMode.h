// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "AlphaCombatGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTALPHA_API AAlphaCombatGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	virtual void PlayerEliminated(class AAlphaCharacter* ElimmedCharacter, class AAlphaPlayerController* VictimController, class AAlphaPlayerController* AttackerController);
	virtual void RequestRespawn(class ACharacter* ElimmedCharacter, AController* ElimmedController);
};
