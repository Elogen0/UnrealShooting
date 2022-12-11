// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProjectileRocket.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "Weapon/RocketMovementComponent.h"

AProjectileRocket::AProjectileRocket()
{
    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
    ProjectileMesh->SetupAttachment(RootComponent);
    ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    RocketMovementComponent = CreateDefaultSubobject<URocketMovementComponent>(TEXT("RocketMovementComponent"));
    RocketMovementComponent->bRotationFollowsVelocity = true;
    RocketMovementComponent->SetIsReplicated(true);
}

void AProjectileRocket::BeginPlay()
{
    Super::BeginPlay();

    if (!HasAuthority())
    {
        CollisionBox->OnComponentHit.AddDynamic(this, &AProjectileRocket::OnHit);
    }

    SpawnTrailSystem();

    if (ProjectileLoopSound && LoopingSoundAttenuation)
    {
        ProjectileLoopSoundComponent = UGameplayStatics::SpawnSoundAttached(
            ProjectileLoopSound,
            GetRootComponent(), 
            FName(), 
            GetActorLocation(), 
            EAttachLocation::KeepWorldPosition, 
            false, 
            1.f, 
            1.f, 
            0.f, 
            LoopingSoundAttenuation, 
            (USoundConcurrency*)nullptr, 
            false);
    }
}

void AProjectileRocket::Destroyed()
{
}

void AProjectileRocket::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor == GetOwner())
        return;

    ExplodeDamage();

    StartDestroyTimer();

    if (ImpactParticles)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
    }
    if (ImpactSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
    }
    if (ProjectileMesh)
    {
        ProjectileMesh->SetVisibility(false);
    }
    if (CollisionBox)
    {
        CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    if (TrailSystemComponent)
    {
        TrailSystemComponent->GetSystemInstance()->Deactivate();
    }
    if (ProjectileLoopSoundComponent && ProjectileLoopSoundComponent->IsPlaying())
    {
        ProjectileLoopSoundComponent->Stop();
    }
}