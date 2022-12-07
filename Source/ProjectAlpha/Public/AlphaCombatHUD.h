// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AlphaCombatHUD.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTALPHA_API AAlphaCombatHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void DrawHUD() override;

	void DrawCrosshair(UTexture2D* Texture, FVector2D Spread, FLinearColor Color);
	FORCEINLINE void SetCrosshair(class UCrossHairDataAsset* aCrosshair) { Crosshair = aCrosshair; }
protected:
	virtual void BeginPlay() override;
	void AddCharacterOverlay();
protected:
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;
	class UCharacterOverlay* CharacterOverlay;

	class UCrossHairDataAsset* Crosshair;
	FVector2D ViewportCenter;
};
