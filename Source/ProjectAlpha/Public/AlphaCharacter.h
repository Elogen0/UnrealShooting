// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/InteractWithCrosshairsInterface.h"
#include "Components/TimelineComponent.h"
#include "Weapon/WeaponTypes.h"
#include "AlphaCharacter.generated.h"

class AWeapon;

UCLASS()
class PROJECTALPHA_API AAlphaCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAlphaCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PostInitializeComponents() override;
	virtual void Jump() override;
	void SetOverlappingWeapon(AWeapon* Weapon);
	void Elim();
	
	UFUNCTION(BlueprintImplementableEvent)
	void PlayFireMontage(bool bAiming);
	UFUNCTION(BlueprintImplementableEvent)
	void PlayHitReactMontage();
	UFUNCTION(BlueprintImplementableEvent)
	void PlayElimMontage();
	UFUNCTION(BlueprintImplementableEvent)
	void PlayReloadMontage(EWeaponType WeaponType);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BP_ReplicatedMovement();

	UFUNCTION(BlueprintCallable)
	void EquipButtonPressed();
	UFUNCTION(BlueprintPure)
	bool IsWeaponEquipped();
	UFUNCTION(BlueprintPure)
	bool IsAiming();
	UFUNCTION(BlueprintPure)
	float CalculateMovementSpeed();

	UFUNCTION(BlueprintCallable)
	void ReloadButtonPressed();

	FORCEINLINE class UCameraComponent* GetFollowCamera() { return FollowCamera; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void HideCameraIfCharacterClose();
	UFUNCTION()
	void ReceiveDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCuaser);

	void ElimTimerFinished();
	///////////
	//Network//
	///////////
	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);
	virtual void OnRep_ReplicatedMovement() override;
	UFUNCTION()	
	void OnRep_Health();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCombatComponent* Combat;

protected:
	UPROPERTY()
	class AAlphaPlayerController* AlphaController;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
	class UCameraComponent* FollowCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
	class USpringArmComponent* CameraBoom;
	UPROPERTY(EditAnywhere, Category = Camera)
	float CameraThreshold = 200.f;
	UPROPERTY(ReplicatedUsing=OnRep_OverlappingWeapon)
	AWeapon* OverlappingWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float MaxHealth = 100.f;
	UPROPERTY(ReplicatedUsing=OnRep_Health, VisibleAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health = 100.f;
	UPROPERTY(BlueprintReadWrite, Category = "Player Stats", meta = (AllowPrivateAccess = "true"))
	bool bElimmed = false;
	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;
	FTimerHandle ElimTimer;

};
