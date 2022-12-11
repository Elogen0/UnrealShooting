// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "AlphaCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

void AShotgun::Fire(const FVector& HitTarget)
{
    AWeapon::Fire(HitTarget);
    auto OwnerPawn = Cast<APawn>(GetOwner());
    if (OwnerPawn == nullptr) return;
    AController* InstigatorController = OwnerPawn->GetController();

    const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
    if (MuzzleFlashSocket)
    {
        FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
        FVector Start = SocketTransform.GetLocation();
        
        uint32 Hits = 0;
        TMap<AAlphaCharacter*, uint32> HitMap;
        for (uint32 i = 0; i < NumberOfPellets; ++i)
        {
            FHitResult FireHit;
            WeaponTraceHit(Start, HitTarget, FireHit);
            auto VictimCharacter = Cast<AAlphaCharacter>(FireHit.GetActor());
            if (VictimCharacter && HasAuthority() && InstigatorController)
            {
                if (!HitMap.Contains(VictimCharacter))
                {
                    HitMap.Emplace(VictimCharacter, 0);
                }
                ++HitMap[VictimCharacter];
            }

            if (ImpactParticles)
            {
                UGameplayStatics::SpawnEmitterAtLocation(
                    GetWorld(),
                    ImpactParticles,
                    FireHit.ImpactPoint,
                    FireHit.ImpactNormal.Rotation()
                );
            }
            if (HitSound)
            {
                UGameplayStatics::PlaySoundAtLocation(
                    this,
                    HitSound,
                    FireHit.ImpactPoint,
                    .5f,
                    FMath::FRandRange(-.5, .5f)
                );
            }
        }

        for (auto& HitPair : HitMap)
        {
            if (HitPair.Key && HasAuthority() && InstigatorController)
            {
                UGameplayStatics::ApplyDamage(
                    HitPair.Key,
                    Damage * HitPair.Value,
                    InstigatorController,
                    this,
                    UDamageType::StaticClass()
                );
            }
        }
    }
}
