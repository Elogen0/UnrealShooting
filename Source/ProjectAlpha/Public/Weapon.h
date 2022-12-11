// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon/WeaponTypes.h"
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
	void Dropped();
	void SpendRound();
	bool IsEmpty();
	bool IsFull();
	void AddAmmo(int32 AmmoToAdd);

	UFUNCTION(BlueprintPure)
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE class UCrossHairDataAsset* GetCrosshair() const { return Crosshair; }
	UFUNCTION(BlueprintPure)
	FORCEINLINE EWeaponType GetWeaponType() { return WeaponType; }
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }
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
	UFUNCTION()
	void OnRep_Ammo();

public:
	UPROPERTY(EditAnywhere, Category = "Weapon")
	float FireDelay = .15f;
	UPROPERTY(EditAnywhere, Category = "Weapon")
	bool bAutomatic = true;

	UPROPERTY(EditAnywhere, Category="Weapon")
	class USoundCue* EquipSound;
protected:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class USphereComponent* AreaSphere;

	UPROPERTY(ReplicatedUsing= OnRep_WeaponState, VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	EWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	class UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABulletCasing> CasingClass;

	UPROPERTY(EditAnywhere, Instanced, Category = "Weapon")
	UCrossHairDataAsset* Crosshair;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", ReplicatedUsing = OnRep_Ammo)
	int32 Ammo = 30;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	int32 MagCapacity = 30;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	EWeaponType WeaponType;
};
