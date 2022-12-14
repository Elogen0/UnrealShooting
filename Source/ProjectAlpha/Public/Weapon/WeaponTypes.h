#pragma once

#define TRACE_LENGTH 80000.F

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    EWT_AssaultRifle UMETA(DisplayName="Assault Rifle"),
    EWT_RocketLauncher UMETA(DisplayName="Rocket Launcher"),
    EWT_Pistol UMETA(DisplayName="Pistol"),
    EWT_SubmachineGun UMETA(DisplayName="SubmachineGun"),
    EWT_Shotgun UMETA(DisplayName="Shotgun"),
    EWT_SniperRifle UMETA(DisplayName="Sniper Rifle"),
    EWT_GranadeLauncher UMETA(DisplayName="Granade Launcher"),
    
    EWT_MAX UMETA(DisplayName="Default Max")
};