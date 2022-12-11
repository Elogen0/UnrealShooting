// Fill out your copyright notice in the Description page of Project Settings.


#include "AlphaComponents/CombatComponent.h"
#include "Weapon.h"
#include "AlphaCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "AlphaPlayerController.h"
#include "CrossHairDataAsset.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 300.f;

}
// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

		if (Character->GetFollowCamera())
		{
			DefaultFOV = Character->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
	}
	InitializeCarriedAmmo();
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
	DOREPLIFETIME(UCombatComponent, bFireButtonPressed);
	DOREPLIFETIME(UCombatComponent, CombatState);
	DOREPLIFETIME_CONDITION(UCombatComponent, CarriedAmmo, COND_OwnerOnly);
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (Character && Character->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		UpdateCrosshair(DeltaTime);
		InterpFOV(DeltaTime);
	}
}

////////////
// Weapon //
////////////

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr) return;
	if (EquippedWeapon && WeaponToEquip != EquippedWeapon)
	{
		EquippedWeapon->Dropped();
		ClientApplyCrosshair(nullptr);
	}

	if (WeaponToEquip)
	{
		ClientApplyCrosshair(WeaponToEquip->GetCrosshair());
		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equippped);
		const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
		}
		EquippedWeapon->SetOwner(Character);
		if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
		{
			CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
		}

		if (EquippedWeapon->EquipSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, EquippedWeapon->EquipSound, Character->GetActorLocation());
		}

		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}

}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
	FireWeapon();
}

bool UCombatComponent::CanFire()
{
	if (EquippedWeapon == nullptr) return false;
	if (!EquippedWeapon->IsEmpty() && bCanFire && CombatState == ECombatState::ECS_Reloading && EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Shotgun) return true;
	if (EquippedWeapon->IsEmpty() || bCanFire == false) return false;
	if (CombatState == ECombatState::ECS_Reloading) return false;
	return true;
}

void UCombatComponent::FireWeapon()
{
	if (CanFire())
	{
		bCanFire = false;
		ServerFire(bFireButtonPressed, HitTarget);

		if (EquippedWeapon)
		{
			CrosshairShootingFactor += 0.75f;
		}
		StartFireTimer();
		UE_LOG(LogTemp, Warning, TEXT("FireWeapon ~ %d"), bFireButtonPressed);
	}
}

void UCombatComponent::StartFireTimer()
{
	if (EquippedWeapon == nullptr || Character == nullptr)
	{
		bCanFire = true;
		return;
	}
	Character->GetWorldTimerManager().SetTimer(
		FireTimerHandle,
		this,
		&UCombatComponent::FireTimerFinished,
		EquippedWeapon->FireDelay
	);
}

void UCombatComponent::FireTimerFinished()
{
	bCanFire = true;
	if (EquippedWeapon == nullptr) return;
	if (bFireButtonPressed && EquippedWeapon->bAutomatic)
	{
		FireWeapon();
	}
	if (EquippedWeapon->IsEmpty())
	{
		Reload();
	}
}

////////////////////
// Aim & Crosshair//
///////////////////


void UCombatComponent::SetAming(bool bIsAiming)
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;

	bAiming = bIsAiming;
	ServerSetAiming(bIsAiming);
	Character->GetCharacterMovement()->MaxWalkSpeed = bAiming ? AimWalkSpeed : BaseWalkSpeed;
	
	Controller = Controller ? Controller : Cast<AAlphaPlayerController>(Character->Controller);
	if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle && Character->IsLocallyControlled() && Controller)
	{
		Controller->ShowSniperScopeWidget(bIsAiming);
	}
}

bool UCombatComponent::CanReload()
{
	if (EquippedWeapon == nullptr) return false;
	if (CarriedAmmo <= 0) return false;
	if (CombatState == ECombatState::ECS_Reloading) return false;
	if (EquippedWeapon->GetAmmo() >= EquippedWeapon->GetMagCapacity()) return false;
	return true;
}

void UCombatComponent::Reload()
{
	if (CanReload())
	{
		ServerReload();
	}
}

