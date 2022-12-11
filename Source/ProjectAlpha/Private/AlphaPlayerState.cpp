// Fill out your copyright notice in the Description page of Project Settings.


#include "AlphaPlayerState.h"
#include "AlphaCharacter.h"
#include "AlphaPlayerController.h"
#include "Net/UnrealNetwork.h"

void AAlphaPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AAlphaPlayerState, Defeats);
}


void AAlphaPlayerState::AddToScore(float ScoreAmount)
{
    SetScore(Score + ScoreAmount);
}

void AAlphaPlayerState::AddToDefeats(int32 DefeatsAmount)
{
    Defeats += DefeatsAmount;
}


void AAlphaPlayerState::OnRep_Score()
{
    Super::OnRep_Score();
}

void AAlphaPlayerState::OnRep_Defeats()
{
    
}
