// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletCasing.generated.h"

UCLASS()
class PROJECTALPHA_API ABulletCasing : public AActor
{
	GENERATED_BODY()
	
public:	
	ABulletCasing();

protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
protected:

	UPROPERTY(EditAnywhere)
	float ShellEjectionImpluse;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CasingMesh;

	UPROPERTY(EditAnywhere)
	class USoundCue* ShellSound;

};
