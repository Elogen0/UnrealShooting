// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/HitScanWeapon.h"
#include "Shotgun.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTALPHA_API AShotgun : public AHitScanWeapon
{
	GENERATED_BODY()
public:
	virtual void Fire(const FVector& HitTarget) override;
protected:
	UPROPERTY(EditAnywhere, Category="Shotgun")
	uint32 NumberOfPellets = 10;
};
