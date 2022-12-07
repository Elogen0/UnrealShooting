// Fill out your copyright notice in the Description page of Project Settings.


#include "AlphaCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Weapon.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "AlphaComponents/CombatComponent.h"
#include "../ProjectAlpha.h"


// Sets default values
AAlphaCharacter::AAlphaCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;
}

bool AAlphaCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool AAlphaCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

float AAlphaCharacter::CalculateMovementSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
}

// Called when the game starts or when spawned
void AAlphaCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &AAlphaCharacter::ReceiveDamage);
	}
}

void AAlphaCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AAlphaCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(AAlphaCharacter, Health);
}

// Called every frame
void AAlphaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	HideCameraIfCharacterClose();
}

// Called to bind functionality to input
void AAlphaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAlphaCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->Character = this;
	}
}

void AAlphaCharacter::Jump()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void AAlphaCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled()) return;
	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

void AAlphaCharacter::ReceiveDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCuaser)
{
	Health = FMath::Clamp(Health - Damage, 0, MaxHealth);
	PlayHitReactMontage();
	UE_LOG(LogTemp, Warning, TEXT("Health : %f"), Health);
}


void AAlphaCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
}



void AAlphaCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (IsLocallyControlled() && OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}

	OverlappingWeapon = Weapon;
	if (IsLocallyControlled() && OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(OverlappingWeapon != nullptr);
	}
}



void AAlphaCharacter::EquipButtonPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("EquipButtonPressed"));
	ServerEquipButtonPressed();
}

void AAlphaCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}
}


void AAlphaCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	BP_ReplicatedMovement();
}

void AAlphaCharacter::OnRep_Health()
{
	PlayHitReactMontage();
}
