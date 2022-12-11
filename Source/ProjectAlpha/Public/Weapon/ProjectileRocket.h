// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileRocket.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTALPHA_API AProjectileRocket : public AProjectile
{
	GENERATED_BODY()
protected:
	AProjectileRocket();
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
protected:

	UPROPERTY(EditAnywhere, Category="Projectile Weapon")
	USoundCue* ProjectileLoopSound;
	UPROPERTY()
	UAudioComponent* ProjectileLoopSoundComponent;
	UPROPERTY(EditAnywhere, Category = "Projectile Weapon")
	USoundAttenuation* LoopingSoundAttenuation;

	UPROPERTY(VisibleAnywhere)
	class URocketMovementComponent* RocketMovementComponent;
};