void UCombatComponent::HandleReload()
{
	if (EquippedWeapon)
	{
		Character->PlayReloadMontage(EquippedWeapon->GetWeaponType());
	}
}

void UCombatComponent::FinishReloading()
{
	if (Character == nullptr) return;
	if (Character->HasAuthority())
	{
		UpdateReloadAmmoValues();
		CombatState = ECombatState::ECS_Unoccupied;
	}
	if (bFireButtonPressed)
	{
		FireWeapon();
	}
}

void UCombatComponent::ReloadShotgunShell()
{
	if (Character && Character->HasAuthority())
	{
		if (UpdateShotgunReloadAmmoValues())
		{
			MulticastJumpToShotgunEnd();
		}
	}
}

bool UCombatComponent::IsCarriedAmmoEmpty()
{
	return CarriedAmmo <= 0;
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrooshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrooshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrooshairLocation,
		CrooshairWorldPosition,
		CrosshairWorldDirection
	);

	if (bScreenToWorld)
	{
		FVector Start = CrooshairWorldPosition;

		if (Character)
		{
			float DistanceTOCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceTOCharacter + 100.f); //Tracing Character Forward Only
		}
		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);

		if (TraceHitResult.bBlockingHit == false)
		{
			TraceHitResult.ImpactPoint = End;
			HitTarget = End;
		}
		else
		{
			HitTarget = TraceHitResult.ImpactPoint;
			/*DrawDebugSphere(
				GetWorld(),
				TraceHitResult.ImpactPoint,
				12.f,
				12,
				FColor::Red
			);*/
		}

		if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairsInterface>())
		{
			if (EquippedWeapon && EquippedWeapon->GetCrosshair())
			{
				EquippedWeapon->GetCrosshair()->Color = FLinearColor::Red;
			}
		}
		else
		{
			if (EquippedWeapon && EquippedWeapon->GetCrosshair())
			{
				EquippedWeapon->GetCrosshair()->Color = FLinearColor::White;
			}
		}
	}
}

void UCombatComponent::UpdateCrosshair(float DeltaTime)
{
	if (EquippedWeapon)
	{
		auto Crosshair = EquippedWeapon->GetCrosshair();
		if (Crosshair == nullptr) return;
		// [0, maxWalkSpeed] -> [0, 1]
		FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
		FVector2D VelocityMuliplierRange(0.f, 1.f);
		FVector Velocity = Character->GetVelocity();
		Velocity.Z = 0.f;

		if (Character->GetCharacterMovement()->IsFalling())
		{
			CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 1, DeltaTime, 30.f);
		}
		else
		{
			CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0, DeltaTime, 30.f);
		}

		if (bAiming)
		{
			CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.58f, DeltaTime, 30.f);
		}
		else
		{
			CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0, DeltaTime, 30.f);
		}

		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0, DeltaTime, 10.f);

		float SpreadAmount =
			FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMuliplierRange, Velocity.Size())
			+ 0.5f
			+ CrosshairInAirFactor
			- CrosshairAimFactor
			+ CrosshairShootingFactor;
		Crosshair->CrosshairSpreadRatio = FMath::Clamp(SpreadAmount, 0, 1);
	}
}

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if (Character == nullptr) return;

	if (EquippedWeapon)
	{
		if (bAiming)
		{
			if (UCrossHairDataAsset* Crosshair = EquippedWeapon->GetCrosshair())
			{
				CurrentFOV = FMath::FInterpTo(CurrentFOV, Crosshair->ZoomFOV, DeltaTime, Crosshair->ZoomInterpSpeed);
			}
		}
		else
		{
			if (UCrossHairDataAsset* Crosshair = EquippedWeapon->GetCrosshair())
			{
				CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, Crosshair->ZoomInterpSpeed);
			}
		}
	}
	else
	{
		if (FMath::IsNearlyEqual(DefaultFOV, CurrentFOV) == false)
		{
			CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, 30.f);
		}
	}


	if (UCameraComponent* Camera = Character->GetFollowCamera())
	{
		if (FMath::IsNearlyEqual(CurrentFOV, Camera->FieldOfView) == false)
			Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}

