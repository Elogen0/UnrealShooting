// Fill out your copyright notice in the Description page of Project Settings.


#include "AlphaCombatHUD.h"
#include "CrossHairDataAsset.h"
#include "GameFramework/PlayerController.h"
#include "CharacterOverlay.h"

void AAlphaCombatHUD::BeginPlay()
{
    Super::BeginPlay();

    AddCharacterOverlay();
}

void AAlphaCombatHUD::AddCharacterOverlay()
{
    APlayerController* PlayerController = GetOwningPlayerController();
    if (PlayerController && CharacterOverlayClass)
    {
        CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
        CharacterOverlay->AddToViewport();
    }
}


void AAlphaCombatHUD::DrawHUD()
{
    Super::DrawHUD();

    FVector2D ViewportSize;
    if (GEngine)
    {
        GEngine->GameViewport->GetViewportSize(ViewportSize);
        ViewportCenter = FVector2D(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

        if (Crosshair)
        {
            float SpreadScaled = Crosshair->CrosshairSpreadMax * Crosshair->CrosshairSpreadRatio;
            DrawCrosshair(Crosshair->CrosshairsCenter, FVector2D(0.f, 0.f), Crosshair->Color);
            DrawCrosshair(Crosshair->CrosshairsLeft, FVector2D(-SpreadScaled, 0.f), Crosshair->Color);
            DrawCrosshair(Crosshair->CrosshairsRight, FVector2D(SpreadScaled, 0.f), Crosshair->Color);
            DrawCrosshair(Crosshair->CrosshairsTop, FVector2D(0.f, -SpreadScaled), Crosshair->Color);
            DrawCrosshair(Crosshair->CrosshairsBottom,FVector2D(0.f, SpreadScaled), Crosshair->Color);
        }
    }
}

void AAlphaCombatHUD::DrawCrosshair(UTexture2D* Texture, FVector2D Spread, FLinearColor Color)
{
    if (Texture == nullptr) return;

    const float TextureWidth = Texture->GetSizeX();
    const float TextureHeight = Texture->GetSizeY();
    const FVector2D TextureDrawPoint(
        ViewportCenter.X - (TextureWidth / 2.f) + Spread.X,
        ViewportCenter.Y - (TextureHeight / 2.f) + Spread.Y
    );

    DrawTexture(
        Texture,
        TextureDrawPoint.X,
        TextureDrawPoint.Y,
        TextureWidth,
        TextureHeight,
        0.f,
        0.f,
        1.f,
        1.f,
        Color
    );
}
