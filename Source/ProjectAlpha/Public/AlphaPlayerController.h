// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AlphaPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTALPHA_API AAlphaPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	void SetCrosshair(class UCrossHairDataAsset* Crosshair);

private:
	class AAlphaCombatHUD* CombatHUD;
};