void UCombatComponent::InitializeCarriedAmmo()
{
	CarriedAmmoMap.Emplace(EWeaponType::EWT_AssaultRifle, 45);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_RocketLauncher, 4);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Pistol, 15);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SubmachineGun, 20);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Shotgun, 5);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SniperRifle, 5);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_GranadeLauncher, 10);
}

int32 UCombatComponent::AmountToReload()
{
	// oooxx : o ~ Ammo , x - RoomInMag
	if (EquippedWeapon == nullptr) return 0;
	int RoomInMag = EquippedWeapon->GetMagCapacity() - EquippedWeapon->GetAmmo();

	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		int32 AmountCarried = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
		int32 Least = FMath::Min(RoomInMag, AmountCarried);
		return FMath::Clamp(RoomInMag, 0, Least);
	}

	return 0;
}

void UCombatComponent::UpdateReloadAmmoValues()
{
	if (EquippedWeapon == nullptr) return;
	int32 ReloadAmount = AmountToReload();
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= ReloadAmount;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	EquippedWeapon->AddAmmo(ReloadAmount);
}

bool UCombatComponent::UpdateShotgunReloadAmmoValues()
{
	if (Character == nullptr || EquippedWeapon == nullptr) return false;

	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= 1;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
		EquippedWeapon->AddAmmo(1);
		if (EquippedWeapon->IsFull() || IsCarriedAmmoEmpty())
		{
			return true;
		}
	}
	
	return false;
}

void UCombatComponent::MulticastJumpToShotgunEnd_Implementation()
{
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_JumpToSection(FName("ShotgunEnd"));
	}
}

///////////////
/// Network ///
///////////////

void UCombatComponent::ClientApplyCrosshair_Implementation(UCrossHairDataAsset* Crosshair)
{
	Controller = Controller ? Controller : Cast<AAlphaPlayerController>(Character->Controller);
	if (Controller)
	{
		//&& Character&& Character->IsLocallyControlled()
		Controller->SetCrosshair(Crosshair);
	}
}

void UCombatComponent::OnRep_EquippedWeapon(AWeapon* LastWeapon)
{
	if (EquippedWeapon && Character)
	{
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equippped);
		const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
		}
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;

		if (EquippedWeapon->EquipSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, EquippedWeapon->EquipSound, Character->GetActorLocation());
		}
	}
}

void UCombatComponent::OnRep_CarriedAmmo()
{

}

void UCombatComponent::OnRep_CombatState()
{
	switch (CombatState)
	{
	case ECombatState::ECS_Unoccupied:
		if (bFireButtonPressed)
		{
			FireWeapon();
		}
		break;
	case ECombatState::ECS_Reloading:
		HandleReload();
		break;
	}
}

void UCombatComponent::ServerReload_Implementation()
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;

	
	CombatState = ECombatState::ECS_Reloading;
	HandleReload();
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAming)
{
	bAiming = bIsAming;
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::ServerFire_Implementation(bool bFire, const FVector_NetQuantize& TraceHitTarget)
{
	UE_LOG(LogTemp, Warning, TEXT("ServerFire ~ %d"), bFire);

	bFireButtonPressed = bFire;
	MulticastFire(bFire, TraceHitTarget);
	if (EquippedWeapon)
	{
		EquippedWeapon->SpendRound();
	}
}

void UCombatComponent::MulticastFire_Implementation(bool bFire, const FVector_NetQuantize& TraceHitTarget)
{
	if (EquippedWeapon == nullptr) return;
	if (Character && CombatState == ECombatState::ECS_Reloading && EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Shotgun)
	{
		Character->PlayFireMontage(bFire);
		EquippedWeapon->Fire(TraceHitTarget);
		CombatState = ECombatState::ECS_Unoccupied;
		return;
	}
	if (Character && CombatState == ECombatState::ECS_Unoccupied)
	{
		Character->PlayFireMontage(bFire);
		EquippedWeapon->Fire(TraceHitTarget);
	}
}
