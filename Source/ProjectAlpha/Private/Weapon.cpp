// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "AlphaCharacter.h"
#include "Animation/AnimationAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "BulletCasing.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnStartSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnEndSphreOverlap);

	PickupWidget->SetVisibility(false);
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeapon, WeaponState);
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::OnStartSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AAlphaCharacter* Character = Cast<AAlphaCharacter>(OtherActor);
	if (Character)
	{
		Character->SetOverlappingWeapon(this);
	}
}

void AWeapon::OnEndSphreOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AAlphaCharacter* Character = Cast<AAlphaCharacter>(OtherActor);
	if (Character)
	{
		Character->SetOverlappingWeapon(nullptr);
	}
}

void AWeapon::OnRep_WeaponState()
{
	WeaponStateChanedProcess();
}

void AWeapon::WeaponStateChanedProcess()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Initial:
		break;
	case EWeaponState::EWS_Equippped:
		ShowPickupWidget(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::EWS_Dropped:
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	case EWeaponState::EWS_Max:
		break;
	default:
		break;
	}
}


void AWeapon::ShowPickupWidget(bool Visibility)
{
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(Visibility);
	}
}

void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
	WeaponStateChanedProcess();
}

void AWeapon::Fire(const FVector& HitTarget)
{
	if (FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}
	if (CasingClass)
	{
		APawn* InstigatorPawn = Cast<APawn>(GetOwner());
		const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName(FName("AmmoEject"));
		if (AmmoEjectSocket)
		{
			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMesh);
            UWorld* World = GetWorld();
            if (World)
            {
                World->SpawnActor<ABulletCasing>(CasingClass, SocketTransform.GetLocation(), SocketTransform.GetRotation().Rotator());
            }
		}
	}
}

