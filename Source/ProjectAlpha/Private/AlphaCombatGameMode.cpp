// Fill out your copyright notice in the Description page of Project Settings.


#include "AlphaCombatGameMode.h"
#include "AlphaPlayerController.h"
#include "AlphaCharacter.h"
#include "AlphaPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

void AAlphaCombatGameMode::PlayerEliminated(AAlphaCharacter* ElimmedCharacter, AAlphaPlayerController* VictimController, AAlphaPlayerController* AttackerController)
{
    AAlphaPlayerState* AttackerPlayerState = AttackerController ? Cast<AAlphaPlayerState>(AttackerController->PlayerState) : nullptr;
    AAlphaPlayerState* VictimPlayerState = VictimController ? Cast<AAlphaPlayerState>(VictimController->PlayerState) : nullptr;
    
    if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState )
    {
        AttackerPlayerState->AddToScore(1.f);
    }
    if (VictimPlayerState)
    {
        VictimPlayerState->AddToDefeats(1);
    }
    if (ElimmedCharacter)
    {
        ElimmedCharacter->Elim();
    }
}

void AAlphaCombatGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
    if (ElimmedCharacter)
    {
        ElimmedCharacter->Reset();
        ElimmedCharacter->Destroy();
    }
    if (ElimmedController)
    {
        TArray<AActor*> PlayerStarts;
        UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
        int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
        RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
    }
}
