// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectAlphaGameMode.h"
#include "ProjectAlphaCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProjectAlphaGameMode::AProjectAlphaGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
