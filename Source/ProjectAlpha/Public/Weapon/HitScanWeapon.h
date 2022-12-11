// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitScanWeapon.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTALPHA_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()
public:
	virtual void Fire(const FVector& HitTarget) override;
protected:
	void WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit);
	FVector TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget);
	
protected:
	UPROPERTY(EditAnywhere, Category="HitScan Weapon")
	float Damage = 20.f;
	UPROPERTY(EditAnywhere, Category = "HitScan Weapon")
	class UParticleSystem* ImpactParticles;
	UPROPERTY(EditAnywhere, Category = "HitScan Weapon")
	class UParticleSystem* BeamParticles;
	UPROPERTY(EditAnywhere, Category = "HitScan Weapon")
	UParticleSystem* MuzzleFlash;
	UPROPERTY(EditAnywhere, Category = "HitScan Weapon")
	USoundCue* FireSound;
	UPROPERTY(EditAnywhere, Category = "HitScan Weapon")
	USoundCue* HitSound;

	////////////////////////
	//Trace end with Scatter
	////////////////////////
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float DistanceToSphere = 800.f;
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float SphereRadius = 75.f;
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	bool bUseScatter = false;
};
