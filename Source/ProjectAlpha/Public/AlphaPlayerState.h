// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AlphaPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTALPHA_API AAlphaPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	virtual void OnRep_Score() override;
	UFUNCTION()	
	virtual void OnRep_Defeats();
	void AddToScore(float ScoreAmount);
	void AddToDefeats(int32 DefeatsAmount);
protected:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	
protected:
	UPROPERTY()
	class AAlphaCharacter* Character;
	UPROPERTY()
	class AAlphaPlayerController* Controller;
	UPROPERTY(ReplicatedUsing= OnRep_Defeats, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	int32 Defeats;
};
