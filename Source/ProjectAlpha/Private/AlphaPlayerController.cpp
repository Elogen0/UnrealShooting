// Fill out your copyright notice in the Description page of Project Settings.


#include "AlphaPlayerController.h"
#include "AlphaCombatHUD.h"

void AAlphaPlayerController::SetCrosshair(UCrossHairDataAsset* Crosshair)
{
    CombatHUD = CombatHUD ? CombatHUD : Cast<AAlphaCombatHUD>(GetHUD());
    if (CombatHUD)
    {
        CombatHUD->SetCrosshair(Crosshair);
    }
}
