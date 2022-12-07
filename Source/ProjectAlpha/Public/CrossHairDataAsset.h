// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CrossHairDataAsset.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class PROJECTALPHA_API UCrossHairDataAsset : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	class UTexture2D* CrosshairsCenter;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsLeft;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsRight;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsTop;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsBottom;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	float CrosshairSpreadMax = 16.f;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	float ZoomFOV = 45.f;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	float ZoomInterpSpeed = 20.f;

	float CrosshairSpreadRatio;
	FLinearColor Color;
};
