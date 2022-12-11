// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapon/WeaponTypes.h"
#include "AlphaTypes/CombatState.h"
#include "CombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTALPHA_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	friend class AAlphaCharacter;
	
	UFUNCTION(Blueprintcallable)
	void EquipWeapon(class AWeapon* WeaponToEquip);
	
	UFUNCTION(Blueprintcallable)
	void FireButtonPressed(bool bPressed);

	UFUNCTION(Blueprintcallable)
	void SetAming(bool bIsAiming);

	bool CanReload();
	void Reload();
	void HandleReload();
	UFUNCTION(BlueprintCallable)
	void FinishReloading();
	UFUNCTION(BlueprintCallable)
	void ReloadShotgunShell();
	bool IsCarriedAmmoEmpty();

	UFUNCTION(BlueprintPure)
	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }
	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsFireButtonPressed() const { return bFireButtonPressed; }
	
protected:
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	bool CanFire();
	void FireWeapon();
	void StartFireTimer();
	void FireTimerFinished();

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);
	void UpdateCrosshair(float DeltaTime);
	void InterpFOV(float DeltaTime);

	void InitializeCarriedAmmo();
	int32 AmountToReload();
	void UpdateReloadAmmoValues();
	bool UpdateShotgunReloadAmmoValues();
	
	///////////
	//Network//
	///////////
	UFUNCTION(Client, Reliable)
	void ClientApplyCrosshair(class UCrossHairDataAsset* Crosshair);
	UFUNCTION()
	void OnRep_EquippedWeapon(AWeapon* LastWeapon);
	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAming);
	UFUNCTION(Server, Reliable)
	void ServerFire(bool bFire, const FVector_NetQuantize& TraceHitTarget);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(bool bFire, const FVector_NetQuantize& TraceHitTarget);
	UFUNCTION()
	void OnRep_CarriedAmmo();
	UFUNCTION(Server, Reliable)
	void ServerReload();
	UFUNCTION()
	void OnRep_CombatState();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastJumpToShotgunEnd();
public:	
	
private:
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class AAlphaCharacter* Character;
    UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class AAlphaPlayerController* Controller;
	
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	AWeapon* EquippedWeapon;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float BaseWalkSpeed;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float AimWalkSpeed;
	UPROPERTY(Replicated)
	bool bFireButtonPressed = false;

	// Corsshair and Aim
	UPROPERTY(Replicated)
	bool bAiming;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector HitTarget;

	float CrosshairInAirFactor = 0.f;
	float CrosshairAimFactor = 0.f;
	float CrosshairShootingFactor = 0.f;
	float DefaultFOV = 0.f;
	float CurrentFOV = 0.f;

	//Automatic Fire
	FTimerHandle FireTimerHandle;
	bool bCanFire = true;

	// Carried Ammo for the currently-equipped weapon
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 CarriedAmmo;
	//todo : not replicate change other container
	TMap<EWeaponType, int32> CarriedAmmoMap;

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;
};
