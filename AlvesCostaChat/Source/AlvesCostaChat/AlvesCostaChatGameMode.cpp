// Copyright Epic Games, Inc. All Rights Reserved.

#include "AlvesCostaChatGameMode.h"
#include "AlvesCostaChatCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAlvesCostaChatGameMode::AAlvesCostaChatGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
