// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName="Initial State"),
	EWS_Equippped UMETA(DisplayName="Equipped"),
	EWS_Dropped UMETA(DisplayName="Dropped"),
	
	EWS_Max UMETA(DisplayName="Max")
};

UCLASS()
class PROJECTALPHA_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	void ShowPickupWidget(bool Visibility);

	void SetWeaponState(EWeaponState State);
	virtual void Fire(const FVector& HitTarget);

	UFUNCTION(BlueprintPure)
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE class UCrossHairDataAsset* GetCrosshair() { return Crosshair; }

protected:
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	virtual void OnStartSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	UFUNCTION()
	virtual void OnEndSphreOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);
	UFUNCTION()
	void OnRep_WeaponState();
	void WeaponStateChanedProcess();

public:
	UPROPERTY(EditAnywhere, Category = "Automatic Fire")
	float FireDelay = .15f;
	UPROPERTY(EditAnywhere, Category = "Automatic Fire")
	bool bAutomatic = true;
protected:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class USphereComponent* AreaSphere;

	UPROPERTY(ReplicatedUsing= OnRep_WeaponState, VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	EWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABulletCasing> CasingClass;

	UPROPERTY(EditAnywhere, Instanced)
	UCrossHairDataAsset* Crosshair;


};
